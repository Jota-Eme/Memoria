#include "crossdock.h"


// CONSTRUCTOR
Crossdock::Crossdock (int x_coord, int y_coord, int ready_time, int due_date ){
	this->x_coord = x_coord;
	this->y_coord = y_coord;
	this->ready_time = ready_time;
	this->due_date = due_date;
}

Crossdock::Crossdock (){
}

Crossdock::~Crossdock(){
}