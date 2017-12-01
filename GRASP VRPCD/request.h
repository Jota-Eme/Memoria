#include "imports.h"
#include "customer.h"
#include "suplier.h"

class Request{
	public:
		//atributos de clase
		Customer customer;
		Suplier suplier;
		int demand;

		tuple < int, int > time_window;
		
		// funciones de la clase
		Request(Customer,Suplier,int);

};
