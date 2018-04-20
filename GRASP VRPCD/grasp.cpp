#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance, int list_size){
	this->instance = instance;
	this->list_size = list_size;
}

Grasp::Grasp(){
}


int Grasp::get_closest_crossdock(Node nodo){
	int best_cd;
	float min_cost = FLT_MAX;
	vector<Crossdock> crossdocks = this->instance.crossdocks;
	for(int i=0; (unsigned)i<crossdocks.size();i++){
		int cost = nodo.get_distance(crossdocks[i]);
		if (cost < min_cost){
			min_cost = cost;
			best_cd = i;
		}
	}
	return	best_cd;
}


// funcion que retorna una lista de loss request mas bajos en costo, que cumplan con las condiciones de TW (request factibles)
tuple<vector<tuple<Request,float, int>>,bool> Grasp::get_cheaper_requests(vector<Request> requests, Vehicle vehicle){

	float suplier_cost, customer_cost, crossdock_cost, total_cost, depot_cost;
	float min_cost = FLT_MAX;
	bool flag = false;
	Suplier suplier;
	Customer customer;
	vector<tuple<Request,float, int>> selected_requests;
	int best_cd;  // indice del crossdock mas cercano para el ultimo nodo de una ruta

	for(int i = 0; (unsigned)i < requests.size(); i++){
		if(requests[i].demand <= vehicle.remaining_capacity){

			if(vehicle.pickup_route.empty()){
				suplier_cost = this->instance.vehicle_depot.get_distance(requests[i].suplier);
				best_cd = this->get_closest_crossdock(requests[i].suplier);
				crossdock_cost = requests[i].suplier.get_distance(this->instance.crossdocks[best_cd]);
				customer_cost = this->instance.crossdocks[best_cd].get_distance(requests[i].customer);
				depot_cost = requests[i].customer.get_distance(this->instance.vehicle_depot);

			}
			else{
				suplier = vehicle.pickup_route.back();
				customer = vehicle.delivery_route.back();
				suplier_cost = suplier.get_distance(requests[i].suplier);
				best_cd = this->get_closest_crossdock(requests[i].suplier);
				crossdock_cost = requests[i].suplier.get_distance(this->instance.crossdocks[best_cd]);
				customer_cost = customer.get_distance(requests[i].customer);
				depot_cost = requests[i].customer.get_distance(this->instance.vehicle_depot);

			}

			total_cost = suplier_cost + crossdock_cost + customer_cost + depot_cost;
			

			if(total_cost < min_cost){
				// se analiza si es factible agregar dicho par de nodos en relacion a los TW  (para MEMORIA deberia considerar los distintos CD tambien)
				Vehicle vehicle_temp = vehicle;
				vehicle_temp.pickup_route.push_back(requests[i].suplier);
				vehicle_temp.crossdock_route.clear();
				vehicle_temp.crossdock_route.push_back(this->instance.crossdocks[best_cd]);
				vehicle_temp.delivery_route.push_back(requests[i].customer);
				vehicle_temp.set_times();

				if(vehicle_temp.feasible_route()){

					if(selected_requests.size() < (unsigned)this->list_size){
						selected_requests.push_back(make_tuple(requests[i],total_cost, best_cd));
					}

					else if(selected_requests.size() == (unsigned)this->list_size){
						selected_requests.push_back(make_tuple(requests[i],total_cost, best_cd));
						std::sort(begin(selected_requests), end(selected_requests), [](tuple<Request, float, int> const &t1, tuple<Request, float, int> const &t2) {
					        return get<1>(t1) < get<1>(t2); // or use a custom compare function
					    	}
					    );
						selected_requests.pop_back();
						min_cost = get<1>(selected_requests.back());

					}

					else{
						cout << "ERROR, SE SOBREPASO EL TAMAÑO DE LA LISTA"<<endl;
					}
					// informa si se encontro un request que cumpla con las restricciones
					flag = true;
				}

			}
		}		
	}

	return make_tuple(selected_requests,flag);

}


// FUncion que genera una solucion inicial mediante greedy

