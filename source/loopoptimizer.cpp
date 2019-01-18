class LoopOptimizer : public Visitor {
	
	inline void setPreCalc(Tree* &now)
	{
#ifdef COMPILE
		Load *p = dynamic_cast<Load*>(now);
		// load dont need to pre calc
		if (p != nullptr)
			return;
#endif
#ifndef COMPILE
		if (now->localConst && !now->isConst) {
			Constant *T = new Constant(0);
			pre->preCalc.push_back(new PreCalc(pre->getLoc(),
							   now,
							   T));
			now = T;
		}
#endif

#ifdef COMPILE
		if (now->localConst && !now->isConst) {
			int id = fun->totSiz++;
			Load *T = new Load(-1, LOCAL, id);
			pre->preCalc.push_back(new PreCalcCC(pre->getLoc(),
							     now,
							     id));
			now = T;
		}
#endif
	}

#ifdef COMPILE
	inline void setArrayIndex(ArrayAt *now)
	{
		int cnt = 0;
		for (size_t i = 0; i < now->at.size(); i++) {
			if (now->at[i]->localConst)
				cnt++;
		}
		if (cnt) {
			int id = fun->totSiz++, clean = 1;
			for (size_t i = 0; i < now->at.size(); i++) {
				if (now->at[i]->localConst) {
					pre->preCalc.push_back(new PreCalcIndex(pre->getLoc(),
										now->at[i],
										id,
										now->indexSuf[i],
										clean));
					now->pre[i] = 1;
					clean = 0;
				} else now->pre[i] = 0;
			}
			now->position = id;
		}
	}
#endif
	
public:

	// current function
	FunDef *fun;
	
	// for loop
	Loop *pre;

	// if defined in loop
	vector<int> vis;

	// uuId -> GlobalId
	unordered_map<int,int> *globalId;

	// < WORD_SIZE
	unsigned long long dp;
	
	// >= WORD_SIZE
	boolean optimizeGlobal;	

	// loopVar
	unordered_map<int,int> *loopVar; 
	
	inline void visitTree(Tree *that)
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that) 
	{ assert(false); }

	virtual void visitVarDef(VarDef *that) 
	{
		for (size_t i = 0; i < that->vars.size(); i++) {
			if (that->vars[i].def != nullptr) {
				that->vars[i].def->accept(*this);
				setPreCalc(that->vars[i].def);
			}
		}
	}

	virtual void visitFunDef(FunDef *that)
	{ assert(false); }

	virtual void visitBlock(Block *that)
	{
		for (size_t i = 0; i < that->lst.size(); i++) {
			that->lst[i]->accept(*this);
		}
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
		that->left->accept(*this);
		that->right->accept(*this);
		if (that->type == '=' && that->selfupdate) {
			return;
		} 
		if (!(that->localConst =
		      that->left->localConst && that->right->localConst)) {
		        setPreCalc(that->left);
			setPreCalc(that->right);
		}
	}

	virtual void visitCinOp(CinOp *that)
	{
		for (size_t i = 0; i < that->vars.size(); i++) { 
			that->vars[i]->accept(*this);
			setPreCalc(that->vars[i]);
		}
	}

	virtual void visitCoutOp(CoutOp *that)
	{
		for (size_t i = 0; i < that->expr.size(); i++) {
			that->expr[i]->accept(*this);
			setPreCalc(that->expr[i]);
		}
	}

	virtual void visitIdent(Ident *that)
	{
		switch(that->type) {
		case INT:
			assert(false);
		case KEYWORD:
			assert(false);
		}
	}
	
	virtual void visitExec(Exec *that)  
	{
		// todo pure function
		for (size_t i = 0; i < that->args.size(); i++) {
			that->args[i]->accept(*this);
			setPreCalc(that->args[i]);
		}
		that->localConst = false;
	}

	virtual void visitArrayAt(ArrayAt *that)
	{
		that->position = -1;
		bool flag = true;
		for (size_t i = 0; i < that->at.size(); i++) {
			that->at[i]->accept(*this);
			if (!that->at[i]->localConst)
				flag = false;
		}
		that->localConst = false;
		// todo only find index
		if (flag && !vis[that->var.uuId]) {
			// check global
			if (that->idScope == GLOBAL) {
				int id = globalId->at(that->var.uuId);
				if ((id < WORD_SIZE && (dp >> id & 1) == 0)
				    || (id >= WORD_SIZE && optimizeGlobal)) {
					that->localConst = true;
					return;
				}
			} else if (!vis[that->var.uuId]) {
				that->localConst = true;
				return;
			}
		}
#ifdef COMPILE
		setArrayIndex(that);
#endif
#ifndef COMPILE
		for (size_t i = 0; i < that->at.size(); i++) {
			that->at[i]->accept(*this);
			setPreCalc(that->at[i]);
		}
#endif
	}

	virtual void visitIf(If *that)  
	{
		that->expr->accept(*this);
		setPreCalc(that->expr);
		if (that->trueBranch != nullptr)
			that->trueBranch->accept(*this);
		if (that->falseBranch != nullptr)
			that->falseBranch->accept(*this);
	}

	virtual void visitWhile(While *that)  
	{ }

	virtual void visitFor(For *that)  
	{ }

	virtual void visitReturn(Return *that)  
	{
		that->expr->accept(*this);
		setPreCalc(that->expr);
	}

	virtual void visitPutchar(Putchar *that)
	{
		that->expr->accept(*this);
		setPreCalc(that->expr);
	}

	virtual void visitConstant(Constant *that)
	{
		that->localConst = true;
	}

	virtual void visitLoad(Load *that)
	{
		that->localConst = false;
		// optimized
		if (that->uuId == -1) 
			return;
		if (vis[that->uuId])
			return;
		// global 
		if (that->idScope == GLOBAL) {
			int id = globalId->at(that->uuId);
			that->localConst = (id < WORD_SIZE && (dp >> id & 1) == 0)
				|| (id >= WORD_SIZE && optimizeGlobal);
		} else {
			that->localConst = !vis[that->uuId];
		}
	}

	virtual void visitAssigner(Assigner *that)
	{
		for (size_t i = 0; i < that->idScope.size(); i++) {
			that->def[i]->accept(*this);
			setPreCalc(that->def[i]);
		}
	}

};
