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

	Solution solution = Solution();
	solution = grasp.initial_solution();

	cout<<"La solucion inicial es"<<endl;
	print_solution(solution);

    return 0;

}