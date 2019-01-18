/*
 * For Type Checking & Var Marking
 */

class CallFlow;

class TypeVisitor : public Visitor {

	ScopeStack scopeStack;
	Report report, warner;
	Scope<FunDef*> functionScope;

	// Variable Id Counter
	uint idCounter;

	// const variable list
	unordered_map<int, int> constList;

	// Function Id Counter
	int funcIdCounter;

	// Global Variable Id Counter
	int globalIdCounter;

	// current function id
	int currentFunc;

	// mark if current ident is left value
	int leftId;

	// size
	uint currentSize;

	// current id;
	uint currentId;

	// number of loops
	uint loopId;
	
	inline void newVar(const Location &loc, Var &cur)
	{
		if (scopeStack.count(cur.name, false)) {
			report.issueError(VariableDefined(loc, cur.name));
		} else {
			if (cur.type != "array") {
				cur.isLeftValue = true;
				cur.uuId = idCounter++;
				cur.idScope = (currentFunc == -1 ? GLOBAL : LOCAL);
				cur.idInScope = currentId++;
				currentSize = max(currentSize, currentId);
				scopeStack.create(cur.name, cur);
				if (currentFunc == -1) {
					globalId[cur.uuId] = globalIdCounter++;
				}				
			} else {
				cur.index.resize(cur.index_tree.size());
				int flag = true;
				for (size_t j = 0; j < cur.index_tree.size(); j++) {
					cur.index_tree[j]->accept(*this);
					check(cur.index_tree[j]);
					if (!cur.index_tree[j]->isConst) {
						report.issueError(ArrayLengthNonConst(loc, cur.name));
						flag = false;
						break;
					}
					if (cur.index_tree[j]->valueConst == 0) {
						report.issueError(ArrayZeroLength(loc, cur.name));
						flag = false;
						break;
					}
					cur.index[j] = cur.index_tree[j]->valueConst;
					delete cur.index_tree[j];
					cur.index_tree[j] = nullptr;
				}
				if (flag) {
					cur.checkSize();
				}
				cur.isLeftValue = false;
				cur.uuId = idCounter++;
				cur.idScope = (currentFunc == -1 ? GLOBAL : LOCAL);
				cur.idInScope = currentId;
				currentId += cur.size;
				currentSize = max(currentSize, currentId);
				scopeStack.create(cur.name, cur, cur.size);
				if (currentFunc == -1) {
					globalId[cur.uuId] = globalIdCounter++;
				}
			}
		}
	}

	inline void useVar(const Location &loc, Var &cur)
	{
		if (!scopeStack.count(cur.name)) {
			report.issueError(VariableOrFunctionNotDefined(loc, cur.name));
		} else {
			cur = scopeStack.lookup(cur.name);
			cur.def = nullptr;
		        if (cur.type == "array") {
				report.issueError(ArrayInExpr(loc, cur.name));
			} else {
				// current function use global variable
				// add edge in graph
				if (cur.idScope == GLOBAL && leftId && currentFunc != -1)
					callFlow->addPoint(currentFunc, globalId[cur.uuId]);
			}
		}
	}

	inline void useArray(const Location &loc, const string &name, ArrayAt *arrayAt)
	{
		if (!scopeStack.count(name)) {
			report.issueError(VariableOrFunctionNotDefined(loc, name));
		} else {
			arrayAt->var = scopeStack.lookup(name);
			arrayAt->var.def = nullptr;
			if (arrayAt->var.type != "array")
				report.issueError(VariableIsNotArray(loc, name));
			else if (arrayAt->at.size() != arrayAt->var.index.size()) {
				report.issueError(ArrayLengthError(loc, name));
			} else {
				arrayAt->idScope = arrayAt->var.idScope;
				arrayAt->idInScope = arrayAt->var.idInScope;
				Var &cur = arrayAt->var;
				if (arrayAt->idScope == GLOBAL && leftId && currentFunc != -1) {
					callFlow->addPoint(currentFunc, globalId[cur.uuId]);
				}
			} 
		}
	}

	inline void check(Tree* &that)
	{
		if (that == nullptr)
			that = new Constant(0);
		else {
			// memory
			Tree *tmp = that;
			if (that->isReplaced) {
				that = that->valueReplaced;
				if (that == nullptr)
					that = new Constant(0);
				delete tmp;
				tmp = nullptr;
			}
		}
	}

