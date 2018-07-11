#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "imports.h"
#include "request.h"
#include "utils.h"
#include "crossdock.h"


class Instance{
	public:
		//atributos de clase
		string input_file;

		int request_number, vehicle_capacity, unit_time_pallet, fixed_time_preparation, crossdocks_number,total_demand;
		//velocidad de vehiculos en KM/H
		float speed;

		Node vehicle_depot;
		vector<Request> requests;
		vector<Crossdock> crossdocks;

		// funciones de la clase
		
		Instance(string);
		Instance();
		void read_instance();

};
#endif


