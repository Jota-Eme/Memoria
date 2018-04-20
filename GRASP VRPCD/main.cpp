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
	
	Solution final_solution = grasp.run(100);	
	if(feasible_solution(final_solution)){
		cout<<"LA SOLUCION FINAL SIIII ES FACTIBLE"<< endl;
	}
	else{
		cout<<"LA SOLUCION FINAL NOOOO ES FACTIBLE"<< endl;
	}

	cout<<"La solucion inicial es"<<endl;
	print_solution(final_solution);
	cout<<"----------------------------------------------------"<<endl;
	cout<<"................Los tiempos son ...................."<<endl;
	print_times(final_solution);

	cout<<"CANTIDAD DE AUTOS USADOS: "<<final_solution.vehicles.size()<<endl;
	cout<<"COSTO TOTAL DE: "<< grasp.evaluation_function(final_solution)<<endl;


    return 0;

}