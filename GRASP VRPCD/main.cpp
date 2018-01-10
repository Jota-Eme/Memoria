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
	vector<Request> requests = instance.requests;

	Solution solution = Solution();
	solution = grasp.initial_solution();



	cout<<"La solucion inicial es"<<endl;
	print_solution(solution);

	print_times(solution);

	cout<<"CANTIDAD DE AUTOS USADOS: "<<solution.vehicles.size()<<endl;
	cout<<"COSTO TOTAL DE: "<< grasp.evaluation_function(solution)<<endl;

	/*vector<tuple<string,int>> asd;
	asd.push_back(make_tuple("quinto",5));
	asd.push_back(make_tuple("tercero",3));
	asd.push_back(make_tuple("primero",1));
	asd.push_back(make_tuple("cuarto",4));
	asd.push_back(make_tuple("segundo",2));*/

	/*for(int i=0; (unsigned)i<asd.size();i++){
		cout<<get<1>(asd[i])<<" ";
	}*/

	/*cout<<endl;
	std::sort(begin(asd), end(asd), [](tuple<string, int> const &t1, tuple<string, int> const &t2) {
        return get<1>(t1) < get<1>(t2); // or use a custom compare function
    	}
    );*/

	/*for(int i=0; (unsigned)i<asd.size();i++){
		cout<<get<1>(asd[i])<<" ";
	}*/

	

    return 0;

}