#include "grasp.h"


// CONSTRUCTOR
Grasp::Grasp (Instance instance){
	this->instance = instance;
}

Grasp::Grasp(){
}

//Funcion que devuelve el request mas barato desde un CD
tuple<Request,float> Grasp::get_cheaper_request(vector<Request> requests){

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

	return make_tuple(cheaper_request,min_cost);

}

//Funcion que devuelve el request mas barato desde un Suplier y Customer dado
tuple<Request,float> Grasp::get_cheaper_request(Suplier suplier, Customer customer, vector<Request> requests){

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

	return make_tuple(cheaper_request,min_cost);

}


// FUncion que genera una solucion inicial mediante greedy

Solution Grasp::initial_solution(){

	Solution solution();
	vector<Request> request = this->instance.requests;

}