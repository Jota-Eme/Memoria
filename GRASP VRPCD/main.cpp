#include "imports.h"
#include "instance.h"
#include "request.h"
#include "grasp.h"
#include "solution.h"


// FUNCION QUE LEERA LA VENTANA DE COMANDOS 
tuple<string,int,int,int,int,int,float,float,int> read_input(int argc, char **argv){

	string input_file = (char *)(argv[1]);
	int list_graps_size = stoi((char *)(argv[2]));
	int time_limit = stoi((char *)(argv[3]));
	int iterations_grasp = stoi((char *)(argv[4]));
	int iterations_phase1 = stoi((char *)(argv[5]));
	int size_window = stoi((char *)(argv[6]));
	float decay_factor = stof((char *)(argv[7]));
	float explore_factor = stof((char *)(argv[8]));
	int seed = stoi((char *)(argv[9]));

	return make_tuple(input_file,list_graps_size,time_limit,iterations_grasp,iterations_phase1,size_window,decay_factor,explore_factor,seed);

}


int main(int argc, char *argv[]) {

	if(argc != 10){
		cout<<argc<<endl;
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada tamaño_lista_grasp tiempo_limite iteraciones_grasp iteraciones_phase1 tamano_ventana_deslizante decay_factor explore_factor semilla"<<endl;
		return 0;
	}

	string input_file;
	int list_graps_size,time_limit,seed,iterations_grasp,iterations_phase1,size_window;
	float decay_factor,explore_factor;
	tie(input_file,list_graps_size,time_limit,iterations_grasp,iterations_phase1,size_window,decay_factor,explore_factor, seed) = read_input(argc,argv);

	if(list_graps_size<=0){
		cout<<"El tamaño de la lista debe ser mayor o igual a 1"<<endl;
		return 0;
	}

	srand(seed);
	Instance instance(input_file);
	instance.read_instance();
	Grasp grasp(instance,list_graps_size,size_window,decay_factor,explore_factor);

	clock_t start_time = clock();
	clock_t end_time,best_solution_time,actual_solution_time;
	double total_time;

	/*cout<<"Parametros:"<<endl;
	cout<<"tamano lista grasp: "<< list_graps_size<<endl;
	cout<<"tiempo limite: "<< time_limit<<endl;
	cout<<"iteraciones grasp: "<< iterations_grasp<<endl;
	cout<<"iteraciones phase 1: "<< iterations_phase1<<endl;
	cout<<"size_window: "<< size_window<<endl;
	cout<<"decay factor: "<< decay_factor<<endl;
	cout<<"explore factor: "<< explore_factor<<endl;
	cout<<"semilla: "<< seed <<endl;*/
	cout<<list_graps_size<<"-"<<time_limit<<"-"<<iterations_grasp<<"-"<<iterations_phase1<<"-"<<size_window<<"-"<<decay_factor<<"-"<<explore_factor<<"-"<<seed<<"/";

	Solution new_solution;
	tie(new_solution,actual_solution_time) = grasp.run(iterations_phase1,time_limit,start_time);	
	Solution best_solution = new_solution;
	best_solution_time= actual_solution_time;
	float new_time = grasp.evaluation_function(new_solution);
	float best_time = new_time;
	int graspit = 1;
	int best_grasp = 1;


	for(int i=1; i<iterations_grasp ; i++){

		end_time = clock();
	    total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;

	    if(total_time>=time_limit){
	    	break;
	    }

		tie(new_solution,actual_solution_time) = grasp.run(iterations_phase1,time_limit,start_time);	
		new_time = grasp.evaluation_function(new_solution);
		if(new_time < best_time){
			best_solution = new_solution;
			best_time = new_time;
			best_solution_time = actual_solution_time;
			best_grasp = graspit;
		}

		graspit+=1;
	}

	end_time = clock();

	if(grasp.feasible_solution(best_solution)){
		//cout<<"LA SOLUCION FINAL SIIII ES FACTIBLE"<< endl;
	}
	else{
		cout<<"ERROR: LA SOLUCION FINAL NOOOO ES FACTIBLE"<< endl;
	}

	//cout<<"La solucion final es"<<endl;
	//print_solution(best_solution);
	//cout<<"----------------------------------------------------"<<endl;
	//cout<<"................Los tiempos son ...................."<<endl;
	//print_times(best_solution);

	//cout<<"La demanda total es de: "<<instance.total_demand<<endl;
	//cout<<"Los vehiculos optimos son: "<<ceil((float)instance.total_demand / instance.vehicle_capacity)<<endl;

	//cout<<"CANTIDAD DE AUTOS USADOS: "<<best_solution.vehicles.size()<<endl;
	total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
	double best_solution_time_calc = (double)(best_solution_time - start_time)/CLOCKS_PER_SEC;

	/*cout<<"---------------------------------------------------------"<<endl;

	cout<<"Costo final de: "<< best_time <<endl;
	cout<<"Se demoro: "<< total_time << " segundos" << endl;
	cout<<"La mejor solucion se encontro en el tiempo: "<<best_solution_time_calc<<endl;
	cout<<"La solucion se encontro en la iteracion grasp: "<<best_grasp<<endl;
	cout<<"iteraciones grasp ejecutadas: "<<graspit<<endl;*/
	cout<<best_time<<"-"<<total_time<<"-"<<best_solution_time_calc<<"-"<<best_grasp<<"-"<<graspit<<endl;


	int pickup_nodes, delivery_nodes;
	tie(pickup_nodes,delivery_nodes) = best_solution.count_nodes();
	//cout<<"CANTIDAD NODOS PICKUP: "<<pickup_nodes<<endl;
	//cout<<"CANTIDAD NODOS DELIVERY: "<<delivery_nodes<<endl;
	export_solution(best_solution);

    return 0;

}