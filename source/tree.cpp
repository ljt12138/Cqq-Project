#include "visitor.h"

///
/// ::AST
///

class Tree {

	Location loc;

public:

	long long weight;
	int size;
	
	bool isConst;
	int valueConst;

	bool isReplaced;
	Tree *valueReplaced;
	bool localConst;
	bool rightMost;
	
	int depth;
        
	Tree()
	{
	}

	Tree(const Location &loc)
	{
	}

	virtual ~Tree()
	{
	}

	void init()
	{ rightMost = localConst = false, isConst = false, isReplaced = false; }
	void setLoc(const Location &loc)
	{
		init();
		this->loc = loc;
	}

	Location getLoc()
	{ return this->loc; }

	virtual void accept(Visitor &v)
	{ v.visitTree(this); }

	virtual void printTo(int tab) = 0;

} ;

class TopLevel : public Tree {

public:
	vector<Tree*> lst;

	TopLevel()
	{ assert(false); }
	
	TopLevel(const Location &loc, const vector<Tree*> &lst)
	{
		setLoc(loc);
		this->lst = lst;
	}

	~TopLevel()
	{
		for (size_t i = 0; i < lst.size(); i++)
			delete lst[i];
	} 

	virtual void accept(Visitor &v)
	{ v.visitTopLevel(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		puts("TopLevel");
		for (size_t i = 0; i < lst.size(); i++)
			lst[i]->printTo(tab+1);
	}
};

class Var {

	bool safe_mul(int a, int b)
	{ return (long long) a * b <= MAX_ARRAY_SIZE; }

	int checkIndex(const vector<int> &index)
	{
		int cur_size = 1;
		for (size_t i = 0; i < index.size(); i++) {
			if (index[i] < 0 || !safe_mul(cur_size, index[i])) 
				killer.issueError(BadArrayOp());
			cur_size = cur_size * index[i];
		}
		return cur_size;
	}

public:

	string type;
	string name;
	vector<Tree*> index_tree;
	vector<int> index;
	int idScope;
	uint idInScope;
	uint isLeftValue;
	int size;
	int uuId;
	bool isConst;
	Tree *def;
	
	Var()
	{
		def = nullptr;
	}

	Var(const string &type, const string &name, const vector<Tree*> &index)
	{
		this->type = type;
		this->name = name;
		this->index_tree = index;
		idInScope = -1;
		idScope = 0;
		isLeftValue = true;
		isConst = false;
		def = nullptr;
	}

	Var(const string &type, const string &name)
	{
		this->type = type;
		this->name = name;
		idInScope = -1;
		idScope = 0;
		isLeftValue = true;
		isConst = false;
		size = 1;
		def = nullptr;
	}

	~Var()
	{
	}
	
	void checkSize()
	{
		size = checkIndex(index);
	}
		
	void print()
	{
		cout << type << " " << name;
		if (type == "array") {
			for (size_t j = 0; j < index.size(); j++)
				cout << "[" << index[j] << "]";
		}
	}
};

class VarDef : public Tree {
	
public:
	vector<Var> vars;
	int totSiz;
	bool constVar;
	
	VarDef()
	{ assert(false); }
	
	VarDef(const Location &loc, const vector<Var> &v, bool isConst = false)
	{
		setLoc(loc);
		vars = v;
	        constVar = isConst;
	}
	
	~VarDef()
	{
	}
	
	virtual void accept(Visitor &v)
	{ v.visitVarDef(this); }
	
	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("VarDef : const = %d\n", this->constVar);
		assert(vars.size() > 0);
		for (size_t i = 0; i < vars.size(); i++) {
			for (int j = 0; j < tab+1; j++)
				putchar(' ');
			vars[i].print();
			cout << endl;
			if (vars[i].def != nullptr)
				vars[i].def->printTo(tab+1);
			else {
				for (int j = 0; j < tab+1; j++)
				putchar(' ');
				cout << "<empty>" << endl;
			}
		}
	}

};

