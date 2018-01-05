#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include "imports.h"
#include "suplier.h"
#include "customer.h"
#include "crossdock.h"

class Vehicle{
	public:
		int capacity,fixed_time,unit_time;
		vector<Suplier> pickup_route;
		vector<Customer> delivery_route;
		// rutas de crossdocks luego del delivery route (para IAA siempre habra solo 1 CD)
		vector<Crossdock> crossdock_route;
		// Lista de la cantidad items que contiene hasta el momento de cada supplier de la forma [{10,1},{11,2}...] 10 elem, del suplier 1, 11 del 2, etc.
		vector<tuple<int,int>> items;
		// Lista de tiempos de llegada y salida en cada nodo de la forma [{t_llegada1,t_salida1},{t_llegada2,t_salida2}...]
		vector<tuple<int,int>> pickup_times,crossdock_times,delivery_times;

		// funciones de la clase
		Vehicle(int,int,int);
		Vehicle();
		tuple<vector<tuple<float,float>> , vector<tuple<float,float>> , vector<tuple<float,float>> > set_times();

};
#endif