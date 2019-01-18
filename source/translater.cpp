#include "visitor.h"

// TODO : get position 

class TransVisitor : public Visitor {

	SimpleVM::instruction** *entrance;

	int leftId;
	int eol;
	int mainId;
	int mainSize;
	stack<SimpleVM::branch*> breakList;
	
	inline void optimizeSave()
	{
		using namespace SimpleVM;
		if (ptop-1 == program)
			return;
		pushimm *last = dynamic_cast<pushimm*>(*(ptop-2));
		if (last != nullptr) {
		        ptop--;
			delete *ptop;
			int dat = last->value;
			int id = last->id;
		        delete *(ptop-1);
			saveto *p = new saveto(dat);
			p->id = id;
			*(ptop-1) = p;
		}
	}

	inline void optimizeSaveAbs()
	{
		using namespace SimpleVM;
		if (ptop-1 == program)
			return;
		pushimm *last = dynamic_cast<pushimm*>(*(ptop-2));
		if (last != nullptr) {
		        ptop--;
			delete *ptop;
			int dat = last->value;
			int id = last->id;
		        delete *(ptop-1);
			saveabsto *p = new saveabsto(dat);
			p->id = id;
			*(ptop-1) = p;
		}
	}

	inline void optimizeSavePop()
	{
		using namespace SimpleVM;
		if (ptop-1 == program)
			return;
		pushimm *last = dynamic_cast<pushimm*>(*(ptop-2));
		if (last != nullptr) {
		        ptop--;
			delete *ptop;
			int dat = last->value;
			int id = last->id;
		        delete *(ptop-1);
			savetopop *p = new savetopop(dat);
			p->id = id;
			*(ptop-1) = p;
		}
	}
	
	inline void optimizeSaveAbsPop()
	{
		using namespace SimpleVM;
		if (ptop-1 == program)
			return;
		pushimm *last = dynamic_cast<pushimm*>(*(ptop-2));
		if (last != nullptr) {
		        ptop--;
			delete *ptop;
			int dat = last->value;
			int id = last->id;
			delete *(ptop-1);
			saveabstopop *p = new saveabstopop(dat);
			p->id = id;
			*(ptop-1) = p;
		}
	}
	
	inline void checkExpr(Tree *T)
	{
		using namespace SimpleVM;
	        Expr *p = dynamic_cast<Expr*>(T);
		if (p != nullptr) {
			instruction *last = *(ptop-1);
			inplace *inc = dynamic_cast<inplace*>(*(ptop-1));			
			/* merge save - pop */
			save *ind = dynamic_cast<save*>(*(ptop-1));
			saveabs *ine = dynamic_cast<saveabs*>(*(ptop-1));
			saveto *inf = dynamic_cast<saveto*>(*(ptop-1));
			saveabsto *ing = dynamic_cast<saveabsto*>(*(ptop-1));
			purepush *inh = dynamic_cast<purepush*>(*(ptop-1));
			affine *ini = dynamic_cast<affine*>(*(ptop-1));
			addto *inj = dynamic_cast<addto*>(*(ptop-1));
			multo *ink = dynamic_cast<multo*>(*(ptop-1));
			if (inc != nullptr || inh != nullptr) {
				delete *(ptop-1);
				ptop--;
			} else if (ind != nullptr) {
				*(ptop-1) = new savepop;
				(*(ptop-1))->id = last->id;
				delete last;
				optimizeSavePop();
			} else if (ine != nullptr) {
				*(ptop-1) = new saveabspop;
				(*(ptop-1))->id = last->id;
				delete last;
				optimizeSaveAbsPop();
			} else if (inf != nullptr) {
				*(ptop-1) = new savetopop(inf->pos);
				(*(ptop-1))->id = last->id;
				delete last;
			} else if (ing != nullptr) {
				*(ptop-1) = new saveabstopop(ing->pos);
				(*(ptop-1))->id = last->id;
				delete last;
			} else if (ini != nullptr) {
				*(ptop-1) = new affinepop(ini->pos, ini->k, ini->b);
				(*(ptop-1))->id = last->id;
				delete last;
			} else if (inj != nullptr) {
				*(ptop-1) = new addbtopop(inj->pos, inj->b);
				(*(ptop-1))->id = last->id;
				delete last;
			} else if (ink != nullptr) {
				*(ptop-1) = new mulktopop(ink->pos, ink->k);
				(*(ptop-1))->id = last->id;
				delete last;
			} else {
				append(new pop);	
			}
		}
	}