Solution Grasp::initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	bool found = true;
	vector <Request>::iterator request_iterator;
	//Vector de costos, actualmente solo se usa para debugear, pero podria servir para algo mas adelante
	vector<int> costs;
	vector<tuple<Request,float,int>> requests_list;
	Request selected_request;
	int random;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot);
		// POR EL MOMENTO SE ASUME QUE EL PUNTO DE COMIENZO DE CADA VEHICULO ES EL PRIMERO CROSSDOCK DE LA INSTANCIA
		//vehicle.crossdock_route.push_back(this->instance.crossdocks[0]);
		found = true;

		while (found){

			tie(requests_list,found) = this->get_cheaper_requests(requests, vehicle);

			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){
				//Se agrega la componente aleatoria
				random = rand() % requests_list.size();
				selected_request = get<0>(requests_list[random]);
				int best_cd = get<2>(requests_list[random]);

				vehicle.pickup_route.push_back(selected_request.suplier);
				vehicle.delivery_route.push_back(selected_request.customer);

				vehicle.crossdock_route.clear();
				vehicle.crossdock_route.push_back(this->instance.crossdocks[best_cd]);

				vehicle.remaining_capacity -= selected_request.demand;

				vehicle.pickup_items.push_back(make_tuple(selected_request.demand, selected_request.suplier.id));
				vehicle.delivery_items.push_back(make_tuple(selected_request.demand, selected_request.customer.id));

				// se elimina el request asignado de la lista de requests
				for (request_iterator = requests.begin(); request_iterator != requests.end(); ++request_iterator) {
				    if (request_iterator->id == selected_request.id) {
				        request_iterator = requests.erase(request_iterator); // luego de borrar el iterador pasa a la siguiente posicion.
				        --request_iterator; // devuelve el iterador si quedo fuera de rango.
				    }
				}
			}
		
		}
		
		vehicle.set_times();
		solution.vehicles.push_back(vehicle);

	}

	return solution;
}


float Grasp::evaluation_function(Solution solution){

	vector <Suplier>::iterator suplier_iterator;
	vector <Customer>::iterator customer_iterator;
	vector <Crossdock>::iterator crossdock_iterator;
	vector <Vehicle>::iterator vehicle_iterator;
	Node current_node;
	float total_cost =0;

	if(solution.vehicles.empty() == false){

		for (vehicle_iterator = solution.vehicles.begin(); vehicle_iterator != solution.vehicles.end(); ++vehicle_iterator){
			//POR EL MOMENTO SOLO HAY 1 CD COMO PUNTO DE PARTIDA, EN LA MEMORIA SE DEBE AGREGAR COMO ATRIBUTO PARA CADA VEHICULO
			current_node = vehicle_iterator->vehicle_depot;

			if(vehicle_iterator->pickup_route.empty() == false){

				for (suplier_iterator = vehicle_iterator->pickup_route.begin(); suplier_iterator != vehicle_iterator->pickup_route.end(); ++suplier_iterator) {

					Suplier &new_suplier = *suplier_iterator;
					total_cost += current_node.get_distance(new_suplier);
					current_node = new_suplier;
				}

			}

			if(vehicle_iterator->crossdock_route.empty() == false){

				for (crossdock_iterator = vehicle_iterator->crossdock_route.begin(); crossdock_iterator != vehicle_iterator->crossdock_route.end(); ++crossdock_iterator) {

					Crossdock &new_crossdock = *crossdock_iterator;
					total_cost += current_node.get_distance(new_crossdock);
					current_node = new_crossdock;
				}

			}

			if(vehicle_iterator->delivery_route.empty() == false){

				for (customer_iterator = vehicle_iterator->delivery_route.begin(); customer_iterator != vehicle_iterator->delivery_route.end(); ++customer_iterator) {

					Customer &new_customer = *customer_iterator;
					total_cost += current_node.get_distance(new_customer);
					current_node = new_customer;
				}

			}

			//EN este caso siempre vuelve al CD inicial, PARA LA MEMORIA SE DEBEN CONSIDERAR LOS LUGARES TERMINALES
			total_cost += current_node.get_distance(vehicle_iterator->vehicle_depot);

		}

	}

	return total_cost;

}