class FunDef : public Tree {

public:
	string name;
	vector<Var> arg;
	Tree *stmt;
	bool isMain;
	size_t size;
	int funcId;
	int totSiz;
	
	FunDef()
	{ assert(false); }
	
	FunDef(const Location &loc, const string &name, const vector<Var> &arg, Tree *stmt)
	{
		setLoc(loc);
		this->name = name;
		this->arg = arg;
		this->stmt = stmt;
		isMain = (name == "main");
		size = arg.size();
	}

	~FunDef()
	{
		if (stmt != nullptr)
			delete stmt;
		for (size_t i = 0; i < arg.size(); i++)
			if (arg[i].def != nullptr)
				delete arg[i].def;
	}

	virtual void accept(Visitor &v)
	{ v.visitFunDef(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("FunDef : ");
		cout << name << " ";
		if (arg.size() == 0)
			printf("<empty>\n");
		for (size_t i = 0; i < arg.size(); i++) {
			arg[i].print();
			putchar((i == arg.size()-1) ? '\n' : ',');
		}
		if (stmt == nullptr)
			puts("<empty>");
		else stmt->printTo(tab+2);
	}
};

class Block : public Tree {

public:

	Block()
	{ assert(false); }
	
	vector<Tree*> lst;

	Block(const Location &loc, const vector<Tree*> lst)
	{ setLoc(loc), this->lst = lst; }

	~Block()
	{
		for (size_t i = 0; i < lst.size(); i++) {
			//cerr << "BLOCK  " << i << endl;
			delete lst[i];
		}
	}

	virtual void accept(Visitor &v)
	{ v.visitBlock(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Block : \n");
		for (size_t i = 0; i < lst.size(); i++)
			lst[i]->printTo(tab+1);
	}
};

class Expr : public Tree {

public:

	// data of affine expr
	map<int,int> table;
	
	~Expr()
	{  }
	virtual void accept(Visitor &v) = 0;
	virtual void printTo(int tab) = 0;
};

class BinaryOp : public Expr {

public:

	string ident;
	int type;
	Tree *left, *right;	
	bool selfupdate;
	int pos, k, b;
	
	BinaryOp()
	{ assert(false); }
	
	BinaryOp(const Location &loc, const int type, const string &ident, Tree *left, Tree *right)
	{
		setLoc(loc);
		this->type = type;
		this->ident = ident;
		this->left = left;
		this->right = right;
	}

	~BinaryOp()
	{
		//cerr << "BinaryOp" << endl;
		if (left != nullptr && left != valueReplaced)
			delete left;
		if (right != nullptr && right != valueReplaced)
			delete right;
	}

	virtual void accept(Visitor &v)
	{ v.visitBinaryOp(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("Binary Op(weight = %lld) : %d, %d --> ", this->weight, this->isConst, this->isReplaced);
		cout << ident << endl;
		if (left != nullptr) left->printTo(tab+1);
		else puts("<empty>");
		if (right != nullptr) right->printTo(tab+1);
		else puts("<empty>");
	}
};

class CinOp : public Tree {

public:

	vector<Tree*> vars;

	CinOp()
	{ assert(false); }
	
	CinOp(const Location &loc, const vector<Tree*> &vars)
	{
		setLoc(loc);
		this->vars = vars;
	}

	~CinOp()
	{
		for (size_t i = 0; i < vars.size(); i++)
			delete vars[i];
	}

	virtual void accept(Visitor &v)
	{ v.visitCinOp(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("cin : \n");
		for (size_t i = 0; i < vars.size(); i++) {
			vars[i]->printTo(tab+1);
		}
	}
};

class CoutOp : public Tree {

public:

	vector<Tree*> expr;

	CoutOp()
	{ assert(false); }
	
	CoutOp(const Location &loc, const vector<Tree*> &T)
	{
		setLoc(loc);
		expr = T;
	}

	~CoutOp()
	{
		for (size_t i = 0; i < expr.size(); i++)
			delete expr[i];
	}

