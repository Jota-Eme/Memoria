
#include "vehicle.h"

// CONSTRUCTOR
Vehicle::Vehicle(int capacity, int fixed_time, int unit_time){
	this->capacity = capacity;
	this->fixed_time = fixed_time;
	this->unit_time = unit_time;

}

Vehicle::Vehicle(){
}


// Funcion que retorna los tiempos de llegada y salida de cada nodo, para la ruta de pickup y delivery

tuple<vector<tuple<float,float>> , vector<tuple<float,float>> , vector<tuple<float,float>> > set_times(){

	vector <Suplier>::iterator suplier_iterator;
	vector <Customer>::iterator customer_iterator;
	vector <Crossdock>::iterator crossdock_iterator;
	float arrival_time, departure_time = 0;
	vector<tuple<float,float>> pickup_times,delivery_times,crossdock_times;

	// PARA EL CASO DE IAA SOLO HABRA 1 CD SIEMPRE.
	Node current_node = this->crossdock_route[0];

	if(!(this->pickup_route.empty() && this->delivery_route.empty())){
		//ITERA SOBRE LOS NODOS DE LA RUTA DE PICKUP Y CALCULA LOS RESPECTIVOS TIEMPOS EN CADA NODO
		for (suplier_iterator = this->pickup_route.begin(); suplier_iterator != this->pickup_route.end(); ++suplier_iterator) {

			arrival_time = departure_time + current_node.get_distance(suplier_iterator);
			departure_time = max(arrival_time,suplier_iterator.ready_time);

			current_node = suplier_iterator;
			this->pickup_times.push_back(make_tuple(arrival_time,departure_time));
		}

		//SE LLEGA AL CROSSDOCK PARA COMENZAR LA CONSOLIDACION (POR AHORA SE ASUME QUE SE ENTREGA LO MISMO QUE SE RETIRA POR LO QUE NO HAY CONSOLIDACION)
		for (crossdock_iterator = this->crossdock_route.begin(); crossdock_iterator != this->crossdock_route.end(); ++crossdock_iterator) {

			arrival_time = departure_time + current_node.get_distance(crossdock_iterator);
			departure_time = max(arrival_time,crossdock_iterator.ready_time);

			current_node = crossdock_iterator;
			crossdock_times.push_back(make_tuple(arrival_time,departure_time));
		}

		//ITERA SOBRE LOS NODOS DE LA RUTA DE DELIVERY Y CALCULA LOS TIEMPOS EN CADA NODO
		for (customer_iterator = this->delivery_route.begin(); customer_iterator != this->delivery_route.end(); ++customer_iterator) {

			arrival_time = departure_time + current_node.get_distance(customer_iterator);
			departure_time = max(arrival_time,customer_iterator.ready_time);

			current_node = customer_iterator;
			this->delivery_times.push_back(make_tuple(arrival_time,departure_time));
		}

	}

	return make_tuple(pickup_times,crossdock_times,delivery_times);

}



