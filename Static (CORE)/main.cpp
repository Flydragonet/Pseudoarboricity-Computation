#include <bits/stdc++.h>
#include <unordered_set>
#include <unordered_map>
using namespace std;

string prefix("../Graphs/");
ifstream in, dataset("dataset.txt");
ofstream out("output.csv");
vector<int> core;
struct Edge
{
	int from, to;
	bool oriented;
	Edge(int t1, int t2) : from(t1), to(t2), oriented(false) {}
	Edge(int t1, int t2, bool t3) : from(t1), to(t2), oriented(t3) {}
	bool operator==(const struct Edge& Y) const
	{
		return (Y.from == from) && (Y.to == to);
	}
	bool operator<(const struct Edge& Y) const
	{
		return min(core[from], core[to]) > min(core[Y.from], core[Y.to]);
	}
};
struct Edge_hush {
	size_t operator()(const struct Edge& _r) const {
		return (_r.from << 16) + _r.to;
	}
};
inline int cei(double x) { return int(ceil(x) + 0.5); }

vector<unordered_set<Edge, Edge_hush> > e;
unordered_set<int> nodeset, nodes;
vector<int> d;
int n, m;
int nowmaxp, pass;
int p0, p;
int pause; double pausetime = 100000000; //If the running time reach the pausetime, the algorithm will terminate.
double timer = 0.0;

void solve(string input);
void resize(int x);
void CoreOrder(); //preprocessing and CORE
void flip(Edge ne);
void orient(int from, int to);
void kcore_display();
int reorientation(unordered_set<int>& Vori);
int final_reorientation();
inline bool checkpause();

void display();
void timer_start(); double timer_end();
char line[1000];

int main()
{
	out << "graph,|V|,|E|,approximate algorithm time consumption,re-orientation network time consumption,whole time consumption,pseudoarboricity\n";
	string input;
	while (dataset >> input)
		solve(input);
	return 0;
}

void solve(string input)
{
	cout << "Now processing graph dataset: " << input << '\n';

	int cnt1 = 0, cnt2 = 0;

	e.clear();
	nodeset.clear();
	nodes.clear();
	d.clear();
	core.clear();
	n = m = 0;
	pause = 0;
	nowmaxp = 0;

	in.open(prefix + input);
	out << input << ',';

	int t1, t2;
	double sumtime = 0.0;
	while (in.getline(line, 1000))
	{
		t1 = 0, t2 = 0;
		int i = 0;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') t1 = t1 * 10 + line[i] - '0', i++;
		while (line[i] < '0' || line[i]>'9')i++;
		while (line[i] >= '0' && line[i] <= '9') t2 = t2 * 10 + line[i] - '0', i++;
		nodeset.insert(t1);
		nodeset.insert(t2);
		m++;
		if (max(t1, t2) > n)
			resize(max(t1, t2));
		e[t1].insert(Edge(t1, t2, false));
		e[t2].insert(Edge(t1, t2, false));
	}
	out << n << ',' << m << ',';

	timer_start();
	CoreOrder();
	sumtime += timer_end();

	timer_start();
	p = reorientation(nodes);
	sumtime += timer_end();

	if (pause)
		out << "TLE,";
	else
		out << sumtime << ',';

	if (pause)
		out << "---\n";
	else
	{
		out << p << '\n';
		cout << "Pseudoarboricity: " << p << '\n';
	}

	if (pause)
		cout << input << " TIMEOUT!\n";
	else
		cout << input << " FINISH!!\n\n";

	in.close();
	in.clear();
	return;
}

void resize(int x)
{
	n = x;
	e.resize(n + 1);
	d.resize(n + 1);
}

void flip(Edge temedge)
{
	e[temedge.from].erase(temedge);
	e[temedge.to].erase(temedge);
	swap(temedge.from, temedge.to);
	e[temedge.from].insert(temedge);
	e[temedge.to].insert(temedge);
}

void orient(int from, int to)
{
	Edge tare(from, to, true);
	auto it = e[from].find(tare);
	if (it == e[from].end())
		flip(Edge(to, from, true));
	else
	{
		Edge nowe(from, to, false);
		e[from].erase(nowe);
		e[to].erase(nowe);
		e[from].insert(tare);
		e[to].insert(tare);
	}
}

bool core_cmp(Edge x, Edge y)
{
	return min(core[x.from], core[x.to]) > min(core[y.from], core[y.to]);
}

