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
		// largo de la lista del GRASP
		int list_size;
		// largo de la lista tabu de more capacity
		int tabu_capacity_size;
		// largo de la lista tabu de worst route
		int tabu_worst_route_size;
		// criterio de cambio de listas tabu
		int criteria;


		// constructores
		Grasp(Instance,int,int,int);
		Grasp();
		//funcion que retorna una lista con los request de menos costo que cumplan las restricciones
		tuple<vector<tuple<Request,int, float>>,bool> get_cheaper_requests(vector<Request>, Vehicle);
		//funcion que retorna una lista con los request que optimicen el espacio a utilizar para el vehiculo
		tuple<vector<tuple<Request, int>>,bool> get_best_demand_requests(vector<Request>, Vehicle);
		//funcion que retorna una lista con los request que optimicen los tw de los proveedores
		tuple<vector<tuple<Request, int>>,bool> get_best_timewindow_requests(vector<Request>, Vehicle);
		//funcion que retorna el cd mas cercano desde un nodo
		int get_closest_crossdock(Node);
		//funcion que retorna una solucion inicial con el criterio de la ruta mas economica
		Solution distance_initial_solution();
		//funcion que retorna una solucion inicial con el criterio de acomodar demanda p ara optimizar vehiculos
		Solution demand_initial_solution();
		// funcion que retorna una solucion inicial de manera hibrida con las 2 anteriores
		Solution hybrid_initial_solution();
		// funcion que retorna una solucion inicial con el criterio de menor time window
		Solution timewindow_initial_solution();
		//funcion que retorna el valor de la funcion de evaluacion de una solucion (costo de la solucion)
		float evaluation_function(Solution);
		// Realiza el movimiento 2-opt a una solucion
		Solution mov_two_opt(Solution);
		// Realiza el proceso de consolidacion, retornando los tiempos de salida del CD de los vehiculos
		tuple<Vehicle,Vehicle,bool> consolidation(tuple<Vehicle,int>, tuple<Vehicle, int>, int);
		// REALIZA EL MOVIMIENTO SWAP NODE, QUE SELECCIONA 2 VEHICULOS E INTERCAMBIA ENTRE ELLOS UN NODO AL AZAR DELIVERY.
		Solution mov_swap_node(Solution,int);
		// FUNCION QUE EJECUTA EL MOVIMIENTO CAMBIO DE CROSSDOCKS
		Solution mov_swap_cd(Solution);
		//funcion que retorna el vehiculo que posea la ruta mas cara y especifica que tipo de ruta es
		tuple<int,int> get_worst_route(Solution,int,vector<int>);
		// funcion que retorna el vehiculo que posea la mayor capacidad de acuerdo a la ruta especificada
		int get_more_capacity(Solution, int, vector<int>);
		// MOVIMIENTO QUE QUITA UN NODO DE LA RUTA MAS LARGA Y LO COLOCA EN EL QUE TENGA MAS ESPACIO.
		tuple<Solution,vector<int>,vector<int>> mov_change_node(Solution, vector<int>,vector<int>);
		// funcion que determina si una solucion es factible respecto a todas las restricciones
		bool feasible_solution(Solution);
		//funcion que realiza todo el proceso de consolidacion y setea el atributo departure_cd_time
		Solution consolidation2(Solution);
		//funcion que obtiene los vehiculos involucrados en la consolidacion (los vehiculos que tiene q esperar para cargar items)
		vector<int> get_involved_vehicles(Vehicle,Solution);
		vector<int> get_involved_vehicles2(Vehicle);
		// FUNCION QUE EJECUTA EL ALGORITMO GRASP, HACIENDO TODOS LOS PASOS DE ESTE
		Solution run(int,int,int,int,int,int,int,int);

};
#endif