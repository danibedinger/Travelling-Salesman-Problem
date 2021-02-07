#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono; //medir tiempo de ejecucion


//IMPLEMENTACION DEL GRAFO
class Grafo {
private:
	int numV;
	int min; //para facilitar la cota 1
	vector<vector<int>> adjList;

public:
	Grafo(int n) {
		numV = n;
		min = INT_MAX;
		for (int i = 0; i < numV; i++)
		{
			vector<int> row;
			adjList.push_back(row);
		}
	}
	
	void addSaliente(int out, int in, int valor);
	int getMin();
	bool existeArista(int v1, int v2);
	int valor(int v1, int v2);
	void display();
	int numVer();
	int sumaMin(int v, int c);
	int suma2Min(int v);
	vector<vector<int>> getAdj();
};

int Grafo::sumaMin(int v, int c) { //obtiene el valor minimo de las aristas salientes de v
	int min = INT_MAX;			   //tiene en cuenta si está en uso ya mediante el contador
	int cont = 1;
	for (int i = 0; i < numV; i++)
	{
		if (adjList[v][i] < min && adjList[v][i] != -1) {
			if (cont > 0) cont--;
			else min = adjList[v][i];
		}
	}
	return min;
}

int Grafo::suma2Min(int v) { //calculo de la suma de las dos aristas minimas salientes de v
	int n1, n2;
	n1 = n2 = INT_MAX;
	for (int i = 0; i < numV; i++)
	{
		if (adjList[v][i] < n1 && adjList[v][i] != -1) {
			n2 = n1;
			n1 = adjList[v][i];
		}
		else if (adjList[v][i] < n2 && adjList[v][i] != -1) {
			n2 = adjList[v][i];
		}
	}
	return (n1 + n2);
}

vector<vector<int>> Grafo::getAdj() { return adjList; }

int Grafo::numVer() { return numV; }

void Grafo::display() { //funcion de prueba solo
	for (int i = 0; i < numV; i++)
	{
		for (int j = 0; j < numV; j++)
		{
			cout << adjList[i][j] << " ";
		}
		cout << "\n";
	}
}

void Grafo::addSaliente(int out, int in, int valor) { //ver si necesitamos quitarlo o no
	adjList[out].push_back(valor);
	//adjList[out][in] = valor;
	if (valor < min) min = valor;
}

int Grafo::getMin() { return min; }

bool Grafo::existeArista(int v1, int v2) {
	if (adjList[v1][v2] > 0) return true;
	return false;
}

int Grafo::valor(int v1, int v2) { return adjList[v1][v2]; }

//FIN DE LA IMPLEMENTACION DEL GRAFO

typedef struct { //Estructura para los nodos de RyP
	vector<int> sol;
	int k;
	int coste;
	int coste_estimado;
	vector<bool> usado;
	vector<vector<int>> reducida; //usado para la cota mejor
}nodo;



//IMPLEMENTACION DE NUESTRA COLA DE PRIORIDAD

class ColaPrioridad {
private:
	vector<nodo> _cola;
	int left(int parent);
	int right(int parent);
	int parent(int child);
	int _k;
	void hundir(int i);
	void flotar(int k);

public:
	ColaPrioridad() {
		_k = 0;
	}

	bool esVacia();
	void insertar(nodo elem);
	void elimMin();
	nodo minimo();
	int tam();
	void display();

};

int ColaPrioridad::left(int parent)
{
	int l = 2 * parent + 1;
	if (l < _k)
		return l;
	else
		return -1;
}

int ColaPrioridad::right(int parent)
{
	int r = 2 * parent + 2;
	if (r < _k)
		return r;
	else
		return -1;
}

int ColaPrioridad::parent(int child)
{
	int p = (child - 1) / 2;
	if (child == 0)
		return -1;
	else
		return p;
}

bool ColaPrioridad::esVacia() { return _k == 0; }

int ColaPrioridad::tam() { return _k; }

void ColaPrioridad::flotar(int k) {
	if (k >= 0 && parent(k) >= 0 && _cola[parent(k)].coste_estimado > _cola[k].coste_estimado)
	{
		nodo temp = _cola[k];
		_cola[k] = _cola[parent(k)];
		_cola[parent(k)] = temp;
		flotar(parent(k));
	}
}

