#include "visitor.h"
#include "loopoptimizer.cpp"

class Optimizer : public Visitor {

	int idTotal;
	int scopeId;
	// mark left value
	int leftId;

	const static int MEM = 3;
	const static int REG = 1;
	const static int EXEC = 10;
	const static int INF = 1000000000;
	
	struct Node {
		enum Type { Define, Call, Forward, Back, Use } type;
		FunDef *call;
		int uuId;
		int tar;
		bool loop;
		int k, b;
	        
		Node(Type type)
		{ this->type = type, this->loop = false; }
		
		Node(int uuId)
		{ this->uuId = uuId, this->type = Define, this->loop = false; }

		Node(FunDef *call)
		{ this->call = call, this->type = Call, this->loop = false; }
	};

	LoopOptimizer optv;
	vector<Node> loopStack;
	stack<int> goBack;

	inline void push(int id)
	{
		if (!optv.vis[id]) {
			goBack.push(id);
		}
		optv.vis[id]++;
	}

	inline void pop(int id)
	{
		optv.vis[id]--;
	}
	
	inline void back()
	{
		while (!goBack.empty()) {
			optv.vis[goBack.top()] = 0;
			goBack.pop();
		}
	}	

	inline int count(int id)
	{ return optv.vis[id]; }
	
	inline void defPoint(int uuId)
	{
		loopStack.push_back(Node(uuId));
	}

	inline void usePoint(int uuId)
	{
		Node tmp = Node(Node::Use);
		tmp.uuId = uuId;
		loopStack.push_back(tmp);
	}
	
	inline void callPoint(FunDef *funDef)
	{
		loopStack.push_back(Node(funDef));
	}

	CallFlow *callFlow;

	unordered_map<int,int> *globalId;

	FunDef *current;

	// about loop var
	int defId;
	int loopVar;
	int k, b, idInScope;
	
public:

	Optimizer(int idTotal, CallFlow *callFlow, unordered_map<int,int> *globalId)
	{
		this->idTotal = idTotal;
		scopeId = 0;
		optv.vis.resize(idTotal);
		leftId = 0;
		this->callFlow = callFlow;
		this->globalId = globalId;
		// cerr << "Build" << endl;
	}

	virtual ~Optimizer()
	{ }

