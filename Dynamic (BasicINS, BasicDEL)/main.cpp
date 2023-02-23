#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>
typedef long long ll;
using namespace std;

string prefix("../Graphs/");
ifstream in, dataset("dataset.txt");
ofstream out("output.csv");

struct Edge
{
	int from, to;
	Edge(int t1, int t2) { from = t1, to = t2; }
	void re() { swap(from, to); }
};

struct Edge_hash
{
	size_t operator()(const struct Edge& ne) const
	{
		return ne.from + ne.to;
	}
};

struct DEdge
{
	int from, to, cap, flow;
};

class Graph
{
public:
	int sz;
	int PA;
	int sum;
	int rd_PA;
	int inf = 100000000;
	vector<unordered_set<Edge, Edge_hash> > e;
	vector<int> rd;
	unordered_set<int> node;
	Graph() { sz = PA = sum = rd_PA = 0; };
	void resize(int n);
	void push_edge(int u, int v);
	void pull_edge(int u, int v);
	int search(int u, int v);
	void insert(int u, int v);
	void insert2(int u, int v);
	void delet(int u, int v);
	void delet2(int u, int v);
	void re(int u, int v);
	void display();
	void depict();
	bool check();
	void PA_decrease();
	bool ReTest(int testd);

	void input_optimal();
	void generate_insert(); //generate the edges to insert and delete
	int insert_num = 10000; vector<Edge> pend_insert;
	int count_edge();

	int n, m, s, t;
	vector<DEdge> De;
	vector<vector<int> > DG;
	vector<int> d, cur;
	vector<bool> vis;
	void make_edge(int from, int to, int cap);
	bool dinic_BFS();
	int dinic_DFS(int x, int a);
};

double timer, sumtime = 0.0, inserttime, deletetime, temtime;
int randnumber;
void timer_start(); double timer_end();
int dotime = 5; //repeat times of the experiments (to reduce the error)

Graph G, Gori, Gempty;
void solve(string input)
{
	G = Gori;
	sumtime = 0.0;
	G.generate_insert();

	//Delete
	sumtime = 0.0;
	timer_start();
	for (auto nowedge : G.pend_insert)
		G.delet(nowedge.from, nowedge.to);
	temtime = timer_end();
	deletetime += temtime;

  G.check();
	reverse(G.pend_insert.begin(), G.pend_insert.end());

	//Insert
	timer_start();
	for (auto nowedge : G.pend_insert)
		G.insert(nowedge.from, nowedge.to);
	temtime = timer_end();
	inserttime += temtime;

  G.check();

	in.close();
	in.clear();
	return;
}
void set_ori(string input)
{
	in.open(prefix + input);
	Gori = Gempty;
	inserttime = deletetime = 0.0;
	Gori.input_optimal();
	out << input << ',';

	out << Gori.node.size() << ',';
	out << Gori.count_edge() << ',';
	if (!Gori.check())
	{
		cout << "ERROR!! Gori is not optimal!!\n";
		assert(0);
	}
}
int main()
{
	out << "graph,|V|,|E|,insertion time,deletion time\n";
	string input;
	srand((unsigned)time(NULL));
	while (dataset >> input)
	{
		set_ori(input);
		cout << input << ": ";
		for (int i = 1; i <= dotime; i++)
		{
			randnumber = rand();
			solve(input);
		}
    out << inserttime / dotime << ',' << deletetime / dotime << '\n';
		cout << "finished!\n";
	}
	return 0;
}

char line[1005];
void Graph::input_optimal()
{
	int u, v;
	while (in.getline(line, 1000))
	{
		u = 0, v = 0;
		int i = 0;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') u = u * 10 + line[i] - '0', i++;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') v = v * 10 + line[i] - '0', i++;
		if (max(u, v) > sz)
			resize(max(u, v));
		node.insert(u);
		node.insert(v);
		push_edge(u, v);
	}
	for (auto i : node)
		PA = max(PA, rd[i]);
	for (auto i : node)
		rd_PA += (rd[i] == PA);
}

