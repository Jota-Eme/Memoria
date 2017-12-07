#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include "imports.h"

class Vehicle{
	public:
		int capacity,fixed_time,unit_time;
		vector<int> pickup_route, delivery_route;
		// Lista de la cantidad items que contiene hasta el momento de cada supplier de la forma [{10,1},{11,2}...]
		vector<tuple<int,int>> items;

		// funciones de la clase
		Vehicle(int,int,int);
		Vehicle();

};
#endif