#include "request.h"


// CONSTRUCTOR
Request::Request (Suplier suplier, Customer customer, int demand){
	this->customer = customer;
	this->suplier = suplier;
	this->demand = demand;
}

Request::Request (){
}