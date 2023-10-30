#include <bits/stdc++.h>
#include <unordered_set>
#include <chrono>
using namespace std;
const unsigned ARRAY_SIZE_INCREMENT = 500000;
const unsigned INF = (unsigned)4000000000;
struct timespec start, finish;
double elapsed;

bool TESTING = true;
namespace TEST
{
	unsigned p0, iteration_time, phase_number;
}

inline unsigned max3(unsigned a, unsigned b, unsigned c) {
	a = (a > b) ? a : b;
	return (a > c) ? a : c;
}

struct Edge
{
	unsigned t1, t2, to;
};
struct Graph
{
	Edge* e;
	unsigned* d;
	unsigned n, m;
	unsigned** adj;
	unsigned* adj_length;

	unsigned pseudoarboricity;

	Graph() {}
	void read_edge(char* graph_name);
	void initiate();

	void approximate_orientation();
	void iterate();
	unsigned get_max_d();

	void construct_adjacency_list();
	void exact_orientation();
	bool ReTest();
	unsigned* cur, * dist, * p;
	unsigned test_value;
	bool DinicBFS(unordered_set<unsigned>& nodes_to_decrease);
	bool DinicDFS(unsigned x);

	~Graph()
	{
		free(e); free(d); free(adj); free(adj_length); free(cur); free(dist); free(p);
	}
};
struct Queue
{
	unsigned* node;
	unsigned head, tail;
	Queue() {}
	Queue(unsigned size)
	{
		node = (unsigned*)malloc(size * sizeof(unsigned));
		head = tail = 0;
	}
	~Queue()
	{
		free(node);
	}
	bool empty() { return head == tail; }
	unsigned pop() { return node[head++]; }
	void push(unsigned x) { node[tail++] = x; }
};

void Graph::read_edge(char* graph_name)
{
	char prefix[100] = "../Graphs/";
	strcat(prefix, graph_name);
	FILE* dataset, * file;
	file = fopen(prefix, "r");

	unsigned Emax = ARRAY_SIZE_INCREMENT;
	n = m = 0;
	e = (Edge*)malloc(Emax * sizeof(Edge));

	char line[200];
	while (fgets(line, 200, file))
	{
		unsigned i = 0;
		e[m].t1 = e[m].t2 = 0;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') e[m].t1 = e[m].t1 * 10 + line[i] - '0', i++;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') e[m].t2 = e[m].t2 * 10 + line[i] - '0', i++;
		n = max3(n, e[m].t1, e[m].t2);
		m++;
		if (m == Emax)
		{
			Emax += ARRAY_SIZE_INCREMENT;
			e = (Edge*)realloc(e, Emax * sizeof(Edge));
		}
	}

	fclose(file);
	n++;
	e = (Edge*)realloc(e, m * sizeof(Edge));
	printf("Reading is ok.\n");
	printf("n = %u, m = %u\n", n, m);
	return;
}
void Graph::initiate()
{
	d = (unsigned*)malloc(n * sizeof(unsigned));
	for (long long i = 0; i < n; i++)
		d[i] = 0;
	for (long long i = 0; i < m; i++)
	{
		if (d[e[i].t1] < d[e[i].t2])
		{
			e[i].to = e[i].t1;
			d[e[i].t1]++;
		}
		else
		{
			e[i].to = e[i].t2;
			d[e[i].t2]++;
		}
	}
}
void Graph::approximate_orientation()
{
	TEST::iteration_time = 0;
	unsigned pre_max_d = 0, now_max_d;
	while (true)
	{
		TEST::iteration_time++;
		iterate();
		now_max_d = get_max_d();
		//printf("%u\n", now_max_d);
		if (pre_max_d == now_max_d)
			break;
		pre_max_d = now_max_d;
	}
	return;
}
void Graph::iterate()
{
	unsigned from;
	for (int x = 0; x < n; x++)
	{
		for (unsigned i = 0; i < adj_length[x]; i++)
		{
			Edge& ne = e[adj[x][i]];
			unsigned from = ne.to == ne.t1 ? ne.t2 : ne.t1;
			if (d[ne.to] >= d[from] + 2)
			{
				d[ne.to]--;
				d[from]++;
				ne.to = from;
			}
		}
	}
}
/*
unsigned* max_d_this_thread = (unsigned*)malloc(threads_num * sizeof(unsigned));
unsigned Graph::get_max_d()
{
	unsigned maxd = 0;
	unsigned block_size = n / threads_num;
	memset(max_d_this_thread, 0, threads_num * sizeof(unsigned));
#pragma omp for
	for (int i = 0; i < n; i++)
		max_d_this_thread[i / block_size] = max(max_d_this_thread[i / block_size], d[i]);
	for (unsigned i = 0; i < threads_num; i++)
		maxd = max(maxd, max_d_this_thread[i]);
	return maxd;
}*/