	inline void setDepth(Tree *that)
	{
		that->depth = scopeStack.getDepth();
	}
	
public:

	// call flow graph
	CallFlow *callFlow;

	// UUID -> GlobalId
	unordered_map<int, int> globalId;
	
	TypeVisitor()
	{
		funcIdCounter = globalIdCounter = idCounter = 0;
		currentFunc = -1;
		leftId = 0;
		loopId = 0;
	}

	virtual void visitTree(Tree *that) 
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that) 
	{
		setDepth(that);
		scopeStack.open();
		for (size_t i = 0; i < that->lst.size(); i++) {
			that->lst[i]->accept(*this);
			check(that->lst[i]);
		}
		scopeStack.close();
	}

	virtual void visitVarDef(VarDef *that) 
	{
		setDepth(that);
		that->totSiz= 0;
		for (size_t i = 0; i < that->vars.size(); i++) {
			newVar(that->getLoc(), that->vars[i]);
			that->totSiz += that->vars[i].size;
			if (that->vars[i].def != nullptr) {
				that->vars[i].def->accept(*this);
				check(that->vars[i].def);
				// fuck you here
				// that->vars[i].def->printTo(0);
			}
			if (that->constVar && that->vars[i].def != nullptr) {
				if (!that->vars[i].def->isConst)
					report.issueError(ConstExprError(that->getLoc()));
			        constList[that->vars[i].uuId] = that->vars[i].def->valueConst;
				// fuck you here
			}
		}
		if (that->constVar) {
			that->isReplaced = true;
			that->valueReplaced = new Null;
			for (size_t i = 0; i < that->vars.size(); i++)
				if (that->vars[i].def != nullptr)
					delete that->vars[i].def;
		} else {
			Assigner *re = new Assigner;
			for (size_t i = 0; i < that->vars.size(); i++) {
				if (that->vars[i].def != nullptr) {
					re->push(that->vars[i].idScope, that->vars[i].idInScope, that->vars[i].uuId, that->vars[i].def);
				}
			}
			that->isReplaced = true;
			that->valueReplaced = re;
			re->size = re->def.size();
		}
	}

	virtual void visitFunDef(FunDef *that) 
	{
		currentFunc = that->funcId = funcIdCounter++;
		int pre = currentSize;
		currentSize = currentId = 0;
		setDepth(that);
		if (functionScope.count(that->name))
			report.issueError(FunctionDefined(that->getLoc(), that->name));
		else {
			functionScope.create(that->name, that);
			scopeStack.open();
			for (size_t i = 0; i < that->arg.size(); i++) 
				newVar(that->getLoc(), that->arg[i]);
			for (size_t i = 0; i < that->arg.size(); i++)
				useVar(that->getLoc(), that->arg[i]);
			if (that->stmt != nullptr) 
				that->stmt->accept(*this);
			scopeStack.close();
		}
		that->totSiz = currentSize;
		currentFunc = -1;
		currentSize = currentId = pre;
	}

	virtual void visitBlock(Block *that)
	{
		setDepth(that);
		scopeStack.open();
		for (size_t i = 0; i < that->lst.size(); i++) {
			that->lst[i]->accept(*this);
			check(that->lst[i]);
		}
		currentId -= scopeStack.topSize();
		scopeStack.close();
	}
	
	virtual void visitIf(If *that)
	{
		setDepth(that);
		that->expr->accept(*this);
		check(that->expr);
		if (that->expr->isConst) {
			if (that->expr->valueConst) {
				that->trueBranch->accept(*this);
				check(that->trueBranch);
				delete that->falseBranch;
				that->falseBranch = nullptr;
				delete that->expr;
				that->expr = nullptr;
				that->isReplaced = true;
				that->valueReplaced = that->trueBranch;
			} else {
				if (that->falseBranch != nullptr) {
					that->falseBranch->accept(*this);
					check(that->falseBranch);
				}
				delete that->trueBranch;
				that->trueBranch = nullptr;
				delete that->expr;
				that->expr = nullptr;
				that->isReplaced = true;
				that->valueReplaced = that->falseBranch;
			}
		} else {
			that->trueBranch->accept(*this);
			check(that->trueBranch);
			if (that->falseBranch != nullptr) {
				that->falseBranch->accept(*this);
				check(that->falseBranch);
			}
		}
	}

	virtual void visitWhile(While *that)
	{
		loopId++;
		setDepth(that);
		that->expr->accept(*this);
		check(that->expr);
		if (that->expr->isConst && !that->expr->valueConst) {
			check(that->stmt);
			that->isReplaced = true;
			that->valueReplaced = new Constant(0);
		} else {
			if (that->stmt != nullptr) {
				that->stmt->accept(*this);
				check(that->stmt);
			}
		}
		loopId--;
	}

	// bug : for is empty & init is not null
	// bug fixed
	
	virtual void visitFor(For *that)
	{
		loopId++;
		setDepth(that);
		scopeStack.open();
		if (that->init != nullptr) {
			that->init->accept(*this);
			check(that->init);
		}
		that->expr->accept(*this);
		check(that->expr);
		if (that->expr->isConst && !that->expr->valueConst) {
			that->isReplaced = true;
			that->valueReplaced = that->init;
		} else {
			if (that->delta != nullptr) {
				that->delta->accept(*this);
				check(that->delta);
			}
			if (that->stmt != nullptr) {
				that->stmt->accept(*this);
				check(that->stmt);
			}
		}
		currentId -= scopeStack.topSize();
		scopeStack.close();
		loopId++;
	}

	virtual void visitReturn(Return *that)
	{
		setDepth(that);
		that->expr->accept(*this);
		check(that->expr);
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
		setDepth(that);
		if (that->type == '=')
			leftId++;
		that->left->accept(*this);
		if (that->type == '=')
			leftId--;
		check(that->left);
		that->right->accept(*this);
		check(that->right);
		assert(that->left != nullptr);
		assert(that->right != nullptr);
		if (that->left->isConst && that->right->isConst) {
			int left = that->left->valueConst, right = that->right->valueConst, value = 0;
			calcBinaryOp(that, left, right, value);
			that->isConst = true;
			that->valueConst = value;
		}
#ifndef UOJ98
		else if (that->left->isConst) {
			try_left(that);
		} else if (that->right->isConst) {
			try_right(that);
		}
#endif
		if (that->isConst) {
			if (that->isReplaced)
				delete that->valueReplaced;
		        that->isReplaced = true;
			that->valueReplaced = new Constant(that->valueConst);
		}
	}