vector<Edge> temvec;
unordered_set<Edge, Edge_hash> temstore;
void Graph::generate_insert()
{
	temstore.clear();
	temvec.clear();
	srand(randnumber);
	int temmod = rand() % 10 + 1;
	for (int i = 0; i < sz; i += temmod)
		for (auto ne : e[i])
			temvec.push_back(ne);
	assert(temvec.size() > 5 * insert_num);
	while (true)
	{
		int u = rand() % temvec.size();
		Edge tem1 = temvec[u], tem2(tem1.to, tem1.from);
		if (temstore.find(tem1) != temstore.end()) continue;
		if (temstore.find(tem2) != temstore.end()) continue;
		temstore.insert(tem1);
		if (temstore.size() == insert_num)
			break;
	}
	for (auto ne : temstore)
		pend_insert.push_back(ne);
}

int Graph::count_edge()
{
	int Enum = 0;
	for (auto i : node)
		Enum += e[i].size();
	return Enum / 2;
}

bool operator==(const struct Edge& X, const struct Edge& Y)
{
	return (Y.from == X.from) && (Y.to == X.to);
}

void Graph::resize(int n)
{
	sz = n;
	e.resize(sz + 1);
	rd.resize(sz + 1);
}

void Graph::push_edge(int u, int v)
{
	Edge ne(u, v);
	e[u].insert(ne);
	e[v].insert(ne);
	rd[v]++;
	if (rd[v] == PA) rd_PA++;
}

void Graph::pull_edge(int u, int v)
{
	Edge ne(u, v);
	auto it = e[u].find(ne);
	if (it != e[u].end())
	{
		e[u].erase(it);
		it = e[v].find(ne);
		e[v].erase(it);
		if (rd[v] == PA) rd_PA--;
		rd[v]--;
	}
	else
	{
		swap(ne.from, ne.to);
		it = e[u].find(ne);
		e[u].erase(it);
		it = e[v].find(ne);
		e[v].erase(it);
		if (rd[u] == PA) rd_PA--;
		rd[u]--;
	}
}

void Graph::re(int u, int v)
{
	Edge ne(u, v), to(v, u);
	auto it = e[u].find(ne);
	e[u].erase(it);
	e[u].insert(to);
	it = e[v].find(ne);
	e[v].erase(it);
	e[v].insert(to);
	if (rd[v] == PA) rd_PA--;
	rd[v]--;
	rd[u]++;
	if (rd[u] == PA) rd_PA++;
}

int Graph::search(int u, int v)
{
	//assert(u <= sz && v <= sz);
	Edge e1(u, v), e2(v, u);
	if (e[u].find(e1) != e[u].end())
		return 1;
	if (e[u].find(e2) != e[u].end())
		return 2;
	return 0;
}

void Graph::insert(int u, int v)
{
	int low, high;
	if (rd[u] <= rd[v]) low = u, high = v;
	else low = v, high = u;
	if (rd[low] <= PA - 1)
		push_edge(high, low);
	else
	{
		push_edge(high, low);
		if (ReTest(PA) == false)
			PA++;
	}
}

void Graph::delet(int u, int v)
{
	int search_result = search(u, v);
	//assert(search_result);
	if (search_result == 2)
		swap(u, v);

	if (rd[v] <= PA - 2)
		pull_edge(u, v);
	else if (rd[v] == PA)
	{
		pull_edge(u, v);
		if (rd_PA == 0)
		{
			PA--;
			ReTest(PA - 1);
			rd_PA = 0;
			for (auto i : node)
				if (rd[i] == PA)
					rd_PA++;
		}
	}
	else
	{
		pull_edge(u, v);
		ReTest(PA - 1);
	}
}

void Graph::delet2(int u, int v)
{
	int search_result = search(u, v);
	//assert(search_result);
	if (search_result == 2)
		swap(u, v);

	pull_edge(u, v);
	if (ReTest(PA - 1))
	{
		PA--;
	}
}

