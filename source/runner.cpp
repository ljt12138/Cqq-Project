#include "semvalue.h"

class RunVisitor : public Visitor {

	vector<int> pool;
	int top;
	int begin;
	
	struct state {

		int retv;
		bool finished;
		int begin;
	        
	};

	vector<state> funStack;
	state *fp;
	
	SemValue value;

	int retv;
	
	inline state& up()
	{ return funStack[funStack.size()-1]; }

	inline int boolean(int dat)
	{ return dat != 0; }

	inline int getPosition(size_t idScope, size_t idInScope)
	{
		return (idScope & begin) + idInScope;
	}

	inline void newFun()
	{
		funStack.push_back((state){0, false, begin = top});
		fp = &up();
	}

	inline void endFun()
	{
		funStack.pop_back();
		fp = &up();
		begin = fp->begin;
	}

	inline void execFun(FunDef *fun, int *args, size_t len)
	{
		newFun();
		top += fun->totSiz;
		pool.resize(top);
		size_t cnt = min(fun->size, len);
		for (size_t i = 0; i < cnt; i++) {
			pool[getPosition(fun->arg[i].idScope, fun->arg[i].idInScope)] = args[i];
		}
		fun->stmt->accept(*this);
		value = SemValue(fp->retv, -1);
		top -= fun->totSiz;
		pool.resize(top);
		endFun();
	}

public:

	inline void openGlobal(int size)
	{
		top = size;
	}
	
	RunVisitor()
	{
		top = 0;
	}

	virtual void visitTopLevel(TopLevel *that)
	{
		newFun();
		pool.resize(top);
		size_t cnt = that->lst.size();
		for (size_t i = 0; i < cnt; i++) 
			that->lst[i]->accept(*this);
	}

	virtual void visitVarDef(VarDef *that)
	{
		assert(false);
	}

	virtual void visitFunDef(FunDef *that)
	{
		if (that->isMain) {
			int *a = new int[1];
			a[0] = 0;
			execFun(that, a, 0);
			delete[] a;
		}
	}

	virtual void visitBlock(Block *that)
	{
		size_t cnt = that->lst.size();
		for (size_t i = 0; i < cnt; i++) {
			that->lst[i]->accept(*this);
			if (fp->finished) 
			        break;
		}
	}

	virtual void visitBinaryOp(BinaryOp *that)
	{
#ifndef UOJ98
		switch(that->type) {
		case LOR:
			value.retv = (that->left->accept(*this), value.retv) || (that->right->accept(*this), value.retv);
			return; 
		case LAND:
			value.retv = (that->left->accept(*this), value.retv) && (that->right->accept(*this), value.retv);
			return; 
		}
#endif
	
		SemValue left, right;	
		that->left->accept(*this);
		left = value;
		that->right->accept(*this);
		right = value;
		
		switch(that->type) {
		case '+':
			value.retv = left.retv + right.retv;
			break;

		case '-':
			value.retv = left.retv - right.retv;
			break;

		case '*':
			value.retv = left.retv * right.retv;
			break;

		case '/':
			value.retv = left.retv / right.retv;
			break;

		case '<':
			value.retv = left.retv < right.retv;
			break;

		case '>':
			value.retv = left.retv > right.retv;
			break;

		case LE:
			value.retv = left.retv <= right.retv;
			break;

		case GE:
			value.retv = left.retv >= right.retv;
			break;

		case NEQ:
			value.retv = left.retv != right.retv;
			break;

		case EQ:
			value.retv = left.retv == right.retv;
			break;

#ifdef UOJ98
		case LAND:
			value.retv = left.retv && right.retv;
			break;

		case LOR:
			value.retv = left.retv || right.retv;
			break;
#endif
			
		case '=':
			
			value.retv = right.retv;
			
#ifdef safe
			if (left.pos < 0 || left.pos >= top)
				killer.issueError(SegmentError(that->getLoc()));
#endif
			pool[left.pos] = right.retv;
			break;

		case '^':
			value.retv = left.retv ^ right.retv;
			break;

		case '%':
			value.retv = left.retv % right.retv;
			break;

		case '!':
			value.retv = !right.retv;
			break;
		}
	}

