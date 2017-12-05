#include "node.h"


// CONSTRUCTOR
/*Nodo::Nodo (int id, int x_coord, int y_coord, int ready_time, int due_date ){
	this->id = id;
	this->x_coord = x_coord;
	this->y_coord = y_coord;
	this->ready_time = ready_time;
	this->due_date = due_date;
}*/

Node::Node (){
}

//Nodo::~Nodo(){
//}


//Funcion que retorna la distancia euclideana del cross-dock a otro nodo.
float Node::get_distance(Node node){
	cout<<"el node que me pasaste tiene coordenadas: "<< node.x_coord << " y " << node.y_coord <<endl;

	return 1.0;
}