void ColaPrioridad::insertar(nodo elem) {
	_cola.push_back(elem);
	_k++;
	flotar(_k - 1);
}

nodo ColaPrioridad::minimo() {
	return _cola.front();
}

void ColaPrioridad::hundir(int i) {
	int izq = left(i);
	int der = right(i);
	if (izq >= 0 && der >= 0 && _cola[izq].coste_estimado > _cola[der].coste_estimado)
	{
		izq = der;
	}
	if (izq > 0 && _cola[i].coste_estimado > _cola[izq].coste_estimado)
	{
		nodo temp = _cola[i];
		_cola[i] = _cola[izq];
		_cola[izq] = temp;
		hundir(izq);
	}
}

void ColaPrioridad::elimMin() {
	_cola[0] = _cola[_k - 1];
	_cola.pop_back();
	_k--;
	hundir(0);
}

void ColaPrioridad::display()
{
	nodo x;
	while (!esVacia())
	{
		x = minimo();
		cout << x.coste_estimado << " ";
		elimMin();
		
	}
}

//AQUI TERMINAN LOS METODOS PARA LA COLA DE PRIORIDAD

void print(vector<int> sol) {
	for (int i = 0; i < 4; i++)
	{
		cout << sol[i] << " ";
	}
	cout << endl;
}

nodo newNodo(int n) {
	nodo X;
	for (int i = 0; i < n; i++)
	{
		X.sol.push_back(-1);
		X.usado.push_back(false);
	}
	return X;
}

vector<vector<int>> reducir(vector<vector<int>> matriz, int n, int &coste_r, int desde, int hasta) {
	vector<vector<int>> reducida = matriz;
	vector<int> minC;
	vector<int> minF;

	

	for (int i = 0; i < n; i++) //INICIALIZACION DE MINIMOS
	{
		minC.push_back(INT_MAX);
		minF.push_back(INT_MAX);
	}

	if (desde >= 0 && hasta >= 0) {
		for (int i = 0; i < n; i++)
		{
			reducida[desde][i] = -1; //indicar que no pueden usarse mas
			reducida[i][hasta] = -1;
		}
		reducida[hasta][desde] = -1; //indica que no podemos volver hacia atras
	}

	for (int i = 0; i < n; i++) //CALCULO DE MINIMOS DE FILAS
	{
		for (int j = 0; j < n; j++)
		{
			//
			if (reducida[i][j] < minF[i] && reducida[i][j] >= 0) minF[i] = reducida[i][j];
		}
	}

	for (int i = 0; i < n; i++) //REDUCCION DE FILAS
	{
		if (minF[i] == INT_MAX) minF[i] = 0; //si toda la fila es -1 no se reduce
		if (minF[i] != 0) { //SI EL MINIMO ES 0 NO SE VA A REDUCIR LA FILA
			for (int j = 0; j < n; j++)
			{
				if (reducida[i][j] > 0) { //SI ES -1 NO HAY VERTICE Y NO HACE FALTA CAMBIAR SU VALOR
					reducida[i][j] -= minF[i];
				}
			}
		}
	}
	//valorar posibilidad de que todos sean -1
	for (int i = 0; i < n; i++) //CALCULO DE MINIMOS DE COLUMNAS
	{
		for (int j = 0; j < n; j++)
		{
			if (reducida[j][i] < minC[i] && reducida[j][i] >= 0) minC[i] = reducida[j][i];
		}
	}

	for (int i = 0; i < n; i++) //REDUCCION DE COLUMNAS
	{
		if (minC[i] == INT_MAX) minC[i] = 0; //si toda la columna es -1, no reducimos
		if (minC[i] != 0) { //SI EL MINIMO ES 0 NO SE VA A REDUCIR LA COLUMNA
			for (int j = 0; j < n; j++)
			{
				if (reducida[j][i] > 0) { //SI ES -1 NO HAY VERTICE Y NO HACE FALTA CAMBIAR SU VALOR
					reducida[j][i] -= minC[i];
				}
			}
		}
	}


	for (int i = 0; i < n; i++) //CALCULO DEL COSTE REDUCIDO
	{
		coste_r += minC[i] + minF[i];
	}
	return reducida;
}

