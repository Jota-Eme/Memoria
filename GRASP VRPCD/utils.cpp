#include "utils.h"

vector<string> split(string line, char delimiter) {
  vector<string> new_vector;
  stringstream new_string(line); 
  string token;
  
  while(getline(new_string, token, delimiter)) {
    new_vector.push_back(token);
  }
  
  return new_vector;
}

vector<int> get_int_vector(string line) {

	line = trim(line);
	vector<int> new_vector;
	stringstream new_string(line); 
	string token;

	while(getline(new_string, token, ' ')) {
		if(token.compare(" ") == -1 ){
			new_vector.push_back(-1);
		}
		else{
			new_vector.push_back(atoi(token.c_str()));
		}
	}

	return new_vector;
}

string trim_left(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

string trim_right(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}

string trim(const string& str)
{
  return trim_left(trim_right(str));
}


void print_solution(Solution solution){
	vector <Vehicle>::iterator vehicle_iterator;
	vector <Suplier>::iterator suplier_iterator;
	vector <Customer>::iterator customer_iterator;
	vector <Crossdock>::iterator crossdock_iterator;
	vector<Vehicle> vehicles = solution.vehicles;

	for (vehicle_iterator = vehicles.begin(); vehicle_iterator != vehicles.end(); ++vehicle_iterator) {
		cout<<"[ ";
	    for (suplier_iterator = vehicle_iterator->pickup_route.begin(); suplier_iterator != vehicle_iterator->pickup_route.end(); ++suplier_iterator) {
	    	cout<<suplier_iterator->id<<" ";
		}
		cout<<"]   ";
		cout<<"[ ";
		for (crossdock_iterator = vehicle_iterator->crossdock_route.begin(); crossdock_iterator != vehicle_iterator->crossdock_route.end(); ++crossdock_iterator) {
	    	cout<<crossdock_iterator->id<<" ";
		}
		cout<<"]   ";
		cout<<"[ ";
		for (customer_iterator = vehicle_iterator->delivery_route.begin(); customer_iterator != vehicle_iterator->delivery_route.end(); ++customer_iterator) {
	    	cout<<customer_iterator->id<<" ";
		}
		cout<<"]"<<"  espacio disponible: "<<vehicle_iterator->remaining_capacity<<endl;

	}

}

void print_times(Solution solution){
	vector <Vehicle>::iterator vehicle_iterator;
	vector<Vehicle> vehicles = solution.vehicles;
	
	for (vehicle_iterator = vehicles.begin(); vehicle_iterator != vehicles.end(); ++vehicle_iterator) {
		cout<<"[ ";
	    for (int i=0; (unsigned)i<vehicle_iterator->pickup_times.size(); i++) {
	    	cout<<"{"<<get<0>(vehicle_iterator->pickup_times[i])<<","<<get<1>(vehicle_iterator->pickup_times[i])<<"} ";
		}
		cout<<"]   [ ";

		for (int i=0; (unsigned)i<vehicle_iterator->crossdock_times.size(); i++) {
	    	cout<<"{"<<get<0>(vehicle_iterator->crossdock_times[i])<<","<<get<1>(vehicle_iterator->crossdock_times[i])<<"} ";
		}
		cout<<"]   [ ";
		for (int i=0; (unsigned)i<vehicle_iterator->delivery_times.size(); i++) {
	    	cout<<"{"<<get<0>(vehicle_iterator->delivery_times[i])<<","<<get<1>(vehicle_iterator->delivery_times[i])<<"} ";
		}
		cout<<" ]"<<endl;

	}

}

bool feasible_solution(Solution solution){
	
	vector <Vehicle>::iterator vehicle_iterator;
	vector<Vehicle> vehicles = solution.vehicles;
	bool feasible = true;
	for (vehicle_iterator = vehicles.begin(); vehicle_iterator != vehicles.end(); ++vehicle_iterator) {
		if(vehicle_iterator->feasible_route() == false) feasible = false;
	}
	return feasible;
}


void export_solution(Solution solution){
	ofstream myfile;
	myfile.open ("solution.txt");
	vector <Vehicle>::iterator vehicle_iterator;
	vector<Vehicle> vehicles = solution.vehicles;

	myfile << solution.vehicles.size() << "\n";

	for (vehicle_iterator = vehicles.begin(); vehicle_iterator != vehicles.end(); ++vehicle_iterator) {

		myfile << vehicle_iterator->pickup_route[0].x_coord << "," << vehicle_iterator->pickup_route[0].y_coord;

		for (int i=1; (unsigned)i<vehicle_iterator->pickup_route.size(); i++) {
			myfile << "-" << vehicle_iterator->pickup_route[i].x_coord << "," << vehicle_iterator->pickup_route[i].y_coord;
	    }
	    for (int i=0; (unsigned)i<vehicle_iterator->crossdock_route.size(); i++) {
			myfile << "-" << vehicle_iterator->crossdock_route[i].x_coord << "," << vehicle_iterator->crossdock_route[i].y_coord;
	    }
	    for (int i=0; (unsigned)i<vehicle_iterator->delivery_route.size(); i++) {
			myfile  << "-" << vehicle_iterator->delivery_route[i].x_coord << "," << vehicle_iterator->delivery_route[i].y_coord;
	    }
	    myfile<<"\n";

	}
	
	myfile.close();
}