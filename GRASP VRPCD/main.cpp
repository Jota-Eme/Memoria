#include "imports.h"
#include "instance.h"
#include "request.h"
#include "grasp.h"
#include "solution.h"


// FUNCION QUE LEERA LA VENTANA DE COMANDOS 
tuple<string,int,int,int,int,int,int,int,int,int,int,int,int> read_input(int argc, char **argv){

	string input_file = (char *)(argv[1]);
	int list_graps_size = stoi((char *)(argv[2]));
	int list_tabu_capacity_size = stoi((char *)(argv[3]));
	int iterations_grasp = stoi((char *)(argv[4]));
	int iterations_phase1 = stoi((char *)(argv[5]));
	int iterations_phase2 = stoi((char *)(argv[6]));
	int iterations_phase3 = stoi((char *)(argv[7]));
	int porc_movimiento_two_opt = stoi((char *)(argv[8]));
	int porc_movimiento_swap_cd = stoi((char *)(argv[9]));
	int porc_movimiento_swap_pickup = stoi((char *)(argv[10]));
	int porc_movimiento_swap_delivery = stoi((char *)(argv[11]));
	int porc_movimiento_change_node = stoi((char *)(argv[12]));
	int seed = stoi((char *)(argv[13]));
	
	return make_tuple(input_file,list_graps_size,list_tabu_capacity_size,iterations_grasp,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,porc_movimiento_change_node,seed);

}


int main(int argc, char *argv[]) {

	if(argc != 14){
		cout<<argc<<endl;
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada tamaño_lista_grasp tamao_lista_tabu_capcity iteraciones_grasp iteraciones_phase1 iteraciones_phase2 iteraciones_phase3 porc_movimiento_two_opt porc_movimiento_swap_cd porc_movimiento_swap_pickup porc_movimiento_swap_delivery porc_movimiento_change_node semilla"<<endl;
		return 0;
	}

	string input_file;
	int list_graps_size,list_tabu_capacity_size,seed,iterations_grasp,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,porc_movimiento_change_node;

	tie(input_file,list_graps_size,list_tabu_capacity_size,iterations_grasp,iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,porc_movimiento_change_node,seed) = read_input(argc,argv);

	if(list_graps_size<=0){
		cout<<"El tamaño de la lista debe ser mayor o igual a 1"<<endl;
		return 0;
	}

	srand(seed);
	Instance instance(input_file);
	instance.read_instance();
	Grasp grasp(instance,list_graps_size,list_tabu_capacity_size);

	clock_t start_time = clock();

	Solution new_solution = grasp.run(iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,porc_movimiento_change_node);	
	Solution best_solution = new_solution;
	float new_time = grasp.evaluation_function(new_solution);
	float best_time = new_time;

	for(int i=1; i<iterations_grasp ; i++){
		new_solution = grasp.run(iterations_phase1,iterations_phase2,iterations_phase3,porc_movimiento_two_opt,porc_movimiento_swap_cd,porc_movimiento_swap_pickup,porc_movimiento_swap_delivery,porc_movimiento_change_node);	
		new_time = grasp.evaluation_function(new_solution);
		if(new_time < best_time){
			best_solution = new_solution;
			best_time = new_time;
		}
	}

	clock_t end_time = clock();

	if(grasp.feasible_solution(best_solution)){
		cout<<"LA SOLUCION FINAL SIIII ES FACTIBLE"<< endl;
	}
	else{
		cout<<"LA SOLUCION FINAL NOOOO ES FACTIBLE"<< endl;
	}

	cout<<"La solucion final es"<<endl;
	print_solution(best_solution);
	cout<<"----------------------------------------------------"<<endl;
	//cout<<"................Los tiempos son ...................."<<endl;
	//print_times(best_solution);

	cout<<"La demanda total es de: "<<instance.total_demand<<endl;
	cout<<"Los vehiculos optimos son: "<<ceil((float)instance.total_demand / instance.vehicle_capacity)<<endl;

	cout<<"CANTIDAD DE AUTOS USADOS: "<<best_solution.vehicles.size()<<endl;
	cout<<"COSTO TOTAL DE: "<< best_time <<endl;
	double total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
	cout<<" Se demoro: "<< total_time << " segundos" << endl;


	export_solution(best_solution);


    return 0;

}