
#include "vehicle.h"

// CONSTRUCTOR
Vehicle::Vehicle(int capacity, int fixed_time, int unit_time, Node vehicle_depot){
	this->total_capacity = capacity;
	this->remaining_capacity = capacity;
	this->fixed_time = fixed_time;
	this->unit_time = unit_time;
	this->vehicle_depot = vehicle_depot;
	this->departure_cd_time = 0;

}

Vehicle::Vehicle(){
}


// Funcion que retorna los tiempos de llegada y salida de cada nodo, para la ruta de pickup y delivery

void Vehicle::set_times(){

	vector <Suplier>::iterator suplier_iterator;
	vector <Customer>::iterator customer_iterator;
	vector <Crossdock>::iterator crossdock_iterator;
	float arrival_time, departure_time = 0;

	this->pickup_times.clear();
	this->crossdock_times.clear();
	this->delivery_times.clear();

	// PARA EL CASO DE IAA SOLO HABRA 1 CD SIEMPRE.
	Node current_node = this->vehicle_depot;

	if(this->pickup_route.empty() == false && this->delivery_route.empty() == false){
		//ITERA SOBRE LOS NODOS DE LA RUTA DE PICKUP Y CALCULA LOS RESPECTIVOS TIEMPOS EN CADA NODO
		for (suplier_iterator = this->pickup_route.begin(); suplier_iterator != this->pickup_route.end(); ++suplier_iterator) {

			Suplier &new_suplier = *suplier_iterator;
			arrival_time = departure_time + current_node.get_distance(new_suplier);
			departure_time = max(arrival_time, (float)new_suplier.ready_time);

			current_node = new_suplier;
			this->pickup_times.push_back(make_tuple(arrival_time,departure_time));
		}

		//SE LLEGA AL CROSSDOCK PARA COMENZAR LA CONSOLIDACION 
		for (crossdock_iterator = this->crossdock_route.begin(); crossdock_iterator != this->crossdock_route.end(); ++crossdock_iterator) {

			Crossdock &new_crossdock = *crossdock_iterator;
			arrival_time = departure_time + current_node.get_distance(new_crossdock);
			int real_arrival = max(arrival_time, (float)new_crossdock.ready_time);
			departure_time = max(real_arrival, this->departure_cd_time);

			current_node = new_crossdock;
			this->crossdock_times.push_back(make_tuple(arrival_time,departure_time));
		}

		//ITERA SOBRE LOS NODOS DE LA RUTA DE DELIVERY Y CALCULA LOS TIEMPOS EN CADA NODO
		for (customer_iterator = this->delivery_route.begin(); customer_iterator != this->delivery_route.end(); ++customer_iterator) {

			Customer &new_customer = *customer_iterator;
			arrival_time = departure_time + current_node.get_distance(new_customer);
			departure_time = max(arrival_time, (float)new_customer.ready_time);

			current_node = new_customer;
			this->delivery_times.push_back(make_tuple(arrival_time,departure_time));
		}

	}

}


//funcion que retorna si la ruta del vehiculo es factible o no (en cuanto a TW)
bool Vehicle::feasible_route(){

	this->set_times();

	bool feasible = true;
	float arrival_time, departure_time;

	for(int i=0; (unsigned)i < this->pickup_times.size();i++){

		arrival_time = get<0>(pickup_times[i]);
		departure_time = get<1>(pickup_times[i]);
		
		// el tiempo de llegada y salida en un nodo debe ser menor que el tiempo de vencimiento de ese nodo
		if((arrival_time <= pickup_route[i].due_date) == false && (departure_time <= pickup_route[i].due_date) == false) {
			/*cout<<arrival_time<<endl;
			cout<<departure_time<<endl;
			cout<<pickup_route[i].due_date<<endl;
			cout<<"murio en el nodo "<< i << " de la ruta de pickup"<<endl;*/
			feasible = false;

		}
	}

	for(int i=0; (unsigned)i < this->crossdock_times.size();i++){

		arrival_time = get<0>(crossdock_times[i]);
		departure_time = get<1>(crossdock_times[i]);
		// el tiempo de llegada y salida en un nodo debe ser menor que el tiempo de vencimiento de ese nodo
		if((arrival_time <= crossdock_route[i].due_date) == false && (departure_time <= crossdock_route[i].due_date) == false) {
			feasible = false;
			/*cout<<arrival_time<<endl;
			cout<<departure_time<<endl;
			cout<<crossdock_route[i].due_date<<endl;
			cout<<"murio en el nodo "<< i << " de la ruta de crossdock"<<endl;*/

		}
	}

	for(int i=0; (unsigned)i < this->delivery_times.size();i++){

		arrival_time = get<0>(delivery_times[i]);
		departure_time = get<1>(delivery_times[i]);
		// el tiempo de llegada y salida en un nodo debe ser menor que el tiempo de vencimiento de ese nodo
		if((arrival_time <= delivery_route[i].due_date) == false && (departure_time <= delivery_route[i].due_date) == false) {
			feasible = false;
			/*cout<<arrival_time<<endl;
			cout<<departure_time<<endl;
			cout<<delivery_route[i].due_date<<endl;
			cout<<"murio en el nodo "<< i << " de la ruta de delivery"<<endl;*/

		}
	}

	return feasible;

}