vector<int> tspMejor(Grafo g, int& coste_mejor) {
	//INICIALIZACION
	auto start = high_resolution_clock::now();
	int nodosExpl = 0;
	int coste_r = 0;
	int min = g.getMin(), num = g.numVer();
	vector<int> sol;
	nodo X;
	nodo Y = newNodo(num);//{new int[g.numVer()], 0, 0, num * min, new bool[g.numVer()]};
	Y.k = 0;
	Y.coste = 0;
	Y.reducida = reducir(g.getAdj(), num, coste_r, -1, -1);
	Y.coste_estimado = coste_r;
	ColaPrioridad C;
	Y.sol[0] = 0;
	Y.usado[0] = true;
	C.insertar(Y);
	coste_mejor = INT_MAX;
	//////////////////////
	int nuevo_r;
	while (!C.esVacia() && C.minimo().coste_estimado < coste_mejor)
	{
		
		nodosExpl++;
		Y = C.minimo();
		C.elimMin();
		X.k = Y.k + 1;
		X.sol = Y.sol;
		X.usado = Y.usado;
		
		int anterior = X.sol[X.k - 1];
		for (int v = 1; v < num; v++)
		{
			coste_r = 0;
			if (!X.usado[v] && g.existeArista(anterior, v)) { //factibilidad
				X.sol[X.k] = v; X.usado[v] = true;
				X.reducida = reducir(Y.reducida, num, coste_r, anterior, v);
				X.coste = Y.coste + g.valor(anterior, v);
				if (X.k == num - 1) {
					if (g.existeArista(X.sol[num - 1], 0) && X.coste + g.valor(X.sol[num - 1], 0) < coste_mejor) {
						sol = X.sol; coste_mejor = X.coste + g.valor(X.sol[num - 1], 0);
					}
				}
				else
				{
					X.coste_estimado = Y.reducida[anterior][v] + Y.coste_estimado + coste_r;
					if (X.coste_estimado < coste_mejor) C.insertar(X);
				}
				X.usado[v] = false;
			}
		}
	}
	cout << "NODOS EXPLORADOS CON COTA 3: " << nodosExpl << "\n";
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "TIEMPO TOTAL: " << duration.count() << " microsegundos\n";
	cout << "TIEMPO MEDIO POR NODO: " << duration.count() / nodosExpl << " microsegundos\n";
	return sol;
}