	virtual void visitCinOp(CinOp *that)  
	{
		size_t cnt = that->vars.size();
		for (size_t i = 0; i < cnt; i++) {
			that->vars[i]->accept(*this);
#ifdef safe
			if (value.pos < 0 || value.pos >= top)
				killer.issueError(SegmentError(that->getLoc()));
#endif
			pool[value.pos] = sysIO->read();
		}
		value = SemValue(0, -1);
	}

	virtual void visitCoutOp(CoutOp *that)  
	{
		size_t cnt = that->expr.size();
		for (size_t i = 0; i < cnt; i++) {
			that->expr[i]->accept(*this);
			if (value.pos == -2)
				sysIO->put('\n');
			else 
				sysIO->print(value.retv);
		}
		value = SemValue(0, -1);
	}

	virtual void visitIdent(Ident *that)  
	{
		value = SemValue(0, -2);
	}

	virtual void visitExec(Exec *that)  
	{
		size_t cnt = that->args.size();
	        int args[cnt?cnt:1];
		for(size_t i = 0; i < cnt; i++) {
			that->args[i]->accept(*this);
			args[i] = value.retv;
		}
		execFun(that->target, args, cnt);
	}

	virtual void visitArrayAt(ArrayAt *that)  
	{
		size_t cnt = that->at.size();
		int c = 0;
		for (size_t i = 0; i < cnt; i++) {
			that->at[i]->accept(*this);
			c += value.retv * that->indexSuf[i];
		}
		int pos = getPosition(that->idScope, that->idInScope) + c;
			
#ifdef safe
		if (pos < 0 || pos >= top)
			killer.issueError(SegmentError(that->getLoc()));
#endif

		value = SemValue(pool[pos], pos);
	}

	virtual void visitIf(If *that)  
	{
		that->expr->accept(*this);
		if (value.retv) {
			if (that->trueBranch != nullptr) 
				that->trueBranch->accept(*this);
		} else {
			if (that->falseBranch != nullptr)
				that->falseBranch->accept(*this);
		}
	}

	virtual void visitWhile(While *that)  
	{
		for (size_t i = 0, cnt = that->preCalc.size(); i < cnt; i++)
			that->preCalc[i]->accept(*this);
		while (!fp->finished && (that->expr->accept(*this), value.retv) ) 
			that->stmt->accept(*this);
	}

	virtual void visitFor(For *that)  
	{
	        if (that->init != nullptr)
			that->init->accept(*this);
		for (size_t i = 0, cnt = that->preCalc.size(); i < cnt; i++)
			that->preCalc[i]->accept(*this);
		for (; !fp->finished && (that->expr->accept(*this), value.retv); that->delta->accept(*this)) 
			that->stmt->accept(*this);
	}

	virtual void visitReturn(Return *that)  
	{
		that->expr->accept(*this);
		fp->finished = true;
		fp->retv = value.retv;
	}

	virtual void visitPutchar(Putchar *that)
	{
		that->expr->accept(*this);
		sysIO->put(value.retv);
	}

	virtual void visitConstant(Constant *that)
	{
		value.retv = that->dat;
	}

	virtual void visitPreCalc(PreCalc *that)
	{
		that->expr->accept(*this);
		that->that->dat = value.retv;
	}

	virtual void visitLoad(Load *that)
	{
		int pos = getPosition(that->idScope, that->idInScope);
		value = SemValue(pool[pos], pos);
	}

	virtual void visitAssigner(Assigner *that)
	{
		for (int i = 0; i < that->size; i++)
			pool[getPosition(that->idScope[i], that->idInScope[i])] = (that->def[i]->accept(*this), value.retv);
	}
} runVisitor;
