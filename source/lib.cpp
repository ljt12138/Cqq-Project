const int MAX_ARRAY_SIZE = INT_MAX;

///
/// ::Log
///

#define LEXLOG "lexer.log"
#define PARSERLOG "parser.log"

///
/// ::Lib
///

///
/// ::Terminals
///

const int INCLUDE = INT_MAX;
const int IOSTREAM = INCLUDE - 1;
const int CSTDIO = IOSTREAM - 1;
const int USING = CSTDIO - 1;
const int NAMESPACE = USING - 1;
const int STD = NAMESPACE - 1;

const int INT = STD - 1;
const int IDENT = INT - 1;
const int NUMBER = IDENT - 1;
const int KEYWORD = NUMBER - 1;

const int CIN = KEYWORD - 1;
const int COUT = CIN - 1;
const int ENDL = COUT - 1;
const int PUTCHAR = ENDL - 1;

const int FOR = PUTCHAR - 1;
const int WHILE = FOR - 1;
const int IF = WHILE - 1;
const int ELSE = IF - 1;
const int RETURN = ELSE - 1;

const int LE = RETURN - 1;
const int GE = LE - 1;
const int EQ = GE - 1;
const int NEQ = EQ - 1;
const int LAND = NEQ - 1;
const int LOR = LAND - 1;

const int SHIFTL = LOR - 1; 
const int SHIFTR = SHIFTL - 1;  
const int ERROR = SHIFTR - 1;
const int EXIT = ERROR - 1;

const int INDEX = EXIT - 1;
const int EXEC = INDEX - 1;
const int COMMENT = EXEC - 1;
const int CONST = COMMENT - 1;
const int BREAK = CONST - 1;

const int LP = '(';
const int RP = ')';
const int LC = '[';
const int RC = ']';
const int BEGIN = '{';
const int END = '}';

const int LNOT = '!';

const int MUL = '*';
const int DIV = '/';
const int MOD = '%';

const int PLUS = '+';
const int MINUS = '-';

const int LS = '<';
const int GT = '>';

const int XOR = '^';
const int ASSIGN = '=';
const int SEP = ';';

///
/// 
///

typedef unsigned int uint;

const uint GLOBAL = 0;
const uint LOCAL = -1;

///
/// ::Location
///

class Location {
public:
	int line, col;

	Location()
	{ }

	Location(int line, int col)
	{ this->line = line, this->col = col; }

	friend ostream& operator << (ostream &oss, const Location &loc)
	{
		oss << "(" << loc.line << "," << loc.col << ")";
		return oss;
	}
};

///
/// ::Report Error
///

class CppError {

public:
	Location loc;
	virtual void print() const = 0;
};

class Report {
	bool error;

public:
	Report()
	{ error = false; }

	void issueError(const CppError &err)
	{
		err.print();
		error = true;
	}

	inline bool isError()
	{ return error; }
};

class Killer : public Report {

public:

	Killer()
	{ }

	void issueError(const CppError &err)
	{
		cerr << "!!!!!!!! Runtime Error !!!!!!!!!" << endl;
		cerr << "-- Interpreter has been killed--" << endl;
		err.print();
		cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		exit(1);
	}
} killer;

class NumberStartWithZero : public CppError {

public:
	NumberStartWithZero(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Lexer Error ** : At " << loc << ", number except 0 can not start with 0" << endl; 
	}
};

class UnmatchableKeyword : public CppError {

public:
	UnmatchableKeyword(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Lexer Error ** : At " << loc << ", there is no matchable keyword" << endl; 
	}
};

class SyntaxError : public CppError {

public:
	SyntaxError(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Parser Error ** : At " << loc << ", Syntax error" << endl;
		exit(0);
	}
};

class SegmentError : public CppError {

public:
	SegmentError(const Location &loc)
	{ this->loc = loc; }

	virtual void print() const
	{
		cerr << "** Runtime Error ** : At " << loc << ", Segment Error" << endl;
	}
};

class BadArrayOp : public CppError {

public:
	BadArrayOp()
	{ }

	virtual void print() const
	{
		cerr << "Bad Array Operation" << endl;
	}
};

class VariableDefined : public CppError {

	string name;

public:

	VariableDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable \'" << name << "\' has been defined before. " << endl;
	}
};

class FunctionDefined : public CppError {

	string name;

public:

	FunctionDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Function \'" << name << "\' has been defined before. " << endl;
	}
};

class VariableOrFunctionNotDefined : public CppError {

	string name;

public:

	VariableOrFunctionNotDefined(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable or Function \'" << name << "\' has been used before defined. " << endl;
	}
};

class VariableIsNotArray : public CppError {

	string name;

public:

	VariableIsNotArray(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Variable \'" << name << "\' is NOT array." << endl;
	}
};

class ArrayInExpr : public CppError {

	string name;

public:

	ArrayInExpr(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", \'" << name << "\' is a Array instead of Variable" << endl;
	}
};

class ArrayLengthError : public CppError {

	string name;

public:

	ArrayLengthError(const Location &loc, const string &name)
	{
		this->name = name;
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", \'" << name << "\' length error. " << endl;
	}
};

class BadAssignError : public CppError {

public:

	BadAssignError(const Location &loc)
	{
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Bad Assign Error." << endl;
	}	
};

class DirtyConstantOperation : public CppError {

public:

	DirtyConstantOperation(const Location &loc)
	{
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Warning ** : At " << loc << ", Expression here has been calculated while type checking. Please check the corrention carefully." << endl;
	}
	
};

class DividedByZero : public CppError {
	
public:

	DividedByZero(const Location &loc)
	{
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Integer divided by zero." << endl;
	}
};

class ConstExprError : public CppError {

public:

	ConstExprError(const Location &loc)
	{
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", ConstExpr is assigned by a non-const expression." << endl;
	}
	
};

class ArrayLengthNonConst : public CppError {

	string name;

public:
	
	ArrayLengthNonConst(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Length of array \'" << name << "\' is a non-const expression." << endl;
	}
};

class ArrayZeroLength : public CppError {

	string name;

public:

	ArrayZeroLength(const Location &loc, const string &name)
	{
		this->loc = loc;
		this->name = name;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Length of array \'" << name << "\' is zero." << endl;
	}
	
};

class BreakNotInLoop : public CppError {

public:

        BreakNotInLoop(const Location &loc)
	{
		this->loc = loc;
	}

	virtual void print() const
	{
		cerr << "** Type Check Error ** : At " << loc << ", Break is not inside any loop." << endl;
	}
	
};

