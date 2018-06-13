#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance, int list_size){
	this->instance = instance;
	this->list_size = list_size;
}

Grasp::Grasp(){
}


int Grasp::get_closest_crossdock(Node nodo){
	int best_cd=0;
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
tuple<vector<tuple<Request,int, float>>,bool> Grasp::get_cheaper_requests(vector<Request> requests, Vehicle vehicle){

	float suplier_cost, customer_cost, crossdock_cost, total_cost, depot_cost;
	float min_cost = FLT_MAX;
	bool flag = false;
	Suplier suplier;
	Customer customer;
	vector<tuple<Request,int, float>> selected_requests;
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
						selected_requests.push_back(make_tuple(requests[i], best_cd,total_cost));
					}

					else if(selected_requests.size() == (unsigned)this->list_size){
						selected_requests.push_back(make_tuple(requests[i],best_cd,total_cost));
						std::sort(begin(selected_requests), end(selected_requests), [](tuple<Request, int, float> const &t1, tuple<Request, int, float> const &t2) {
					        return get<2>(t1) < get<2>(t2); 
					    	}
					    );
						selected_requests.pop_back();
						min_cost = get<2>(selected_requests.back());

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

Solution Grasp::distance_initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	bool found = true;
	vector <Request>::iterator request_iterator;
	//Vector de costos, actualmente solo se usa para debugear, pero podria servir para algo mas adelante
	vector<int> costs;
	vector<tuple<Request,int,float>> requests_list;
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
				int best_cd = get<1>(requests_list[random]);

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




// funcion que retorna una lista de loss request mas bajos en costo, que cumplan con las condiciones de TW (request factibles)
tuple<vector<tuple<Request, int>>,bool> Grasp::get_best_demand_requests(vector<Request> requests, Vehicle vehicle){

	vector<Request> temp_requests = requests;
	// Se ordenan los request de mayor a menor demanda
	std::sort(begin(temp_requests), end(temp_requests), [](Request const &r1, Request const &r2) {
        return r1.demand > r2.demand; 
    	}
    );

    bool flag = false;
	vector<tuple<Request, int>> selected_requests;
	int best_cd;  // indice del crossdock mas cercano para el ultimo nodo de una ruta

	for(int i = 0; (unsigned)i < temp_requests.size(); i++){
		if(temp_requests[i].demand <= vehicle.remaining_capacity){

			best_cd = this->get_closest_crossdock(temp_requests[i].suplier);


			// se analiza si es factible agregar dicho par de nodos en relacion a los TW  (para MEMORIA deberia considerar los distintos CD tambien)
			Vehicle vehicle_temp = vehicle;
			vehicle_temp.pickup_route.push_back(temp_requests[i].suplier);
			vehicle_temp.crossdock_route.clear();
			vehicle_temp.crossdock_route.push_back(this->instance.crossdocks[best_cd]);
			vehicle_temp.delivery_route.push_back(temp_requests[i].customer);

			if(vehicle_temp.feasible_route()){

				flag = true;

				if(selected_requests.size() < (unsigned)this->list_size){
					selected_requests.push_back(make_tuple(temp_requests[i],best_cd));
				}

				else if(selected_requests.size() == (unsigned)this->list_size){
					return make_tuple(selected_requests,flag);
				}

				else{
					cout << "ERROR, SE SOBREPASO EL TAMAÑO DE LA LISTA"<<endl;
				}
				
			}
			
	
		}		
	}

	return make_tuple(selected_requests,flag);

}



Solution Grasp::demand_initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	bool found = true;
	vector <Request>::iterator request_iterator;
	vector<tuple<Request, int>> requests_list;
	Request selected_request;
	int random;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot);
		// POR EL MOMENTO SE ASUME QUE EL PUNTO DE COMIENZO DE CADA VEHICULO ES EL PRIMERO CROSSDOCK DE LA INSTANCIA
		//vehicle.crossdock_route.push_back(this->instance.crossdocks[0]);
		found = true;

		while (found){

			tie(requests_list,found) = this->get_best_demand_requests(requests, vehicle);

			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){
				//Se agrega la componente aleatoria
				random = rand() % requests_list.size();
				selected_request = get<0>(requests_list[random]);
				int best_cd = get<1>(requests_list[random]);

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


Solution Grasp::hybrid_initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	bool found = true;
	vector <Request>::iterator request_iterator;
	vector<tuple<Request, int>> requests_list_1;
	vector<tuple<Request,int,float>> requests_list_2;
	Request selected_request;
	int random;
	bool changed = false;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot);
		// POR EL MOMENTO SE ASUME QUE EL PUNTO DE COMIENZO DE CADA VEHICULO ES EL PRIMERO CROSSDOCK DE LA INSTANCIA
		//vehicle.crossdock_route.push_back(this->instance.crossdocks[0]);
		found = true;
		int best_cd = -1;

		while (found){

			if(!changed){
				tie(requests_list_1,found) = this->get_best_demand_requests(requests, vehicle);
			}
			else{
				tie(requests_list_2,found) = this->get_cheaper_requests(requests, vehicle);
			}


			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){
				//Se agrega la componente aleatoria
				if(!changed){
					random = rand() % requests_list_1.size();
					selected_request = get<0>(requests_list_1[random]);
					best_cd = get<1>(requests_list_1[random]);
				}
				else{
					random = rand() % requests_list_2.size();
					selected_request = get<0>(requests_list_2[random]);
					best_cd = get<1>(requests_list_2[random]);
				}
			

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

		if(vehicle.remaining_capacity >= 5){
			changed = true;
		}

	}

	return solution;
}



