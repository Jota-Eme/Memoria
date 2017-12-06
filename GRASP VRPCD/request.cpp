#include "request.h"


// CONSTRUCTOR
Request::Request (Suplier suplier, Customer customer, int demand, int id){
	this->customer = customer;
	this->id = id;
	this->suplier = suplier;
	this->demand = demand;
}

Request::Request (){
}