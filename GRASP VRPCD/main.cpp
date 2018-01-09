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

	if(argc != 3){
		cout << "El formato para la ejecucion es: " << argv[0] << " archivo_entrada semilla"<<endl;
		return 0;
	}

	int seed=atoi(argv[2]);
	srand(seed);

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

	print_times(solution);

	cout<<"CANTIDAD DE AUTOS USADOS: "<<solution.vehicles.size()<<endl;

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