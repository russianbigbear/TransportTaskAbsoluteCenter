#include <iostream> 
#include <vector> 
#include <locale.h>
#include <fstream>
using namespace std;

int N; // кол-во вершин
const int INF = 999; // бесконечность
int h = 1; //шаг (точность)

struct step {
	int ep;					// расстояние от первой вершины, до точки "деления"
	int Xi;					// номер первой вершины
	int Xj;					// номер второй вершины
	int max_of_the_min;		// максимальное из минимальных растояний

	vector<int> min;		
	vector<int> vector_Xi;
	vector<int> vector_Xj;

	void set_min() {
		for (unsigned i = 0; i < vector_Xi.size(); i++)
			if (vector_Xi[i] >= vector_Xj[i]) min.push_back(vector_Xj[i]);
			else min.push_back(vector_Xi[i]);
	}

	void set_max_of_the_min() {
		max_of_the_min = min[0];
		for (unsigned i = 0; i < min.size(); i++)
			if (max_of_the_min < min[i])
				max_of_the_min = min[i];
	}

	void set_all() {
		set_min();
		set_max_of_the_min();
	}
};

/*ввод графа из файла, заданного при помощи матрицы */
void input_matrix(ifstream &in, int **graph) {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> graph[i][j];
}

/* печать графа c веcом рёбер(вывод в graphic.png) */
void print_graph(int **graph) {
	ofstream out;
	out.open("graphic.dot");
	out << "graph G{" << "\n";
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (graph[i][j] != 0 && i < j)
				out << i + 1 << " -- " << j + 1 << "[label = \" " << graph[i][j] << "\"];\n";
	out << "}";
	out.close();
	system("dot -Tpng graphic.dot -o graphic.png ");
	system("graphic.png");
}

/*перевод из массива в векторную форму*/
void matrix_to_vector(int **graph, vector<vector<int>>& graph_vector) {
	for (int i = 0; i < N; i++) {
		graph_vector.push_back(vector<int>());
		for (int j = 0; j < N; j++)
			graph_vector[i].push_back(graph[i][j]);
	}
}

/*
Поиск кратчайших путей, между вершинами Xi и Xj
Ищем кратчайшие пути рекурсивно для каждой вершины, подавая на вход:
матрицу смежности, матрицу кратчайших путей, матрицу использованных вершин, стартовую вершину, предыдущую вершину, длину пути.
*/
void calc_short_way(vector<vector<int>>& vector_graph, vector<vector<int>>& short_way, vector<bool>& used, int start, int last, int sizePath) {
	for (int i = 0; i < N; i++) {
		// если вершина не использовалась//
		if (!used[i]  && vector_graph[last][i] != 0) {												
			//расстояние от стартовой до текущей = расстояние из предыдущей до текущей + путь до предыдущей//
			//если путь до текущей из стартовой короче, чем старый путь из текущей до стартовой, то меняем занчение в матрице//
			if (short_way[start][i] > (vector_graph[last][i] + sizePath))									
				short_way[start][i] = vector_graph[last][i] + sizePath;

			used[i] = true;																					//помечаем вершину как использованную
			calc_short_way(vector_graph, short_way, used, start, i, (sizePath + vector_graph[last][i]));	//теперь идем в другую вершину, Помня что путь увеличился
			used[i] = false;																				//при выходе из рекурсии снимаем флаг использованности вершины
		}
	}
}

/*
Поиск всех распределений, для определенного ребра, с шагом = h
Рассматриваем каждое ребро по отдельности.
Разделяем ребра с шагом h.
Допустим есть ребро {I, J}. Начинаем двигаться от вершины I к вершине J с шагом h. Обозначим расстояние от I до текущей точки как ep.
Вычисляя при этом минимальное расстояние от текущей точки до всех остальных (через вершины I или J), используя построенную ранее матрицу кратчайших путей.
Запоминаем эти расстояния.
*/
void distribution(int I, int J, vector<vector<int>>& vector_graph, vector<vector<int>>& short_way, vector<vector<step*>>& all_steps) {
	int size_edge = vector_graph[I][J];													// вес ребра {I,J}
	vector<step*> steps_edge;															// вектор шагов

	for (int e = 1; e < vector_graph[I][J]; e = e + h) {								// с шагом Н, идем от начальной вершине, и пересчитываем расстояния
		step* first = new step();														// от точки на ребре, до всех других вершин
		vector<int> vector_I(N); vector<int> vector_J(N);								

		for (int j = 0; j < N; j++) {
			vector_I[j] = short_way[I][j] + e;										// расстояние от точки до всех других вершин, проходя через В1
			vector_J[j] = short_way[J][j] + (size_edge - e);						// расстояние от точки до всех других вершин, проходя через В2
		}

		/*записываем полученные данные в структуру*/
		first->ep = e;
		first->Xi = I; first->Xj = J;
		first->vector_Xi = vector_I;
		first->vector_Xj = vector_J;
		first->set_all();
		steps_edge.push_back(first);				// записываем структуру, в список точек на ребре (для каждой из них рассчитано число РАСПРЕДЕЛЕНИЯ)
	}
	all_steps.push_back(steps_edge);
}