private:

	inline void calcBinaryOp(BinaryOp *that, int left, int right, int &value)
	{
		switch(that->type) {
		case '+':
			value = left + right;
			break;

		case '-':
			value = left - right;
			break;

		case '*':
			value = left * right;
			break;

		case '/':
			if (right == 0) {
				report.issueError(DividedByZero(that->getLoc()));
			} else {
				value = left / right;
			}
			break;

		case '<':
			value = left < right;
			break;

		case '>':
			value = left > right;
			break;

		case LE:
			value = left <= right;
			break;

		case GE:
			value = left >= right;
			break;

		case NEQ:
			value = left != right;
			break;

		case EQ:
			value = left == right;
			break;

		case LAND:
			value = left && right;
			break;

		case LOR:
			value = left || right;
			break;
			
		case '=':
			report.issueError(BadAssignError(that->getLoc()));
			value = right;
			break;

		case '^':
			value = left ^ right;
			break;

		case '%':
			if (right == 0) {
				report.issueError(DividedByZero(that->getLoc()));
			} else {
				value = left / right;
			}
			break;
				
		case '!':
			value = !right;
			break;
		}
	}
	
	inline void try_left(BinaryOp *that)
	{
		switch (that->type) {
		case '+':
			if (that->left->valueConst == 0) {
				that->isReplaced = true;
				delete that->left;
				that->left = nullptr;
				that->valueReplaced = that->right;
			}
			break;
		case '*':
			if (that->left->valueConst == 1) {
				that->isReplaced = true;
				delete that->left;
				that->left = nullptr;
				that->valueReplaced = that->right;
				break;
			}
		case '%':
		case '/':
			if (that->left->valueConst == 0) {
				warner.issueError(DirtyConstantOperation(that->getLoc()));
			}
		case LAND:
			if (that->left->valueConst == 0) {
				that->isConst = true;
				that->valueConst = 0;
			} else if (that->type == LAND) {
				that->isReplaced = true;
				delete that->left;
				that->left = nullptr;
				that->valueReplaced = that->right;
			}
			break;
		case LOR:
			if (that->left->valueConst == 1 || (that->left->valueConst && that->type == LOR)) {
				that->isConst = true;
				that->valueConst = 1;
			} else if (that->left->valueConst == 0 && that->type == LAND) {
				that->isReplaced = true;
				delete that->left;
				that->left = nullptr;
				that->valueReplaced = that->right;
			}
			break;
		}
	}

	inline void try_right(BinaryOp *that)
	{
		assert(that->left != nullptr);
		assert(that->right != nullptr);
		switch(that->type) {
		case '+':
		case '-':
			if (that->right->valueConst == 0) {
				that->isReplaced = true;
				delete that->right;
				that->right = nullptr;
				that->valueReplaced = that->left;
				//break;
			}
			break;
		case '*':
			if (that->right->valueConst == 1) {
				that->isReplaced = true;
				delete that->right;
				that->right = nullptr;
				that->valueReplaced = that->left;
				break;
			}
		case LAND:
			if (that->right->valueConst == 0) {
				warner.issueError(DirtyConstantOperation(that->getLoc()));
				that->isConst = true;
				that->valueConst = 0;
			}
			break;
		case '%':			
			if (that->right->valueConst == 1) {
				warner.issueError(DirtyConstantOperation(that->getLoc()));
				that->isConst = true;
				that->valueConst = 0;
				break;
			} 
		case '/':
			if (that->right->valueConst == 1) {
				that->isReplaced = true;
				delete that->right;
				that->right = nullptr;
				that->valueReplaced = that->left;
				break;
			} 
			if (that->right->valueConst == 0) {
				report.issueError(DividedByZero(that->getLoc()));
			}
			break;
		case LOR:
			warner.issueError(DirtyConstantOperation(that->getLoc()));
			if (that->right->valueConst == 1) {
				that->isConst = true;
				that->valueConst = 1;
			}
			break;				
		}
	}
	