vector<unordered_set<int> > ud, UD;
vector<int> nd, ND;
vector<int> node_order;
inline void dminus(int x) { ud[nd[x]].erase(x); nd[x]--; ud[nd[x]].insert(x); }
void CoreOrder()
{
	nd.assign(n + 1, 0);
	core.assign(n + 1, 0);
	ud.clear();
	for (int i = 0; i <= n; i++)
	{
		int nowd = e[i].size();
		nd[i] = nowd;
		if (nowd + 1 > ud.size())
			ud.resize(nowd + 1);
		ud[nowd].insert(i);
	}
	UD = ud; ND = nd;

	int nowcore = 0, worked = 0, maxsize = ud.size(), corenum = 0, Enum = m;
	unordered_set<int> V = nodeset;
	nodeset.clear();
	double nowmax = 1.0 * Enum / V.size();
	while (nowcore < maxsize && ud[nowcore].empty())
		nowcore++;
	while (true)
	{
		while (nowcore < maxsize && ud[nowcore].empty())
			nowcore++;
		if (nowcore == maxsize)
			break;
		corenum = max(corenum, nowcore);
		auto it = ud[nowcore].begin();
		int now = *it; ud[nowcore].erase(it);
		core[now] = corenum;
		vector<Edge> orient_list;
		for (auto ne : e[now])
			orient_list.push_back(ne);
		for (auto ne : orient_list)
		{
			int tar = (ne.from == now) ? ne.to : ne.from;
			if (V.find(tar) == V.end()) continue;
			Enum--;
			ud[nd[tar]].erase(tar);
			nd[tar]--;
			ud[nd[tar]].insert(tar);
		}
		V.erase(now); node_order.push_back(now);
		if (V.empty()) break;
		double nowdensity = 1.0 * Enum / V.size();
		nowmax = max(nowmax, nowdensity);
		if (nowcore > 0) nowcore--;
	}
	p0 = cei(nowmax);
	int len = node_order.size(), pnum;
	for (pnum = len - 1; pnum >= 0; pnum--)
	{
		if (core[node_order[pnum]] < p0) break;
		nodes.insert(node_order[pnum]);
	}
	pnum++;
	for (int i = pnum; i < len; i++)
	{
		int now = node_order[i];
		vector<Edge> orient_list;
		for (auto ne : e[now])
      if (!ne.oriented)
			  orient_list.push_back(ne);
		for (auto ne : orient_list)
		{
			int tar = (ne.from == now) ? ne.to : ne.from;
			if (core[tar] < p0) continue;
			orient(tar, now);
			d[now]++;
		}
	}

	ud.clear();
	nd.clear();
	return;
}

void kcore_display()
{
	out << nodes.size() << ',';
	int Ecnt = 0, maxd = 0;
	for (auto i : nodes)
	{
		maxd = max(maxd, d[i]);
		for (auto ne : e[i])
		{
			int tar = (ne.from == i) ? ne.to : ne.from;
			if (nodes.find(tar) == nodes.end()) continue;
			Ecnt++;
		}
	}
	Ecnt /= 2;
	out << Ecnt << ',';
	out << maxd << ',';
}

