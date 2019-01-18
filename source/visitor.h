#pragma once

class Tree;
class TopLevel;
class VarDef;
class FunDef;
class Block;
class BinaryOp;
class CinOp;
class CoutOp;
class Ident;
class If;
class While;
class For;
class Exec;
class ArrayAt;
class Return;
class Putchar;
class Constant;
class PreCalc;
class Loop;
class Load;
class Assigner;
class PreCalcCC;
class PreCalcIndex;
class Break;

///
/// ::Visitor
///

class Visitor {

public:

	virtual ~Visitor()
        {}
	
	virtual void visitTree(Tree *that)  
	{ assert(false); }

	virtual void visitTopLevel(TopLevel *that)  
	{}

	virtual void visitVarDef(VarDef *that)  
	{}

	virtual void visitFunDef(FunDef *that)  
	{}

	virtual void visitBlock(Block *that)  
	{}

	virtual void visitBinaryOp(BinaryOp *that)  
	{}

	virtual void visitCinOp(CinOp *that)  
	{}

	virtual void visitCoutOp(CoutOp *that)  
	{}

	virtual void visitIdent(Ident *that)  
	{}

	virtual void visitExec(Exec *that)  
	{}

	virtual void visitArrayAt(ArrayAt *that)  
	{}

	virtual void visitIf(If *that)  
	{}

	virtual void visitWhile(While *that)  
	{}

	virtual void visitFor(For *that)  
	{}

	virtual void visitReturn(Return *that)  
	{}

	virtual void visitPutchar(Putchar *that)
	{}

	virtual void visitConstant(Constant *that)
	{}

	virtual void visitPreCalc(PreCalc *that)
	{}

	virtual void visitLoad(Load *that)
	{}

	virtual void visitLoop(Loop *that)
	{ assert(false); }

	virtual void visitAssigner(Assigner *that)
	{}

	virtual void visitPreCalcCC(PreCalcCC *that)
	{}

	virtual void visitPreCalcIndex(PreCalcIndex *that)
	{}

	virtual void visitBreak(Break *that)
	{}
};
