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

		// constructores
		Grasp(Instance,int);
		Grasp();
		//funcion que retorna una lista con los request de menos costo que cumplan las restricciones
		tuple<vector<tuple<Request,int, float>>,bool> get_cheaper_requests(vector<Request>, Vehicle);
		//funcion que retorna una lista con los request que optimicen el espacio a utilizar para el vehiculo
		tuple<vector<tuple<Request, int>>,bool> get_best_demand_requests(vector<Request>, Vehicle);
		//funcion que retorna el cd mas cercano desde un nodo
		int get_closest_crossdock(Node);
		//funcion que retorna una solucion inicial con el criterio de la ruta mas economica
		Solution initial_solution();
		//funcion que retorna una solucion inicial con el criterio de acomodar demanda p ara optimizar vehiculos
		Solution initial_solution_2();
		// funcion que retorna una solucion inicial de manera hibrida con las 2 anteriores
		Solution hybrid_initial_solution();
		//funcion que retorna el valor de la funcion de evaluacion de una solucion (costo de la solucion)
		float evaluation_function(Solution);
		// Realiza el movimiento 2-opt a una solucion
		Solution two_opt(Solution);
		// Realiza el proceso de consolidacion, retornando los tiempos de salida del CD de los vehiculos
		tuple<Vehicle,Vehicle,bool> consolidation(tuple<Vehicle,int>, tuple<Vehicle, int>, int);
		// REALIZA EL MOVIMIENTO SWAP NODE, QUE SELECCIONA 2 VEHICULOS E INTERCAMBIA ENTRE ELLOS UN NODO AL AZAR DELIVERY.
		Solution swap_node(Solution,int);
		// FUNCION QUE EJECUTA EL MOVIMIENTO CAMBIO DE CROSSDOCKS
		Solution swap_cd(Solution);
		// FUNCION QUE EJECUTA EL ALGORITMO GRASP, HACIENDO TODOS LOS PASOS DE ESTE
		Solution run(int,int,int,int,int,int,int);

};
#endif