	inline void genAddOn(int dat)
	{
		using namespace SimpleVM;
		if (dat == 1)
			append(new incr);
		else if (dat == -1)
			append(new decr);
		else if (dat != 0)
			append(new addon(dat));
	}

	////////////////////////////

	inline void genAdd()
	{
		using namespace SimpleVM;
		mulon *inc = dynamic_cast<mulon*>(*(ptop-1));
		mul *ind = dynamic_cast<mul*>(*(ptop-1));
		loadfrom *ine = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *inf = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (ine != nullptr) { // loadfrom
			int pos = ine->pos;
			delete *(ptop-1);
			ptop--;
			append(new addfromload(pos));
		} else if (inf != nullptr) { // loadfrom
			int pos = inf->pos;
			delete *(ptop-1);
			ptop--;
			append(new addfromloadabs(pos));
		} else if (inc != nullptr) {
			int value = inc->value;
			delete *(ptop-1);
			ptop--;
			append(new leap(value));
		} else if (ind != nullptr) { 
			delete *(ptop-1);
			ptop--;
			append(new leapcxy);
		} else {
			append(new add);
		}
	}

	inline void genMul()
	{
		using namespace SimpleVM;
		loadfrom *ine = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *inf = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (ine != nullptr) {
			int pos = ine->pos;
			delete *(ptop-1);
			ptop--;
			append(new mulfromload(pos));
		} else if (inf != nullptr) { // loadfrom
			int pos = inf->pos;
			delete *(ptop-1);
			ptop--;
			append(new mulfromloadabs(pos));
		}  else {
			append(new mul);
		}
	}

	inline void genSub()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new subfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new subfromloadabs(pos));
		} else {
			append(new sub);
		}
	}

	inline void genBSub()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new bsubfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new bsubfromloadabs(pos));
		} else {
			append(new bsub);
		}
	}

	inline void genDiv()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new divfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new divfromloadabs(pos));
		} else {
			append(new xdiv);
		}
	}

	inline void genBDiv()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new bdivfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new bdivfromloadabs(pos));
		} else {
			append(new bdiv);
		}
	}

	inline void genMod()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new modfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new modfromloadabs(pos));
		} else {
			append(new mod);
		}
	}

	inline void genBMod()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new bmodfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new bmodfromloadabs(pos));
		} else {
			append(new bmod);
		}
	}

	inline void genLes()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--;
			append(new lesfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--;
			append(new lesfromloadabs(pos));
		} else {
			append(new les);
		}
	}
	
	inline void genLeq()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new leqfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new leqfromloadabs(pos));
		} else {
			append(new leq);
		}
	}
	
	inline void genGrt()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new grtfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new grtfromloadabs(pos));
		} else {
			append(new grt);
		}
	}

	inline void genGeq()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new geqfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new geqfromloadabs(pos));
		} else {
			append(new geq);
		}
	}
	
	inline void genLxor()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new lxorfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new lxorfromloadabs(pos));
		} else {
			append(new lxor);
		}
	}

	inline void genLnot()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new lnotfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new lnotfromloadabs(pos));
		} else {
			append(new lnot);
		}
	}

	inline void genNeq()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new neqfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new neqfromloadabs(pos));
		} else {
			append(new neq);
		}
	}
	
	inline void genEq()
	{
		using namespace SimpleVM;
		loadfrom *inc = dynamic_cast<loadfrom*>(*(ptop-1));
		loadabsfrom *ind = dynamic_cast<loadabsfrom*>(*(ptop-1));
		if (inc != nullptr) {
			int pos = inc->pos;
			delete *(ptop-1);
			ptop--; append(new eqfromload(pos));
		} else if (ind != nullptr) {
			int pos = ind->pos;
			delete *(ptop-1);
			ptop--; append(new eqfromloadabs(pos));
		} else {
			append(new eq);
		}
	}

	//////////////////////
