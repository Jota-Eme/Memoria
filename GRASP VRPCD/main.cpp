#include "imports.h"
#include "instance.h"
#include "request.h"
#include "grasp.h"
#include "solution.h"


// FUNCION QUE LEERA LA VENTANA DE COMANDOS 
string read_input(int argc, char **argv){

	string input_file = (char *)(argv[1]);

	return input_file;

}


int main(int argc, char *argv[]) {

	if(argc != 2){
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada "<<endl;
		return 0;
	}

	string input_file;
	input_file = read_input(argc,argv);

	Instance instance(input_file);
	instance.read_instance();

	Grasp grasp(instance);
	vector<Request> requests = instance.requests;
	//Request cheaper_request = Request();
	/*float min_cost;
	tie(cheaper_request,min_cost) = grasp.get_cheaper_request(requests);
	cout<< "El request mas barato desde el CD es el numero "<< cheaper_request.id << " con un coste de " << std::setprecision(16) << min_cost <<endl;*/

	Solution solution = Solution();
	solution = grasp.initial_solution();

	//cout << "la distancia es " << std::setprecision(16) << test <<endl;
	/*cout<< instance.requests[499].suplier.x_coord << endl;
	cout<< instance.requests[499].suplier.y_coord << endl;
	cout<< instance.requests[499].suplier.ready_time << endl;
	cout<< instance.requests[499].suplier.due_date << endl;
	cout<< instance.requests[499].customer.x_coord << endl;
	cout<< instance.requests[499].customer.y_coord << endl;
	cout<< instance.requests[499].customer.ready_time << endl;
	cout<< instance.requests[499].customer.due_date << endl;
	cout<< instance.requests[499].demand << endl;*/

    return 0;

}