#pragma once

class SemValue {
public:

	int retv;
	int pos;

	SemValue()
	{ retv = pos = 0; }

	SemValue(int retv, int pos)
	{ this->retv = retv, this->pos = pos; } 
};
