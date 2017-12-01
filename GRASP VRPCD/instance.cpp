#include "instance.h"

// CONSTRUCTOR
Instance::Instance (string input){
	this->input_file = input;
}

//funcion que lee las instancias (actualmente las de Morais)
void Instance::read_instance(){


	ifstream file; 
	file.open(input_file,ios::in);


	if (!file.is_open()){
		cout << "ERROR: El archivo especificado no existe" << endl;
		file.close();
		return;
	}

	/*string line;
	vector<string> prev_data;
	// LINEA 1: obtencion del numero de buses y capacidad desde la primera linea del archivo
	getline(file, line);
  	prev_data = split(line, ':');
  	this->bus_number = stoi(prev_data[0]);
  	this->bus_capacity = stoi(prev_data[1]);

	// LINEA 2 :obtencion de la cantidad de estaciones de buses y la cantidad de buses en cada una
	getline(file, line);
  	prev_data = split(line, ':');
	this->bus_stations_number = stoi(prev_data[0]);
	this->bus_in_station = get_int_vector(prev_data[1]);

	// LINEA 3: obtencion de los puntos de encuentro con su gente respectiva
	getline(file, line);
  	prev_data = split(line, ':');
	this->points_number = stoi(prev_data[0]);
	this->total_persons = stoi(prev_data[1]);
	this->persons_in_points = get_int_vector(prev_data[2]);

	// LINEA 4: obtencion de los refugios con sus capacidades respectivas
	getline(file, line);
  	prev_data = split(line, ':');
	this->shelters_number = stoi(prev_data[0]);
	this->total_shelters_capacity = stoi(prev_data[1]);
	this->shelters_capacity = get_int_vector(prev_data[2]);

	// LINEA 5 en adelante: obtencion de las distancias iniciales de cada estacion a los puntos de encuentro
	getline(file, line);

	for(int i=0; i < this->bus_stations_number; i++){
		getline(file, line);
	  	prev_data = split(line, ':');
		this->initial_distances.push_back(get_int_vector(prev_data[1]));
	}
	
	// MATRIZ: obtencion de las distancias de cada punto de encuentro a cada refugio
	getline(file, line);
	for(int i=0; i < this->points_number; i++){
		getline(file, line);
	  	prev_data = split(line, ':');
		this->point_distances.push_back(get_int_vector(prev_data[1]));
	}

	file.close();
	*/

}