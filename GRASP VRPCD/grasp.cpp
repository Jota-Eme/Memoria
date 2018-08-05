#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance, int list_size, int tabu_capacity_size, int tabu_worst_route_size, int size_window,float decay_factor,float explore_factor ){
	this->instance = instance;
	this->list_size = list_size;
	this->tabu_capacity_size = tabu_capacity_size;
	this->tabu_worst_route_size = tabu_worst_route_size;
	this->criteria = 0;
	this->size_window = size_window;
	this->decay_factor = decay_factor;
	this->explore_factor = explore_factor;

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
						sort(begin(selected_requests), end(selected_requests), [](tuple<Request, int, float> const &t1, tuple<Request, int, float> const &t2) {
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
	int id=0;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot, id, this->instance.speed);
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

				vehicle.involved_pickup.push_back(vehicle.id);
				vehicle.involved_delivery.push_back(vehicle.id);

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
		id++;

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
	int id=0;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot, id,this->instance.speed);
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

				vehicle.involved_pickup.push_back(vehicle.id);
				vehicle.involved_delivery.push_back(vehicle.id);

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
		id++;
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
	int id=0;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot, id,this->instance.speed);
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

				vehicle.involved_pickup.push_back(vehicle.id);
				vehicle.involved_delivery.push_back(vehicle.id);

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
		id++;

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
	int id=0;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet, this->instance.vehicle_depot, id,this->instance.speed);
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

				vehicle.involved_pickup.push_back(vehicle.id);
				vehicle.involved_delivery.push_back(vehicle.id);

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
		id++;

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


	for (vehicle_iterator = solution.vehicles.begin(); vehicle_iterator != solution.vehicles.end(); ++vehicle_iterator){
		//POR EL MOMENTO SOLO HAY 1 CD COMO PUNTO DE PARTIDA, EN LA MEMORIA SE DEBE AGREGAR COMO ATRIBUTO PARA CADA VEHICULO
		current_node = vehicle_iterator->vehicle_depot;

		if(vehicle_iterator->pickup_route.empty() == false){

			for (suplier_iterator = vehicle_iterator->pickup_route.begin(); suplier_iterator != vehicle_iterator->pickup_route.end(); ++suplier_iterator) {

				Suplier &new_suplier = *suplier_iterator;
				total_cost += current_node.get_distance(new_suplier)/vehicle_iterator->speed;
				current_node = new_suplier;
			}

		}

		if(vehicle_iterator->crossdock_route.empty() == false){

			for (crossdock_iterator = vehicle_iterator->crossdock_route.begin(); crossdock_iterator != vehicle_iterator->crossdock_route.end(); ++crossdock_iterator) {

				Crossdock &new_crossdock = *crossdock_iterator;
				total_cost += current_node.get_distance(new_crossdock)/vehicle_iterator->speed;
				current_node = new_crossdock;
			}

		}

		if(vehicle_iterator->delivery_route.empty() == false){

			for (customer_iterator = vehicle_iterator->delivery_route.begin(); customer_iterator != vehicle_iterator->delivery_route.end(); ++customer_iterator) {

				Customer &new_customer = *customer_iterator;
				total_cost += current_node.get_distance(new_customer)/vehicle_iterator->speed;
				current_node = new_customer;
			}

		}

		//EN este caso siempre vuelve al CD inicial, PARA LA MEMORIA SE DEBEN CONSIDERAR LOS LUGARES TERMINALES
		total_cost += current_node.get_distance(vehicle_iterator->vehicle_depot)/vehicle_iterator->speed;

	}


	return total_cost;

}



