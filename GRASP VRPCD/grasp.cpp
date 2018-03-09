#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance, int list_size){
	this->instance = instance;
	this->list_size = list_size;
}

Grasp::Grasp(){
}


// funcion que retorna una lista de loss request mas bajos en costo, que cumplan con las condiciones de TW (request factibles)
tuple<vector<tuple<Request,float>>,bool> Grasp::get_cheaper_requests(vector<Request> requests, Vehicle vehicle){

	float suplier_cost, customer_cost, total_cost;
	float min_cost = FLT_MAX;
	bool flag = false;
	Suplier suplier;
	Customer customer;
	Vehicle vehicle_temp;
	vector<tuple<Request,float>> selected_requests;

	for(int i = 0; (unsigned)i < requests.size(); i++){
		// PARA MEMORIA se deben considerar los otros CD tambien 
		if(requests[i].demand <= vehicle.remaining_capacity){

			if(vehicle.pickup_route.empty()){
				suplier_cost = this->instance.crossdock.get_distance(requests[i].suplier);
				customer_cost = this->instance.crossdock.get_distance(requests[i].customer);
			}
			else{
				suplier = vehicle.pickup_route.back();
				customer = vehicle.delivery_route.back();
				suplier_cost = suplier.get_distance(requests[i].suplier);
				customer_cost = customer.get_distance(requests[i].customer);
			}

			total_cost = suplier_cost + customer_cost;

			if(total_cost < min_cost){
				// se analiza si es factible agregar dicho par de nodos en relacion a los TW  (para MEMORIA deberia considerar los distintos CD tambien)
				vehicle_temp = vehicle;
				vehicle_temp.pickup_route.push_back(requests[i].suplier);
				vehicle_temp.delivery_route.push_back(requests[i].customer);
				vehicle_temp.set_times();

				if(vehicle_temp.feasible_route()){

					if(selected_requests.size() < (unsigned)this->list_size){
						selected_requests.push_back(make_tuple(requests[i],total_cost));
					}

					else if(selected_requests.size() == (unsigned)this->list_size){
						selected_requests.push_back(make_tuple(requests[i],total_cost));
						std::sort(begin(selected_requests), end(selected_requests), [](tuple<Request, float> const &t1, tuple<Request, float> const &t2) {
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
	vector<tuple<Request,float>> requests_list;
	Request selected_request;
	int random;

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet);
		// PARA IAA SE AGREGA SIEMPRE EL MISMO CD PARA TODOS LOS VEHICULOS
		vehicle.crossdock_route.push_back(this->instance.crossdock);
		found = true;

		while (found){

			tie(requests_list,found) = this->get_cheaper_requests(requests, vehicle);

			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){
				//Se agrega la componente aleatoria

				random = rand() % requests_list.size();
				selected_request = get<0>(requests_list[random]);

				vehicle.pickup_route.push_back(selected_request.suplier);
				vehicle.delivery_route.push_back(selected_request.customer);
				vehicle.remaining_capacity -= selected_request.demand;

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
			current_node = vehicle_iterator->crossdock_route[0];

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
			total_cost += current_node.get_distance(vehicle_iterator->crossdock_route[0]);

		}

	}

	return total_cost;

}