Solution Grasp::two_opt(Solution solution){
	vector<Node> selected_route, new_route;
	vector<tuple<int,int>> selected_items;

	//se selecciona el vehiculo al azar
	int random_vehicle = rand() % solution.vehicles.size();
	Vehicle selected_vehicle = solution.vehicles[random_vehicle];
	// se selecciona la ruta a modificar del vehiculo al azar
	int random_type_route = rand() % 2;

	if(random_type_route == 0){
		//SE retorna la misma solucion si se elije una ruta con 1 solo nodo
		if(selected_vehicle.pickup_route.size() == 1){
			return solution;
		}

		for(int l=0; (unsigned)l < selected_vehicle.pickup_route.size(); l++){
			selected_route.push_back(selected_vehicle.pickup_route[l]);
			selected_items.push_back(selected_vehicle.pickup_items[l]);
		}

		//cout<< "LA RUTA SELECCIONADA ES DE PICKUP" <<endl;
	}

	else{
		//SE retorna la misma solucion si se elije una ruta con 1 solo nodo
		if(selected_vehicle.delivery_route.size() == 1){
			return solution;
		}

		for(int l=0; (unsigned)l < selected_vehicle.delivery_route.size(); l++){
			selected_route.push_back(selected_vehicle.delivery_route[l]);
			selected_items.push_back(selected_vehicle.delivery_items[l]);
		}

		//cout<< "LA RUTA SELECCIONADA ES DE DELIVERY" <<endl;
	}

	/*cout<< "[";
	for(int l=0; (unsigned)l < selected_route.size();l++){
		cout<<" "<<selected_route[l].id;
	}
	cout<< " ]" <<endl;*/

	//SE comienza a realizar el 2-opt, primero se seleccionan los 2 putnos de corte
	// y se verifica que no sean el mismo, luego se ordenan para que el i sea el menor y el k el mayor por conveniencia
	int i = rand() % selected_route.size();
	int k = rand() % selected_route.size();
	while(i == k){
		i = rand() % selected_route.size();
		k = rand() % selected_route.size();
	}
	if(i>k){
		int temp = i;
		i=k;
		k=temp;
	}

	//cout<<"Los puntos escogidos son: "<<i<<" y "<<k<<endl;

	// ahora se comienza a crear la nueva ruta, intercambiando los nodos correspondientes
	//PRIMERA PARTE 2-OPT, ANTES DEL PRIMER PUNTO SE AGREGA TODO NORMAL
	vector<tuple<int,int>> new_items;

	for(int j=0; j<i;j++){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
	}
	//SEGUNDA PARTE 2-OPT, DESPUES DEL PRIMER PUNTO SE AGREGA EN ORDEN INVERSO LOS NODOS HASTA EL 2 PUNTO
	for(int j=k; j>=i;j--){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
	}
	//TERCERA PARTE 2-OPT, LUEGO DEL 2 PUNTO SE AGREGA NORMAL
	for(int j=(k+1); (unsigned)j < selected_route.size();j++){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
	}
	// Se reemplaza la ruta antigua por la nueva segun corresponda
	if(random_type_route == 0){

		selected_vehicle.pickup_route.clear();

		for(int l=0; (unsigned)l < new_route.size(); l++){
			Suplier *suplier =  (Suplier *) &new_route[l];
			selected_vehicle.pickup_route.push_back(*suplier);
		}

		selected_vehicle.pickup_items = selected_items;

		/*cout<<"La nueva ruta de pickup es: "<<endl;
		cout<< "[";
		for(int l=0; (unsigned)l < selected_vehicle.pickup_route.size(); l++){
			cout<<" "<<selected_vehicle.pickup_route[l].id;
		}
		cout<< " ]" <<endl;*/
	}
	else{

		selected_vehicle.delivery_route.clear();

		for(int l=0; (unsigned)l < new_route.size(); l++){
			Customer *customer =  (Customer *) &new_route[l];
			selected_vehicle.delivery_route.push_back(*customer);
		}

		selected_vehicle.delivery_items = selected_items;

		/*cout<<"La nueva ruta de delivery es: "<<endl;
		cout<< "[";
		for(int l=0; (unsigned)l < selected_vehicle.delivery_route.size();l++){
			cout<<" "<<selected_vehicle.delivery_route[l].id;
		}
		cout<< " ]" <<endl;*/
	}

	//selected_vehicle.set_times();

	if(selected_vehicle.feasible_route()){
		// FINALMENTE SE REEMPLAZA EL VEHICULO CAMBIADO CON LA NUEVA RUTA SI ESTA ES FACTIBLE
		solution.vehicles[random_vehicle] = selected_vehicle;
		//cout<<"SII SOY FACTIBLE"<<endl;
	}
	else{
		//cout<<"NOOOO SOY FACTIBLE"<<endl;
	}

	/*cout<<"estoy devolviendo esta ruta "<<endl;
	cout<< "[";
	if(random_type_route == 0){
		for(int l=0; (unsigned)l < selected_vehicle.pickup_route.size();l++){
			cout<<" "<<solution.vehicles[random_vehicle].pickup_route[l].id;
		}
		cout<< " ]" <<endl;
	}
	else{
		for(int l=0; (unsigned)l < selected_vehicle.delivery_route.size();l++){
				cout<<" "<<solution.vehicles[random_vehicle].delivery_route[l].id;
			}
		cout<< " ]" <<endl;	
	}*/

	return solution;
}

