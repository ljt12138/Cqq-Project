#include <bits/stdc++.h>
using namespace std;

#include "lib.cpp"
#include "lexer.cpp"
#include "tree.cpp"
#include "scope.cpp"
#include "callflow.cpp"
#include "typecheck.cpp"
#include "io.cpp"
#include "optimizer.cpp"
#include "runner.cpp"
#include "vm.cpp"
#include "translater.cpp"
#include "parser.cpp"

void lexer_init()
{
	lexer.append(INCLUDE, "include");
	lexer.append(IOSTREAM, "iostream");
	lexer.append(CSTDIO, "cstdio");
	lexer.append(USING, "using");
	lexer.append(NAMESPACE, "namespace");
	lexer.append(STD, "std");

	lexer.append(INT, "int");

	lexer.setIdent(IDENT);
	lexer.setNumber(NUMBER);
	lexer.setError(ERROR);

	lexer.append(CIN, "cin");
	lexer.append(COUT, "cout");
	lexer.append(ENDL, "endl");
	lexer.append(PUTCHAR, "putchar");

	lexer.append(FOR, "for");
	lexer.append(WHILE, "while");
	lexer.append(IF, "if");
	lexer.append(ELSE, "else");
	lexer.append(RETURN, "return");

	lexer.append(LE, "<=");
	lexer.append(GE, ">=");
	lexer.append(NEQ, "!=");
	lexer.append(EQ, "==");
	lexer.append(LAND, "&&");
	lexer.append(LOR, "||");

	lexer.append(SHIFTL, "<<");
	lexer.append(SHIFTR, ">>");
	lexer.append(CONST, "const");
	lexer.append(BREAK, "break");	
	lexer.append(COMMENT, "//");
	lexer.setSimple("#+-*/%()[]{}=!^;,<>");
}

class ScanFromFile : public Scanner {

	ifstream iss;
	
public:

	virtual ~ScanFromFile()
	{

	}
	
	ScanFromFile( const string & file )
	{
		iss.open(file.c_str());	
	}

	virtual int getchar()
	{
		return iss.get();
	}
};

void init(const string & file_name)
{
	lexer_init();
	sysIO = new StdIO;
	__std_scanner = new ScanFromFile(file_name);
}

void clean()
{
	delete sysIO;
	delete __std_scanner;
}

int main(int argc, char *argv[])
{
	assert(argc == 2);
	init(argv[1]);
	parser.main(argv[1]);
	clean();
	return 0;
}
