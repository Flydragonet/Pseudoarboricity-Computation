#include <bits/stdc++.h>
using namespace std;
typedef int type;
struct Edge
{
	type t1, t2;
	Edge(type a, type b) : t1(a), t2(b) {}
	bool operator<(const Edge& b) const
	{
		if (t1 != b.t1)
			return t1 < b.t1;
		else return t2 < b.t2;
	}
};
Edge* e;
int main(int argc, char** argv) // ./preprocess <input_address> <output_address>
{
	type n, m;
	const type ARRAY_SIZE_INCREMENT = 500000;
	FILE* file = fopen(argv[1], "r"), * out = fopen(argv[2], "w");

	type Emax = ARRAY_SIZE_INCREMENT;
	n = m = 0;
	e = (Edge*)malloc(Emax * sizeof(Edge));

	char line[200];
	while (fgets(line, 200, file))
	{
		type t1, t2;
		type i = 0;
		t1 = t2 = 0;
		while (line[i] < '0' || line[i]>'9') i++;
		while (line[i] >= '0' && line[i] <= '9') t1 = t1 * 10 + line[i] - '0', i++;
		while (line[i] < '0' || line[i]>'9') i++;
		while (line[i] >= '0' && line[i] <= '9') t2 = t2 * 10 + line[i] - '0', i++;
		if (t1 == t2) continue;
		if (t1 > t2) swap(t1, t2);
		e[m].t1 = t1, e[m].t2 = t2;
		n = max(n, max(e[m].t1, e[m].t2));
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
	sort(e, e + m);
	fprintf(out, "%d %d\n", e[0].t1, e[0].t2);
	type cnt = 1;
	for (type i = 1; i < m; i++)
	{
		if (e[i].t1 == e[i - 1].t1 && e[i].t2 == e[i - 1].t2) continue;
		cnt++;
		fprintf(out, "%d %d\n", e[i].t1, e[i].t2);
	}
	printf("Graph_address: %s\n", argv[1]);
	printf("n = %d, m = %d\n", n, cnt);
	fclose(out);
	return 0;
}


