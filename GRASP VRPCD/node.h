#include "imports.h"

class Node{
	public:
		//atributos de clase
		
		//TYPE 0=CD, 1=SUPLIER, 2=CUSTOMER
		int x_coord, y_coord, ready_time, due_date, id, type;
		
		// funciones de la clase
		//Nodo(int,int,int,int,int);
		Node();
		//~Nodo();
		float get_distance(Node);


};