tuple<vector<tuple<Request, int>>,bool> Grasp::get_best_timewindow_requests(vector<Request> requests, Vehicle vehicle){

	vector<Request> temp_requests = requests;
	// Se ordenan los request de menor a mayor ready_time del proveedor
	std::sort(begin(temp_requests), end(temp_requests), [](Request const &r1, Request const &r2) {
        return r1.suplier.ready_time < r2.suplier.ready_time; 
    	}
    );

    bool flag = false;
	vector<tuple<Request, int>> selected_requests;
	int best_cd;  // indice del crossdock mas cercano para el ultimo nodo de una ruta

	for(int i = 0; (unsigned)i < temp_requests.size(); i++){
		if(temp_requests[i].demand <= vehicle.remaining_capacity){

			best_cd = this->get_closest_crossdock(temp_requests[i].suplier);


			// se analiza si es factible agregar dicho par de nodos en relacion a los TW 
			Vehicle vehicle_temp = vehicle;
			vehicle_temp.pickup_route.push_back(temp_requests[i].suplier);
			vehicle_temp.crossdock_route.clear();
			vehicle_temp.crossdock_route.push_back(this->instance.crossdocks[best_cd]);
			vehicle_temp.delivery_route.push_back(temp_requests[i].customer);

			if(vehicle_temp.feasible_route()){

				flag = true;

				if(selected_requests.size() < (unsigned)this->list_size){
					selected_requests.push_back(make_tuple(temp_requests[i],best_cd));
				}

				else if(selected_requests.size() == (unsigned)this->list_size){
					return make_tuple(selected_requests,flag);
				}

				else{
					cout << "ERROR, SE SOBREPASO EL TAMAÑO DE LA LISTA"<<endl;
				}
				
			}
			
	
		}		
	}

	return make_tuple(selected_requests,flag);

}



