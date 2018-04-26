#include "imports.h"
#include "solution.h"
#include "suplier.h"
#include "customer.h"

// FUNCION QUE SEPARA UN STRING POR UN DELIMITADOR Y LAS METE A UN VECTOR DE STRINGS (SIMILAR A EXPLODE)
vector<string> split(string,char);
// FUNCIONES QUE ELIMINAN LOS ESPACIOS EN BLANCO AL FINAL Y AL COMIENZO DE UN STRING
string trim_left(const string& str);
string trim_right(const string& str);
string trim(const string& str);
// FUNCION QUE TRANSFORMA UN STRING SEPARADO POR ESPACIOS A UN VECTOR DE ENTEROS
vector<int> get_int_vector(string line);

void print_solution(Solution solution);
void print_times(Solution solution);
bool feasible_solution(Solution solution);
void export_solution(Solution solution);