public:
	
	virtual void visitCinOp(CinOp *that)
	{
		setDepth(that);
		leftId++;
		for (size_t i = 0; i < that->vars.size(); i++) {
			that->vars[i]->accept(*this);
			check(that->vars[i]);
		}
		leftId--;
	}

	virtual void visitCoutOp(CoutOp *that)
	{
		setDepth(that);
		for (size_t i = 0; i < that->expr.size(); i++) {
			that->expr[i]->accept(*this);
			check(that->expr[i]);
		}
	}

	virtual void visitIdent(Ident *that)
	{
		setDepth(that);
		if (that->type == KEYWORD) {
			that->var = Var("int", that->name);
			useVar(that->getLoc(), that->var);
			// check if variable is const
			if (constList.count(that->var.uuId)) {
				that->isConst = true;
				that->valueConst = constList[that->var.uuId];
				that->isReplaced = true;
				that->valueReplaced = new Constant(that->valueConst);
			} else {
				that->isConst = false;
				that->isReplaced = true;
				that->valueReplaced = new Load(that->var.uuId, that->var.idScope, that->var.idInScope);
			}
		} else if (that->type == INT) {
			that->isConst = true;
			that->valueConst = that->dat;
			that->isReplaced = true;
			that->valueReplaced = new Constant(that->dat);
		} 
	}

	virtual void visitExec(Exec *that)
	{
		setDepth(that);
		if (!functionScope.count(that->name))
			report.issueError(VariableOrFunctionNotDefined(that->getLoc(), that->name));
		else that->target = functionScope.lookup(that->name);
		// add edge in call graph
		if (currentFunc != -1)
			callFlow->addEdge(currentFunc, that->target->funcId);
		for (size_t i = 0; i < that->args.size(); i++) {
			that->args[i]->accept(*this);
			check(that->args[i]);
		}
	}

	virtual void visitArrayAt(ArrayAt *that)
	{
		setDepth(that);
		useArray(that->getLoc(), that->name, that);
		int pre = leftId;
		leftId = 0;
		for (size_t i = 0; i < that->at.size(); i++) {
			that->at[i]->accept(*this);
			check(that->at[i]);
		}
		leftId = pre;
		that->fixIndexSuf();
	}
	
	virtual void visitPutchar(Putchar *that)
	{
		setDepth(that);
		that->expr->accept(*this);
		check(that->expr);
	}

	virtual void visitBreak(Break *that)
	{
		setDepth(that);
		if (!loopId)
			report.issueError(BreakNotInLoop(that->getLoc()));
	}
	
	inline bool isError()
	{ return report.isError(); }

	inline int getSize()
	{ return idCounter; }

	inline int getGlobalSize()
	{ return currentSize; }

	inline int getFuncNumber()
	{ return funcIdCounter; }


} typeVisitor;