unsigned Graph::get_max_d()
{
	unsigned maxd = 0;
	for (int i = 0; i < n; i++)
		maxd = maxd > d[i] ? maxd : d[i];
	return maxd;
}
void Graph::construct_adjacency_list()
{
	adj = (unsigned**)malloc(n * sizeof(unsigned*));
	adj_length = (unsigned*)malloc(n * sizeof(unsigned));
	unsigned* now_edge_num = (unsigned*)malloc(n * sizeof(unsigned));
	for (unsigned i = 0; i < n; i++) adj_length[i] = now_edge_num[i] = 0;
	for (unsigned i = 0; i < m; i++) { adj_length[e[i].t1]++, adj_length[e[i].t2]++; }
	for (unsigned i = 0; i < n; i++) adj[i] = (unsigned*)malloc(adj_length[i] * sizeof(unsigned));
	for (unsigned i = 0; i < m; i++)
	{
		unsigned t1 = e[i].t1, t2 = e[i].t2;
		adj[t1][now_edge_num[t1]++] = i;
		adj[t2][now_edge_num[t2]++] = i;
	}
	free(now_edge_num);
	return;
}
void Graph::exact_orientation()
{
	p = (unsigned*)malloc(n * sizeof(unsigned));
	while (true)
	{
		unsigned max_d = get_max_d();
		test_value = max_d - 1;
		if (!ReTest())
		{
			pseudoarboricity = max_d;
			break;
		}
	}
}
bool Graph::ReTest()
{
	unordered_set<unsigned> nodes_to_decrease;
	dist = (unsigned*)malloc(n * sizeof(unsigned));
	cur = (unsigned*)malloc(n * sizeof(unsigned));
	for (unsigned i = 0; i < n; i++)
	{
		if (d[i] > test_value)
		{
			nodes_to_decrease.insert(i);
		}
	}
	while (DinicBFS(nodes_to_decrease))
	{
		memset(cur, 0, n * sizeof(unsigned));
		vector<unsigned> to_erase;
		for (auto i : nodes_to_decrease)
		{
			while (d[i] > test_value)
			{
				p[i] = INF + 1;
				if (!DinicDFS(i))
					break;
			}
			if (d[i] <= test_value)
				to_erase.push_back(i);
		}
		for (auto i : to_erase)
			nodes_to_decrease.erase(i);
	}
	return nodes_to_decrease.empty();
}
bool Graph::DinicBFS(unordered_set<unsigned>& nodes_to_decrease)
{
	if (TESTING) TEST::phase_number++;
	if (nodes_to_decrease.empty()) return false;
	bool can_reach_low_indegree = false;
	Queue Q(n);
	for (unsigned i = 0; i < n; i++)
		dist[i] = INF;
	for (auto i : nodes_to_decrease)
	{
		Q.push(i);
		dist[i] = 0;
	}
	while (!Q.empty())
	{
		unsigned x = Q.pop();
		for (unsigned i = 0; i < adj_length[x]; i++)
		{
			Edge& ne = e[adj[x][i]];
			if (ne.to != x) continue;
			unsigned from = ne.to == ne.t1 ? ne.t2 : ne.t1;
			if (dist[from] != INF) continue;
			dist[from] = dist[x] + 1;
			if (d[from] < test_value) can_reach_low_indegree = true;
			Q.push(from);
		}
	}
	return can_reach_low_indegree;
}
bool Graph::DinicDFS(unsigned x)
{
	if (d[x] < test_value)
	{
		d[e[p[x]].to]--;
		d[x]++;
		e[p[x]].to = x;
		return true;
	}
	for (unsigned& i = cur[x]; i < adj_length[x]; i++)
	{
		Edge& ne = e[adj[x][i]];
		if (ne.to != x) continue;
		unsigned from = ne.to == ne.t1 ? ne.t2 : ne.t1;
		if (dist[from] != dist[x] + 1) continue;
		p[from] = adj[x][i];
		if (DinicDFS(from))
		{
			if (p[x] >= INF) return true;
			d[e[p[x]].to]--;
			d[x]++;
			e[p[x]].to = x;
			return true;
		}
	}
	return false;
}

int main()
{
	FILE* dataset = fopen("./dataset.txt", "r");
	char graph_name[100];
	double runtime;
	while (fscanf(dataset, "%s", &graph_name) == 1)
	{
		if (TESTING) { TEST::iteration_time = 0; TEST::phase_number = 0; }

		printf("!!!!!Now processing graph: %s\n", graph_name);
		Graph G;
		G.read_edge(graph_name);
		G.construct_adjacency_list();

		auto start = clock();
		G.initiate();
		G.approximate_orientation();
		auto stop = clock();
		runtime = stop - start;

		if (TESTING)
		{
			TEST::p0 = G.get_max_d();
			printf("p0 = %u\n", TEST::p0);
			printf("Number of iterations = %u\n", TEST::iteration_time);
		}
		printf("Approximation time: %.2lf\n", runtime / CLOCKS_PER_SEC);

		start = clock();
		G.exact_orientation();
		stop = clock();
		runtime = stop - start;

		printf("Re-orientation time: %.2lf\n", runtime / CLOCKS_PER_SEC);

		printf("Pseudoarboricity: %u\n", G.pseudoarboricity);
	}
	return 0;
}
