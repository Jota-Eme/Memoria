#include "imports.h"
#include "instance.h"
#include "request.h"
#include "grasp.h"
#include "solution.h"


// FUNCION QUE LEERA LA VENTANA DE COMANDOS 
tuple<string,int,int> read_input(int argc, char **argv){

	string input_file = (char *)(argv[1]);
	int list_size = stoi((char *)(argv[2]));
	int seed = stoi((char *)(argv[3]));

	return make_tuple(input_file,list_size,seed);

}


int main(int argc, char *argv[]) {

	if(argc != 4){
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada tamaño_lista semilla"<<endl;
		return 0;
	}

	string input_file;
	int list_size,seed;

	tie(input_file,list_size,seed) = read_input(argc,argv);

	if(list_size<=0){
		cout<<"El tamaño de la lista debe ser mayor o igual a 1"<<endl;
		return 0;
	}

	srand(seed);
	Instance instance(input_file);
	instance.read_instance();

	Grasp grasp(instance,list_size);
	
	Solution solution = Solution();
	solution = grasp.initial_solution();	

	cout<<"La solucion inicial es"<<endl;
	print_solution(solution);
	cout<<"----------------------------------------------------"<<endl;
	cout<<"................Los tiempos son ...................."<<endl;
	print_times(solution);

	cout<<"CANTIDAD DE AUTOS USADOS: "<<solution.vehicles.size()<<endl;
	cout<<"COSTO TOTAL DE: "<< grasp.evaluation_function(solution)<<endl;


    return 0;

}