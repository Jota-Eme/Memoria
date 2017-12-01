#include "suplier.h"


// CONSTRUCTOR
Suplier::Suplier (int x_coord, int y_coord, int ready_date, int due_date ){
	this->x_coord = x_coord;
	this->y_coord = y_coord;
	this->time_window = make_tuple(ready_date,due_date);
}

Suplier::Suplier (){

}