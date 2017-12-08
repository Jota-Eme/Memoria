#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance){
	this->instance = instance;
}

Grasp::Grasp(){
}

//Funcion que devuelve el request mas barato desde un CD
tuple<Request,float,bool> Grasp::get_cheaper_request(vector<Request> requests,int capacity){

	float suplier_cost, customer_cost, total_cost;
	float min_cost = FLT_MAX;
	Request cheaper_request;
	bool flag = false;

	for(int i = 0; (unsigned)i < requests.size(); i++){

		if(requests[i].demand <= capacity){
			suplier_cost = this->instance.crossdock.get_distance(requests[i].suplier);
			customer_cost = this->instance.crossdock.get_distance(requests[i].customer);
			total_cost = suplier_cost + customer_cost;

			if(total_cost < min_cost){
				cheaper_request = requests[i];
				min_cost = total_cost;
			}
			// informa si se encontro un request que cumpla con las restricciones
			flag = true;
		}		
	}

	return make_tuple(cheaper_request,min_cost,flag);

}

//Funcion que devuelve el request mas barato desde un Suplier y Customer dado
tuple<Request,float, bool> Grasp::get_cheaper_request(Suplier suplier, Customer customer, vector<Request> requests, int capacity){

	float suplier_cost, customer_cost, total_cost;
	float min_cost = FLT_MAX;
	Request cheaper_request;
	bool flag = false;

	for(int i = 0; (unsigned)i < requests.size(); i++){

		if(requests[i].demand <= capacity){
			suplier_cost = suplier.get_distance(requests[i].suplier);
			customer_cost = customer.get_distance(requests[i].customer);
			total_cost = suplier_cost + customer_cost;

			if(total_cost < min_cost){
				cheaper_request = requests[i];
				min_cost = total_cost;
			}
			// informa si se encontro un request que cumpla con las restricciones
			flag = true;
		}

	}

	return make_tuple(cheaper_request,min_cost,flag);

}


// FUncion que genera una solucion inicial mediante greedy

Solution Grasp::initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	Request selected_request;
	int selected_cost;
	bool found = true;
	vector <Request>::iterator request_iterator;
	//Vector de costos, actualmente solo se usa para debugear, pero podria servir para algo mas adelante
	vector<int> costs;
	bool initial_request = true;

	Customer actual_customer = Customer();
	Suplier actual_suplier = Suplier();

	while(requests.size() != 0){
		
		Vehicle vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet);

		initial_request = true;
		found = true;

		while (found){

			if(initial_request){
				// SE realiza la primera busqueda entre los request desde el CD
				tie(selected_request,selected_cost,found) = this->get_cheaper_request(requests, vehicle.capacity);
				initial_request = false;
			}

			else{
				tie(selected_request,selected_cost,found) = this->get_cheaper_request(actual_suplier,actual_customer,requests, vehicle.capacity);
			}
			
			//si encontro un request que cumpliera con las restricciones se le asigna el request al vehiculo
			if(found){

				vehicle.pickup_route.push_back(selected_request.suplier);
				vehicle.delivery_route.push_back(selected_request.customer);
				actual_suplier = selected_request.suplier;
				actual_customer = selected_request.customer;

				vehicle.capacity -= selected_request.demand;

				// se elimina el request asignado de la lista de requests
				for (request_iterator = requests.begin(); request_iterator != requests.end(); ++request_iterator) {
				    if (request_iterator->id == selected_request.id) {
				        request_iterator = requests.erase(request_iterator); // luego de borrar el iterador pasa a la siguiente posicion.
				        --request_iterator; // devuelve el iterador si quedo fuera de rango.
				    }
				}
			}
		
		}

		solution.vehicles.push_back(vehicle);

	}

	

	


	/*cout<<"vector antes: [";
	for(int i=0; (unsigned)i<vec.size();i++){
		cout<<vec[i].capacity<<" ";
	}
	cout<<"]"<<endl;



	cout<<"vector despues: [";
	for(int i=0; (unsigned)i<vec.size();i++){
		cout<<vec[i].capacity<<" ";
	}
	cout<<"]"<<endl;*/

	return solution;
}