	virtual void accept(Visitor &v)
	{ v.visitCoutOp(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("cout : \n");
		for (size_t i = 0; i < expr.size(); i++)
			expr[i]->printTo(tab+1);
	}
};

class Ident : public Expr {

public:	

	int dat;
	int type;
	string name;	

	Var var;

	Ident()
	{ assert(false); }
	
	Ident(const Location &loc, int dat)
	{
		setLoc(loc);
		this->dat = dat;
		type = INT;
	}

	Ident(const Location &loc, int type, int dat)
	{
		setLoc(loc);
		this->dat = dat;
		this->type = type;
	}

	Ident(const Location &loc, const string &name)
	{
		setLoc(loc);
		this->type = KEYWORD;
		this->name = name;
	}

	~Ident()
	{
		if (var.def != nullptr) {
			delete var.def;
		}
	}
	
	virtual void accept(Visitor &v)
	{ v.visitIdent(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		if (type == INT) 
			printf("int_const : %d\n", dat);
		else if (type == KEYWORD) {
			printf("var : %s\n", name.c_str());
			if (var.def == nullptr) {
				for (int i = 0; i < tab+1; i++) putchar(' ');
				printf("<empty>");
			} else {
				var.def->printTo(tab+1);
			}
		}
		else printf("end of line\n");
	}
};

class ArrayAt : public Expr {

public:

	string name;
	vector<Tree*> at;

	uint idInScope;
	uint idScope;
	Var var;
	int *indexSuf;
	int *pre;
	int position;
	
	ArrayAt()
	{ assert(false); }
	
	ArrayAt(const Location &loc, const string &name, const vector<Tree*> &index)
	{
		setLoc(loc);
		this->name = name;
		this->at = index;
		this->indexSuf = nullptr;
		this->position = -1;
	}

	~ArrayAt()
	{
		assert(var.def == nullptr);
		for (size_t i = 0; i < at.size(); i++)
			delete at[i];
		if (indexSuf != nullptr)
			delete[] indexSuf;
		if (pre != nullptr)
			delete[] pre;
	}

	inline void fixIndexSuf()
	{
		int p = 1;
		indexSuf = new int[var.index.size()];
		pre = new int[var.index.size()];
		int len = var.index.size();
		for (int i = len-1; i >= 0; i--) {
			indexSuf[i] = p;
			p = p * var.index[i];
			pre[i] = 0;
		}
	}

	virtual void accept(Visitor &v)
	{
		v.visitArrayAt(this);
	}

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("Array : ");
		cout << name << " size = " << at.size() << endl;
		for (size_t i = 0; i < at.size(); i++)
			at[i]->printTo(tab+1);
	}
};

class Exec : public Expr {

public:
	
	string name;
	vector<Tree*> args;

	FunDef *target;

	Exec()
	{ assert(false); }
	
	Exec(const Location &loc, const string &name, const vector<Tree*> &args)
	{
		setLoc(loc);
		this->name = name;
		this->args = args;
		target = nullptr;
	}

	~Exec()
	{
		for (size_t i = 0; i < args.size(); i++)
			delete args[i];
	}

	virtual void accept(Visitor &v)
	{ v.visitExec(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("exec : ");
		cout << name << endl;
		for (size_t i = 0; i < args.size(); i++)
			args[i]->printTo(tab+1);
	}
};

class If : public Tree {

public:

	Tree* expr;
	Tree* trueBranch;
	Tree* falseBranch;

	If()
	{ assert(false); }
	
	If(const Location &loc, Tree *expr, Tree *trueBranch)
	{
		setLoc(loc);
		this->expr = expr;
		this->trueBranch = trueBranch;
		this->falseBranch = nullptr;
	}

	If(const Location &loc, Tree *expr, Tree *trueBranch, Tree *falseBranch)
	{
		setLoc(loc);
		this->expr = expr;
		this->trueBranch = trueBranch;
		this->falseBranch = falseBranch;
	}

	~If()
	{
		if (expr != nullptr && expr != valueReplaced)
			delete expr;
		if (trueBranch != nullptr && trueBranch != valueReplaced)
			delete trueBranch;
		if (falseBranch != nullptr && falseBranch != valueReplaced)
			delete falseBranch;
	}

