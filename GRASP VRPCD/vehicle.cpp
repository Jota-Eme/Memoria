
#include "vehicle.h"

// CONSTRUCTOR
Vehicle::Vehicle(int capacity, int fixed_time, int unit_time){
	this->capacity = capacity;
	this->fixed_time = fixed_time;
	this->unit_time = unit_time;

}

Vehicle::Vehicle(){
}


Vehicle::Vehicle(int capacity){
	this->capacity = capacity;
}
