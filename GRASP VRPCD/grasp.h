#ifndef __GRASP_H__
#define __GRASP_H__

#include "imports.h"
#include "instance.h"
#include "utils.h"
#include "request.h"
#include "node.h"
#include "crossdock.h"
#include "vehicle.h"
#include "solution.h"



class Grasp{
	public:
		//atributos de clase
		Instance instance;

		// funciones de la clase
		Grasp(Instance);
		Grasp();
		tuple<Request,float,float,bool> get_cheaper_request(vector<Request>, Vehicle);
		//tuple<Request,float,bool> get_cheaper_request(Suplier, Customer ,vector<Request>, int);
		Solution initial_solution();

};
#endif