struct DEdge
{
	int from, to, cap, flow;
	DEdge(int t1, int t2, int t3, int t4) : from(t1), to(t2), cap(t3), flow(t4) {}
};
unordered_map<int, int> dist, cur;
unordered_map<int, bool> vis;
vector<DEdge> De, De_store;
unordered_map<int, vector<int> > DG, DG_store;
int Ds, Dt, Edgenum;
const int inf = 1000000000;
void makeedge(int from, int to, int cap)
{
	De.push_back({ from, to, cap, 0 });
	DG[from].push_back(De.size() - 1);
	De.push_back({ to, from, 0, 0 });
	DG[to].push_back(De.size() - 1);
}
bool DBFS()
{
	for (auto& i : vis)
		i.second = false;
	queue<int> Q; Q.push(Ds); dist[Ds] = 0, vis[Ds] = true;
	while (!Q.empty())
	{
		//if (checkpause()) return false;
		int x = Q.front(); Q.pop();
		for (int i = 0; i < DG[x].size(); i++)
		{
			DEdge& ne = De[DG[x][i]];
			if (!vis[ne.to] && ne.cap > ne.flow) { vis[ne.to] = true; dist[ne.to] = dist[x] + 1; Q.push(ne.to); }
		}
	}
	return vis[Dt];
}
int DDFS(int x, int a)
{
	//if (checkpause()) return 1;
	if (x == Dt || a == 0) return a;
	int flow = 0, f;
	for (int& i = cur[x]; i < DG[x].size(); i++)
	{
		DEdge& ne = De[DG[x][i]];
		if (dist[x] + 1 == dist[ne.to] && (f = DDFS(ne.to, min(a, ne.cap - ne.flow))) > 0)
		{
			//if (checkpause()) return 1;
			ne.flow += f;
			De[DG[x][i] ^ 1].flow -= f;
			flow += f;
			a -= f;
			if (a == 0) break;
		}
	}
	return flow;
}
bool Dcheck(unordered_set<int>& Vori, int dtest)
{
	De = De_store;
	DG = DG_store;
	int sums = 0;
	for (auto i : Vori)
	{
		if (d[i] > dtest)
		{
			makeedge(Ds, i, d[i] - dtest);
			sums += d[i] - dtest;
		}
		else if (d[i] < dtest)
			makeedge(i, Dt, dtest - d[i]);
	}

	dist.clear();
	vis.clear();
	cur.clear();
	for (auto i : Vori)
	{
		dist[i] = 0;
		vis[i] = 0;
	}
	dist[Ds] = dist[Dt] = vis[Ds] = vis[Dt] = 0;
	int flow = 0;
	while (DBFS())
	{
		//if (checkpause()) return false;
		for (auto i : Vori)
			cur[i] = 0;
		cur[Ds] = cur[Dt] = 0;
		flow += DDFS(Ds, inf);
	}
	return flow == sums;
}
int final_reorientation()
{
	//if (checkpause()) return 0;
	//初始化
	Ds = n + 1, Dt = n + 2;
	int Dl = 0, Dr = -1;
	for (auto i : nodes)
		d[i] = 0;
	for (auto i : nodes)
		for (auto ne : e[i])
			if (ne.to == i && nodes.find(ne.from) != nodes.end())
				d[i]++;
	Dr = nowmaxp;

	//De_store.clear();
	vector<DEdge>().swap(De_store);
	DG_store.clear();
	for (auto i : nodes)
	{
		for (auto ne : e[i])
		{
			if (ne.from == i) continue;
			if (nodes.find(ne.from) == nodes.end()) continue;
			De_store.push_back({ i, ne.from, 1, 0 });
			DG_store[i].push_back(De_store.size() - 1);
			De_store.push_back({ ne.from, i, 0, 0 });
			DG_store[ne.from].push_back(De_store.size() - 1);
		}
	}
	Edgenum = De_store.size();

	Dcheck(nodes, Dr);
	for (int i = 0; i < Edgenum; i += 2)
	{
		if (De[i].flow == 1)
		{
			Edge ne(De[i].to, De[i].from);
			d[De[i].to]++;
			d[De[i].from]--;
			flip(ne);
		}
	}
	for (auto i : nodes)
		Dl = max(Dl, d[i]);
	return Dl;
}

int reorientation(unordered_set<int>& Vori)
{
	//if (checkpause()) return 0;
	//初始化
	Ds = n + 1, Dt = n + 2;
	int Dl = p0, Dr = -1;
	for (auto i : Vori)
		d[i] = 0;
	for (auto i : Vori)
		for (auto ne : e[i])
			if (ne.to == i && Vori.find(ne.from) != Vori.end())
				d[i]++;

	//De_store.clear();
	vector<DEdge>().swap(De_store);
	DG_store.clear();
	for (auto i : Vori)
	{
		for (auto ne : e[i])
		{
			if (ne.from == i) continue;
			if (Vori.find(ne.from) == Vori.end()) continue;
			De_store.push_back({ i, ne.from, 1, 0 });
			DG_store[i].push_back(De_store.size() - 1);
			De_store.push_back({ ne.from, i, 0, 0 });
			DG_store[ne.from].push_back(De_store.size() - 1);
		}
	}
	Edgenum = De_store.size();

	if (Dcheck(Vori, p0))
		Dl = p0;
	else
	{
		for (auto i : Vori)
			Dr = max(Dr, d[i] + 1);
		while (Dr > Dl)
		{
			//if (checkpause()) return 0;
			int mid = (Dr + Dl) / 2;
			if (Dcheck(Vori, mid))
				Dr = mid;
			else
				Dl = mid + 1;
		}
		Dcheck(Vori, Dl);
	}
	for (int i = 0; i < Edgenum; i += 2)
	{
		if (De[i].flow == 1)
		{
			Edge ne(De[i].to, De[i].from);
			flip(ne);
		}
	}
	return Dl;
}

//-----------------------以下是功能性函数------------------------//

void display()
{
	for (auto i : nodeset)
		for (auto ne : e[i])
			if (ne.from == i)
				cout << ne.from << ' ' << ne.to << '\n';
}

void timer_start() { timer = clock(); }
double timer_end()
{
	if (pause == 1)
	{
		out << ">3h,";
		pause = 2;
		return 0.0;
	}
	else if (pause == 2)
	{
		out << "skipped,";
		return 0.0;
	}
	double t;
	out << (t = (clock() - timer) / CLOCKS_PER_SEC) << ',';
	return t;
}

inline bool checkpause()
{
	if (pause == 1 || pause == 2 || ((clock() - timer) / CLOCKS_PER_SEC > pausetime))
	{
		pause = 1;
		return true;
	}
	return false;
}
