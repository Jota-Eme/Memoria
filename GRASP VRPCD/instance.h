#include "imports.h"
#include "request.h"
#include "utils.h"


class Instance{
	public:
		//atributos de clase
		string input_file;

		int request_number, vehicle_capacity, unit_time_pallet, fixed_time_preparation;

		vector<Request> requests;

		// funciones de la clase
		
		Instance(string);
		void read_instance();

};



