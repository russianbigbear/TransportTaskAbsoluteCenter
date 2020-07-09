#include <iostream> 
#include <vector> 
#include <locale.h>
#include <fstream>
using namespace std;

int N; // ���-�� ������
const int INF = 999; // �������������
int h = 1; //��� (��������)

struct step {
	int ep;					// ���������� �� ������ �������, �� ����� "�������"
	int Xi;					// ����� ������ �������
	int Xj;					// ����� ������ �������
	int max_of_the_min;		// ������������ �� ����������� ���������

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

/*���� ����� �� �����, ��������� ��� ������ ������� */
void input_matrix(ifstream &in, int **graph) {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> graph[i][j];
}

/* ������ ����� c ��c�� ����(����� � graphic.png) */
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

/*������� �� ������� � ��������� �����*/
void matrix_to_vector(int **graph, vector<vector<int>>& graph_vector) {
	for (int i = 0; i < N; i++) {
		graph_vector.push_back(vector<int>());
		for (int j = 0; j < N; j++)
			graph_vector[i].push_back(graph[i][j]);
	}
}

/*
����� ���������� �����, ����� ��������� Xi � Xj
���� ���������� ���� ���������� ��� ������ �������, ������� �� ����:
������� ���������, ������� ���������� �����, ������� �������������� ������, ��������� �������, ���������� �������, ����� ����.
*/
void calc_short_way(vector<vector<int>>& vector_graph, vector<vector<int>>& short_way, vector<bool>& used, int start, int last, int sizePath) {
	for (int i = 0; i < N; i++) {
		// ���� ������� �� ��������������//
		if (!used[i]  && vector_graph[last][i] != 0) {												
			//���������� �� ��������� �� ������� = ���������� �� ���������� �� ������� + ���� �� ����������//
			//���� ���� �� ������� �� ��������� ������, ��� ������ ���� �� ������� �� ���������, �� ������ �������� � �������//
			if (short_way[start][i] > (vector_graph[last][i] + sizePath))									
				short_way[start][i] = vector_graph[last][i] + sizePath;

			used[i] = true;																					//�������� ������� ��� ��������������
			calc_short_way(vector_graph, short_way, used, start, i, (sizePath + vector_graph[last][i]));	//������ ���� � ������ �������, ����� ��� ���� ����������
			used[i] = false;																				//��� ������ �� �������� ������� ���� ���������������� �������
		}
	}
}

/*
����� ���� �������������, ��� ������������� �����, � ����� = h
������������� ������ ����� �� �����������.
��������� ����� � ����� h.
�������� ���� ����� {I, J}. �������� ��������� �� ������� I � ������� J � ����� h. ��������� ���������� �� I �� ������� ����� ��� ep.
�������� ��� ���� ����������� ���������� �� ������� ����� �� ���� ��������� (����� ������� I ��� J), ��������� ����������� ����� ������� ���������� �����.
���������� ��� ����������.
*/
void distribution(int I, int J, vector<vector<int>>& vector_graph, vector<vector<int>>& short_way, vector<vector<step*>>& all_steps) {
	int size_edge = vector_graph[I][J];													// ��� ����� {I,J}
	vector<step*> steps_edge;															// ������ �����

	for (int e = 1; e < vector_graph[I][J]; e = e + h) {								// � ����� �, ���� �� ��������� �������, � ������������� ����������
		step* first = new step();														// �� ����� �� �����, �� ���� ������ ������
		vector<int> vector_I(N); vector<int> vector_J(N);								

		for (int j = 0; j < N; j++) {
			vector_I[j] = short_way[I][j] + e;										// ���������� �� ����� �� ���� ������ ������, ������� ����� �1
			vector_J[j] = short_way[J][j] + (size_edge - e);						// ���������� �� ����� �� ���� ������ ������, ������� ����� �2
		}

		/*���������� ���������� ������ � ���������*/
		first->ep = e;
		first->Xi = I; first->Xj = J;
		first->vector_Xi = vector_I;
		first->vector_Xj = vector_J;
		first->set_all();
		steps_edge.push_back(first);				// ���������� ���������, � ������ ����� �� ����� (��� ������ �� ��� ���������� ����� �������������)
	}
	all_steps.push_back(steps_edge);
}

/*����� ����������� ������*/
step find_center(vector<vector<int>> &matrix, vector<vector<int>> &shortWay) {
	vector<vector<step*>> all_Steps;
	/*������������ ������������� ��� ���� ����*/
	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			if (matrix[i][j] != 0) {
				distribution(i, j, matrix, shortWay, all_Steps);		// ��� ������� �����, ������� ��� ����� ������������� � ����� �
			}
		}
	}

	step ans;
	/*��� ������������� ����������, ������ ������ ����������� �������������*/
	ans.Xi = (*all_Steps[0][0]).Xi;
	ans.Xj = (*all_Steps[0][0]).Xj;
	ans.ep = (*all_Steps[0][0]).ep;
	ans.max_of_the_min = (*all_Steps[0][0]).max_of_the_min;			//������ ����������� �������������
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
		cout << "������������� ���� �����? (1-��, 0 -���)." << endl;
		cin >> ans;
		if (ans == 1) system("input.txt");
	} while (ans != 1 && ans != 0);

	ifs.open("input.txt");
	ifs >> N;						//����� �� ����� ���������� ������

	/*�������� ������ ��� ������� ���������*/
	int **graph = new int*[N];		
	for (int i = 0; i < N; i++) {
		graph[i] = new int[N];
		for (int j = 0; j < N; j++)
			graph[i][j] = 0;
	}
	input_matrix(ifs, graph);

	do {
		cout << "������� ����? (1-��, 0 -���)." << endl;
		cin >> ans;
		if (ans == 1) print_graph(graph);
	} while (ans != 1 && ans != 0);

	ifs.close();

	vector<vector<int>> graph_vector;
	matrix_to_vector(graph, graph_vector);


	/*��������� ������� ���������� �����*/
	vector<vector<int>> short_way;
	for (int i = 0; i < N; i++) {
		short_way.push_back(vector<int>());
		for (int j = 0; j < N; j++) 
			short_way[i].push_back(INF);	
	}

	/*����� ���������� �����, ����� ��������� Xi � Xj*/
	vector<bool> used;
	for (int i = 0; i < N; i++) used.push_back(false); // �������� ������ �������������� ������

	/*�����*/
	for (int i = 0; i < N; i++) {
		used[i] = true;												
		calc_short_way(graph_vector, short_way, used, i, i, 0);
		used[i] = false;
	}
	for (int i = 0; i < N; i++) short_way[i][i] = 0; //������� �������� ������� ���������� ����� �� ���������

	/*����� ����������� ������ � ��������� h*/
	step answer = find_center(graph_vector, short_way);

	ofs.open("output.txt");
	ofs << "���������� ����� ����� ����� ��������� '" << answer.Xi + 1 << "' � '" << answer.Xj + 1 << "' �� ���������� " << answer.ep << " �� ������� '" << answer.Xi + 1 << "'." ;
	ofs.close();

	/*�������� ����������*/
	do {
		cout << "������� ���������? (1-�������, 0-��������)." << endl;
		cin >> ans;
		if (ans == 1) system("output.txt");
	} while (ans != 1 && ans != 0);

	return 0;
}
