class Scanner {

public:
	virtual int getchar() = 0;

	virtual ~Scanner()
	{}
} ;

Scanner *__std_scanner;

///
/// ::Token : type, value  
///

class Token {

public:
	Location loc;
	int type;
	string ident;
	int number;

	Token()
	{ }

	Token(Location loc, int id)
	{ this->loc = loc, type = id; }

	Token(Location loc, int id, const string &str)
	{ this->loc = loc, type = id, ident = str; }

	Token(Location loc, int id, int num)
	{ this->loc = loc, type = id, number = num; }

	void print()
	{
		if (type == ERROR)
			cout << "error" << endl;
		else if (type == IDENT)
			cout << "ident " << ident << endl;
		else if (type == NUMBER)
			cout << "number " << number << endl;
		else cout << "keyword " << ident << endl;
	}
};

///
/// ::Lexer
///

class Lexer {
	map<int, string> table;
	map<string, int> anti;
	set<string> prefix;

	Report report;
	int IDENT;
	int NUMBER;
	int ERROR;

	Location loc;
	char ch;


	inline bool isblank(int c)
	{ return c == ' ' || c == '\n' || c == '\t' || c == '\r'; }

	inline void read()
	{
		ch = __std_scanner->getchar();
		if (ch == '\n') loc.line++, loc.col = 0;
		else loc.col++;
	}

	inline void skip()
	{
		assert(isblank(ch));
		while (isblank(ch)) 
			read();
	}

	inline Token getIdent()
	{
		assert(isalpha(ch) || ch == '_');
		Location curloc = loc;
		string cur;
		while (isalnum(ch) || ch == '_') {
			cur += ch;
			read();
		}
		if (anti.count(cur)) return Token(curloc, anti[cur], cur);
		return Token(curloc, IDENT, cur);
	}

	inline Token getNumber()
	{
		assert(isdigit(ch));
		int number = 0;
		Location curloc = loc;
		if (ch == '0') {
			read();
			if (isdigit(ch)) {
				report.issueError(NumberStartWithZero(loc));
				while (isdigit(ch))
					read();
				// skip error
			}
		} else {
			while (isdigit(ch)) {
				number = number * 10 + ch - '0';
				read();
			}
		}
		return Token(curloc, NUMBER, number);
	}

	inline Token getKeyword()
	{
		Location curloc = loc;
		string cur;
		while (prefix.count(cur + ch)) {
			cur += ch;
			read();
		}
		if (cur == "") {
			report.issueError(UnmatchableKeyword(loc));
			while (!isblank(ch))
				read();
			return Token(curloc, ERROR, cur);
		}
		return Token(curloc, anti[cur], cur);
	}

public:	
	Lexer()
	{
		ch = ' ';
		loc.line = 1, loc.col = 0;
	}

	inline void append(int id, const string &str)
	{
		assert(!table.count(id));
		assert(!anti.count(str));
		table[id] = str;
		anti[str] = id;
		string now;
		for (size_t i = 0; i < str.size(); i++) {
			now = now + str[i];
			prefix.insert(now);
		}
	}

	inline void setIdent(int Id)
	{ IDENT = Id; }

	inline void setNumber(int Id)
	{ NUMBER = Id; }

	inline void setError(int Id)
	{ ERROR = Id; }

	inline void setSimple(const string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
			append(str[i], str.substr(i, 1));
	}

	inline Token getN()
	{
		if (isblank(ch)) skip();
		if (ch == EOF) 
			return Token(loc, EXIT, "eof");
		if (ch == '_' || isalpha(ch)) return getIdent();
		else if (isdigit(ch)) return getNumber();
		else return getKeyword();
	}

	inline Token getNext()
	{
		Token tok = getN();
		while (tok.type == COMMENT) {
			while (ch != '\n')
				read();
			read();
			tok = getN();
		}
		return tok;
	}

	inline void start()
	{ skip(); }

	inline bool isError()
	{ return report.isError(); }
} lexer;
