#include "instance.h"

// CONSTRUCTOR
Instance::Instance (string input){
	this->input_file = input;
	this->total_demand = 0;
}

Instance::Instance(){
	this->total_demand = 0;
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

	this->total_demand = 0;
	
	string line;
	vector<int> prev_data;
	// LINEA 1: obtencion del numero de Request
	getline(file, line);
	getline(file, line);
  	this->request_number = stoi(line);

	// LINEA 2 : obtencion de la cantidad de crossdocks
	getline(file, line);
	getline(file, line);
	getline(file, line);
  	this->crossdocks_number = stoi(line);

  	// LINEA 3 : obtencion de la capacidad de los vehiculos
	getline(file, line);
	getline(file, line);
	getline(file, line);
	prev_data = get_int_vector(line);

  	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());

  	this->vehicle_capacity = prev_data[0];
	this->speed = (float)prev_data[1];

	// LINEA 4: obtencion de los tiempos de carga y descarga
  	getline(file, line);
	getline(file, line);
	getline(file, line);
  	prev_data = get_int_vector(line);

  	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());

	this->unit_time_pallet = prev_data[0];
	this->fixed_time_preparation = prev_data[1];


	// LINEA 5 en adelante: obtencion de la informacion de los pedidos
	getline(file, line);
	getline(file, line);

	// se comienzan a leer los crossdocks
	for(int i=0; i<this->crossdocks_number; i++){
		getline(file, line);
		prev_data = get_int_vector(line);
		prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());
		
		Crossdock crossdock(prev_data[0],prev_data[1],prev_data[2],prev_data[3],prev_data[4]);
		this->crossdocks.push_back(crossdock);
	}


	// se comienzan a leer los requests
	for(int i=0; i<this->request_number; i++){
		getline(file, line);
	  	prev_data = get_int_vector(line);
	  	prev_data.erase(remove(prev_data.begin(), prev_data.end(), -1), prev_data.end());
	  	//se crean las respectivas clases
	  	Suplier suplier(prev_data[0],prev_data[1],prev_data[2],prev_data[3],prev_data[4]);
	  	Customer customer((prev_data[0]*(-1)),prev_data[5],prev_data[6],prev_data[7],prev_data[8]);
	  	Request request(suplier,customer,prev_data[9],prev_data[0]);

	  	this->total_demand += prev_data[9];
	  	this->requests.push_back(request);
  	}


  	// SE ASUME POR EL MOMENTO QUE EL ESTACIONAMIENTO ES EL CROSSDOCK NUMERO 0
  	this->vehicle_depot = Node(this->crossdocks[0].x_coord,this->crossdocks[0].y_coord, this->crossdocks[0].ready_time, this->crossdocks[0].due_date);


	file.close();
	
}