// FUNCION QUE REALIZA LA CONSOLIDACION ENTRE 2 VEHICULOS Y CALCULA EL TIEMPO DE SALIDA DEL CD. (POR AHORA SOLO CONSIDERA EL DELIVERY ROUTE)
// VERIFICA EN PRIMER LUGAR QUE LA CONSOLIDACION SEA FACTIBLE EN CUANTO A CAPACIDAD
// SETEA EL CAMPO DEPARTURE_CD_TIME DE CADA VEHICULO, SETEA EL REMAINING_CAPACITY Y MODIFICA LOS DELIVERY_ITEMS SEGUN LA CONSOLIDACION
tuple<Vehicle,Vehicle,bool> Grasp::consolidation(tuple<Vehicle,int> tuple1, tuple<Vehicle, int> tuple2){

	Vehicle v1 = get<0>(tuple1);
	int n1 = get<1>(tuple1);
	Vehicle v2 = get<0>(tuple2);
	int n2 = get<1>(tuple2);

	// CANTIDAD DE ITEMS A DESCARGAR DE CADA VEHICULO
	int unload_items_v1 = get<0>(v1.delivery_items[n1]);
	int unload_items_v2 = get<0>(v2.delivery_items[n2]);


	//verificacion de factibilidad en la capacidad
	if(v1.remaining_capacity + unload_items_v1 < unload_items_v2){
		return make_tuple(v1, v2, false);
	}

	if(v2.remaining_capacity + unload_items_v2 < unload_items_v1){
		return make_tuple(v1, v2, false);
	}


	// TIEMPOS DE DESCARGA Y CARGA DE CADA VEHICULO
	int unload_time_v1 = unload_items_v1 * v1.unit_time + v1.fixed_time;
	int unload_time_v2 = unload_items_v2 * v2.unit_time + v2.fixed_time;
	int reload_time_v1 = unload_items_v2 * v1.unit_time + v1.fixed_time;
	int reload_time_v2 = unload_items_v1 * v2.unit_time + v2.fixed_time;

	// SE considera que los vehiculos solo pasan por un crossdock
	int cdtime1 = get<0>(v1.crossdock_times[0]);
	int cdtime2 = get<0>(v2.crossdock_times[0]);
	int arrival_time_v1 = max(cdtime1, v1.crossdock_route[0].ready_time);
	int arrival_time_v2 = max(cdtime2, v2.crossdock_route[0].ready_time);

	int total_unload_time_v1 = arrival_time_v1 + unload_time_v1;
	int total_unload_time_v2 = arrival_time_v2 + unload_time_v2;

	int ready_load_time_v1 = max(total_unload_time_v1, total_unload_time_v2);
	int ready_load_time_v2 = max(total_unload_time_v1, total_unload_time_v2);

	int final_departure_time_v1 = ready_load_time_v1 + reload_time_v1;
	int final_departure_time_v2 = ready_load_time_v2 + reload_time_v2;

	// SETEO DEL ATRIBUTO DEPARTURE_CD_TIME
	v1.departure_cd_time = final_departure_time_v1;
	v2.departure_cd_time = final_departure_time_v2;
	// SETEO DE REMAINING CAPACITY
	v1.remaining_capacity = v1.remaining_capacity + unload_items_v1 - unload_items_v2;
	v2.remaining_capacity = v2.remaining_capacity + unload_items_v2 - unload_items_v1;

	// CAMBIO DE DELIVERY_ITEMS
	tuple<int,int> tuple_temp = v1.delivery_items[n1];
	v1.delivery_items[n1] = v2.delivery_items[n2];
	v2.delivery_items[n2] = tuple_temp;

	// CAMBIO DE NODOS 
	Customer customer_temp = v1.delivery_route[n1];
	v1.delivery_route[n1] = v2.delivery_route[n2];
	v2.delivery_route[n2] = customer_temp;

	return make_tuple(v1, v2, true);
}




