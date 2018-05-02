#include "imports.h"
#include "instance.h"
#include "request.h"
#include "grasp.h"
#include "solution.h"


// FUNCION QUE LEERA LA VENTANA DE COMANDOS 
tuple<string,int,int,int,int,int,int,int,int,int> read_input(int argc, char **argv){

	string input_file = (char *)(argv[1]);
	int list_size = stoi((char *)(argv[2]));
	int iterations_phase1 = stoi((char *)(argv[3]));
	int iterations_phase2 = stoi((char *)(argv[4]));
	int iterations_phase3 = stoi((char *)(argv[5]));
	int porc_movimiento_two_opt = stoi((char *)(argv[6]));
	int porc_movimiento_swap_cd = stoi((char *)(argv[7]));
	int porc_movimiento_swap_pickup = stoi((char *)(argv[8]));
	int porc_movimiento_swap_delivery = stoi((char *)(argv[9]));
	int seed = stoi((char *)(argv[10]));
	
	return make_tuple(input_file,list_size,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,seed);

}


int main(int argc, char *argv[]) {

	if(argc != 11){
		cout<<argc<<endl;
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada tamaño_lista iteraciones_phase1 iteraciones_phase2 iteraciones_phase3 porc_movimiento_two_opt porc_movimiento_swap_cd porc_movimiento_swap_pickup porc_movimiento_swap_delivery semilla"<<endl;
		return 0;
	}

	string input_file;
	int list_size,seed,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery;

	tie(input_file,list_size,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,seed) = read_input(argc,argv);

	if(list_size<=0){
		cout<<"El tamaño de la lista debe ser mayor o igual a 1"<<endl;
		return 0;
	}

	srand(seed);
	Instance instance(input_file);
	instance.read_instance();

	Grasp grasp(instance,list_size);
	clock_t start_time = clock();
	Solution final_solution = grasp.run(iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery);	
	clock_t end_time = clock();

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
	double total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
	cout<<" Se demoro: "<< total_time << " segundos" << endl;

	export_solution(final_solution);


    return 0;

}