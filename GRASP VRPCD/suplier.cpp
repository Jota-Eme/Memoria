#include "suplier.h"


// CONSTRUCTOR
Suplier::Suplier (int id, int x_coord, int y_coord, int ready_time, int due_date ){
	this->type = 1;
	this->id = id;
	this->x_coord = x_coord;
	this->y_coord = y_coord;
	this->ready_time = ready_time;
	this->due_date = due_date;
}

Suplier::Suplier (){
}

//Suplier::~Suplier(){
//}