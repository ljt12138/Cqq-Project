const int WORD_SIZE = 64;
typedef int8_t boolean;

class CallFlow {

	vector< vector<int> > nxt;
	vector< boolean > nonPure;       // is nonPure;
	vector< unsigned long long > dp; // visit global var
	
	int maxId;
	
	inline void checkId(int id)
	{
		if (id >= maxId) {
			maxId = id + 1;
			nxt.resize(maxId);
			nonPure.resize(maxId);
			dp.resize(maxId);
		}
	}

	class SccCalculater {

		stack< int > stk;
		vector< boolean > vis;
		vector< int > sccId;
		vector< int > dfn, low;
		CallFlow *graph;
		int dfn_top, scc_top;

		void tarjan(int nd)
		{
			low[nd] = dfn[nd] = ++dfn_top;
			stk.push(nd), vis[nd] = 1;
			for (int to : graph->nxt[nd]) {
				if (!dfn[to]) {
					tarjan(to);
					low[nd] = min(low[nd], low[to]);
				} else if (vis[to]) {
					low[nd] = min(low[nd], dfn[to]);
				}
			}
			if (low[nd] == dfn[nd]) {
				int now, id = scc_top++;
				do {
					now = stk.top();
					stk.pop();
					vis[now] = 0;
					sccId[now] = id;
				} while (now != nd);
			}
		}
	        
	public:

		SccCalculater(CallFlow *graph, int size)
		{
			this->graph = graph;
			vis.resize(size);
			dfn.resize(size), low.resize(size);
			sccId.resize(size);
			dfn_top = scc_top = 0;
		}

		~SccCalculater()
		{ }
		
		inline void dfs(int nd)
		{
			if (dfn[nd] == 0)
				tarjan(nd);
		}

		inline int getId(int nd)
		{
			return sccId[nd];
		}

		inline int getSccNumber()
		{ return scc_top; }
		
	};

	class DAGClosure {

		vector< vector<int> > pre;
		vector< int > degree;
		vector< unsigned long long > dp;
		vector< boolean > nonPure;
		queue< int > que;
		
		CallFlow *callFlow;
		size_t maxId;
		
	public:
		
		DAGClosure(CallFlow *callFlow, size_t maxId)
		{
			this->callFlow = callFlow;
			this->maxId = maxId;
			pre.resize(maxId);
			degree.resize(maxId);
			dp.resize(maxId);
			nonPure.resize(maxId);
		}

		~DAGClosure()
		{ }

		inline void addPoint(int gp, int nd)
		{
			dp[gp] |= callFlow->dp[nd];
			nonPure[gp] |= callFlow->nonPure[nd];
		}

		inline void addEdge(int u, int v)
		{
			pre[v].push_back(u);
			degree[u]++;
		}

		inline unsigned long long getDp(int nd)
		{ return dp[nd]; }

		inline boolean getNonPure(int nd)
		{ return nonPure[nd]; }

		inline void analyze()
		{
			for (size_t i = 0; i < this->maxId; i++) {
				if (!degree[i])
					que.push(i);
			}
			while (!que.empty()) {
				int nd = que.front();
				que.pop();
				for (int to : pre[nd]) {
					dp[to] |= dp[nd];
					nonPure[to] |= nonPure[nd];
					if ((--degree[to]) == 0)
						que.push(to);
				}
			}
		}
		
	};
	
public:

	CallFlow()
	{ maxId = 0; }
	
	inline void addPoint(int id, int varId)
	{
		checkId(id);
		if (varId < WORD_SIZE)
			dp[id] |= (1ull << varId);
		else nonPure[id] = 1;
	}

	inline void addEdge(int u, int v)
	{
		checkId(u);
		checkId(v);
		nxt[u].push_back(v);
	}

	inline void analyze()
	{
		/*	cerr << "CALL GRAPH" << endl;
		for (int i = 0; i < maxId; i++) {
			cerr << dp[i] << " -- " << (int)nonPure[i] << endl;
			}*/
		SccCalculater* calc = new SccCalculater(this, maxId);
		for (int i = 0; i < maxId; i++)
			calc->dfs(i);
		DAGClosure* dag = new DAGClosure(this, calc->getSccNumber());
		for (int i = 0; i < maxId; i++)
			dag->addPoint(calc->getId(i), i);
		for (int i = 0; i < maxId; i++)
			for (int j : nxt[i])
				if (calc->getId(i) != calc->getId(j))
					dag->addEdge(calc->getId(i),  calc->getId(j));
		dag->analyze();
		for (int i = 0; i < maxId; i++) {
			dp[i] = dag->getDp(calc->getId(i));
			nonPure[i] = dag->getNonPure(calc->getId(i));
		}
		delete calc;
		delete dag;
	}

	inline pair<unsigned long long, int> getInfo(int nd)
	{
		return make_pair(dp[nd], nonPure[nd]);
	}
};
