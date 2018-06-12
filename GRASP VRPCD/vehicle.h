#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include "imports.h"
#include "suplier.h"
#include "customer.h"
#include "crossdock.h"
#include "node.h"

class Vehicle{
	public:
		int total_capacity,fixed_time,unit_time,remaining_capacity;
		vector<Suplier> pickup_route;
		vector<Customer> delivery_route;
		// rutas de crossdocks luego del pickup route (para IAA siempre habra solo 1 CD)
		vector<Crossdock> crossdock_route;
		// Lista de la cantidad items que contien de cada supplier, en cada ruta de la forma [{10,1},{11,2}...] 10 elem, del suplier 1, 11 del 2, etc.
		vector<tuple<int,int>> pickup_items, delivery_items;
		// Lista de tiempos de llegada y salida en cada nodo de la forma [{t_llegada1,t_salida1},{t_llegada2,t_salida2}...]
		vector<tuple<float,float>> pickup_times,crossdock_times,delivery_times;
		//Lugar de partida y termino de los vehiculos, por el momento se asume el primer CD leido
		Node vehicle_depot;
		// TIempo en el que el vehiculo deja un crossdock luego de la consolidacion
		int departure_cd_time;


		// funciones de la clase
		Vehicle(int,int,int, Node);
		Vehicle();
		//funcion que setea los tiempos de llegada y salida de cada nodo de todas las rutas
		void set_times();
		//funcion que determina si la rita cumple con los TW
		bool feasible_route();
		float get_pickup_cost();
		float get_crossdock_cost();
		float get_delivery_cost();
		//funcion que retorna el costo del vehiculo
		float get_total_cost();
		//funcion que obtiene la capacidad restante con la que llega al CD luego de la ruta de pickup
		int get_pickup_capacity();

};
#endif