Solution Grasp::timewindow_initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	bool found = true;
	vector <Request>::iterator request_iterator;
	vector<tuple<Request, int>> requests_list;
	Request selected_request;
	int random;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot);
		// POR EL MOMENTO SE ASUME QUE EL PUNTO DE COMIENZO DE CADA VEHICULO ES EL PRIMERO CROSSDOCK DE LA INSTANCIA
		//vehicle.crossdock_route.push_back(this->instance.crossdocks[0]);
		found = true;

		while (found){

			tie(requests_list,found) = this->get_best_timewindow_requests(requests, vehicle);

			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){
				//Se agrega la componente aleatoria
				random = rand() % requests_list.size();
				selected_request = get<0>(requests_list[random]);
				int best_cd = get<1>(requests_list[random]);

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

	/*vector <Vehicle>::iterator vehicle_iterator;
	Node current_node;
	float total_cost =0;

	if(solution.vehicles.empty() == false){

		for (vehicle_iterator = solution.vehicles.begin(); vehicle_iterator != solution.vehicles.end(); ++vehicle_iterator){
			
			total_cost += vehicle_iterator->get_total_cost();
		}

	}

	return total_cost;*/

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



Solution Grasp::mov_two_opt(Solution solution){
	vector<Node> selected_route, new_route;
	vector<tuple<int,int>> selected_items;
	int vehicle_position = -1;
	int type_route = -1;

	tie(vehicle_position,type_route) = this->get_worst_route(solution,-1);

	//se selecciona la ruta mas cara
	//int random_vehicle = rand() % solution.vehicles.size();



	Vehicle selected_vehicle = solution.vehicles[vehicle_position];
	//cout<<"VEhiculo escogido: "<<vehicle_position<<endl;
	// se selecciona la ruta a modificar del vehiculo al azar
	//int random_type_route = rand() % 2;

	if(type_route == 0){
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
	if(type_route == 0){

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
		solution.vehicles[vehicle_position] = selected_vehicle;
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

// FUNCION QUE REALIZA LA CONSOLIDACION ENTRE 2 VEHICULOS Y CALCULA EL TIEMPO DE SALIDA DEL CD. 
// VERIFICA EN PRIMER LUGAR QUE LA CONSOLIDACION SEA FACTIBLE EN CUANTO A CAPACIDAD
// SETEA EL CAMPO DEPARTURE_CD_TIME DE CADA VEHICULO, SETEA EL REMAINING_CAPACITY Y MODIFICA LOS DELIVERY_ITEMS SEGUN LA CONSOLIDACION
tuple<Vehicle,Vehicle,bool> Grasp::consolidation(tuple<Vehicle,int> tuple1, tuple<Vehicle, int> tuple2, int type){

	Vehicle v1 = get<0>(tuple1);
	int n1 = get<1>(tuple1);
	Vehicle v2 = get<0>(tuple2);
	int n2 = get<1>(tuple2);
	int unload_items_v1,unload_items_v2;

	if(type == 0){
		// CANTIDAD DE ITEMS A DESCARGAR DE CADA VEHICULO
		unload_items_v1 = get<0>(v1.pickup_items[n1]);
		unload_items_v2 = get<0>(v2.pickup_items[n2]);
	}
	else{
		// CANTIDAD DE ITEMS A DESCARGAR DE CADA VEHICULO
		unload_items_v1 = get<0>(v1.delivery_items[n1]);
		unload_items_v2 = get<0>(v2.delivery_items[n2]);
	}


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


	if(type == 0){
		// CAMBIO DE NODOS 
		Suplier suplier_temp = v1.pickup_route[n1];
		v1.pickup_route[n1] = v2.pickup_route[n2];
		v2.pickup_route[n2] = suplier_temp;
	}
	else{
		// CAMBIO DE NODOS 
		Customer customer_temp = v1.delivery_route[n1];
		v1.delivery_route[n1] = v2.delivery_route[n2];
		v2.delivery_route[n2] = customer_temp;
	}

	v1.set_times();
	v2.set_times();

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

	if(type == 0){
		// CAMBIO DE DELIVERY_ITEMS
		tuple<int,int> tuple_temp = v1.pickup_items[n1];
		v1.pickup_items[n1] = v2.pickup_items[n2];
		v2.pickup_items[n2] = tuple_temp;
	}
	else{
		// CAMBIO DE DELIVERY_ITEMS
		tuple<int,int> tuple_temp = v1.delivery_items[n1];
		v1.delivery_items[n1] = v2.delivery_items[n2];
		v2.delivery_items[n2] = tuple_temp;
	}

	v1.set_times();
	v2.set_times();

	return make_tuple(v1, v2, true);
}

// MOVIMIENTO QUE INTERCAMBIA DOS NODOS DEL MISMO TIPO DE RUTA DE DOS VEHICULOS DISTINTOS

Solution Grasp::mov_swap_node(Solution solution, int type){

	//int random_vehicle_1 = rand() % solution.vehicles.size();
	//int random_vehicle_2 = rand() % solution.vehicles.size();

	//SE encuentra el vehiculo que posea la ruta mas cara
	int type_temp,pos_vehicle_1,pos_vehicle_2;
	tie(pos_vehicle_1,type_temp) = get_worst_route(solution,type);

	Solution temp_solution = solution;

	temp_solution.vehicles.erase(temp_solution.vehicles.begin() + pos_vehicle_1);
	//SE ENCUENTRA EL SEGUNDO VEHICULO CON RUTA MAS CARA
	tie(pos_vehicle_2,type_temp) = get_worst_route(temp_solution,type);

	//SE calcula el indice del vehiculo 2, considerando la solucion con todo los vehiculos (sin eliminar 1)
	if(pos_vehicle_2 >= pos_vehicle_1){
		pos_vehicle_2 += 1;
	}

	//cout<<"Los vehiculos fueron: "<<random_vehicle_1<<" y "<<random_vehicle_2<<endl;

	/*while(random_vehicle_1 == random_vehicle_2 || solution.vehicles[random_vehicle_1].crossdock_route[0].id != solution.vehicles[random_vehicle_2].crossdock_route[0].id){
		//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
		random_vehicle_2 = rand() % solution.vehicles.size();
		random_vehicle_1 = rand() % solution.vehicles.size();
	}*/

	//cout<<"Los vehiculos fueron: "<<pos_vehicle_1<<" y "<<pos_vehicle_2<<endl;

	Vehicle vehicle_1 = solution.vehicles[pos_vehicle_1];
	Vehicle vehicle_2 = solution.vehicles[pos_vehicle_2];

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

	int random_node_1, random_node_2;
	bool feasible;


	if(type == 0){
		random_node_1 = rand() % vehicle_1.pickup_route.size();
		random_node_2 = rand() % vehicle_2.pickup_route.size();
		tie(vehicle_1, vehicle_2, feasible) = this->consolidation(make_tuple(vehicle_1,random_node_1), make_tuple(vehicle_2,random_node_2), 0);
	}
	else{
		random_node_1 = rand() % vehicle_1.delivery_route.size();
		random_node_2 = rand() % vehicle_2.delivery_route.size();
		tie(vehicle_1, vehicle_2, feasible) = this->consolidation(make_tuple(vehicle_1,random_node_1), make_tuple(vehicle_2,random_node_2), 1);
	}

	//cout<<"Los nodos fueron: "<<random_node_1<<"y"<<random_node_2<<endl;

	if(!feasible){
		//cout<<"INFACTIBLE CAPACIDAD"<<endl;
		return solution;
	}
	else{

		if(vehicle_1.feasible_route() && vehicle_2.feasible_route()){
			//cout<<"movimiento SIIIIII FACTIBLE"<<endl;

			solution.vehicles[pos_vehicle_1] = vehicle_1;
			solution.vehicles[pos_vehicle_2] = vehicle_2;

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

// MOVIMIENTO QUE INTERCAMBIA LOS CD DE UN VEHICULO
Solution Grasp::mov_swap_cd(Solution solution){

	if(this->instance.crossdocks_number == 1){
		return solution;
	}

	int random_vehicle = rand() % solution.vehicles.size();
	int random_cd = rand() % this->instance.crossdocks_number;

	Vehicle vehicle = solution.vehicles[random_vehicle];

	while(random_cd == vehicle.crossdock_route[0].id){
		random_cd = rand() % this->instance.crossdocks_number;
	}

	//cout<<"Cd vehiculo antes: "<< vehicle.crossdock_route[0].id <<endl;


	Crossdock cd = this->instance.crossdocks[random_cd];

	vehicle.crossdock_route[0] = cd;

	if(vehicle.feasible_route()){
		solution.vehicles[random_vehicle] = vehicle;
		//cout<< "movimiento factible"<< endl;
		//cout<<"Cd vehiculo despues: "<< solution.vehicles[random_vehicle].crossdock_route[0].id <<endl;
	}
	else{
		//cout<< "movimiento NOOO factible"<< endl;
		//cout<<"Cd vehiculo despues: " << solution.vehicles[random_vehicle].crossdock_route[0].id <<endl;
	}

	return solution;

}


//FUNCION QUE RETORNA LA RUTA MAS CARA ENTRE TODOS LOS VEHICULOS DE LA FORMA <POS_VEHICULO, TIPO_RUTA>, SI TYPE ES -1 BUSCA ENTRE TODAS LAS RUTAS, SI ES 0 EN PICKUP SI ES 1 EN DELIVERY
tuple<int,int> Grasp::get_worst_route(Solution solution, int type){

	float max_pickup_cost = 0;
	float max_delivery_cost = 0;
	int max_pickup_vehicle = -1;
	int max_delivery_vehicle = -1;
	float actual_pickup_cost,actual_delivery_cost;

  	for(int i=0; (unsigned)i<solution.vehicles.size(); i++){

  		if(type == -1){

	  		actual_pickup_cost = solution.vehicles[i].get_pickup_cost();
	  		actual_delivery_cost = solution.vehicles[i].get_delivery_cost();

	  		if (actual_pickup_cost > max_pickup_cost){
	  			max_pickup_cost = actual_pickup_cost;
	  			max_pickup_vehicle = i;
	  		}

	  		if (actual_delivery_cost > max_delivery_cost){
	  			max_delivery_cost = actual_delivery_cost;
	  			max_delivery_vehicle = i;
	  		}
	  	}


	  	else if(type==0){
	  		actual_pickup_cost = solution.vehicles[i].get_pickup_cost();
	  		if (actual_pickup_cost > max_pickup_cost){
	  			max_pickup_cost = actual_pickup_cost;
	  			max_pickup_vehicle = i;
	  		}
	  	}

	  	else{
	  		actual_delivery_cost = solution.vehicles[i].get_delivery_cost();
			if (actual_delivery_cost > max_delivery_cost){
	  			max_delivery_cost = actual_delivery_cost;
	  			max_delivery_vehicle = i;
	  		}
	  	}
  	}

  	if(type == -1){
	  	if(max_pickup_cost > max_delivery_cost){
	  		return make_tuple(max_pickup_vehicle,0);
	  	}
	  	else if(max_delivery_cost > max_pickup_cost){
	  		return make_tuple(max_delivery_vehicle,1);
	  	}
	  	else{
	  		int random_type_route = rand() % 2;
	  		if(random_type_route == 0){
	  			return make_tuple(max_pickup_vehicle,0);
	  		}
	  		else{
	  			return make_tuple(max_delivery_vehicle,1);
	  		}
	  	}
  	}
  	else if(type == 0){
  		return make_tuple(max_pickup_vehicle,0);
  	}
  	else{
  		return make_tuple(max_delivery_vehicle,1);
  	}

}


int Grasp::get_more_capacity(Solution solution, int type){

	float max_pickup_capacity = 0;
	float max_delivery_capacity = 0;
	int max_pickup_vehicle = -1;
	int max_delivery_vehicle = -1;
	float actual_pickup_capacity,actual_delivery_capacity;

  	for(int i=0; (unsigned)i<solution.vehicles.size(); i++){

	  	if(type==0){
	  		actual_pickup_capacity = solution.vehicles[i].get_pickup_capacity();
	  		if (actual_pickup_capacity > max_pickup_capacity){
	  			max_pickup_capacity = actual_pickup_capacity;
	  			max_pickup_vehicle = i;
	  		}
	  	}

	  	else{
	  		actual_delivery_capacity = solution.vehicles[i].remaining_capacity;
			if (actual_delivery_capacity > max_delivery_capacity){
	  			max_delivery_capacity = actual_delivery_capacity;
	  			max_delivery_vehicle = i;
	  		}
	  	}
  	}

  	if(type == 0){
  		return max_pickup_vehicle;
  	}

  	else{
  		return max_delivery_vehicle;
  	}
  	
}


vector<int> Grasp::get_involved_vehicles(Vehicle vehicle, Solution solution){

	vector<int> upload_item_position;
	vector<int> vehicles_position;
	vector<int> pickup_id;
	int id;
	// primero se obtienen la posicion de los items que estan en delivery y no en pickup (los que debe cargar)
	upload_item_position = vehicle.get_items(1);
	// luego se obtienen los id de los nodos correspondientes a los items
	for(int i : upload_item_position){
		// verificacion para testear, luego se puede sacar
		id = (get<1>(vehicle.delivery_items[i])) * -1;
		if(vehicle.delivery_route[i].id != id * -1){
			cout<<" ERROR EN LA CONSOLIDACION, EL ITEM NO CORRESPONDE AL NODO GET INVOLVED VEHICLES" <<endl;
		}
		// -------------------------------------------------------------- 

		// SE busca entre todos los vehiculos, aquel que posea el nodo correspondiente en la ruta de pickup y se guarda la posicion
		for( int i=0; (unsigned)i < solution.vehicles.size(); i++ ){

			pickup_id.clear();

			// se crea el vector de enteros con los id para usar el find
			for(int j=0;(unsigned)j < solution.vehicles[i].pickup_route.size();j++){

				pickup_id.push_back(solution.vehicles[i].pickup_route[j].id );

			}
			// si el id se encuentra en la ruta de pickup del vehiculo i, se agrega esta posicion como vehiculo involucrado
			if ( find(pickup_id.begin(), pickup_id.end(), id) != pickup_id.end() ){

				if(!(find(vehicles_position.begin(), vehicles_position.end(), i) != vehicles_position.end())){
					vehicles_position.push_back(i);
				}

				break;
			}

		}

	}

	return vehicles_position;
}



Solution Grasp::consolidation2(Solution solution){

	// SE debe calcular el tiempo de descarga para todos los vehiculos
	vector<int> download_item_position;
	vector <Vehicle>::iterator vehicle_iterator;
	int cdtime, arrival_time, unload_items;
	// tiempo en el que termina de descargar todos los productos
	int d_time;
	// vector que contiene los tiempos de descarga de todos los vehiculos
	vector<int> download_times;
	Solution temp_solution = solution;

	for (vehicle_iterator = temp_solution.vehicles.begin(); vehicle_iterator != temp_solution.vehicles.end(); ++vehicle_iterator) {

		download_item_position = vehicle_iterator->get_items(0);
		cdtime = get<0>(vehicle_iterator->crossdock_times[0]);
		arrival_time = max(cdtime, vehicle_iterator->crossdock_route[0].ready_time);
		d_time = 0;

		for(int i : download_item_position){
			// verificacion para testear, luego se puede sacar
			int id = get<1>(vehicle_iterator->pickup_items[i]);
			if(vehicle_iterator->pickup_route[i].id != id){
				cout<<" ERROR EN LA CONSOLIDACION, EL ITEM NO CORRESPONDE AL NODO CONSOLIDATION2 DOWNLOAD" <<endl;
			}
			// -------------------------------------------------------------- 

			unload_items = get<0>(vehicle_iterator->pickup_items[i]);
	 		d_time +=  unload_items * vehicle_iterator->unit_time;

		}

		// si no hay items que descargar, el tiempo en el que esta listo es el arrival time
		if(download_item_position.empty()){
			d_time += arrival_time;
		}
		else{
			d_time += arrival_time + vehicle_iterator->fixed_time;
		}

	 	download_times.push_back(d_time);

	}

	//---------------------------------------------------------------------

	// PARA CADA VEHICULO SE DEBEN ENCONTRAR LOS VEHICULOS INVOLUCRADOS (LOS QUE SE ENCUENTREN EN DELIVERY Y NO EN PICKUP)
	vector<int> involved_vehicles_pos; 
	int ready_load_time, u_time, reload_items;


	for (int i = 0; (unsigned)i < temp_solution.vehicles.size(); i++){

		involved_vehicles_pos = this->get_involved_vehicles(temp_solution.vehicles[i],solution);
		// si no hay vehiculos involucrados el departure_cd_time = d_time
		if(involved_vehicles_pos.empty()){
			temp_solution.vehicles[i].departure_cd_time = download_times[i];
		}
		else{

			vector<int> download_involved_times;

			for(int j : involved_vehicles_pos){
				download_involved_times.push_back(download_times[j]);
			}
			// se agrega el tiempo de descarga del vehiculo i 
			download_involved_times.push_back(download_times[i]);

			// SE CALCULA EL READY LOAD TIME
			vector<int>::const_iterator it;
			it = max_element(download_involved_times.begin(), download_involved_times.end());
			ready_load_time = *it;

			// SE CALCULA EL TIEMPO DE CARGA DE ITEMS
			vector<int> upload_item_position = temp_solution.vehicles[i].get_items(1);

			u_time = 0;

			for(int j : upload_item_position){
				// verificacion para testear, luego se puede sacar
				int id = get<1>(temp_solution.vehicles[i].delivery_items[j]);
				if(temp_solution.vehicles[i].delivery_route[j].id != id){
					cout<<" ERROR EN LA CONSOLIDACION, EL ITEM NO CORRESPONDE AL NODO CONSOLIDATION2 UPLOAD" <<endl;
				}
				// -------------------------------------------------------------- 

				reload_items = get<0>(temp_solution.vehicles[i].delivery_items[j]);
		 		u_time +=  reload_items * temp_solution.vehicles[i].unit_time;

			}

			// tiempo de carga total para el vehiculo i
			u_time += temp_solution.vehicles[i].fixed_time;

			// finalmente se setea el departure_time
			temp_solution.vehicles[i].departure_cd_time = ready_load_time + u_time;

		}
		
	}


	return temp_solution;

}


bool Grasp::feasible_solution(Solution solution){

	//verifica si es factible en cuanto a capacidad
	if(!(solution.feasible_capacity())) return false;

	// se aplica set_times() a todos los vehiculos
	solution.set_vehicles_times();

	//se aplica la consolidacion
	solution = this->consolidation2(solution);

	// se aplica nuevamente el set_times() a todos los vehiculos para setear el departure_cd_time
	solution.set_vehicles_times();

	// se revisa la factibilidad en cuanto a TW
	if(!(solution.feasible_tw())) return false;


	return true;
}




Solution Grasp::mov_change_node(Solution solution){

	int type_route,pos_vehicle_1,pos_vehicle_2;
	tie(pos_vehicle_1,type_route) = get_worst_route(solution,-1);

	Solution temp_solution = solution;

	temp_solution.vehicles.erase(temp_solution.vehicles.begin() + pos_vehicle_1);
	//SE ENCUENTRA EL  VEHICULO QUE TENGA MAS CAPACIDAD SEGUN LA RUTA ESPECIFICADA
	pos_vehicle_2 = get_more_capacity(temp_solution,type_route);

	//SE calcula el indice del vehiculo 2, considerando la solucion con todo los vehiculos (sin eliminar 1)
	if(pos_vehicle_2 >= pos_vehicle_1){
		pos_vehicle_2 += 1;
	}


    return solution;

}



Solution Grasp::run(int iterations_phase1, int iterations_phase2,int iterations_phase3, int porc_two_opt, int porc_swap_cd, int porc_swap_node_pick, int porc_swap_node_del){


	cout<<"iteraciones phase 1: "<< iterations_phase1<<endl;
	cout<<"iteraciones phase 2: "<< iterations_phase2<<endl;
	cout<<"iteraciones phase 3: "<< iterations_phase3<<endl;
	cout<<"porcentaje 2opt: "<< porc_two_opt<<endl;
	cout<<"porcentaje swap_cd: "<< porc_swap_cd<<endl;
	cout<<"porcentaje swap pickup: "<< porc_swap_node_pick<<endl;
	cout<<"porcentaje swap delivery: "<< porc_swap_node_del<<endl;

	ofstream myfile;
	myfile.open ("result.txt");

	//------------------------------------------------------------------------------

	Solution new_solution = this->distance_initial_solution();
	//Solution new_solution = this->demand_initial_solution();
	//Solution new_solution = this->hybrid_initial_solution();
	//Solution new_solution = this->timewindow_initial_solution();

	//Solution asd = this->mov_change_node(new_solution);

	Solution best_solution = new_solution;
	float best_time = this->evaluation_function(best_solution);
	float new_time;

	clock_t start_time, end_time;
	double total_time;
	start_time = clock();

	// Se comienzan las iteraciones haciendo el 2-opt, solo se acepta el cambio en la solucion si esta mejora
	for(int i = 1; i <= iterations_phase1; i++){

		int random_move_1 = rand() % 101;
		int random_move_2 = rand() % 101;
		int random_move_3 = rand() % 101;

		if(random_move_1<porc_two_opt){
			new_solution = this->mov_two_opt(new_solution);
		}

		if(random_move_2<porc_swap_node_pick){
			new_solution = this->mov_swap_node(new_solution,0);
		}

		if(random_move_3<porc_swap_node_del){
			new_solution = this->mov_swap_node(new_solution,1);
		}


		new_time = this->evaluation_function(new_solution);
				
		if(new_time < best_time){
			cout<<"Mejor= "<<best_time<<endl;
			cout<<"Actual= "<<new_time<<endl;
			best_solution = new_solution;
			best_time = new_time;
			cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;

			end_time = clock();
			total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
			myfile << total_time <<"-"<< best_time << "\n";
		}
		else{
			new_solution = best_solution;
		}
		
	}


	//--------------------------------------------------------------------------------------

/*-------------------------------------------------------------------------------------------------------
	Solution new_solution = this->initial_solution();
	Solution best_solution = new_solution;
	int best_time = this->evaluation_function(best_solution);
	int new_time;
	// COMIENZA la FASE 1 en donde se aplican los movimientos 2-opt
	clock_t start_time, end_time;
	double total_time;
	start_time = clock();
	for(int i = 1; i <= iterations_phase1; i++){

		new_solution = this->two_opt(new_solution);

		new_time = this->evaluation_function(new_solution);
				
		if(new_time < best_time){
			cout<<"Mejor= "<<best_time<<endl;
			cout<<"Actual= "<<new_time<<endl;
			best_solution = new_solution;
			best_time = new_time;
			cout<<"-------------- MEJORE LA SOLUCION EN 2-OPT------------------"<<endl;

			end_time = clock();
			total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
			myfile << total_time <<"-"<< best_time << "\n";

		}
		else{
			new_solution = best_solution;
		}
		
	}

	// COMIENZA la FASE 2 en donde se aplican los movimientos swap CD

	for(int j = 1; j <= iterations_phase2; j++){
	
		new_solution = this->swap_cd(new_solution);

		new_time = this->evaluation_function(new_solution);
				
		if(new_time < best_time){
			cout<<"Mejor= "<<best_time<<endl;
			cout<<"Actual= "<<new_time<<endl;
			best_solution = new_solution;
			best_time = new_time;
			cout<<"-------------- MEJORE LA SOLUCION EN SWAP CD------------------"<<endl;

			end_time = clock();
			total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
			myfile << total_time <<"-"<< best_time<< "\n";
		}
		else{
			new_solution = best_solution;
		}
		
	}

	for(int k=1; k<=iterations_phase3;k++){
		
		int random_move_swap_node_del = rand() % 101;
		int random_move_swap_node_pick = rand() % 101;

		if(random_move_swap_node_pick<porc_swap_node_pick){
			new_solution = this->swap_node(new_solution,0);
		}

		if(random_move_swap_node_del<porc_swap_node_del){
			new_solution = this->swap_node(new_solution,1);
		}

		new_time = this->evaluation_function(new_solution);

		if(new_time < best_time){
			cout<<"Mejor= "<<best_time<<endl;
			cout<<"Actual= "<<new_time<<endl;
			best_solution = new_solution;
			best_time = new_time;
			cout<<"-------------- MEJORE LA SOLUCION EN SWAP NODE------------------"<<endl;
			
			end_time = clock();
			total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
			myfile << total_time <<"-"<< best_time<< "\n";
		}
		else{
			new_solution = best_solution;
		}

	}

	--------------------------------------------------------------------------------------------------------*/

	myfile.close();

	return best_solution;

}