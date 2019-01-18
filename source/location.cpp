#include "header.h"

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
