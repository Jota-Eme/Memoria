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

	for(int i = 0; (unsigned)i < requests.size(); i++){
		suplier_cost = this->instance.crossdock.get_distance(requests[i].suplier);
		customer_cost = this->instance.crossdock.get_distance(requests[i].customer);
		total_cost = suplier_cost + customer_cost;

		if(total_cost < min_cost){
			cheaper_request = requests[i];
			min_cost = total_cost;
		}
	}

	return make_tuple(cheaper_request,min_cost,true);

}

//Funcion que devuelve el request mas barato desde un Suplier y Customer dado
tuple<Request,float, bool> Grasp::get_cheaper_request(Suplier suplier, Customer customer, vector<Request> requests, int capacity){

	float suplier_cost, customer_cost, total_cost;
	float min_cost = FLT_MAX;
	Request cheaper_request;

	for(int i = 0; (unsigned)i < requests.size(); i++){
		suplier_cost = suplier.get_distance(requests[i].suplier);
		customer_cost = customer.get_distance(requests[i].customer);
		total_cost = suplier_cost + customer_cost;

		if(total_cost < min_cost){
			cheaper_request = requests[i];
			min_cost = total_cost;
		}
	}

	return make_tuple(cheaper_request,min_cost,true);

}


// FUncion que genera una solucion inicial mediante greedy

Solution Grasp::initial_solution(){

	Solution solution = Solution();
	vector<Request> requests = this->instance.requests;
	Request selected_request;
	int selected_cost;
	bool flag;
	//Vector de costos, actualmente solo se usa para debugear, pero podria servir para algo mas adelante
	vector<int> costs;

	// SE realiza la primera asignacion del primer request desde el CD
	Vehicle vehicle = Vehicle(this->instance.vehicle_capacity, this->instance.fixed_time_preparation, this->instance.unit_time_pallet);
	tie(selected_request,selected_cost,flag) = this->get_cheaper_request(requests, vehicle.capacity);



	vector<Vehicle> vec;
	Vehicle v1 = Vehicle(1);
	Vehicle v2 = Vehicle(2);
	Vehicle v3 = Vehicle(3);
	Vehicle v4 = Vehicle(4);

	vec.push_back(v1);
	vec.push_back(v2);
	vec.push_back(v3);
	vec.push_back(v4);

	cout<<"vector antes: [";
	for(int i=0; (unsigned)i<vec.size();i++){
		cout<<vec[i].capacity<<" ";
	}
	cout<<"]"<<endl;

	vector <Vehicle>::iterator it3;
	for (it3 = vec.begin(); it3 != vec.end(); ++it3) {
	    if (it3->capacity == 3) {
	        it3 = vec.erase(it3); // luego de borrar el iterador pasa a la siguiente posicion.
	        --it3; // devuelve el iterador si quedo fuera de rango.
	    }
	}

	cout<<"vector despues: [";
	for(int i=0; (unsigned)i<vec.size();i++){
		cout<<vec[i].capacity<<" ";
	}
	cout<<"]"<<endl;

	return solution;
}