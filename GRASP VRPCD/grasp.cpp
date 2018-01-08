#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance){
	this->instance = instance;
}

Grasp::Grasp(){
}



tuple<Request,float,float,bool> Grasp::get_cheaper_request(vector<Request> requests, Vehicle vehicle){

	float suplier_cost, customer_cost, total_cost;
	float min_cost = FLT_MAX;
	float min_suplier_cost, min_customer_cost;
	Request cheaper_request;
	bool flag = false;
	Suplier suplier;
	Customer customer;
	Vehicle vehicle_temp;

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
					cheaper_request = requests[i];
					min_cost = total_cost;
					min_suplier_cost = suplier_cost;
					min_customer_cost = customer_cost;
					// informa si se encontro un request que cumpla con las restricciones
					flag = true;
				}

			}
		}		
	}

	return make_tuple(cheaper_request,min_suplier_cost,min_customer_cost,flag);

}


// FUncion que genera una solucion inicial mediante greedy

Solution Grasp::initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	Request selected_request;
	int min_suplier_cost,min_customer_cost;
	bool found = true;
	vector <Request>::iterator request_iterator;
	//Vector de costos, actualmente solo se usa para debugear, pero podria servir para algo mas adelante
	vector<int> costs;


	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet);
		// PARA IAA SE AGREGA SIEMPRE EL MISMO CD PARA TODOS LOS VEHICULOS
		vehicle.crossdock_route.push_back(this->instance.crossdock);
		found = true;

		while (found){

			tie(selected_request,min_suplier_cost,min_customer_cost,found) = this->get_cheaper_request(requests, vehicle);
			
			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){

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