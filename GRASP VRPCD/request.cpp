#include "request.h"


// CONSTRUCTOR
Request::Request (Customer customer, Suplier suplier, int demand){
	this->customer = customer;
	this->suplier = suplier;
	this->demand = demand;
}