public:

	TransVisitor(int funId)
	{
		entrance = new SimpleVM::instruction**[funId];
		leftId = 0;
	}

	~TransVisitor()
	{ delete[] entrance; }
	
	virtual void visitTree(Tree *that)
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that)
	{
		using namespace SimpleVM;
		for (size_t i = 0, size = that->lst.size(); i < size; i++) {
			FunDef *p = dynamic_cast<FunDef*>(that->lst[i]);
			if (p != nullptr) {
				that->lst[i]->accept(*this);
			}
		}
	        pc = ptop;
		for (size_t i = 0, size = that->lst.size(); i < size; i++) {
			Assigner *p = dynamic_cast<Assigner*>(that->lst[i]);
			if (p != nullptr) {
				that->lst[i]->accept(*this);
			}
		}
		append(new alloc(mainSize, entrance[mainId] - 1));
	}

	virtual void visitVarDef(VarDef *that)
	{
		assert(false);
	}

	virtual void visitFunDef(FunDef *that)
	{
		using namespace SimpleVM;
#ifdef DEBUGC
		cout << that->name << " " << ptop - program << ",";
#endif
		if (that->isMain) {
			pc = ptop;
			mainId = that->funcId;
			mainSize = that->totSiz;
		}
		entrance[that->funcId] = ptop;
		that->stmt->rightMost = true;
		that->stmt->accept(*this);
	        append(new freespc);
#ifdef DEBUGC
		cout << ptop - program << endl;
#endif 
	}

	virtual void visitBlock(Block *that)
	{
		using namespace SimpleVM;
		for (size_t i = 0, size = that->lst.size(); i < size; i++) {
			that->lst[i]->rightMost = (that->rightMost) && (i == size - 1);
			that->lst[i]->accept(*this);
			if (!that->lst[i]->rightMost) 
				checkExpr(that->lst[i]);
		}
	}
	
	virtual void visitBinaryOp(BinaryOp *that)
	{
		using namespace SimpleVM;
		bnez *q;
		beqz *p;
		switch(that->type) {
		case LOR:
			that->left->accept(*this);
			q = new bnez(nullptr);
		        append(q);
			append(new pop);
			that->right->accept(*this);
			q->tar = ptop - 1;
			return;
		case LAND:
			that->left->accept(*this);
			p = new beqz(nullptr);
			append(p);
			append(new pop);
			that->right->accept(*this);
			p->tar = ptop - 1;
			return;
		}

		Constant *left = dynamic_cast<Constant*>(that->left);
		Constant *right = dynamic_cast<Constant*>(that->right);
		
		if (that->type == '=') {
			if (that->selfupdate) {
				int k = that->k, b = that->b;
				if (k == 1 && b == 0) append(new loadfrom(that->pos));
				else if (k == 1) append(new addto(that->pos, b));
				else if (b == 0) append(new multo(that->pos, k));
				else append(new affine(that->pos, k, b));
				return;
			}
			that->right->accept(*this);
			leftId++;
			that->left->accept(*this);
			leftId--;
			fptopool *p = dynamic_cast<fptopool*>(*(ptop-1));
			if (p != nullptr) {
				instruction** inc = ptop;
				inc--;
				int id = (*inc)->id;
				delete *inc;
				*inc = new save;
				(*inc)->id = id;
				optimizeSave();
			} else {
				append(new saveabs);
				optimizeSaveAbs();
			}
			return;
		}

		// one of the sub-tree is a constant
		
	        if (left != nullptr || right != nullptr) {
			assert(left == nullptr || right == nullptr);
			int dat;
			if (left != nullptr) {
				that->right->accept(*this);
				dat = left->dat;
			} else {
				that->left->accept(*this);
				dat = right->dat;
			}
			switch(that->type) {
			case '+':
				if (dat != 0)
					genAddOn(dat);
				break;
			case '-':
				if (left != nullptr)
					append(new subon(dat));
				else if (dat != 0)
				        genAddOn(-dat);
				break;
			case '*':
				if (dat != 1)
					append(new mulon(dat));
				break;
			case '/':
				if (left != nullptr)
					append(new bdivon(dat));
				else if (dat != 1)
					append(new divon(dat));
				break;
			case '%':
				if (left != nullptr)
					append(new bmodon(dat));
				else append(new modon(dat));
				break;
			case '<':
				if (left != nullptr)
					append(new isgrt(dat));
				else append(new isles(dat));
				break;
			case '>':
				if (left != nullptr)
					append(new isles(dat));
				else append(new isgrt(dat));
				break;
			case LE:
				if (left != nullptr)
					append(new isgeq(dat));
				else append(new isleq(dat));
				break;
			case GE:
				if (left != nullptr)
					append(new isleq(dat));
				else append(new isgeq(dat));
				break;
			case NEQ:
				append(new isneq(dat));
				break;
			case EQ:
				append(new iseq(dat));
				break;
			case '^':
				append(new lxoron(dat));
				break;
			case '!':
				append(new lnot);
				break;
			default: assert(false);
			}
			return; 
		}

		bool rf = false;
		
		if (that->left->weight < that->right->weight) {
			that->right->accept(*this);
			that->left->accept(*this);
			rf = true;
		} else {
			that->left->accept(*this);
			that->right->accept(*this);
		}
		
		switch(that->type) {
		case '+': genAdd();  break;
		case '-': if (rf) genBSub(); else genSub(); break;
		case '*': genMul();  break;
		case '/': if (rf) genBDiv(); else genDiv(); break;
		case '%': if (rf) genBMod(); else genMod(); break;
		case '<': if (rf) genGrt();  else genLes(); break;
		case '>': if (rf) genLes();  else genGrt(); break;
		case LE:  if (rf) genGeq();  else genLeq(); break;
		case GE:  if (rf) genLeq();  else genGeq(); break;
		case NEQ: genNeq();  break;
		case EQ:  genEq();   break;
		case '^': genLxor(); break;
		case '!': assert(false); break;
		default:assert(false);
		}
	}

	virtual void visitCinOp(CinOp *that)
	{
		using namespace SimpleVM;
		for (size_t i = 0; i < that->vars.size(); i++) {
			leftId++;
			that->vars[i]->accept(*this);
			leftId--;
			append(new read);
		}
	}

	virtual void visitCoutOp(CoutOp *that)
	{
		using namespace SimpleVM;
		for (size_t i = 0; i < that->expr.size(); i++) {
			eol = 0;
			that->expr[i]->accept(*this);
			if (!eol) append(new write);
			else append(new put);
		}
	}

	virtual void visitIdent(Ident *that)
	{
		using namespace SimpleVM;
		eol = 1;
		append(new pushimm('\n'));
	}

	virtual void visitExec(Exec *that)
	{
		using namespace SimpleVM;
		if (that->args.size() == 0) {
			append(new alloc(that->target->totSiz, entrance[that->target->funcId] - 1));
		} else {
			append(new topmove);
			for (size_t i = 0; i < that->args.size(); i++) 
				that->args[i]->accept(*this);
			append(new allocparm(that->args.size(), that->target->totSiz, entrance[that->target->funcId] - 1));
		}
	}

	virtual void visitArrayAt(ArrayAt *that)
	{
		using namespace SimpleVM;
		int pre = leftId;
		leftId = 0;
		int cnt = that->at.size();
		if (that->position == -1) {
			that->at[cnt-1]->accept(*this);
			for (size_t i = cnt-2; i+1 != 0; i--) {
				that->at[i]->accept(*this);
				append(new leap(that->indexSuf[i]));
			}
		} else {
			append(new loadfrom(that->position));
			for (size_t i = cnt-1; i+1 != 0; i--) {
				if (!that->pre[i]) {
					that->at[i]->accept(*this);
					if (that->indexSuf[i] != 1)
						append(new leap(that->indexSuf[i]));
					else genAdd();
				} 
			}
		}
		if (!pre) { // get position 
			if (that->idScope == LOCAL) {
				append(new loadshift(that->idInScope));
			} else {
				append(new loadabsshift(that->idInScope));
			}
		} else {
			genAddOn(that->idInScope);
			if (that->idScope == LOCAL)
				append(new fptopool);
		}
		leftId = pre;
	}
	
	virtual void visitIf(If *that)
	{
		using namespace SimpleVM;
		that->expr->accept(*this);
		beqzpop *q = new beqzpop(nullptr);
	        append(q);
		that->trueBranch->accept(*this);
		checkExpr(that->trueBranch);
		if (that->falseBranch != nullptr) {
			branch *b = new branch(nullptr);
			append(b);
			q->tar = ptop - 1;
			that->falseBranch->accept(*this);
			checkExpr(that->falseBranch);
			b->tar = ptop - 1;
		} else {
			q->tar = ptop - 1;
		}
	}

	virtual void visitWhile(While *that)
	{
		using namespace SimpleVM;
		int breakId = breakList.size();
		instruction ** loop = ptop;
		that->expr->accept(*this);
		// pre calc
		for (size_t i = 0; i < that->preCalc.size(); i++)
			that->preCalc[i]->accept(*this);
		beqzpop *q = new beqzpop(nullptr);
		append(q);
		that->stmt->accept(*this);
		checkExpr(that->stmt);
		append(new branch(loop - 1));
		while (breakList.size() > breakId) {
			breakList.top()->tar = ptop - 1;
			breakList.pop();
		}
		q->tar = ptop - 1;
	}

	virtual void visitFor(For *that)
	{
		using namespace SimpleVM;
		int breakId = breakList.size();
		if (that->init != nullptr)
			that->init->accept(*this);
		// pre calc
		for (size_t i = 0; i < that->preCalc.size(); i++)
			that->preCalc[i]->accept(*this);
		instruction ** loop = ptop;
		that->expr->accept(*this);
		beqzpop *q = new beqzpop(nullptr);
		append(q);
		that->stmt->accept(*this);
		checkExpr(that->stmt);
		that->delta->accept(*this);
		checkExpr(that->delta);
		append(new branch(loop - 1));
		while (breakList.size() > breakId) {
			breakList.top()->tar = ptop - 1;
			breakList.pop();
		}
		q->tar = ptop - 1;
	}

	virtual void visitReturn(Return *that)
	{
		using namespace SimpleVM;
		that->expr->accept(*this);
		append(new saveto(-2));
		append(new freespc);
	}

	virtual void visitPutchar(Putchar *that)
	{
		using namespace SimpleVM;
		that->expr->accept(*this);
		append(new put);
	}

	virtual void visitConstant(Constant *that)
	{
		using namespace SimpleVM;
		append(new pushimm(that->dat));
	}

	virtual void visitPreCalcCC(PreCalcCC *that)
	{
		using namespace SimpleVM;
		that->expr->accept(*this);
		append(new savetopop(that->idInScope));
	}

	virtual void visitLoad(Load *that)
	{
		using namespace SimpleVM;
		if (leftId) {
			append(new pushimm(that->idInScope));
			if (that->idScope == LOCAL) 
			        append(new fptopool);
		} else {
			if (that->idScope == LOCAL)
				append(new loadfrom(that->idInScope));
			else append(new loadabsfrom(that->idInScope));
		}
	}

	virtual void visitAssigner(Assigner *that)
	{
		using namespace SimpleVM;
		for (int i = 0; i < that->size; i++) {
			if (that->idScope[i] == LOCAL) {
				that->def[i]->accept(*this);
				append(new savetopop(that->idInScope[i]));
			} else {
				that->def[i]->accept(*this);
				append(new saveabstopop(that->idInScope[i]));
			}
		}
	}

	virtual void visitPreCalcIndex(PreCalcIndex *that)
	{
		using namespace SimpleVM;
		that->expr->accept(*this);
		append(new mulon(that->mul));
		if (that->clean)
			append(new savetopop(that->idInScope));
		else append(new addtopop(that->idInScope));
	}

	virtual void visitBreak(Break *that)
	{
		using namespace SimpleVM;
		branch *b = new branch(nullptr);
		append(b);
		breakList.push(b);
	}
};