	virtual void accept(Visitor &v)
	{ v.visitIf(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("IF\n");
		expr->printTo(tab+1);
		if (trueBranch != nullptr)
			trueBranch->printTo(tab+1);
		else {
			for (int i = 0; i < tab; i++)
				putchar(' ');
			puts("<empty>");
		}
		if (falseBranch != nullptr)
			falseBranch->printTo(tab+1);
		else {
			for (int i = 0; i < tab; i++)
				putchar(' ');
			puts("<empty>");
		}
	}
};


class Loop : public Tree {
public:
	vector<Tree*> preCalc;

	virtual ~Loop()
	{} 
};

class While : public Loop {

public:

	Tree* expr;
	Tree* stmt;

	While()
	{ assert(false); }
	
	While(const Location &loc, Tree *expr, Tree *stmt)
	{
		setLoc(loc);
		this->expr = expr;
		this->stmt = stmt;
	}

	~While()
	{
		if (expr != nullptr)
			delete expr;
		if (stmt != nullptr)
			delete stmt;
		for (size_t i = 0; i < preCalc.size(); i++)
			delete preCalc[i];
	}

	virtual void accept(Visitor &v)
	{ v.visitWhile(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("WHILE\n");
		for (size_t i = 0; i < preCalc.size(); i++)
			preCalc[i]->printTo(tab+1);
		expr->printTo(tab+1);
		if (stmt != nullptr)
			stmt->printTo(tab+1);
		else puts("<empty>");
	}
};

class For : public Loop {

public:

	Tree *init;
	Tree *expr;
	Tree *delta;
	Tree *stmt;

	For()
	{ assert(false); }

	For(const Location &loc, Tree *init, Tree *expr, Tree *delta, Tree *stmt)
	{
		setLoc(loc);
		this->init = init;
		this->expr = expr;
		this->delta = delta;
		this->stmt = stmt;
	}

	~For()
	{
		if (init != nullptr && init != valueReplaced)
			delete init;
		if (expr != nullptr)
			delete expr;
		if (delta != nullptr)
			delete delta;
		if (stmt != nullptr)
			delete stmt;
		for (size_t i = 0; i < preCalc.size(); i++)
			delete preCalc[i];
		//cerr << "FOR" << endl;
	}

	virtual void accept(Visitor &v)
	{ v.visitFor(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("FOR def = %d\n", (int)preCalc.size());
		for (size_t i = 0; i < preCalc.size(); i++)
			preCalc[i]->printTo(tab+1);
		if (init != nullptr)
			init->printTo(tab+1);
		else puts("<empty>");
		if (expr != nullptr)
			expr->printTo(tab+1);
		else puts("<empty>");
		if (delta != nullptr)
			delta->printTo(tab+1);
		else puts("<empty>");
		if (stmt != nullptr)
			stmt->printTo(tab+1);
		else puts("<empty>");
	}
};

class Return : public Tree {
public:

	Tree *expr;

	Return()
	{ assert(false); }
	
	Return(const Location &loc, Tree *expr)
	{
		setLoc(loc);
		this->expr = expr;
	}

	~Return()
	{
		if (expr != nullptr)
			delete expr;
	}

	virtual void accept(Visitor &v)
	{ v.visitReturn(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("RETURN\n");
		if (expr != nullptr)
			expr->printTo(tab+1);
	}
};

class Putchar : public Tree {
public:

	Tree *expr;

	Putchar()
	{ assert(false); }
	
	Putchar(const Location &loc, Tree *expr)
	{
		setLoc(loc);
		this->expr = expr;
	}

	~Putchar()
	{
		delete expr;
	}

	virtual void accept(Visitor &v)
	{ v.visitPutchar(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++) putchar(' ');
		printf("PUTCHAR\n");
		if (expr != nullptr)
			expr->printTo(tab+1);
	}
};

class Constant : public Expr {

public:
	int dat;

