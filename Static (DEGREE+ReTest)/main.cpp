#include <bits/stdc++.h>
#include <unordered_set>
using namespace std;
const unsigned ARRAY_SIZE_INCREMENT = 500000;
const unsigned INF = (unsigned)4000000000;

bool TESTING = true;
namespace TEST
{
	unsigned p0, iteration_time, phase_number;
}

inline unsigned max3(unsigned a, unsigned b, unsigned c) {
	a = (a > b) ? a : b;
	return (a > c) ? a : c;
}
double timer;
inline void timer_start() { timer = clock(); }
inline double timer_end()
{
	double t = (clock() - timer) / CLOCKS_PER_SEC;
	//out << (t = (clock() - timer) / CLOCKS_PER_SEC) << ',';
	return t;
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
	unordered_set<unsigned> nodes;
	unsigned p_upper, p_lower;

	unsigned pseudoarboricity;

	Graph() {}
	void read_edge(char* graph_name);

	void approximate_orientation();
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
		if (e[m].t1 % 5 > 3 || e[m].t2 % 5 > 3) continue;
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
void Graph::approximate_orientation()
{
	unsigned* nd = (unsigned*)malloc(n * sizeof(unsigned));
	unsigned* core = (unsigned*)malloc(n * sizeof(unsigned));
	vector<unordered_set<unsigned> > ud;
	unordered_set<unsigned> V;
	for (int i = 0; i < n; i++)
	{
		V.insert(i);
		int nowd = adj_length[i];
		nd[i] = nowd;
		if (nowd + 1 > ud.size())
			ud.resize(nowd + 1);
		ud[nowd].insert(i);
	}
	unsigned nowcore = 0, maxsize = ud.size(), corenum = 0;
	while (true)
	{
		while (nowcore < maxsize && ud[nowcore].empty())
			nowcore++;
		if (nowcore == maxsize)
			break;
		corenum = max(corenum, nowcore);
		auto it = ud[nowcore].begin();
		unsigned now = *it; ud[nowcore].erase(it);
		core[now] = corenum;
		for (unsigned i = 0; i < adj_length[now]; i++)
		{
			Edge& ne = e[adj[now][i]];
			unsigned tar = (ne.t1 == now) ? ne.t2 : ne.t1;
			if (V.find(tar) == V.end()) continue;
			ne.to = now;
			d[now]++;
			ud[nd[tar]].erase(tar);
			nd[tar]--;
			ud[nd[tar]].insert(tar);
		}
		V.erase(now);
		if (V.empty()) break;
		if (nowcore > 0) nowcore--;
	}
	p_upper = get_max_d(), p_lower = p_upper / 2;
	for (unsigned i = 0; i < n; i++)
		if (core[i] >= p_lower)
			nodes.insert(i);

	memset(adj_length, 0, n * sizeof(unsigned));
	for (unsigned i = 0; i < m; i++)
	{
		Edge& ne = e[i];
		if (nodes.find(ne.t1) == nodes.end() || nodes.find(ne.t2) == nodes.end()) continue;
		adj[ne.t1][adj_length[ne.t1]++] = adj[ne.t2][adj_length[ne.t2]++] = i;
	}
	free(nd);
	free(core);
}
unsigned Graph::get_max_d()
{
	unsigned maxd = 0;
	for (unsigned i = 0; i < n; i++)
		maxd = max(maxd, d[i]);
	return maxd;
}
void Graph::construct_adjacency_list()
{
	d = (unsigned*)malloc(n * sizeof(unsigned)); memset(d, 0, n * sizeof(unsigned));
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
	p_upper++;
	while (p_upper > p_lower)
	{
		test_value = (p_upper + p_lower) / 2;
		if (ReTest())
			p_upper = test_value;
		else
			p_lower = test_value + 1;
	}
	pseudoarboricity = p_lower;
	return;
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
		timer_start();
		G.approximate_orientation();
		runtime = timer_end();
		if (TESTING)
		{
			TEST::p0 = G.p_upper;
			printf("p0 = %u\n", TEST::p0);
			//printf("iteration_time = %u\n", TEST::iteration_time);
		}
		printf("Approximation time: %lf\n", runtime);

		timer_start();
		G.exact_orientation();
		runtime = timer_end();

		printf("Re-orientation time: %lf\n", runtime);

		printf("Pseudoarboricity: %u\n", G.pseudoarboricity);
	}
	return 0;
}