	inline void visitTree(Tree *that)
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that) 
	{
		for (size_t i = 0; i < that->lst.size(); i++)
			that->lst[i]->accept(*this);
	}

	virtual void visitVarDef(VarDef *that) 
	{
		for (size_t i = 0; i < that->vars.size(); i++) {
			defPoint(that->vars[i].uuId);
			if (that->vars[i].def != nullptr)
				that->vars[i].def->accept(*this);
		}
	}

	virtual void visitFunDef(FunDef *that)
	{
		current = that;
		for (size_t i = 0; i < that->arg.size(); i++)
			defPoint(that->arg[i].uuId);
		that->stmt->accept(*this);
	}

	virtual void visitBlock(Block *that)
	{
		for (size_t i = 0; i < that->lst.size(); i++)
			that->lst[i]->accept(*this);
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
		that->selfupdate = false;
		if (that->type == '=') {
			leftId++;
			that->left->accept(*this);
			int id = defId;
			int pos = loopStack.size()-1;
			leftId--;
			that->right->accept(*this);
			if (id != -1 && loopVar) {
				that->selfupdate = true;
				that->pos = idInScope;
				that->k = k;
				that->b = b;
				loopStack[pos].loop = true;
				loopStack[pos].k = k;
				loopStack[pos].b = b;
			}
			// outside is not a loop var
			loopVar = 0;
			k = b = 0;
		} else {
			int Ld, L, Lk, Lb;
			that->left->accept(*this);
			L = loopVar, Lk = k, Lb = b;
			that->right->accept(*this);
			if (loopVar && L) {
				switch(that->type) {
				case '+':
					k = Lk + k;
					b = Lb + b;
					break;
				case '-':
					k = Lk - k;
					b = Lb - b;
					break;
				default:
					loopVar = 0;
					break;
				}
			} else {
				loopVar = 0;
			}
		}
		if (that->type == LNOT)
			that->weight = that->right->weight;
		else 
			that->weight = max(that->left->size, max(that->right->size, min(that->left->size, that->right->size)+1));
	}

	virtual void visitCinOp(CinOp *that)
	{
		for (size_t i = 0; i < that->vars.size(); i++) {
			leftId++;
			that->vars[i]->accept(*this);
			leftId--;
		}
	}

	virtual void visitCoutOp(CoutOp *that)
	{
		for (size_t i = 0; i < that->expr.size(); i++)
			that->expr[i]->accept(*this);
	}

	virtual void visitIdent(Ident *that)
	{
		if (leftId) {
			assert(false);
		}
	}
	
	virtual void visitExec(Exec *that)  
	{
		that->weight = INF;
		for (size_t i = 0; i < that->args.size(); i++)
			that->args[i]->accept(*this);
		callPoint(that->target);
		loopVar = 0;
	}

	virtual void visitArrayAt(ArrayAt *that)
	{
		that->weight = 1;
		if (leftId) {
		        defPoint(that->var.uuId);
		}
		loopVar = 0;
		int pre = leftId;
	        leftId = 0;
		for (size_t i = 0; i < that->at.size(); i++) {
			that->at[i]->accept(*this);
			that->weight = max(that->weight, that->at[i]->weight + (i != 0));
		}
		leftId = pre;
		loopVar = 0;
		defId = -1;
	}

	virtual void visitIf(If *that)  
	{
		that->expr->accept(*this);
		loopStack.push_back(Node(Node::Forward));
		size_t id = loopStack.size();
		if (that->trueBranch != nullptr)
			that->trueBranch->accept(*this);
		if (that->falseBranch != nullptr)
			that->falseBranch->accept(*this);
		Node pre = Node(Node::Back);
		pre.tar = id-1;
		loopStack.push_back(pre);
		loopStack[id-1].tar = loopStack.size()-1;
	}

	
	void solveLoop(Loop *that, Tree *stmt, int id)
	{
		boolean nonpure = 0;
		unsigned long long dp = 0;
		for (size_t i = id; i < loopStack.size(); i++) {
			if (loopStack[i].type == Node::Define) {
				push(loopStack[i].uuId);
			} else if (loopStack[i].type == Node::Call) {
				pair<unsigned long long, boolean>
					info = callFlow->getInfo(loopStack[i].call->funcId);
				nonpure |= info.second;
				dp |= info.first;
			}
		}
		optv.globalId = this->globalId;
		optv.dp = dp;
		optv.optimizeGlobal = !nonpure;
		optv.pre = that;
		optv.fun = current;
#ifdef COMPILE
		unordered_set<int> liveUse;
		unordered_map<int,int> loopVar;
		for (size_t i = loopStack.size()-1; i+1 != 0; i--) {
			if (loopStack[i].type == Node::Back) {
				// skip control instruction
				for (size_t j = i-1; j != loopStack[i].tar; j--)
					if (loopStack[j].type == Node::Use)
						liveUse.insert(loopStack[j].uuId);
				i = loopStack[i].tar;
			} else if (loopStack[i].type == Node::Define) {
				int id = loopStack[i].uuId;
			        if (!liveUse.count(id) && count(id) == 1 && loopStack[i].loop) {
					// not use after define && define only once
					// cerr << "Find Loop Var At " << that->getLoc() << " : id = " << id << ", step = " << loopStack[i].k << "," << loopStack[i].b << endl;
					loopVar[id] = loopStack[i].b;
				}
			} else if (loopStack[i].type == Node::Use) {
				liveUse.insert(loopStack[i].uuId);
			}
		}
		optv.loopVar = &loopVar;
#endif
		stmt->accept(optv);
		back();
		Node pre = Node(Node::Back);
		pre.tar = id-1;
		loopStack.push_back(pre);
		loopStack[id-1].tar = loopStack.size()-1;
	}
	
	virtual void visitWhile(While *that)  
	{
		loopStack.push_back(Node(Node::Forward));
		size_t id = loopStack.size();
		that->expr->accept(*this);
		that->stmt->accept(*this);
		solveLoop(that, that->stmt, id);
	}

	virtual void visitFor(For *that)  
	{
		if (that->init != nullptr)
			that->init->accept(*this);
		loopStack.push_back(Node(Node::Forward));
		size_t id = loopStack.size();
		that->expr->accept(*this);
		that->stmt->accept(*this);
		that->delta->accept(*this);
		solveLoop(that, that->stmt, id);
	}

	virtual void visitReturn(Return *that)  
	{
		that->expr->accept(*this);
	}

	virtual void visitPutchar(Putchar *that)
	{
		that->expr->accept(*this);
	}

	virtual void visitConstant(Constant *that)
	{
		that->weight = 1;
		if (loopVar) {
			k = 0;
			b = that->dat;
		}
	}

	virtual void visitLoad(Load *that)
	{
		that->weight = 1;
		if (leftId) {
			// global var is never loop var
			if (that->idScope == LOCAL) {
				idInScope = that->idInScope;
				defId = that->uuId;
			} else defId = -1;
			defPoint(that->uuId);
		} else {
			loopVar = (defId == that->uuId);
			if (loopVar) {
				k = 1;
				b = 0;
			} else {
				usePoint(that->uuId);
			}
		}
	}

	virtual void visitAssigner(Assigner *that)
	{
		for (size_t i = 0; i < that->idScope.size(); i++) {
			defPoint(that->uuId[i]);
			// loop var is never defined in block
		}
		for (size_t i = 0; i < that->idScope.size(); i++)
			that->def[i]->accept(*this);
	}
};
