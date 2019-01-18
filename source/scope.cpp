template<typename T>
class Scope {

	map<string, T > table;
	int id;

public:

	Scope()
	{ id = 0; }

	inline bool count(const string &str) 
	{ return table.count(str); }

	inline T lookup(const string &str) 
	{
		assert(count(str));
		return table[str];
	}

	inline void create(const string &str, const T &var, int len = 1)
	{
		assert(!count(str));
		id += len;
		table[str] = var;
	}

	inline int size()
	{ return id; }
};

class ScopeStack {

	vector<Scope<Var> > stk;
	int id;
	int max_siz;

public:

	ScopeStack()
	{ id = 0, max_siz = 0; }

	inline void open()
	{ stk.push_back(Scope<Var>()); }

	inline int topSize()
	{ return stk[stk.size()-1].size(); }

	inline void close()
	{
		id = id - stk[stk.size()-1].size();
		stk.pop_back();
	}

	inline bool count(const string &str, bool through = true) 
	{
		if (!through)
			return stk[stk.size()-1].count(str);
		for (int i = stk.size()-1; i >= 0; i--)
			if (stk[i].count(str))
				return true;
		return false;
	}

	inline Var lookup(const string &str) 
	{
		assert(count(str));
		assert(stk.size() != 0);
		for (size_t i = stk.size()-1; i >= 0; i--) {
			if (stk[i].count(str)) 
				return stk[i].lookup(str);
		}
	}

	inline void create(const string &str, const Var &var, int len = 1)
	{
		id += len;
		max_siz = max(max_siz, id);
		stk[stk.size()-1].create(str, var, len);
	}

	inline int getSize()
	{ return max_siz; }

	inline int getDepth()
	{ return stk.size(); }
	
} scopeStack;
