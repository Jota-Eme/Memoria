#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "imports.h"
#include "customer.h"
#include "suplier.h"

class Request{
	public:
		//atributos de clase
		Customer customer;
		Suplier suplier;
		int demand;

		// funciones de la clase
		Request(Suplier,Customer,int);
		Request();

};
#endif