Solution Grasp::swap_node(Solution solution){

	int random_vehicle_1 = rand() % solution.vehicles.size();
	int random_vehicle_2 = rand() % solution.vehicles.size();
	while(random_vehicle_1 == random_vehicle_2){
		random_vehicle_2 = rand() % solution.vehicles.size();
	}

	while(solution.vehicles[random_vehicle_1].crossdock_route[0].id != solution.vehicles[random_vehicle_2].crossdock_route[0].id){
		random_vehicle_2 = rand() % solution.vehicles.size();
		random_vehicle_1 = rand() % solution.vehicles.size();
	}

	Vehicle vehicle_1 = solution.vehicles[random_vehicle_1];
	Vehicle vehicle_2 = solution.vehicles[random_vehicle_2];

	/*cout<<"ruta vehiculo 1 antes"<<endl;

	cout<< " [";

	for(int l=0; (unsigned)l < solution.vehicles[random_vehicle_1].delivery_route.size();l++){
		cout<<" "<<solution.vehicles[random_vehicle_1].delivery_route[l].id;
	}
	cout<< " ]" <<endl;

	cout<<"ruta vehiculo 2 antes"<<endl;

	cout<< " [";
	for(int l=0; (unsigned)l < solution.vehicles[random_vehicle_2].delivery_route.size();l++){
		cout<<" "<<solution.vehicles[random_vehicle_2].delivery_route[l].id;
	}
	cout<< " ]" <<endl;*/


	int random_node_1 = rand() % vehicle_1.delivery_route.size();
	int random_node_2 = rand() % vehicle_2.delivery_route.size();
	bool feasible;

	tie(vehicle_1, vehicle_2, feasible) = this->consolidation(make_tuple(vehicle_1,random_node_1), make_tuple(vehicle_2,random_node_2));

	if(!feasible){
		//cout<<"INFACTIBLE CAPACIDAD"<<endl;
		return solution;
	}
	else{

		if(vehicle_1.feasible_route() && vehicle_2.feasible_route()){
			//cout<<"movimiento SIIIIII FACTIBLE"<<endl;

			solution.vehicles[random_vehicle_1] = vehicle_1;
			solution.vehicles[random_vehicle_2] = vehicle_2;

			cout<<"Los vehiculos fueron: "<<random_vehicle_1<<"y"<<random_vehicle_2<<endl;
			cout<<"Los nodos fueron: "<<random_node_1<<"y"<<random_node_2<<endl;


			/*cout<<"ruta vehiculo 1 despues"<<endl;

			cout<< " [";

			for(int l=0; (unsigned)l < solution.vehicles[random_vehicle_1].delivery_route.size();l++){
				cout<<" "<<solution.vehicles[random_vehicle_1].delivery_route[l].id;
			}
			cout<< " ]" <<endl;

			cout<<"ruta vehiculo 2 despues"<<endl;

			cout<< " [";

			for(int l=0; (unsigned)l < solution.vehicles[random_vehicle_2].delivery_route.size();l++){
				cout<<" "<<solution.vehicles[random_vehicle_2].delivery_route[l].id;
			}
			cout<< " ]" <<endl;*/


		}
		else{
			//cout<<"movimiento NOOO factible"<<endl;

		}

	}

	return solution;

}






Solution Grasp::run(int iterations){

	Solution new_solution = this->initial_solution();
	Solution best_solution = new_solution;
	int best_time = this->evaluation_function(best_solution);
	int new_time;
	// Se comienzan las iteraciones haciendo el 2-opt, solo se acepta el cambio en la solucion si esta mejora
	for(int i = 1; i <= iterations; i++){

		//new_solution = this->two_opt(new_solution);
		new_solution = this->swap_node(new_solution);
		new_time = this->evaluation_function(new_solution);
			
		if(new_time < best_time){
			cout<<"Mejor= "<<best_time<<endl;
			cout<<"Actual= "<<new_time<<endl;
			best_solution = new_solution;
			best_time = new_time;
			cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;
		}
		else{
			new_solution = best_solution;
		}
		
	}

	return best_solution;

}