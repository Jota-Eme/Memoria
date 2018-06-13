#include "solution.h"

Solution::Solution(){
}


bool Solution::feasible_capacity(){

	vector <Vehicle>::iterator vehicle_iterator;
	int demand,remaining_pickup_capacity,remaining_delivery_capacity;


	for (vehicle_iterator = this->vehicles.begin(); vehicle_iterator != this->vehicles.end(); ++vehicle_iterator) {

		remaining_pickup_capacity = vehicle_iterator->total_capacity;
		remaining_delivery_capacity = vehicle_iterator->total_capacity;

		for(int i=0; (unsigned)i<vehicle_iterator->pickup_items.size();i++){

			demand = get<0>(vehicle_iterator->pickup_items[i]);
			remaining_pickup_capacity -= demand;
		}

		for(int i=0; (unsigned)i<vehicle_iterator->delivery_items.size();i++){

			demand = get<0>(vehicle_iterator->delivery_items[i]);
			remaining_delivery_capacity -= demand;
		}

		if(remaining_delivery_capacity < 0 || remaining_pickup_capacity <0 ){
			return false;
		}

	}

	return true;
}

bool Solution::feasible_tw(){
	
	for (int i=0; (unsigned)i < this->vehicles.size(); i++) {
		if(!this->vehicles[i].feasible_route()) return false;
	}
	return true
}

void Solution::set_vehicles_times(){
	// se aplica set_times() a todos los vehiculos
	for (int i=0; (unsigned)i < this->vehicles.size(); i++) {
		this->vehicles[i].set_times();
	}
}


