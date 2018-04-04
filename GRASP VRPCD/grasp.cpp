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
		// PARA MEMORIA se deben considerar los otros CD tambien 
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