void Graph::make_edge(int from, int to, int cap)
{
	De.push_back({ from, to, cap, 0 });
	DG[from].push_back(De.size() - 1);
	De.push_back({ to, from, 0, 0 });
	DG[to].push_back(De.size() - 1);
}

bool Graph::dinic_BFS()
{
	vis.assign(sz + 3, false); queue<int> Q; Q.push(s); d[s] = 0, vis[s] = true;
	while (!Q.empty())
	{
		int x = Q.front(); Q.pop();
		for (int i = 0; i < DG[x].size(); i++)
		{
			DEdge& ne = De[DG[x][i]];
			if (!vis[ne.to] && ne.cap > ne.flow) { vis[ne.to] = true; d[ne.to] = d[x] + 1; Q.push(ne.to); }
		}
	}
	return vis[t];
}

int Graph::dinic_DFS(int x, int a)
{
	if (x == t || a == 0) return a;
	int flow = 0, f;
	for (int& i = cur[x]; i < DG[x].size(); i++)
	{
		DEdge& ne = De[DG[x][i]];
		if (d[x] + 1 == d[ne.to] && (f = dinic_DFS(ne.to, min(a, ne.cap - ne.flow))) > 0)
		{
			ne.flow += f;
			De[DG[x][i] ^ 1].flow -= f;
			flow += f;
			a -= f;
			if (a == 0) break;
		}
	}
	return flow;
}

bool Graph::ReTest(int testd)
{
	s = sz + 1, t = sz + 2;
	De.clear();
	DG.clear(); DG.resize(sz + 3);
	d.resize(sz + 3);
	vis.resize(sz + 3);

	for (auto i : node)
	{
		if (e[i].size() == 0) continue;
		if (rd[i] > testd)
			make_edge(s, i, rd[i] - testd);
		else if (rd[i] < testd)
			make_edge(i, t, testd - rd[i]);
	}
	int tag = De.size();
	for (auto i : node)
		for (auto ne : e[i])
			if (ne.from == i)
				make_edge(ne.to, ne.from, 1);

	int flow = 0;
	while (dinic_BFS())
	{
		cur.assign(sz + 3, 0);
		flow += dinic_DFS(s, inf);
	}

	int esize = De.size();
	for (int i = tag; i < esize; i += 2)
	{
		DEdge& ne = De[i];
		if (ne.flow)
			re(ne.to, ne.from);
	}

	for (auto i : DG[s])
	{
		if (De[i].flow != De[i].cap)
			return false;
	}
	return true;
}

void Graph::display()
{
	/*
	int m = 0;
	for (int i = 0; i <= sz; i++)
		m += e[i].size();
	m /= 2;*/
	out << PA << '\n';
}

void Graph::depict()
{
	cout << "----------\n";
	for (int i = 0; i <= sz; i++)
		for (auto ne : e[i])
			if (ne.to == i)
				cout << ne.from << ' ' << ne.to << '\n';
	cout << "----------\n";
}

bool Graph::check()
{
	queue<int> Q; unordered_set<int> vis; unordered_map<int, int> p;
	for (int i = 0; i <= sz; i++)
		if (rd[i] == PA)
		{
			Q.push(i);
			vis.insert(i);
		}
	int ans = -1;
	while (!Q.empty())
	{
		int now = Q.front(); Q.pop();
		if (rd[now] <= PA - 2)
		{
			ans = now;
			break;
		}
		for (auto ne : e[now])
		{
			if (ne.from == now) continue;
			if (vis.find(ne.from) != vis.end()) continue;
			vis.insert(ne.from);
			Q.push(ne.from);
			p[ne.from] = now;
		}
	}
	int now = ans;
	while (p.find(now) != p.end())
	{
		now = p[now];
	}
	if (ans != -1)
		return false;

	int cnt = 0;
	for (int i = 0; i <= sz; i++)
		if (rd[i] == PA)
			cnt++;
	if (cnt != rd_PA)
		return false;
	return true;
}

void timer_start() { timer = clock(); }
double timer_end()
{
	double t;
	t = (clock() - timer) / CLOCKS_PER_SEC;
	return t;
}