/*поиск абсолютного центра*/
step find_center(vector<vector<int>> &matrix, vector<vector<int>> &shortWay) {
	vector<vector<step*>> all_Steps;
	/*подсчитываем распределения для всех рёбер*/
	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			if (matrix[i][j] != 0) {
				distribution(i, j, matrix, shortWay, all_Steps);		// Для каждого ребра, находим все точки распределения с шагом Н
			}
		}
	}

	step ans;
	/*все распределения подсчитаны, теперь найдем минимальное распределение*/
	ans.Xi = (*all_Steps[0][0]).Xi;
	ans.Xj = (*all_Steps[0][0]).Xj;
	ans.ep = (*all_Steps[0][0]).ep;
	ans.max_of_the_min = (*all_Steps[0][0]).max_of_the_min;			//первое минимальное распределение
	for (unsigned i = 0; i < all_Steps.size(); i++) {
		for (unsigned j = 0; j < all_Steps[i].size(); j++) {
			if (ans.max_of_the_min >= (*all_Steps[i][j]).max_of_the_min) {
				ans.Xi = (*all_Steps[i][j]).Xi;
				ans.Xj = (*all_Steps[i][j]).Xj;
				ans.ep = (*all_Steps[i][j]).ep;
				ans.max_of_the_min = (*all_Steps[i][j]).max_of_the_min;
			}
		}
	}
	return ans;
}

int main()
{
	setlocale(LC_ALL, "rus");
	ifstream ifs; ofstream ofs;

	int ans;
	do {
		cout << "Редактировать файл ввода? (1-да, 0 -нет)." << endl;
		cin >> ans;
		if (ans == 1) system("input.txt");
	} while (ans != 1 && ans != 0);

	ifs.open("input.txt");
	ifs >> N;						//вывод из файла количества вершин

	/*зануляем массив для матрицы смежности*/
	int **graph = new int*[N];		
	for (int i = 0; i < N; i++) {
		graph[i] = new int[N];
		for (int j = 0; j < N; j++)
			graph[i][j] = 0;
	}
	input_matrix(ifs, graph);

	do {
		cout << "Вывести граф? (1-да, 0 -нет)." << endl;
		cin >> ans;
		if (ans == 1) print_graph(graph);
	} while (ans != 1 && ans != 0);

	ifs.close();

	vector<vector<int>> graph_vector;
	matrix_to_vector(graph, graph_vector);


	/*зануление матрицы кратчайших путей*/
	vector<vector<int>> short_way;
	for (int i = 0; i < N; i++) {
		short_way.push_back(vector<int>());
		for (int j = 0; j < N; j++) 
			short_way[i].push_back(INF);	
	}

	/*Поиск кратчайших путей, между вершинами Xi и Xj*/
	vector<bool> used;
	for (int i = 0; i < N; i++) used.push_back(false); // зануляем вектор использованных вершин

	/*поиск*/
	for (int i = 0; i < N; i++) {
		used[i] = true;												
		calc_short_way(graph_vector, short_way, used, i, i, 0);
		used[i] = false;
	}
	for (int i = 0; i < N; i++) short_way[i][i] = 0; //занулим значение матрицы кратчайших путей по диагонали

	/*поиск абсолютного центра с точностью h*/
	step answer = find_center(graph_vector, short_way);

	ofs.open("output.txt");
	ofs << "Абсолютный центр лежит между вершинами '" << answer.Xi + 1 << "' и '" << answer.Xj + 1 << "' на расстоянии " << answer.ep << " от вершины '" << answer.Xi + 1 << "'." ;
	ofs.close();

	/*открытие результата*/
	do {
		cout << "Вывести результат? (1-вывести, 0-выводить)." << endl;
		cin >> ans;
		if (ans == 1) system("output.txt");
	} while (ans != 1 && ans != 0);

	return 0;
}
