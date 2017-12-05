#include "instance.h"

// CONSTRUCTOR
Instance::Instance (string input){
	this->input_file = input;
}


// funcion que agrega un request
/*void Instance::add_request(Request request){
	this->requests.push_back(request);
}*/

//funcion que lee las instancias (actualmente las de Morais)
void Instance::read_instance(){


	ifstream file; 
	file.open(input_file,ios::in);


	if (!file.is_open()){
		cout << "ERROR: El archivo especificado no existe" << endl;
		file.close();
		return;
	}

	string line;
	vector<int> prev_data;
	// LINEA 1: obtencion del numero de Request
	getline(file, line);
	getline(file, line);
  	this->request_number = stoi(line);

	// LINEA 2 : obtencion de la capacidad de los vehiculos
	getline(file, line);
	getline(file, line);
	getline(file, line);
  	this->vehicle_capacity = stoi(line);

	// LINEA 3: obtencion de los tiempos de carga y descarga
  	getline(file, line);
	getline(file, line);
	getline(file, line);
  	prev_data = get_int_vector(line);

  	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());

  	/*for(int j = 0; (unsigned)j < prev_data.size(); ++j){
		cout <<  prev_data[j] << endl;
	}*/

	this->unit_time_pallet = prev_data[0];
	this->fixed_time_preparation = prev_data[1];

	// LINEA 5 en adelante: obtencion de la informacion de los pedidos
	getline(file, line);
	getline(file, line);
	getline(file, line);

	// se lee la primera linea correspondiente al cross-dock
	prev_data = get_int_vector(line);
	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());
	Crossdock crossdock(prev_data[1],prev_data[2],prev_data[3],prev_data[4]);

	for(int i=0; i<this->request_number; i++){
		getline(file, line);
	  	prev_data = get_int_vector(line);
	  	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());
	  	//se crean las respectivas clases
	  	Suplier suplier(prev_data[1],prev_data[2],prev_data[3],prev_data[4]);
	  	Customer customer(prev_data[5],prev_data[6],prev_data[7],prev_data[8]);
	  	Request request(suplier,customer,prev_data[9]);

	  	this->requests.push_back(request);
  	}

	file.close();
	
}