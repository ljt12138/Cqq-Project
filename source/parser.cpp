class Parser {
	vector<Token> token;
	Report report;
	size_t pos;
	Tree *prog;
	SystemIO *systemIO;

	inline Token lookahead()
	{ return token[pos]; }

	inline void match(int id)
	{
		if (pos+1 == 0) return;
		else if (pos >= token.size()) {
			report.issueError(SyntaxError(token[token.size()-1].loc));
			pos = -1;
		} else if (token[pos].type != id) {
			cout << "MATCH " << id << " " << token[pos].type << endl;
			report.issueError(SyntaxError(token[pos].loc));
		} else {
			pos++;
		}
	}

	inline void match(int id, const CppError &error)
	{
		if (pos+1 == 0) return;
		else if (pos >= token.size()) {
			report.issueError(error);
			pos = -1;
		} else if (token[pos].type != id) {
			report.issueError(error);
		} else {
			pos++;
		}
	}

public:

#define FIRST_EXPR case IDENT:			\
	case NUMBER:				\
	case '(':case '+':case '-':case '!':	\
	case CIN:case COUT:

	Tree* program()
	{
		match('#'), match(INCLUDE), match('<'), match(IOSTREAM), match('>');
		match('#'), match(INCLUDE), match('<'), match(CSTDIO), match('>');
		match(USING), match(NAMESPACE), match(STD),  match(';');
		return top_level();
	}

	Tree* top_level()
	{
		vector<Tree*> arg;
		Location loc = lookahead().loc;
		bool isConst = false;
		while (1) {
			isConst = false;
			switch(lookahead().type) {
			case EXIT:
				match(EXIT);
				return new TopLevel(loc, arg);
			case CONST:
				match(CONST);
				isConst = true;
				match(INT);
				break;
			case INT:
				match(INT);
				break;
			default:
				killer.issueError(SyntaxError(lookahead().loc));
			}
			arg.push_back(var_and_func(isConst));
		}
	}

	Tree* var_and_func(bool isConst = false)
	{
		string name = lookahead().ident;
		Location loc = lookahead().loc;
		vector<Var> opt;
		Tree *T;
		match(IDENT);
		switch(lookahead().type) {
		case '(':
			T = func_def(opt);
			return new FunDef(loc, name, opt, T);
		case ',':
		case ';':
		case '[':
		case '=':
			var_def(name, opt);
			vars_def(opt);
			match(';');
			return new VarDef(loc, opt, isConst);
		default:
			killer.issueError(SyntaxError(lookahead().loc));
		}
		assert(false);
	}

	void opt_params(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case INT:
			match(INT);
			name = lookahead().ident;
			match(IDENT);
			opt.push_back(Var("int", name));
			if (lookahead().type == ',') {
				match(',');
				params(opt);
			}
			break;
		}
	}

	void params(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case INT:
			match(INT);
			name = lookahead().ident;
			match(IDENT);
			opt.push_back(Var("int", name));
			if (lookahead().type == ',') {
				match(',');
				params(opt);
			}
			break;
		}
	}

	Tree* func_def(vector<Var> &opt)
	{
		match('(');
		opt_params(opt);
		match(')');
		Tree *T = block();
		return T;
	}

	Tree* block()
	{
		Location loc = lookahead().loc;
		match('{');
		vector<Tree*> T;
		stmts(T);
		match('}');
		return new Block(loc, T);
	}

	// TODO HERE

	Tree* stmt()
	{
		Location loc = lookahead().loc;
		vector<Var> vars;
		vector<Tree*> T;
		Tree *tree;
		switch(lookahead().type) {
		case PUTCHAR:
			match(PUTCHAR);
			match('(');
			tree = expr();
			match(')');
			match(';');
			return new Putchar(loc, tree);
		case CONST:
			match(CONST);
			match(INT);
			var_def(vars);
			vars_def(vars);
			match(';');
			return new VarDef(loc, vars, true);
		case BREAK:
			match(BREAK);
			match(';');
			return new Break(loc);
		case INT:
			match(INT);
			var_def(vars);
			vars_def(vars);
			match(';');
			return new VarDef(loc, vars);
		case '{':
			match('{');
			stmts(T);
			match('}');
			return new Block(loc, T);
		case IF:
			return ifstmt();
		case FOR:
			return forstmt();
		case WHILE:
			return whilestmt();
		case RETURN:
			return returnstmt();
		case ';':
			match(';');
			return nullptr;
		default:
			tree = expr();
			match(';');
			return tree;
		}
	}

	Tree* ifstmt()
	{
		assert(lookahead().type == IF);
		Location loc = lookahead().loc;
		match(IF);
		match('(');
		Tree *e = expr();
		match(')');
		Tree *tb = stmt();
		if (lookahead().type == ELSE) {
			match(ELSE);
			return new If(loc, e, tb, stmt());
		}
		return new If(loc, e, tb);
	}

	Tree* forstmt()
	{
		assert(lookahead().type == FOR);
		Location loc = lookahead().loc;
		match(FOR);
		match('(');
		Tree *init = stmt_in_for();
		match(';');
		Tree *check = opt_expr();
		match(';');
		Tree *delta = opt_expr();
		match(')');
		return new For(loc, init, check, delta, stmt());
	}

	Tree* stmt_in_for()
	{
		vector<Var> vars;
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case INT:
			match(INT);
			var_def(vars);
			vars_def(vars);
			return new VarDef(loc, vars);
	        FIRST_EXPR
			return expr();
		}
		return nullptr;
	}

	Tree* opt_expr()
	{
		switch(lookahead().type) {
			FIRST_EXPR
				return expr();
		default:
			return new Constant(1);
		}
	}

	Tree* whilestmt()
	{
		assert(lookahead().type == WHILE);
		Location loc = lookahead().loc;
		match(WHILE);
		match('(');
		Tree *check = expr();
		match(')');
		return new While(loc, check, stmt());
	}

	Tree* returnstmt()
	{
		assert(lookahead().type == RETURN);
		Location loc = lookahead().loc;
		match(RETURN);
		Tree *e = expr();
		match(';');
		return new Return(loc, e);
	}


	void stmts(vector<Tree*> &vec)
	{
		switch(lookahead().type) {
			FIRST_EXPR
		case '{':
		case INT:
		case IF:
		case FOR:
		case WHILE:
		case RETURN:
		case PUTCHAR:
		case CONST:
		case BREAK:
			vec.push_back(stmt());
			stmts(vec);
			break;
		}
	}

	void var_def(const string &name, vector<Var> &opt)
	{
		vector<Tree*> index;
		Var var;
		switch(lookahead().type) {
		case '[':
			array_index(index);
			var = Var("array", name, index);
			break;
		default:
			var = Var("int", name);
			break;
		}
		var.def = nullptr;
		if (lookahead().type == '=') {
			if (var.type == "array") {
				report.issueError(SyntaxError(lookahead().loc));
			}
			match('=');
			var.def = unit8();
		}
		opt.push_back(var);
	}

	void array_index(vector<Tree*> &index)
	{
		switch(lookahead().type) {
		case '[':
			match('[');
			index.push_back(unit8());
			match(']');
			array_index(index);
		}
	}

	void array_index_expr(vector<Tree*> &index)
	{
		switch(lookahead().type) {
		case '[':
			match('[');
			index.push_back(expr());
			match(']');
			array_index_expr(index);
		}
	}

	void var_def(vector<Var> &opt)
	{
		string name;
		switch(lookahead().type) {
		case IDENT:
			name = lookahead().ident;
			match(IDENT);
			var_def(name, opt);
			break;
		}
	}

	void vars_def(vector<Var> &opt)
	{
		switch(lookahead().type) {
		case ',':
			match(',');
			var_def(opt);
			vars_def(opt);
			break;
		}
	}

	Tree* expr()
	{
		Location loc = lookahead().loc;
		vector<Tree*> exps;
		switch(lookahead().type) {
		case CIN:
			match(CIN);
			match(SHIFTR);
			cin_clause(exps);
			return new CinOp(loc, exps);
		case COUT:
			match(COUT);
			match(SHIFTL);
			cout_clause(exps);
			return new CoutOp(loc, exps);
		default:
			return unit9();
		}
	}

	void cin_clause(vector<Tree*> &vars)
	{
		vars.push_back(left_value());
		switch(lookahead().type) {
		case SHIFTR:
			match(SHIFTR);
			cin_clause(vars);
		}
	}

	void cout_clause(vector<Tree*> &exps)
	{
		exps.push_back(expr());
		switch(lookahead().type) {
		case SHIFTL:
			match(SHIFTL);
			cout_clause(exps);
			break;
		}
	}

	Tree* unit0()
	{
		Location loc = lookahead().loc;
		int dat;
		Tree *T;
		switch(lookahead().type) {
		case NUMBER:
			dat = lookahead().number;
			match(NUMBER);
			return new Ident(loc, dat);
		case '(':
			match('(');
			T = expr();
			match(')');
			return T;
		case IDENT:
			return unit0_ident();
		case ENDL:
			match(ENDL);
			return new Ident(loc, ENDL, '\n');
		default:
			report.issueError(SyntaxError(loc));
		}
		return nullptr;
	}

	Tree* unit0_ident()
	{
		assert(lookahead().type == IDENT);
		Location loc = lookahead().loc;
		string name = lookahead().ident;
		match(IDENT);
		vector<Tree*> index;
		switch(lookahead().type) {
		case '[':
			array_index_expr(index);
			return new ArrayAt(loc, name, index);
		case '(':
			match('(');
			product(index);
			match(')');
			return new Exec(loc, name, index);
		default:
			return new Ident(loc, name);
		}
	}

	Tree* left_value()
	{
		assert(lookahead().type == IDENT);
		Location loc = lookahead().loc;
		string name = lookahead().ident;
		match(IDENT);
		vector<Tree*> index;
		switch(lookahead().type) {
		case '[':
			array_index_expr(index);
			return new ArrayAt(loc, name, index);
		default:
			return new Ident(loc, name);
		}
	}


	Tree* unit1()
	{
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case '+':
			match('+');
			return new BinaryOp(loc, '+', "+", (Tree*)new Ident(loc, 0), unit1());
		case '-':
			match('-');
			return new BinaryOp(loc, '-', "-", (Tree*)new Ident(loc, 0), unit1());
		case '!':
			match('!');
			return new BinaryOp(loc, '!', "!", (Tree*)new Ident(loc, 0), unit1());
		default:
			return unit0();
		}
	}

	Tree* unit2()
	{
		Tree *left = unit1();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '*':
				match('*');
				left = new BinaryOp(loc, '*', "*", left, unit1());
				break;
			case '/':
				match('/');
				left = new BinaryOp(loc, '/', "/", left, unit1());
				break;
			case '%':
				match('%');
				left = new BinaryOp(loc, '%', "%", left, unit1());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit3()
	{
		Tree *left = unit2();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '+':
				match('+');
				left = new BinaryOp(loc, '+', "+", left, unit2());
				break;
			case '-':
				match('-');
				left = new BinaryOp(loc, '-', "-", left, unit2());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit4()
	{
		Tree *left = unit3();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '<':
				match('<');
				left = new BinaryOp(loc, '<', "<", left, unit3());
				break;
			case LE:
				match(LE);
				left = new BinaryOp(loc, LE, "<=", left, unit3());
				break;
			case '>':
				match('>');
				left = new BinaryOp(loc, '>', ">", left, unit3());
				break;
			case GE:
				match(GE);
				left = new BinaryOp(loc, GE, ">=", left, unit3());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit5()
	{
		Tree *left = unit4();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case EQ:
				match(EQ);
				left = new BinaryOp(loc, EQ, "==", left, unit4());
				break;
			case NEQ:
				match(NEQ);
				left = new BinaryOp(loc, NEQ, "!=", left, unit4());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit6()
	{
		Tree *left = unit5();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case '^':
				match('^');
				left = new BinaryOp(loc, '^', "^", left, unit5());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit7()
	{
		Tree *left = unit6();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case LAND:
				match(LAND);
				left = new BinaryOp(loc, LAND, "&&", left, unit6());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit8()
	{
		Tree *left = unit7();
		while (1) {
			Location loc = lookahead().loc;
			switch(lookahead().type) {
			case LOR:
				match(LOR);
				left = new BinaryOp(loc, LOR, "||", left, unit7());
				break;
			default:
				return left;
			}
		}
	}

	Tree* unit9()
	{
		Tree *right = unit8();
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case '=':
			match('=');
			return new BinaryOp(loc, '=', "=", right, unit9());
		default:
			return right;
		}
	}

	void product(vector<Tree*> &exps)
	{
		Location loc = lookahead().loc;
		switch(lookahead().type) {
		case ')':
			break;
			FIRST_EXPR
				exps.push_back(expr());
			switch(lookahead().type) {
			case ',':
				match(',');
				break;
			case ')':
				break;
			default:
				report.issueError(SyntaxError(loc));
			}
			product(exps);
			break;
		default:
			report.issueError(SyntaxError(loc));
		}
	}

	Parser()
	{
		pos = 0;
	}

	~Parser()
	{
		if (prog != nullptr)
			delete prog;
	}

	Optimizer *optimizeVisitor;

	void main(const char *filename)
	{
		lexer.start();
		Token cur;
		do {
			cur = lexer.getNext();
			token.push_back(cur);
		} while (cur.type != EXIT);
		if (lexer.isError())
			exit(0);
		prog = program();
		if (prog == nullptr) {
			puts("<empty>");
		} 

		CallFlow *callFlow;
		
		typeVisitor.callFlow = callFlow = new CallFlow;

		prog->accept(typeVisitor);
		
		if (typeVisitor.isError()) {
			cerr << " --- Killed --- " << endl;
			exit(0);
		}

		callFlow->analyze();

		optimizeVisitor = new Optimizer(typeVisitor.getSize(), callFlow, &typeVisitor.globalId);
		
#ifndef Ofuck
		prog->accept(*optimizeVisitor);
#endif

#ifdef DEBUG
		prog->printTo(0);
#endif

		runVisitor.openGlobal(typeVisitor.getGlobalSize());
		
#ifndef DEBUG
#ifndef COMPILE
		prog->accept(runVisitor);
#endif
		
#ifdef COMPILE
		SimpleVM::SimpleVMInit(filename, typeVisitor.getGlobalSize());
		TransVisitor *trans = new TransVisitor(typeVisitor.getFuncNumber());
		prog->accept(*trans);
		SimpleVM::print();
		SimpleVM::run();
		SimpleVM::SimpleVMClean();
		delete trans;

#endif		
#endif
		
		delete optimizeVisitor;
		delete callFlow;
		
	}
} parser;