	Constant()
	{
		assert(false);
	}

	Constant(int dat)
	{
		init();
		this->dat = this->valueConst = dat;
		this->isReplaced = false;
		this->isConst = true;
	}

	~Constant()
	{
		//cerr << "CONSTANT " << this << endl;
	}
	
	virtual void accept(Visitor &v)
	{ v.visitConstant(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Constant : %d\n", dat);
	}
};

class PreCalc : public Tree {

public:
	
	Tree *expr;
	Constant *that;

	PreCalc()
	{ assert(false); }

	PreCalc(const Location &loc, Tree *expr, Constant *that)
	{
		setLoc(loc);
		this->expr = expr;
		this->that = that;
	}

	virtual ~PreCalc()
	{
		if (expr != nullptr)
			delete expr;
	}
	
	virtual void accept(Visitor &v)
	{ v.visitPreCalc(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("PreCalculater\n");
		expr->printTo(tab+1);
		that->printTo(tab+1);
	}
};

// pre calc for translater
class PreCalcCC : public Tree {

public:
	
	Tree *expr;
	int idInScope;

	PreCalcCC()
	{ assert(false); }

	PreCalcCC(const Location &loc, Tree *expr, int idInScope)
	{
		setLoc(loc);
		this->expr = expr;
		this->idInScope = idInScope;
	}

	virtual ~PreCalcCC()
	{
		if (expr != nullptr)
			delete expr;
	}
	
	virtual void accept(Visitor &v)
	{ v.visitPreCalcCC(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("PreCalculaterCC id = %d\n", idInScope);
		expr->printTo(tab+1);
	}
};

class PreCalcIndex : public Tree {

public:

	Tree *expr;
	int idInScope;
	int mul;
	bool clean;
	
	PreCalcIndex()
	{ assert(false); }

        PreCalcIndex(const Location &loc, Tree *expr, int idInScope, int mul, bool clean = false)
	{
		setLoc(loc);
		this->expr = expr;
		this->idInScope = idInScope;
		this->mul = mul;
		this->clean = clean;
	}

	virtual ~PreCalcIndex()
	{
	}
	
	virtual void accept(Visitor &v)
	{ v.visitPreCalcIndex(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("PreCalcIndex id = %d, mul = %d\n", idInScope, mul);
		expr->printTo(tab+1);
	}
};

class Load : public Expr {

public:
	int uuId;
	uint idScope;
	int idInScope;

	Load(int uuId, uint idScope, int idInScope)
	{
		init();
		this->uuId = uuId;
		this->idScope = idScope;
		this->idInScope = idInScope;
		this->isConst = false;
	}

	virtual ~Load()
	{ }
	
	virtual void accept(Visitor &v)
	{ v.visitLoad(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Load(weight = %lld) (%d, %d) : uuId = %d\n", weight, idScope, idInScope, uuId);
	}
};

class Null : public Tree {

public:

	Null()
	{ }

	virtual ~Null()
	{ }
	
	virtual void accept(Visitor &v)
	{ }

	virtual void printTo(int tab)
	{ }
} ;

class Assigner : public Tree {

public:

	vector<uint> idScope;
	vector<int> idInScope;
	vector<int> uuId;
	vector<Tree*> def;
	
	Assigner()
	{ init(); }

	virtual ~Assigner()
	{
		for (int i = 0; i < size; i++)
			delete def[i];
	}

	inline void push(int id, int idS, int uuid, Tree* t)
	{
		idScope.push_back(id);
		idInScope.push_back(idS);
		uuId.push_back(uuid);
		def.push_back(t);
	}

	virtual void accept(Visitor &v)
	{ v.visitAssigner(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Assigner\n");
	}
};

class Break : public Tree {

public:

	Break(Location loc)
	{ setLoc(loc); }

	virtual ~Break()
	{}

	virtual void accept(Visitor &v)
	{ v.visitBreak(this); }

	virtual void printTo(int tab)
	{
		for (int i = 0; i < tab; i++)
			putchar(' ');
		printf("Break\n");
	}
};

