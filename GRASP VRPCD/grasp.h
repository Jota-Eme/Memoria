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
		int list_size;

		// funciones de la clase
		Grasp(Instance,int);
		Grasp();
		tuple<vector<tuple<Request,float>>,bool> get_cheaper_requests(vector<Request>, Vehicle);
		Solution initial_solution();

};
#endif