Solution Grasp::mov_two_opt(Solution solution){
	vector<Node> selected_route, new_route;
	vector<tuple<int,int>> selected_items;
	vector<int> selected_involved;
	//int vehicle_position = -1;
	//int type_route = -1;
	//vector<int> tabu_worst_route;

	//tie(vehicle_position,type_route) = this->get_worst_route(solution,-1,tabu_worst_route);

	//se selecciona la ruta mas cara
	int type_route = rand() % 2;
	int vehicle_position = rand() % solution.vehicles.size();

	if(type_route == 0){
		while(solution.vehicles[vehicle_position].pickup_route.size() == 0){
			//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
			vehicle_position = rand() % solution.vehicles.size();
		}
	}
	else{
		while(solution.vehicles[vehicle_position].delivery_route.size() == 0 ){
			//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
			vehicle_position = rand() % solution.vehicles.size();
		}
	}

// -----------------------------------------------------------------------------

	Vehicle selected_vehicle = solution.vehicles[vehicle_position];
	//cout<<"VEhiculo escogido: " << vehicle_position << endl;
	// se selecciona la ruta a modificar del vehiculo al azar

	if(type_route == 0){
		//SE retorna la misma solucion si se elije una ruta con 1 solo nodo
		if(selected_vehicle.pickup_route.size() == 1){
			return solution;
		}

		for(int l=0; (unsigned)l < selected_vehicle.pickup_route.size(); l++){
			selected_route.push_back(selected_vehicle.pickup_route[l]);
			selected_items.push_back(selected_vehicle.pickup_items[l]);
			selected_involved.push_back(selected_vehicle.involved_pickup[l]);
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
			selected_involved.push_back(selected_vehicle.involved_delivery[l]);
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
	/*int k = rand() % selected_route.size();
	while(i == k){
		i = rand() % selected_route.size();
		k = rand() % selected_route.size();
	}
	if(i>k){
		int temp = i;
		i=k;
		k=temp;
	}*/

	int k=-1;

	if((unsigned)i == selected_route.size() -1){
		k = i-1;
	}
	else{
		k = i+1;
	}





	//cout<<"Los puntos escogidos son: "<<i<<" y "<<k<<endl;

	// ahora se comienza a crear la nueva ruta, intercambiando los nodos correspondientes
	//PRIMERA PARTE 2-OPT, ANTES DEL PRIMER PUNTO SE AGREGA TODO NORMAL
	vector<tuple<int,int>> new_items;
	vector<int> new_involved;

	for(int j=0; j<i;j++){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
		new_involved.push_back(selected_involved[j]);
	}
	//SEGUNDA PARTE 2-OPT, DESPUES DEL PRIMER PUNTO SE AGREGA EN ORDEN INVERSO LOS NODOS HASTA EL 2 PUNTO
	for(int j=k; j>=i;j--){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
		new_involved.push_back(selected_involved[j]);

	}
	//TERCERA PARTE 2-OPT, LUEGO DEL 2 PUNTO SE AGREGA NORMAL
	for(int j=(k+1); (unsigned)j < selected_route.size();j++){
		new_route.push_back(selected_route[j]);
		new_items.push_back(selected_items[j]);
		new_involved.push_back(selected_involved[j]);

	}
	// Se reemplaza la ruta antigua por la nueva segun corresponda
	if(type_route == 0){

		selected_vehicle.pickup_route.clear();

		for(int l=0; (unsigned)l < new_route.size(); l++){
			Suplier *suplier =  (Suplier *) &new_route[l];
			selected_vehicle.pickup_route.push_back(*suplier);
		}

		selected_vehicle.pickup_items = new_items;
		selected_vehicle.involved_pickup = new_involved;

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

		selected_vehicle.delivery_items = new_items;
		selected_vehicle.involved_delivery = new_involved;


		/*cout<<"La nueva ruta de delivery es: "<<endl;
		cout<< "[";
		for(int l=0; (unsigned)l < selected_vehicle.delivery_route.size();l++){
			cout<<" "<<selected_vehicle.delivery_route[l].id;
		}
		cout<< " ]" <<endl;*/
	}

	//selected_vehicle.set_times();

	Solution temp_solution = solution;
	temp_solution.vehicles[vehicle_position] = selected_vehicle;

	if(this->feasible_solution(temp_solution)){
		// FINALMENTE SE REEMPLAZA EL VEHICULO CAMBIADO CON LA NUEVA RUTA SI ESTA ES FACTIBLE
		//cout<<"SII SOY FACTIBLE"<<endl;
		return temp_solution;
	}
	else{
		//cout<<"NOOOO SOY FACTIBLE"<<endl;
		return solution;
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

/*tuple<Vehicle,Vehicle,bool> Grasp::consolidation(tuple<Vehicle,int> tuple1, tuple<Vehicle, int> tuple2, int type){

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
}*/

// MOVIMIENTO QUE INTERCAMBIA DOS NODOS DEL MISMO TIPO DE RUTA DE DOS VEHICULOS DISTINTOS

Solution Grasp::mov_swap_node(Solution solution, int type){

	int pos_vehicle_1 = rand() % solution.vehicles.size();
	int pos_vehicle_2 = rand() % solution.vehicles.size();

	if(type == 0){
		while(pos_vehicle_1 == pos_vehicle_2 || solution.vehicles[pos_vehicle_1].crossdock_route[0].id != solution.vehicles[pos_vehicle_2].crossdock_route[0].id || solution.vehicles[pos_vehicle_1].pickup_route.size() == 0 || solution.vehicles[pos_vehicle_2].pickup_route.size() == 0){
			//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
			pos_vehicle_1 = rand() % solution.vehicles.size();
			pos_vehicle_2 = rand() % solution.vehicles.size();
		}
	}
	else{
		while(pos_vehicle_1 == pos_vehicle_2 || solution.vehicles[pos_vehicle_1].crossdock_route[0].id != solution.vehicles[pos_vehicle_2].crossdock_route[0].id || solution.vehicles[pos_vehicle_1].delivery_route.size() == 0 || solution.vehicles[pos_vehicle_2].delivery_route.size() == 0){
			//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
			pos_vehicle_1 = rand() % solution.vehicles.size();
			pos_vehicle_2 = rand() % solution.vehicles.size();
		}
	}




	//SE encuentra el vehiculo que posea la ruta mas cara
	//int type_temp,pos_vehicle_1,pos_vehicle_2;
	//vector<int> tabu_worst_route;
	//tie(pos_vehicle_1,type_temp) = get_worst_route(solution,type,tabu_worst_route);

	Solution temp_solution = solution;

	//temp_solution.vehicles.erase(temp_solution.vehicles.begin() + pos_vehicle_1);
	//SE ENCUENTRA EL SEGUNDO VEHICULO CON RUTA MAS CARA
	//tie(pos_vehicle_2,type_temp) = get_worst_route(temp_solution,type,tabu_worst_route);

	//SE calcula el indice del vehiculo 2, considerando la solucion con todo los vehiculos (sin eliminar 1)
	/*if(pos_vehicle_2 >= pos_vehicle_1){
		pos_vehicle_2 += 1;
	}*/

	//cout<<"Los vehiculos fueron: "<<random_vehicle_1<<" y "<<random_vehicle_2<<endl;

	

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
	temp_solution = solution;

	if(type == 0){
		random_node_1 = rand() % vehicle_1.pickup_route.size();
		random_node_2 = rand() % vehicle_2.pickup_route.size();

		//cout<<"VEHICULO 1: "<<vehicle_1.id << " nodo: "<<random_node_1<<endl;
		//cout<<"VEHICULO 2: "<<vehicle_2.id << " nodo: "<<random_node_2<<endl;

		//print_solution(temp_solution);


		//CAMBIO ESTRUCTURAS INVOLVED
		// SE buscan los vehiculos que va a entregar cada nodo a cambiar
		// caso en el que el primer nodo pertenezca al V1
		if(vehicle_1.id == vehicle_1.involved_pickup[random_node_1]){

			int id = vehicle_1.pickup_route[random_node_1].id * -1;
			auto it = find_if(vehicle_1.delivery_route.begin(), vehicle_1.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_1.delivery_route.end()){

			 	auto index = distance(vehicle_1.delivery_route.begin(), it);
				vehicle_1.involved_delivery[index] = vehicle_2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP"<<endl;
				cout<<"vehiculo: "<<vehicle_1.id << "nodo a encontrar en delivery: "<<id<<endl;
			}

		}
		// caso en el que el primer nodo pertenezca al V2
		else if(vehicle_2.id == vehicle_1.involved_pickup[random_node_1]){
			int id = vehicle_1.pickup_route[random_node_1].id * -1;
			auto it = find_if(vehicle_2.delivery_route.begin(), vehicle_2.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_2.delivery_route.end()){

			 	auto index = distance(vehicle_2.delivery_route.begin(), it);
				vehicle_2.involved_delivery[index] = vehicle_2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP"<<endl;
				cout<<"vehiculo: "<<vehicle_2.id << "nodo a encontrar en delivery: "<< id<<endl;

			}
		}

		// caso en el que el primer nodo pertenezca a otro vehiculo
		else{
			// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
			Vehicle vehicle_3 = temp_solution.vehicles[vehicle_1.involved_pickup[random_node_1]];
			int id = vehicle_1.pickup_route[random_node_1].id * -1;
			auto it = find_if(vehicle_3.delivery_route.begin(), vehicle_3.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_3.delivery_route.end()){

			 	auto index = distance(vehicle_3.delivery_route.begin(), it);
				vehicle_3.involved_delivery[index] = vehicle_2.id;
				temp_solution.vehicles[vehicle_1.involved_pickup[random_node_1]] = vehicle_3;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP TERCERO"<<endl;
				cout<<"vehiculo: "<<vehicle_3.id << "nodo a encontrar en delivery: "<< id<<endl;
				cout<<"id correcto: "<<vehicle_1.involved_pickup[random_node_1]<<endl;


			}

		}

		// -------------------------------------------------------------------------------------------------

		// caso en el que el segundo nodo pertenezca al V1
		if(vehicle_1.id == vehicle_2.involved_pickup[random_node_2]){

			int id = vehicle_2.pickup_route[random_node_2].id * -1;
			auto it = find_if(vehicle_1.delivery_route.begin(), vehicle_1.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_1.delivery_route.end()){

			 	auto index = distance(vehicle_1.delivery_route.begin(), it);
				vehicle_1.involved_delivery[index] = vehicle_1.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP"<<endl;
				cout<<"vehiculo: "<<vehicle_1.id << "nodo a encontrar en delivery: "<< id<<endl;

			}

		}
		// caso en el que el segundo nodo pertenezca al V2
		else if(vehicle_2.id == vehicle_2.involved_pickup[random_node_2]){
			int id = vehicle_2.pickup_route[random_node_2].id * -1;
			auto it = find_if(vehicle_2.delivery_route.begin(), vehicle_2.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_2.delivery_route.end()){

			 	auto index = distance(vehicle_2.delivery_route.begin(), it);
				vehicle_2.involved_delivery[index] = vehicle_1.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP"<<endl;
				cout<<"vehiculo: "<<vehicle_2.id << "nodo a encontrar en delivery: "<< id<<endl;

			}
		}

		// caso en el que el primer nodo pertenezca a otro vehiculo
		else{
			// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
			Vehicle vehicle_3 = temp_solution.vehicles[vehicle_2.involved_pickup[random_node_2]];
			int id = vehicle_2.pickup_route[random_node_2].id * -1;
			auto it = find_if(vehicle_3.delivery_route.begin(), vehicle_3.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle_3.delivery_route.end()){

			 	auto index = distance(vehicle_3.delivery_route.begin(), it);
				vehicle_3.involved_delivery[index] = vehicle_1.id;
				temp_solution.vehicles[vehicle_2.involved_pickup[random_node_2]] = vehicle_3;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP TERCERO"<<endl;
				cout<<"vehiculo: "<<vehicle_3.id << "nodo a encontrar en delivery: "<< id<<endl;
				cout<<"id correcto: "<<vehicle_2.involved_pickup[random_node_2]<<endl;


			}

		}


		//CAMBIO EN ESTRUCTURA DE PICKUP
		int id_temp = vehicle_1.involved_pickup[random_node_1];
		vehicle_1.involved_pickup[random_node_1] = vehicle_2.involved_pickup[random_node_2];
		vehicle_2.involved_pickup[random_node_2] = id_temp;


		// CAMBIO DE NODOS 
		Suplier suplier_temp = vehicle_1.pickup_route[random_node_1];
		vehicle_1.pickup_route[random_node_1] = vehicle_2.pickup_route[random_node_2];
		vehicle_2.pickup_route[random_node_2] = suplier_temp;

		// CAMBIO DE DELIVERY_ITEMS
		tuple<int,int> tuple_temp = vehicle_1.pickup_items[random_node_1];
		vehicle_1.pickup_items[random_node_1] = vehicle_2.pickup_items[random_node_2];
		vehicle_2.pickup_items[random_node_2] = tuple_temp;

	}
	else{
		random_node_1 = rand() % vehicle_1.delivery_route.size();
		random_node_2 = rand() % vehicle_2.delivery_route.size();

		//cout<<"VEHICULO 1: "<<vehicle_1.id << " nodo: "<<random_node_1<<endl;
		//cout<<"VEHICULO 2: "<<vehicle_2.id << " nodo: "<<random_node_2<<endl;

		//print_solution(temp_solution);

		//CAMBIO ESTRUCTURAS INVOLVED
		// SE buscan los vehiculos que va a entregar cada nodo a cambiar
		// caso en el que el primer nodo pertenezca al V1
		if(vehicle_1.id == vehicle_1.involved_delivery[random_node_1]){

			int id = vehicle_1.delivery_route[random_node_1].id * -1;
			auto it = find_if(vehicle_1.pickup_route.begin(), vehicle_1.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_1.pickup_route.end()){

			 	auto index = distance(vehicle_1.pickup_route.begin(), it);
				vehicle_1.involved_pickup[index] = vehicle_2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle_1.id << "nodo a encontrar en pickup: "<< id<<endl;

			}

		}
		// caso en el que el primer nodo pertenezca al V2
		else if(vehicle_2.id == vehicle_1.involved_delivery[random_node_1]){
			int id = vehicle_1.delivery_route[random_node_1].id * -1;
			auto it = find_if(vehicle_2.pickup_route.begin(), vehicle_2.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_2.pickup_route.end()){

			 	auto index = distance(vehicle_2.pickup_route.begin(), it);
				vehicle_2.involved_pickup[index] = vehicle_2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle_2.id << "nodo a encontrar en pickup: "<< id<<endl;

			}
		}

		// caso en el que el primer nodo pertenezca a otro vehiculo
		else{
			// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
			Vehicle vehicle_3 = temp_solution.vehicles[vehicle_1.involved_delivery[random_node_1]];
			int id = vehicle_1.delivery_route[random_node_1].id * -1;
			auto it = find_if(vehicle_3.pickup_route.begin(), vehicle_3.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_3.pickup_route.end()){

			 	auto index = distance(vehicle_3.pickup_route.begin(), it);
				vehicle_3.involved_pickup[index] = vehicle_2.id;
				temp_solution.vehicles[vehicle_1.involved_delivery[random_node_1]] = vehicle_3;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY TERCERO"<<endl;
				cout<<"vehiculo: "<<vehicle_3.id << "nodo a encontrar en pickup: "<< id<<endl;
				cout<<"id correcto: "<<vehicle_1.involved_delivery[random_node_1]<<endl;


			}

		}

		// -------------------------------------------------------------------------------------------------

		// caso en el que el segundo nodo pertenezca al V1
		if(vehicle_1.id == vehicle_2.involved_delivery[random_node_2]){

			int id = vehicle_2.delivery_route[random_node_2].id * -1;
			auto it = find_if(vehicle_1.pickup_route.begin(), vehicle_1.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_1.pickup_route.end()){

			 	auto index = distance(vehicle_1.pickup_route.begin(), it);
				vehicle_1.involved_pickup[index] = vehicle_1.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle_1.id << "nodo a encontrar en pickup: "<< id<<endl;

			}

		}
		// caso en el que el segundo nodo pertenezca al V2
		else if(vehicle_2.id == vehicle_2.involved_delivery[random_node_2]){
			int id = vehicle_2.delivery_route[random_node_2].id * -1;
			auto it = find_if(vehicle_2.pickup_route.begin(), vehicle_2.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_2.pickup_route.end()){

			 	auto index = distance(vehicle_2.pickup_route.begin(), it);
				vehicle_2.involved_pickup[index] = vehicle_1.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle_2.id << " nodo a encontrar en pickup: "<< id<<endl;

			}
		}

		// caso en el que el segundo nodo pertenezca a otro vehiculo
		else{
			// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
			Vehicle vehicle_3 = temp_solution.vehicles[vehicle_2.involved_delivery[random_node_2]];
			int id = vehicle_2.delivery_route[random_node_2].id * -1;
			auto it = find_if(vehicle_3.pickup_route.begin(), vehicle_3.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle_3.pickup_route.end()){

			 	auto index = distance(vehicle_3.pickup_route.begin(), it);
				vehicle_3.involved_pickup[index] = vehicle_1.id;
				temp_solution.vehicles[vehicle_2.involved_delivery[random_node_2]] = vehicle_3;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY TERCERO"<<endl;
				cout<<"vehiculo: "<<vehicle_3.id << "nodo a encontrar en pickup: "<< id<<endl;
				cout<<"id correcto: "<<vehicle_2.involved_delivery[random_node_2]<<endl;


			}

		}


		//CAMBIO EN ESTRUCTURA DE DELIVERY
		int id_temp = vehicle_1.involved_delivery[random_node_1];
		vehicle_1.involved_delivery[random_node_1] = vehicle_2.involved_delivery[random_node_2];
		vehicle_2.involved_delivery[random_node_2] = id_temp;

		// CAMBIO DE NODOS 
		Customer customer_temp = vehicle_1.delivery_route[random_node_1];
		vehicle_1.delivery_route[random_node_1] = vehicle_2.delivery_route[random_node_2];
		vehicle_2.delivery_route[random_node_2] = customer_temp;

		// CAMBIO DE DELIVERY_ITEMS
		tuple<int,int> tuple_temp = vehicle_1.delivery_items[random_node_1];
		vehicle_1.delivery_items[random_node_1] = vehicle_2.delivery_items[random_node_2];
		vehicle_2.delivery_items[random_node_2] = tuple_temp;

	}

	//cout<<"Los nodos fueron: "<<random_node_1<<"y"<<random_node_2<<endl;


	temp_solution.vehicles[pos_vehicle_1] = vehicle_1;
	temp_solution.vehicles[pos_vehicle_2] = vehicle_2;

	if(this->feasible_solution(temp_solution)){
		//cout<<"movimiento SIIIIII FACTIBLE"<<endl;
		temp_solution.vehicles[pos_vehicle_1].set_remaining_capacity();
		temp_solution.vehicles[pos_vehicle_2].set_remaining_capacity();
		temp_solution.vehicles[pos_vehicle_1].set_download_time();
		temp_solution.vehicles[pos_vehicle_2].set_download_time();
		temp_solution.vehicles[pos_vehicle_1].set_reload_time();
		temp_solution.vehicles[pos_vehicle_2].set_reload_time();

		return temp_solution;
		

		/*cout<<"ruta vehiculo 1 despues"<<endl;

		cout<< " [";

		for(int l=0; (unsigned)l < solution.vehicles[pos_vehicle_1].delivery_route.size();l++){
			cout<<" "<<solution.vehicles[pos_vehicle_1].delivery_route[l].id;
		}
		cout<< " ]" <<endl;

		cout<<"ruta vehiculo 2 despues"<<endl;

		cout<< " [";

		for(int l=0; (unsigned)l < solution.vehicles[pos_vehicle_2].delivery_route.size();l++){
			cout<<" "<<solution.vehicles[pos_vehicle_2].delivery_route[l].id;
		}
		cout<< " ]" <<endl;*/


	}
	else{
		//cout<<"movimiento NOOO factible"<<endl;

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

	Solution temp_solution = solution;
	temp_solution.vehicles[random_vehicle] = vehicle;


	if(this->feasible_solution(temp_solution)){
		return temp_solution;
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
tuple<int,int> Grasp::get_worst_route(Solution solution, int type, vector<int> tabu_worst_route){

	float max_pickup_cost = 0;
	float max_delivery_cost = 0;
	int max_pickup_vehicle = -1;
	int max_delivery_vehicle = -1;
	float actual_pickup_cost,actual_delivery_cost;

  	for(int i=0; (unsigned)i<solution.vehicles.size(); i++){
  		if (!(find(tabu_worst_route.begin(), tabu_worst_route.end(), solution.vehicles[i].id) != tabu_worst_route.end())){

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


int Grasp::get_more_capacity(Solution solution, int type, vector<int> tabu_more_capacity){

	float max_pickup_capacity = 0;
	float max_delivery_capacity = 0;
	int max_pickup_vehicle = -1;
	int max_delivery_vehicle = -1;
	float actual_pickup_capacity,actual_delivery_capacity;

  	for(int i=0; (unsigned)i<solution.vehicles.size(); i++){

		if (!(find(tabu_more_capacity.begin(), tabu_more_capacity.end(), solution.vehicles[i].id) != tabu_more_capacity.end())){

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

  	}

  	if(type == 0){
  		return max_pickup_vehicle;
  	}

  	else{
  		return max_delivery_vehicle;
  	}
  	
}


/*vector<int> Grasp::get_involved_vehicles(Vehicle vehicle, Solution solution){

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
}*/


vector<int> Grasp::get_involved_vehicles(Vehicle vehicle){

	vector<int> involved;

	for(int i : vehicle.involved_delivery){

		if(i != vehicle.id && !(find(involved.begin(), involved.end(), i) != involved.end()) ){
			involved.push_back(i);
		}

	}

	return involved;
}


Solution Grasp::consolidation2(Solution solution){

	// SE debe calcular el tiempo de descarga para todos los vehiculos
	vector<int> download_item_position;
	vector <Vehicle>::iterator vehicle_iterator;
	// tiempo en el que termina de descargar todos los productos
	float d_time,cdtime,arrival_time;
	// vector que contiene los tiempos de descarga de todos los vehiculos
	vector<float> download_times;
	Solution temp_solution = solution;

	//cout<<"consolidation antes primer FOR"<<endl;


	for (vehicle_iterator = temp_solution.vehicles.begin(); vehicle_iterator != temp_solution.vehicles.end(); ++vehicle_iterator) {

		cdtime = get<0>(vehicle_iterator->crossdock_times[0]);
		arrival_time = max(cdtime, (float)vehicle_iterator->crossdock_route[0].ready_time);

		d_time = vehicle_iterator->download_time + arrival_time;

	 	download_times.push_back(d_time);

	}

	//---------------------------------------------------------------------

	// PARA CADA VEHICULO SE DEBEN ENCONTRAR LOS VEHICULOS INVOLUCRADOS (LOS QUE SE ENCUENTREN EN DELIVERY Y NO EN PICKUP)
	vector<int> involved_vehicles_pos; 
	float ready_load_time;
	//cout<<"consolidation antes 2 FOR"<<endl;


	for (int i = 0; (unsigned)i < temp_solution.vehicles.size(); i++){

		involved_vehicles_pos = this->get_involved_vehicles(temp_solution.vehicles[i]);

		// si no hay vehiculos involucrados el departure_cd_time = d_time
		if(involved_vehicles_pos.empty()){
			temp_solution.vehicles[i].departure_cd_time = download_times[i];
		}
		else{

			vector<float> download_involved_times;

			for(int j : involved_vehicles_pos){
				download_involved_times.push_back(download_times[j]);
			}
			// se agrega el tiempo de descarga del vehiculo i 
			download_involved_times.push_back(download_times[i]);

			// SE CALCULA EL READY LOAD TIME
			vector<float>::const_iterator it;
			it = max_element(download_involved_times.begin(), download_involved_times.end());
			ready_load_time = *it;

			// SE CALCULA EL TIEMPO DE CARGA DE ITEMS
			
			// finalmente se setea el departure_time
			temp_solution.vehicles[i].departure_cd_time = ready_load_time + temp_solution.vehicles[i].reload_time;

		}
		
	}


	return temp_solution;

}


bool Grasp::feasible_solution(Solution solution){

	//verifica si es factible en cuanto a capacidad
	//cout<<"ANTES DE  capacity"<<endl;

	if(!(solution.feasible_capacity())) return false;
	//cout<<"pase  capacity"<<endl;


	// se aplica set_times() a todos los vehiculos
	solution.set_vehicles_times();
	//cout<<"pase  times"<<endl;


	//se aplica la consolidacion
	solution = this->consolidation2(solution);
	//cout<<"pase  consolidation"<<endl;


	// se aplica nuevamente el set_times() a todos los vehiculos para setear el departure_cd_time
	solution.set_vehicles_times();
	//cout<<"pase  times 2"<<endl;


	// se revisa la factibilidad en cuanto a TW
	if(!(solution.feasible_tw())) return false;
	//cout<<"pase  tw"<<endl;



	return true;
}



tuple<Solution,vector<int>, vector<int>> Grasp::mov_change_node(Solution solution, vector<int> tabu_more_capacity,vector<int> tabu_worst_route){

	int pos_vehicle_1 = rand() % solution.vehicles.size();
	int type_route = rand() % 2;

	if(type_route == 0){
		while(solution.vehicles[pos_vehicle_1].pickup_route.size() == 0){
			pos_vehicle_1 = rand() % solution.vehicles.size();
		}
	}
	else{
		while(solution.vehicles[pos_vehicle_1].delivery_route.size() == 0){
			pos_vehicle_1 = rand() % solution.vehicles.size();
		}
	}

	int pos_vehicle_2 = rand() % solution.vehicles.size();

	while(pos_vehicle_1 == pos_vehicle_2 || solution.vehicles[pos_vehicle_1].crossdock_route[0].id != solution.vehicles[pos_vehicle_2].crossdock_route[0].id){
		//cout<<"mismos vehiculos, o distintos CD, ahora los cambio"<<endl;
		pos_vehicle_2 = rand() % solution.vehicles.size();
	}



/*
	int type_route,pos_vehicle_1,pos_vehicle_2;
	// SE encuentra el vehiculo con la ruta mas cara

	tie(pos_vehicle_1,type_route) = get_worst_route(solution,-1,tabu_worst_route);

	Solution temp_solution = solution;

	temp_solution.vehicles.erase(temp_solution.vehicles.begin() + pos_vehicle_1);
	//SE ENCUENTRA EL  VEHICULO QUE TENGA MAS CAPACIDAD SEGUN LA RUTA ESPECIFICADA

	pos_vehicle_2 = get_more_capacity(temp_solution,type_route,tabu_more_capacity);

	//SE calcula el indice del vehiculo 2, considerando la solucion con todo los vehiculos (sin eliminar 1)
	if(pos_vehicle_2 >= pos_vehicle_1){
		pos_vehicle_2 += 1;
	}
*/
	// se setean los vehiculos correspondietne
	Solution temp_solution = solution;

	Vehicle vehicle1 = temp_solution.vehicles[pos_vehicle_1];
	Vehicle vehicle2 = temp_solution.vehicles[pos_vehicle_2];


	//cout<<"vehiuclo mas caro: "<<pos_vehicle_1<<endl;
	//cout<<"vehiuclo mas capacidad: "<<pos_vehicle_2<<endl;

	//se quita el nodo de la ruta mas cara y se pone en todos los posibles lugares del vehiculo con mas capacidad
	int random_node;
	Solution best_solution = solution;
	float best_cost = FLT_MAX;
	float actual_cost;

	bool add_tabu = true;

	if(type_route == 0){

		/*cout<<"RUTA VEHICULO 1 id: "<<vehicle1.id<<" PICKUP: [ ";
		for(int i=0; (unsigned)i<vehicle1.pickup_route.size();i++){

			cout<<vehicle1.pickup_route[i].id<<" ";

		}
		cout<<"]"<<endl;

		cout<<"INVOLVED VEHICULO 1 id: "<<vehicle1.id<<" PICKUP: [ ";
		for(int i=0; (unsigned)i<vehicle1.involved_pickup.size();i++){

			cout<<vehicle1.involved_pickup[i]<<" ";

		}
		cout<<"]"<<endl;

		cout<<"RUTA VEHICULO 2 id: "<< vehicle2.id <<" PICKUP: [ ";
		for(int i=0; (unsigned)i<vehicle2.involved_pickup.size();i++){

			cout<<vehicle2.pickup_route[i].id<<" ";

		}
		cout<<"]"<<endl;

		cout<<"INVOLVED VEHICULO 2 id: "<<vehicle2.id<<" PICKUP: [ ";
		for(int i=0; (unsigned)i<vehicle2.involved_pickup.size();i++){

			cout<<vehicle2.involved_pickup[i]<<" ";

		}
		cout<<"]"<<endl;

		print_solution(temp_solution);*/


		random_node = rand() % vehicle1.pickup_route.size();
		Suplier suplier_node = vehicle1.pickup_route[random_node];
		//cout<<"nodo a cambiar: "<<suplier_node.id<<endl;

		tuple<int,int> item = vehicle1.pickup_items[random_node];
		// estructura involved
		int id_involved = vehicle1.involved_pickup[random_node];
		//cout<<"TAMANO PICKUP ANTES V1: "<<vehicle1.pickup_route.size()<<endl;
		//cout<<"TAMANO PICKUP ANTES V2: "<<vehicle2.pickup_route.size()<<endl;

		vehicle1.pickup_route.erase(vehicle1.pickup_route.begin() + random_node);

		vehicle1.pickup_items.erase(vehicle1.pickup_items.begin() + random_node);
		// estructura involved
		vehicle1.involved_pickup.erase(vehicle1.involved_pickup.begin() + random_node);
		// agrego el vehiculo con el nodo quitado a la solucion
		// se comienza a probar todos los posibles lugares de insercion del nodo

		if(vehicle1.id == id_involved){

			int id = suplier_node.id * -1;
			auto it = find_if(vehicle1.delivery_route.begin(), vehicle1.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

			if (it != vehicle1.delivery_route.end()){

			 	auto index = distance(vehicle1.delivery_route.begin(), it);
				vehicle1.involved_delivery[index] = vehicle2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle1.id << "nodo a encontrar en pickup: "<< id<<endl;
				cout<<"id correcto: "<<id_involved<<endl;


			}

		}


		int iterations = vehicle2.pickup_route.size();

		for(int i=0; i <= iterations; i++){

			temp_solution = solution;
			vehicle2 = temp_solution.vehicles[pos_vehicle_2];

			vehicle2.pickup_route.insert(vehicle2.pickup_route.begin()+i, suplier_node);
			vehicle2.pickup_items.insert(vehicle2.pickup_items.begin()+i, item);
			vehicle2.involved_pickup.insert(vehicle2.involved_pickup.begin()+i, id_involved);


			if(vehicle2.id == id_involved){

				int id = suplier_node.id * -1;
				auto it = find_if(vehicle2.delivery_route.begin(), vehicle2.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

				if (it != vehicle2.delivery_route.end()){

				 	auto index = distance(vehicle2.delivery_route.begin(), it);
					vehicle2.involved_delivery[index] = vehicle2.id;

				}
				else{
					cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
					cout<<"vehiculo: "<<vehicle2.id << "nodo a encontrar en pickup: "<< id<<endl;
					cout<<"id correcto: "<<id_involved<<endl;

				}
			}

		//cout<<"TAMANO pickup justo despues V1: "<<vehicle1.pickup_route.size()<<endl;

			temp_solution.vehicles[pos_vehicle_2] = vehicle2;
			temp_solution.vehicles[pos_vehicle_1] = vehicle1;
			


			if(this->feasible_solution(temp_solution)){
				//cout<<"ENCONTRE FACTIBILIDAD"<<endl;

				add_tabu = false;

				actual_cost = this->evaluation_function(temp_solution);

				if(actual_cost <= best_cost){
					//cout<<"ENCONTRE BEST COST"<<endl;

					best_cost = actual_cost;
					best_solution = temp_solution;
				}
			}

		}

		//cout<<"TAMANO PICKUP DESPUES V1: "<<best_solution.vehicles[pos_vehicle_1].pickup_route.size()<<endl;
		//cout<<"TAMANO PICKUP DESPUES V2: "<<best_solution.vehicles[pos_vehicle_2].pickup_route.size()<<end;

		//Vehicle vehicle3 = temp_solution.vehicles[id_involved];
		//CAMBIO ESTRUCTURAS INVOLVED
		// SE buscan los vehiculos que va a entregar cada nodo a cambiar
		// caso en el que el primer nodo pertenezca al V1

		if(!add_tabu){
			if(vehicle2.id != id_involved && vehicle1.id != id_involved){
				Vehicle vehicle = best_solution.vehicles[id_involved];
			
				// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
				int id = suplier_node.id * -1;
				auto it = find_if(vehicle.delivery_route.begin(), vehicle.delivery_route.end(), [&id](const Customer& customer) {return customer.id == id;});

				if (it != vehicle.delivery_route.end()){
				 	auto index = distance(vehicle.delivery_route.begin(), it);
					vehicle.involved_delivery[index] = vehicle2.id;

				}
				else{
					cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP CHANGE NODE TERCERO"<<endl;
					cout<<"vehiculo: "<<vehicle.id << "nodo a encontrar en delivery: "<< id<<endl;
					cout<<"id correcto: "<<id_involved<<endl;


				}

				best_solution.vehicles[id_involved] = vehicle;

			}
		}
	
		


	}
	else{

/*
		cout<<"RUTA VEHICULO 1 id: "<<vehicle1.id<<" DELIVERY: [ ";
		for(int i=0; (unsigned)i<vehicle1.delivery_route.size();i++){

			cout<<vehicle1.delivery_route[i].id<<" ";

		}
		cout<<"]"<<endl;

		cout<<"INVOLVED VEHICULO 1 id: "<<vehicle1.id<<" DELIVERY: [ ";
		for(int i=0; (unsigned)i<vehicle1.involved_delivery.size();i++){

			cout<<vehicle1.involved_delivery[i]<<" ";

		}
		cout<<"]"<<endl;


		cout<<"RUTA VEHICULO 2 id: "<<vehicle2.id <<" DELIVERY: [ ";
		for(int i=0; (unsigned)i<vehicle2.delivery_route.size();i++){

			cout<<vehicle2.delivery_route[i].id<<" ";

		}
		cout<<"]"<<endl;

		cout<<"INVOLVED VEHICULO 2 id: "<<vehicle2.id<<" DELIVERY: [ ";
		for(int i=0; (unsigned)i<vehicle2.involved_delivery.size();i++){

			cout<<vehicle2.involved_delivery[i]<<" ";

		}
		cout<<"]"<<endl;


		print_solution(temp_solution);*/
		



		random_node = rand() % vehicle1.delivery_route.size();
		
		Customer customer_node = vehicle1.delivery_route[random_node];
		//cout<<"nodo a cambiar: "<<customer_node.id<<endl;

		tuple<int,int> item = vehicle1.delivery_items[random_node];
		// estructura involved
		int id_involved = vehicle1.involved_delivery[random_node];

		//cout<<"TAMANO DELIVERY ANTES V1: "<<vehicle1.delivery_route.size()<<endl;
		//cout<<"TAMANO DELIVERY ANTES V2: "<<vehicle2.delivery_route.size()<<endl;


		vehicle1.delivery_route.erase(vehicle1.delivery_route.begin() + random_node);
		vehicle1.delivery_items.erase(vehicle1.delivery_items.begin() + random_node);

		// estructura involved
		vehicle1.involved_delivery.erase(vehicle1.involved_delivery.begin() + random_node);
		// agrego el vehiculo con el nodo quitado a la solucion
		// se comienza a probar todos los posibles lugares de insercion del nodo

		//Vehicle vehicle3 = solution.vehicles[id_involved];
		//CAMBIO ESTRUCTURAS INVOLVED
		// SE buscan los vehiculos que va a entregar cada nodo a cambiar
		// caso en el que el primer nodo pertenezca al V1

		if(vehicle1.id == id_involved){

			int id = customer_node.id * -1;
			auto it = find_if(vehicle1.pickup_route.begin(), vehicle1.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

			if (it != vehicle1.pickup_route.end()){

			 	auto index = distance(vehicle1.pickup_route.begin(), it);
				vehicle1.involved_pickup[index] = vehicle2.id;

			}
			else{
				cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
				cout<<"vehiculo: "<<vehicle1.id << "nodo a encontrar en pickup: "<< id<<endl;
				cout<<"id correcto: "<<id_involved<<endl;


			}

		}


		// agrego el vehiculo con el nodo quitado a la solucion
		// se comienza a probar todos los posibles lugares de insercion del nodo

		int iterations = vehicle2.delivery_route.size();



		for(int i=0; i <= iterations; i++){
			
			temp_solution = solution;
			vehicle2 = solution.vehicles[pos_vehicle_2];


			vehicle2.delivery_route.insert(vehicle2.delivery_route.begin()+i, customer_node);
			vehicle2.delivery_items.insert(vehicle2.delivery_items.begin()+i, item);
			vehicle2.involved_delivery.insert(vehicle2.involved_delivery.begin()+i, id_involved);



			if(vehicle2.id == id_involved){

				int id = customer_node.id * -1;
				auto it = find_if(vehicle2.pickup_route.begin(), vehicle2.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

				if (it != vehicle2.pickup_route.end()){

				 	auto index = distance(vehicle2.pickup_route.begin(), it);
					vehicle2.involved_pickup[index] = vehicle2.id;

				}
				else{
					cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN DELIVERY"<<endl;
					cout<<"vehiculo: "<<vehicle2.id << "nodo a encontrar en pickup: "<< id<<endl;
					cout<<"id correcto: "<<id_involved<<endl;

				}
			}


		//cout<<"TAMANO DELIVERY justo despues V1: "<<vehicle1.delivery_route.size()<<endl;

			temp_solution.vehicles[pos_vehicle_2] = vehicle2;
			temp_solution.vehicles[pos_vehicle_1] = vehicle1;

		//cout<<"TAMANO DELIVERY justo despues22 V1: "<<temp_solution.vehicles[pos_vehicle_1].delivery_route.size()<<endl;


			if(this->feasible_solution(temp_solution)){
			//cout<<"-------------ENCONTRE FACTIBILIDAD--------------"<<endl;

				add_tabu = false;

				actual_cost = this->evaluation_function(temp_solution);

				if(actual_cost <= best_cost){
					//cout<<"ENCONTRE BEST COST"<<endl;

					best_cost = actual_cost;
					best_solution = temp_solution;

				}
			}
		}

		//cout<<"TAMANO DELIVERY DESPUES V1: "<<best_solution.vehicles[pos_vehicle_1].delivery_route.size()<<endl;
		//cout<<"TAMANO DELIVERY DESPUES V2: "<<best_solution.vehicles[pos_vehicle_2].delivery_route.size()<<endl;

		if(!add_tabu){
			if(vehicle2.id != id_involved && vehicle1.id != id_involved){
				Vehicle vehicle = best_solution.vehicles[id_involved];
			
				// LA POSICION DEL VEHICULO DEBERIA SER IGUAL A LA ID
				int id = customer_node.id * -1;
				auto it = find_if(vehicle.pickup_route.begin(), vehicle.pickup_route.end(), [&id](const Suplier& suplier) {return suplier.id == id;});

				if (it != vehicle.pickup_route.end()){
				 	auto index = distance(vehicle.pickup_route.begin(), it);
					vehicle.involved_pickup[index] = vehicle2.id;

				}
				else{
					cout<<"ERROR, NO ENCONTRE EL ID PARA EL CAMBIO DE ESTRUCTURA EN PICKUP CHANGE NODE TERCERO"<<endl;
					cout<<"vehiculo: "<<vehicle.id << "nodo a encontrar en delivery: "<< id<<endl;
					cout<<"id correcto: "<<id_involved<<endl;


				}

				best_solution.vehicles[id_involved] = vehicle;

			}
		}
		

	}

	// ACTUALIZACION LISTA TABU 
	/*
	if(add_tabu){
		if(tabu_more_capacity.size() == (unsigned)this->tabu_capacity_size){
			tabu_more_capacity.erase(tabu_more_capacity.begin() + 0);
			tabu_more_capacity.push_back(solution.vehicles[pos_vehicle_2].id);
			this->criteria ++;
		}
		else{
			tabu_more_capacity.push_back(solution.vehicles[pos_vehicle_2].id);
		}

		// CRITERIO PARA TRABAJAR CON EL TABU DE WORST ROUTE
		// actualmente si se actualiza la lista tabu la misma cantidad de veces que su tamano se utiliza la otra lista
		if(this->criteria == this->tabu_capacity_size){
			if(tabu_worst_route.size() == (unsigned)this->tabu_worst_route_size){
				tabu_worst_route.erase(tabu_worst_route.begin() + 0);
				tabu_worst_route.push_back(solution.vehicles[pos_vehicle_1].id);
			}
			else{
				tabu_worst_route.push_back(solution.vehicles[pos_vehicle_1].id);
			}
			this->criteria = 0;
			//tabu_more_capacity.clear();
		}

	}*/

	/*if(best_solution.vehicles[pos_vehicle_1].pickup_route.empty() && best_solution.vehicles[pos_vehicle_1].delivery_route.empty()){
		best_solution.vehicles.erase(best_solution.vehicles.begin() + pos_vehicle_1);
	}*/

	best_solution.vehicles[pos_vehicle_2].set_remaining_capacity();
	best_solution.vehicles[pos_vehicle_1].set_remaining_capacity();
	best_solution.vehicles[pos_vehicle_1].set_download_time();
	best_solution.vehicles[pos_vehicle_2].set_download_time();
	best_solution.vehicles[pos_vehicle_1].set_reload_time();
	best_solution.vehicles[pos_vehicle_2].set_reload_time();

    return make_tuple(best_solution,tabu_more_capacity,tabu_worst_route);

}


tuple<vector<float>,vector<int>> get_rewards(vector<tuple<int,float>> sliding_window){

	vector<float> rewards = {0,0,0,0};
	vector<int> times_used = {0,0,0,0};
	float fir;
	int operator_id;

	for(tuple<int,int> element : sliding_window){
		operator_id = get<0>(element);
		fir = get<1>(element);
		times_used[operator_id] += 1;
		rewards[operator_id] += fir;
	}

	return make_tuple(rewards,times_used);

}


vector<int> get_ranking(vector<tuple<int,float>> rewards){
	vector<int> ranking = {0,0,0,0};
	for(int i=0; (unsigned)i<rewards.size();i++){
		int operator_id = get<0>(rewards[i]);
		ranking[operator_id] = i+1;
	}
	return ranking;
}



Solution Grasp::run(int iterations_phase1, int iterations_phase2,int iterations_phase3, int porc_two_opt, int porc_swap_cd, int porc_swap_node_pick, int porc_swap_node_del, int porc_change_node,int time_limit ,clock_t global_start_time){

	cout<<"iteraciones phase 1: "<< iterations_phase1<<endl;
	cout<<"iteraciones phase 2: "<< iterations_phase2<<endl;
	cout<<"iteraciones phase 3: "<< iterations_phase3<<endl;
	cout<<"porcentaje 2opt: "<< porc_two_opt<<endl;
	cout<<"porcentaje swap_cd: "<< porc_swap_cd<<endl;
	cout<<"porcentaje swap pickup: "<< porc_swap_node_pick<<endl;
	cout<<"porcentaje swap delivery: "<< porc_swap_node_del<<endl;
	cout<<"porcentaje change node: "<< porc_change_node<<endl;
	cout<<"size_window: "<< this->size_window<<endl;
	cout<<"decay factor: "<< this->decay_factor<<endl;
	cout<<"explore factor: "<< this->explore_factor<<endl;

	ofstream myfile;
	myfile.open ("result.txt");

	//------------------------------------------------------------------------------
	this->criteria = 0;
	Solution new_solution = this->distance_initial_solution();
	//Solution new_solution = this->demand_initial_solution();
	//Solution new_solution = this->hybrid_initial_solution();
	//Solution new_solution = this->timewindow_initial_solution();

	Solution best_solution = new_solution;
	Solution parent_solution = new_solution;
	float best_time = this->evaluation_function(best_solution);
	float new_time,parent_time;
	vector<int> tabu_more_capacity;
	vector<int> tabu_worst_route;

	clock_t start_time, end_time,global_end_time;
	double total_time,global_total_time;

	// vector que almacena el fir y el operador a traves del tiempo de la forma <(op_id,fir)>
	vector<tuple<int,float>> sliding_window;
	// vector que almacena el operador con su recompenza(suma de fir en ventana) <(op_id,suma_fir)>
	vector<tuple<int,float>> rewards;
	// vector que almacena la cantidad de veces que se han seleccionado cada movimiento
	vector<int> times_used = {0,0,0,0};
	// vector que almacena el ranking de cada operador
	vector<int> ranking = {0,0,0,0};
	// vector que almacena el decay de cada operador
	vector<float> decay = {0.0,0.0,0.0,0.0};
	// vector que almacena el FRR 
	vector<float> frr = {0.0,0.0,0.0,0.0};
	// vector que almacena el score final de la iteracion para cada movimiento <(operator,score)>
	vector<tuple<int,float>> final_score;
	// indicador de score del movimiento en cierta iteracion (fitness improvement)
	float fir;

	int size_window = this->size_window;
	float decay_factor = this->decay_factor;
	float explore_factor = this->explore_factor;
	int selected_operator = -1;


	start_time = clock();

	// Se comienzan las iteraciones haciendo el 2-opt, solo se acepta el cambio en la solucion si esta mejora
	for(int i = 1; i <= iterations_phase1; i++){

		global_end_time = clock();
		global_total_time = (double)(global_end_time - global_start_time)/CLOCKS_PER_SEC;
		if(global_total_time >= time_limit){
			break;
		}

		//  ---------- SELECCION DE OPERADOR ......
		if(i<=4){
			selected_operator = rand() % 4;
			while(times_used[selected_operator] != 0){
				selected_operator = rand() % 4;
			}
		}

		else{
			sort(begin(final_score), end(final_score), [](tuple<int,float> const &t1, tuple<int,float> const &t2) {
		    	return get<1>(t1) > get<1>(t2); 
		    	}
		    );
			selected_operator = get<0>(final_score[0]);
		}


		if(selected_operator == 0){
			tie(new_solution,tabu_more_capacity,tabu_worst_route) = this->mov_change_node(parent_solution,tabu_more_capacity,tabu_worst_route);
			//cout<<"termine change node"<<endl;
			parent_time = this->evaluation_function(parent_solution);
			new_time = this->evaluation_function(new_solution);

			fir = (parent_time - new_time)/parent_time;
				
			if(new_time < best_time){
				cout<<"Mejor= "<<best_time<<endl;
				cout<<"Actual= "<<new_time<<endl;

				best_solution = new_solution;
				best_time = new_time;
				cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;

				end_time = clock();
				total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
				myfile << total_time <<"-"<< best_time << "\n";
				parent_solution = new_solution;

			}
			
			parent_solution = new_solution;
			

		
		}


		else if(selected_operator == 1){
			new_solution = this->mov_two_opt(parent_solution);
			//cout<<"termine 2opt"<<endl;
			parent_time = this->evaluation_function(parent_solution);
			new_time = this->evaluation_function(new_solution);

			fir = (parent_time - new_time)/parent_time;
				
			if(new_time < best_time){
				cout<<"Mejor= "<<best_time<<endl;
				cout<<"Actual= "<<new_time<<endl;

				best_solution = new_solution;
				best_time = new_time;
				cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;

				end_time = clock();
				total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
				myfile << total_time <<"-"<< best_time << "\n";
				parent_solution = new_solution;

			}
			
			parent_solution = new_solution;



		}

		else if(selected_operator == 2){
			new_solution = this->mov_swap_node(parent_solution,0);
			//cout<<"termine swap pick"<<endl;

			parent_time = this->evaluation_function(parent_solution);
			new_time = this->evaluation_function(new_solution);

			fir = (parent_time - new_time)/parent_time;
				
			if(new_time < best_time){
				cout<<"Mejor= "<<best_time<<endl;
				cout<<"Actual= "<<new_time<<endl;

				best_solution = new_solution;
				best_time = new_time;
				cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;

				end_time = clock();
				total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
				myfile << total_time <<"-"<< best_time << "\n";
				parent_solution = new_solution;

			}
		
			parent_solution = new_solution;
			


		}

		else if(selected_operator == 3){
			new_solution = this->mov_swap_node(parent_solution,1);
			//cout<<"termine swap delivery"<<endl;

			parent_time = this->evaluation_function(parent_solution);
			new_time = this->evaluation_function(new_solution);

			fir = (parent_time - new_time)/parent_time;
				
			if(new_time < best_time){
				cout<<"Mejor= "<<best_time<<endl;
				cout<<"Actual= "<<new_time<<endl;

				best_solution = new_solution;
				best_time = new_time;
				cout<<"-------------- MEJORE LA SOLUCION ------------------"<<endl;

				end_time = clock();
				total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
				myfile << total_time <<"-"<< best_time << "\n";
				parent_solution = new_solution;

			}
			
			parent_solution = new_solution;


		}

		else{
			cout<<"------ERROR: EL OPERADOR SELECCIONADO NO EXISTE "<<endl;
			break;
		}


		// SE AGREGA EL OPERADOR SELECCIONADO Y SU FIR CORRESPONDIENTE A LA VENTANA
		if(sliding_window.size() == (unsigned)size_window){
			sliding_window.erase(sliding_window.begin() + 0);
			sliding_window.push_back(make_tuple(selected_operator,fir));
		
		}
		else{
			sliding_window.push_back(make_tuple(selected_operator,fir));
		}

		// SE CALCULA LA RECOMPENZA (SUMA DE FIR EN VENTANA)
		vector<float> rewards_temp;
		tie(rewards_temp,times_used) = get_rewards(sliding_window);
		for(int i=0; (unsigned)i<rewards_temp.size();i++){
			rewards.push_back(make_tuple(i,rewards_temp[i]));
		}
		
		// SE REALIZA EL RANKING DE OPERADORES
		sort(begin(rewards), end(rewards), [](tuple<int,float> const &t1, tuple<int,float> const &t2) {
	    	return get<1>(t1) > get<1>(t2); 
	    	}
	    );

		ranking = get_ranking(rewards);

		float decay_sum = 0;
		// SE CALCULA EL DECAY Y la suma de los decay
		for(int i=0; (unsigned)i<decay.size();i++){
			float dec_value = pow(decay_factor,ranking[i]) * rewards_temp[i];
			decay[i] = dec_value;
			decay_sum += dec_value;
		}

		// SE calcula el FRR (fitness rate rank)
		for(int i=0; (unsigned)i<frr.size();i++){
			float frr_value = decay[i]/decay_sum;
			frr[i] = frr_value;
		}

		// SE calcula el score final para cada operador
		int times_used_sum = 0;
		for(int i=0; (unsigned) i< times_used.size();i++){
			times_used_sum += times_used[i];
		}
		float explore_numerator = 2*log(times_used_sum);
		for(int i=0; (unsigned)i<frr.size();i++){
			final_score.push_back(make_tuple(i,frr[i] + explore_factor * sqrt(explore_numerator/times_used[i])));
		}

	}


	myfile.close();

	return best_solution;

}