vector<int> tspFactible(Grafo g, int& coste_mejor) {
	//INICIALIZACION
	auto start = high_resolution_clock::now();
	int nodosExpl = 0;
	int min = g.getMin(), num = g.numVer();
	vector<int> sol;
	nodo X;
	nodo Y = newNodo(num);//{new int[g.numVer()], 0, 0, num * min, new bool[g.numVer()]};
	Y.k = 0;
	Y.coste = 0;
	Y.coste_estimado = num * min;
	ColaPrioridad C;
	Y.sol[0] = 0;
	Y.usado[0] = true;
	C.insertar(Y);
	coste_mejor = INT_MAX;
	//////////////////////

	while (!C.esVacia())
	{
		nodosExpl++;
		Y = C.minimo();
		C.elimMin();
		X.k = Y.k + 1;
		X.sol = Y.sol;
		X.usado = Y.usado;
		int anterior = X.sol[X.k - 1];
		for (int v = 1; v < num; v++)
		{
			if (!X.usado[v] && g.existeArista(anterior, v)) { //factibilidad
				
				X.sol[X.k] = v; X.usado[v] = true;
				X.coste = Y.coste + g.valor(anterior, v);
				if (X.k == num - 1) {
					if (g.existeArista(X.sol[num - 1], 0) && X.coste + g.valor(X.sol[num - 1], 0) < coste_mejor) {
						sol = X.sol; coste_mejor = X.coste + g.valor(X.sol[num - 1], 0);
					}
				}
				else
				{
					C.insertar(X); //lo añadimos igualmente, con que sea factible nos vale en este caso
				}
				X.usado[v] = false;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "NODOS EXPLORADOS CON FACTIBILIDAD: " << nodosExpl << "\n";
	cout << "TIEMPO TOTAL: " << duration.count() << " microsegundos\n";
	cout << "TIEMPO MEDIO POR NODO: " << duration.count() / nodosExpl << " microsegundos\n";
	return sol;
}


vector<int> tsp(Grafo g, int& coste_mejor) {
	//INICIALIZACION
	auto start = high_resolution_clock::now();
	int nodosExpl = 0;
	int min = g.getMin(), num = g.numVer();
	vector<int> sol;
	nodo X;
	nodo Y = newNodo(num);
	Y.k = 0;
	Y.coste = 0;
	Y.coste_estimado = num * min;
	ColaPrioridad C;
	Y.sol[0] = 0;
	Y.usado[0] = true;
	C.insertar(Y);
	coste_mejor = INT_MAX;
	//////////////////////

	while (!C.esVacia() && C.minimo().coste_estimado < coste_mejor)
	{
		nodosExpl++;
		Y = C.minimo();
		C.elimMin();
		X.k = Y.k + 1;
		X.sol = Y.sol;
		X.usado = Y.usado;
		int anterior = X.sol[X.k - 1];
		for (int v = 1; v < num; v++)
		{
			
			if (!X.usado[v] && g.existeArista(anterior, v)) { //factibilidad
				X.sol[X.k] = v; X.usado[v] = true;
				X.coste = Y.coste + g.valor(anterior, v);
				if (X.k == num - 1) {
					if (g.existeArista(X.sol[num - 1], 0) && X.coste + g.valor(X.sol[num - 1], 0) < coste_mejor) {
						sol = X.sol; coste_mejor = X.coste + g.valor(X.sol[num - 1], 0);
					}
				}
				else
				{
					X.coste_estimado = X.coste + (num - X.k) * min;
					if (X.coste_estimado < coste_mejor) C.insertar(X);
				}
				X.usado[v] = false;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "NODOS EXPLORADOS CON COTA 1: " << nodosExpl << "\n";
	cout << "TIEMPO TOTAL: " << duration.count() << " microsegundos\n";
	cout << "TIEMPO MEDIO POR NODO: " << duration.count() / nodosExpl << " microsegundos\n";
	return sol;
}

int aristasUsadas(vector<int> sol, int v, int k, int &index) { //llamar solo cuando estamos explorando los hijos del vertice raiz

	for (int i = 1; i < k; i++) //si no es un extremo de la solucion usa dos aristas
	{
		if (sol[i] == v) {
			index = i;
			return 2;
		}
	}
	return 1;
}

bool tieneSig(vector<int> sol, int v) { //para cuando sabemos que solo una arista del camino esta unida a v
	if (sol[0] == v) return true; //si se cumple su siguiente esta en posicion 1
	else return false; //si no estara en posicion size - 2
}

int getEstimado(Grafo g, vector<int> sol, vector<bool> usado, int n, int k) {

	int sum = 0;

	for (int i = 0; i < n; i++)
	{
		if (!usado[i]) { //no ha sido usado -> usamos las dos aristas con menor valor
			sum += g.suma2Min(i);
		}
		else
		{
			int idx;
			if (aristasUsadas(sol, i, k, idx) == 2) { //usa dos aristas, cogemos sus valores
				
				sum += g.valor(sol[idx - 1], sol[idx]) + g.valor(sol[idx], sol[idx + 1]); 
			}
			else {
				int c;
				//ver su conexion
				if (tieneSig(sol, i)) c = g.valor(sol[0], sol[1]); //arista entre sol[0] y sol[1]
				else c = g.valor(sol[k], sol[k - 1]); //arista entre los dos ultimos
				sum += g.sumaMin(i, c) + c;
			}
		}
	}
	return sum / 2 + (sum % 2 != 0);
}

vector<int> tspMedio(Grafo g, int& coste_mejor) {
	//INICIALIZACION
	auto start = high_resolution_clock::now();
	int nodosExpl = 0;
	int min = g.getMin(), num = g.numVer();
	vector<int> sol;
	nodo X;
	nodo Y = newNodo(num);
	Y.k = 0;
	Y.coste = 0;
	Y.coste_estimado = getEstimado(g, Y.sol, Y.usado, num, Y.k);
	ColaPrioridad C;
	Y.sol[0] = 0;
	Y.usado[0] = true;
	C.insertar(Y);
	coste_mejor = INT_MAX;
	//////////////////////

	while (!C.esVacia() && C.minimo().coste_estimado < coste_mejor)
	{
		nodosExpl++;
		Y = C.minimo();
		C.elimMin();
		X.k = Y.k + 1;
		X.sol = Y.sol;
		X.usado = Y.usado;
		int anterior = X.sol[X.k - 1];
		for (int v = 1; v < num; v++)
		{

			if (!X.usado[v] && g.existeArista(anterior, v)) { //factibilidad
				X.sol[X.k] = v; X.usado[v] = true;
				X.coste = Y.coste + g.valor(anterior, v);
				if (X.k == num - 1) {
					if (g.existeArista(X.sol[num - 1], 0) && X.coste + g.valor(X.sol[num - 1], 0) < coste_mejor) {
						sol = X.sol; coste_mejor = X.coste + g.valor(X.sol[num - 1], 0);
					}
				}
				else
				{
					X.coste_estimado = getEstimado(g, X.sol, X.usado, num, X.k);
					if (X.coste_estimado <= coste_mejor) C.insertar(X);
				}
				X.usado[v] = false;
			}
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "NODOS EXPLORADOS CON COTA 2: " << nodosExpl << "\n";
	cout << "TIEMPO TOTAL: " << duration.count() << " microsegundos\n";
	cout << "TIEMPO MEDIO POR NODO: " << duration.count() / nodosExpl << " microsegundos\n";
	return sol;
}

int main() {
	int numV, valor;
	ifstream file;
	file.open("grafo.txt");
	if (!file) {
		cout << "NO ES POSIBLE ABRIR EL ARCHIVO\n";
		exit(1);
	}
	file >> numV;
	Grafo g(numV);

	for (int i = 0; i < numV; i++)
	{
		for (int j = 0; j < numV; j++) //COMPROBAR SI ES INFINITO
		{
			file >> valor;
			g.addSaliente(i, j, valor); //CAMBIAR A -1 LOS PROPIOS (-1 es que no existe arista)
		}
	}

	//COMENTAR Y DESCOMENTAR LAS PARTES DEL PROGRAMA QUE QUIERAN USARSE, GRACIAS
	int coste_mejor;
	//int coste_mejor1, coste_mejor2, coste_mejor3;
	//vector<int> sol = tspFactible(g, coste_mejor);
	//vector<int> sol1 = tsp(g, coste_mejor1);
	//vector<int> sol2 = tspMedio(g, coste_mejor2);
	vector<int> sol = tspFactible(g, coste_mejor);
	if (sol.size() != numV) cout << "NO EXISTE SOLUCION\n";
	else
	{
		cout << "SOLUCION CON COTA FACTIBLE:\n";
		for (int i = 0; i < numV; i++) //igual hay que cambiar por vectores
		{
			cout << sol[i] << " -> ";
		}
		cout << "0" << endl;
		cout << "COSTE CON COTA FACTIBLE: " << coste_mejor << "\n";

		/*cout << "SOLUCION CON COTA 1:\n";
		for (int i = 0; i < numV; i++) //igual hay que cambiar por vectores
		{
			cout << sol1[i] << " -> ";
		}
		cout << "0" << endl;
		cout << "COSTE CON COTA 1: " << coste_mejor1 << "\n";*/

		/*cout << "SOLUCION CON COTA 2:\n";
		for (int i = 0; i < numV; i++) //igual hay que cambiar por vectores
		{
			cout << sol2[i] << " -> ";
		}
		cout << "0" << endl;
		cout << "COSTE CON COTA 2: " << coste_mejor2 << "\n";*/

		/*cout << "SOLUCION CON COTA 3:\n";
		for (int i = 0; i < numV; i++)
		{
			cout << sol3[i] << " -> ";
		}
		cout << "0" << endl;
		cout << "COSTE CON COTA 3: " << coste_mejor3 << "\n";*/
	}
	
	
	return 0;
}