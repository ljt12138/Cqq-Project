namespace SimpleVM {

#ifdef COMPILE
	const int MAX_SIZE = 1 << 28;
	const int MAX_PROGRAM_SIZE = 1 << 17;
	const int MAX_STACK_SIZE = 1 << 17;
#endif
	
#ifndef COMPILE
	const int MAX_SIZE = 1;
	const int MAX_PROGRAM_SIZE = 1;
	const int MAX_STACK_SIZE = 1;
#endif	
	
	FILE *fl;
	
	class instruction {
	public:
		int id;
		virtual void run() = 0;
		virtual void print() = 0;
		virtual ~instruction()
		{}
	};

	class inplace : public instruction {
	public:
		virtual void run() = 0;
		virtual void print() = 0;
		virtual ~inplace()
		{}
	};

	class purepush : public instruction {
	public:
		virtual void run() = 0;
		virtual void print() = 0;
		virtual ~purepush()
		{}
	};
	
	class pushstack : public instruction {
	public:
		virtual void run() = 0;
		virtual void print() = 0;
		virtual ~pushstack()
		{}
	};

	class brancher : public instruction {
	public:
		instruction **tar;
		virtual void run() = 0;
		virtual void print() = 0;
		virtual ~brancher()
		{}
	};
	
	int pool[MAX_SIZE];

#ifndef NOREGISTER
	register int *top asm ("r12"); // top of stack
	register int ax asm ("r13");  // cache of stack top
	register int *fp asm ("r14"); // top of last start function
	register instruction ** pc asm ("r15");  // current command
#endif
#ifdef NOREGISTER
	int *top; // top of stack
	int ax;  // cache of stack top
	int *fp; // top of last start function
	instruction ** pc;  // current command
#endif

	instruction ** ptop; // top of program 
	instruction * program[MAX_PROGRAM_SIZE];
	instruction ** stp[MAX_STACK_SIZE]; // function stack
	instruction *** sp; // function stack top

	class add : public instruction {
	public:
		add()
		{ }
		virtual void run()
		{
			--top;
			ax = *top + ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : ADD\n", id); }
	};

	class sub : public instruction {
	public:
		sub()
		{ }
		virtual void run()
		{
			--top;
			ax = *top - ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : SUB\n", id); }
	};

	class bsub : public instruction {
	public:
		bsub()
		{ }
		virtual void run()
		{
			--top;
			ax -= *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : BSUB\n", id); }
	};

	class mul : public instruction {
	public:
		mul()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top) * ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : MUL\n", id); }
	};
	
	class xdiv : public instruction {
	public:
		xdiv()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top) / ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : DIV\n", id); }
	};

	class bdiv : public instruction {
	public:
		bdiv()
		{ }
		virtual void run()
		{
			--top;
			ax /= (*top);
		}
		virtual void print()
		{ fprintf(fl, "%d : BDIV\n", id); }
	};

	class mod : public instruction {
	public:
		mod()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top) % ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : MOD\n", id); }
	};

	class bmod : public instruction {
	public:
		bmod()
		{ }
		virtual void run()
		{
			--top;
			ax %= (*top);
		}
		virtual void print()
		{ fprintf(fl, "%d : BMOD\n", id); }
	};
		
	class les : public instruction {
	public:
		les()
		{ }
		virtual void run()
		{
			--top;
			ax = ((*top) < ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : LES\n", id); }
	};

	class grt : public instruction {
	public:
		grt()
		{ }
		virtual void run()
		{
			--top;
			ax = ((*top) > ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : GRT\n", id); }
	};

	class leq : public instruction {
	public:
		leq()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top <= ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : LEQ\n", id); }
	};

	class geq : public instruction {
	public:
		geq()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top >= ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : GEQ\n", id); }
	};

	class eq : public instruction {
	public:
		eq()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top == ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : EQ\n", id); }
	};

	class neq : public instruction {
	public:
		neq()
		{ }
		virtual void run()
		{
			--top;
			ax = (*top != ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : NEQ\n", id); }
	};

	class lxor : public instruction {
	public:
		lxor()
		{ }
		virtual void run()
		{
			--top;
			ax = ((*top) ^ ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : LXOR\n", id); }
	};

	class lnot : public inplace {
	public:
		lnot()
		{ }
		virtual void run()
		{ ax = !ax; }
		virtual void print()
		{ fprintf(fl, "%d : LNOT*\n", id); }
	};

	class addfromload : public inplace {
	public:
		int pos;
		addfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax += fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : ADDFROMLOAD* %d\n", id, pos); }
	};

	class subfromload : public inplace {
	public:
		int pos;
		subfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax -= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : SUBFROMLOAD* %d\n", id, pos); }
	};

	class bsubfromload : public inplace {
	public:
		int pos;
		bsubfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = fp[pos] - ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BSUBFROMLOAD* %d\n", id, pos); }
	};
	
	class mulfromload : public inplace {
	public:
		int pos;
		mulfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax *= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : MULFROMLOAD* %d\n", id, pos); }
	};

	class divfromload : public inplace {
	public:
		int pos;
		divfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax /= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : DIVFROMLOAD* %d\n", id, pos); }
	};

	class bdivfromload : public inplace {
	public:
		int pos;
		bdivfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = fp[pos] / ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BDIVFROMLOAD* %d\n", id, pos); }
	};

	class modfromload : public inplace {
	public:
		int pos;
		modfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax %= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : MODFROMLOAD* %d\n", id, pos); }
	};

	class bmodfromload : public inplace {
	public:
		int pos;
		bmodfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = fp[pos] % ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BMODFROMLOAD* %d\n", id, pos); }
	};

	class lesfromload : public inplace {
	public:
		int pos;
		lesfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax < fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LESFROMLOAD* %d\n", id, pos); }
	};

	class grtfromload : public inplace {
	public:
		int pos;
		grtfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax > fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : GRTFROMLOAD* %d\n", id, pos); }
	};
	
	class leqfromload : public inplace {
	public:
		int pos;
		leqfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax <= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LEQFROMLOAD* %d\n", id, pos); }
	};

	class geqfromload : public inplace {
	public:
		int pos;
		geqfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax >= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : GEQFROMLOAD* %d\n", id, pos); }
	};

	class neqfromload : public inplace {
	public:
		int pos;
		neqfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
		        ax = ax != fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : NEQFROMLOAD* %d\n", id, pos); }
	};

	class eqfromload : public inplace {
	public:
		int pos;
		eqfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax == fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : EQFROMLOAD* %d\n", id, pos); }
	};

	class lxorfromload : public inplace {
	public:
		int pos;
		lxorfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax ^= fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LXORFROMLOAD* %d\n", id, pos); }
	};

	class lnotfromload : public purepush {
	public:
		int pos;
		lnotfromload(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			*top = ax;
			++top;
			ax = !fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LNOTFROMLOAD* %d\n", id, pos); }
	};

	//
	
	class addfromloadabs : public inplace {
	public:
		int pos;
		addfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax += pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : ADDFROMLOADABS* %d\n", id, pos); }
	};

	class subfromloadabs : public inplace {
	public:
		int pos;
		subfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax -= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : SUBFROMLOADABS* %d\n", id, pos); }
	};

	class bsubfromloadabs : public inplace {
	public:
		int pos;
		bsubfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = pool[pos] - ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BSUBFROMLOADABS* %d\n", id, pos); }
	};

	
	class mulfromloadabs : public inplace {
	public:
		int pos;
		mulfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax *= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : MULFROMLOADABS* %d\n", id, pos); }
	};

	class divfromloadabs : public inplace {
	public:
		int pos;
		divfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax /= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : DIVFROMLOADABS* %d\n", id, pos); }
	};

	class bdivfromloadabs : public inplace {
	public:
		int pos;
		bdivfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = pool[pos] / ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BDIVFROMLOADABS* %d\n", id, pos); }
	};

	class modfromloadabs : public inplace {
	public:
		int pos;
		modfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax %= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : MODFROMLOADABS* %d\n", id, pos); }
	};

	class bmodfromloadabs : public inplace {
	public:
		int pos;
		bmodfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = pool[pos] % ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : BMODFROMLOADABS* %d\n", id, pos); }
	};

	class lesfromloadabs : public inplace {
	public:
		int pos;
		lesfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax < pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LESFROMLOADABS* %d\n", id, pos); }
	};

	class grtfromloadabs : public inplace {
	public:
		int pos;
		grtfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax > pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : GRTFROMLOADABS* %d\n", id, pos); }
	};
	
	class leqfromloadabs : public inplace {
	public:
		int pos;
		leqfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax <= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LEQFROMLOADABS* %d\n", id, pos); }
	};

	class geqfromloadabs : public inplace {
	public:
		int pos;
		geqfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax >= pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : GEQFROMLOADABS* %d\n", id, pos); }
	};

	class neqfromloadabs : public inplace {
	public:
		int pos;
		neqfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax != pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : NEQFROMLOADABS* %d\n", id, pos); }
	};

	class eqfromloadabs : public inplace {
	public:
		int pos;
		eqfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax == pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : EQFROMLOADABS* %d\n", id, pos); }
	};

	class lxorfromloadabs : public inplace {
	public:
		int pos;
		lxorfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			ax = ax ^ pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LXORFROMLOADABS* %d\n", id, pos); }
	};

	class lnotfromloadabs : public purepush {
	public:
		int pos;
		lnotfromloadabs(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			*top = ax;
			++top;
			ax = !pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LNOTFROMLOADABS* %d\n", id, pos); }
	};

	//
	
	class addon : public inplace {
	public:
		int value;
		addon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax += value; }
		virtual void print()
		{ fprintf(fl, "%d : ADDON* %d\n", id, value); }
	};

	class incr : public inplace {
	public:
		incr()
		{ }
		virtual void run()
		{ ax++; }
		virtual void print()
		{ fprintf(fl, "%d : INCR*\n", id); }
	};

	class subon : public inplace {
	public:
		int value;
		subon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = value - ax; }
		virtual void print()
		{ fprintf(fl, "%d : SUBON* %d\n", id, value); }
	};

	class decr : public inplace {
	public:
		decr()
		{ }
		virtual void run()
		{ ax--; }
		virtual void print()
		{ fprintf(fl, "%d : DECR*\n", id); }
	};

	class mulon : public inplace {
	public:
		int value;
		mulon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax *= value; }
		virtual void print()
		{ fprintf(fl, "%d : MULON* %d\n", id, value); }
	};

	class leap : public instruction {
	public:
		int value;
		leap(int value)
		{ this->value = value; }
		virtual void run()
		{
			top--;
			ax = (*top) + ax * value;
		}
		virtual void print()
		{ fprintf(fl, "%d : LEAP %d\n", id, value); }
	};

	class leapxab : public instruction {
	public:
		leapxab()
		{ }
		virtual void run()
		{
			top -= 2;
			ax = (*top)*((*(top+1)) + ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : LEAPXAB\n", id); }
	};

	class leapcxy : public instruction {
	public:
		leapcxy()
		{ }
		virtual void run()
		{
			top -= 2;
			ax = (*top) + ((*(top+1)) * ax);
		}
		virtual void print()
		{ fprintf(fl, "%d : LEAPCXY\n", id); }
	};

	class leapxyz : public instruction {
	public:
		leapxyz()
		{ }
		virtual void run()
		{
			top -= 2;
			ax = (*top) * (*(top+1)) * ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : LEAPXYZ\n", id); }
	};
	
	class divon : public inplace {
	public:
		int value;
		divon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax /= value; }
		virtual void print()
		{ fprintf(fl, "%d : DIVON* %d\n", id, value); }
	};

	class bdivon : public inplace {
	public:
		int value;
		bdivon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = value / (ax); }
		virtual void print()
		{ fprintf(fl, "%d : BDIVON* %d\n", id, value); }
	};

	class modon : public inplace {
	public:
		int value;
		modon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax %= value; }
		virtual void print()
		{ fprintf(fl, "%d : MODON* %d\n", id, value); }
	};

	class bmodon : public inplace {
	public:
		int value;
		bmodon(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = value % (ax); }
		virtual void print()
		{ fprintf(fl, "%d : BMODON* %d\n", id, value); }
	};

	class lxoron : public inplace {
	public:
		int value;
		lxoron(int value)
		{ this->value = value; }
		virtual void run()
		{ ax ^= value; }
		virtual void print()
		{ fprintf(fl, "%d : LXORON* %d\n", id, value); }
	};

	class isles : public inplace {
	public:
		int value;
		isles(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax < value); }
		virtual void print()
		{ fprintf(fl, "%d : ISLES* %d\n", id, value); }
	};

	class isgrt : public inplace {
	public:
		int value;
	        isgrt(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax > value); }
		virtual void print()
		{ fprintf(fl, "%d : ISGRT* %d\n", id, value); }
	};

	class isleq : public inplace {
	public:
		int value;
		isleq(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax <= value); }
		virtual void print()
		{ fprintf(fl, "%d : ISLEQ* %d\n", id, value); }
	};

	class isgeq : public inplace {
	public:
		int value;
	        isgeq(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax >= value); }
		virtual void print()
		{ fprintf(fl, "%d : ISGEQ* %d\n", id, value); }
	};

	class isneq : public inplace {
	public:
		int value;
	        isneq(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax != value); }
		virtual void print()
		{ fprintf(fl, "%d : ISNEQ* %d\n", id, value); }
	};

	class iseq : public inplace {
	public:
		int value;
	        iseq(int value)
		{ this->value = value; }
		virtual void run()
		{ ax = (ax == value); }
		virtual void print()
		{ fprintf(fl, "%d : ISEQ* %d\n", id, value); }
	};
	
	class load : public inplace {
	public:
		load()
		{ }
		virtual void run()
		{ ax = fp[ax]; }
		virtual void print()
		{ fprintf(fl, "%d : LOAD*\n", id); }
	};

	class loadshift : public inplace {
		int pos;
	public:
		loadshift(int pos)
		{ this->pos = pos; }
		virtual void run()
		{ ax = fp[ax + pos]; }
		virtual void print()
		{ fprintf(fl, "%d : LOADSHIFT*\n", id); }
	};

	class loadfrom : public purepush {
	public:
		int pos;
		loadfrom(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			*top = ax;
			++top;
			ax = fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LOADFROM %d\n", id, pos); }
	};
	
	class loadabs : public inplace {
	public:
		loadabs()
		{ }
		virtual void run()
		{ ax = pool[ax]; }
		virtual void print()
		{ fprintf(fl, "%d : LOADABS*\n", id); }
	};

	class loadabsshift : public inplace {
		int pos;
	public:
		loadabsshift(int pos)
		{ this->pos = pos; }
		virtual void run()
		{ ax = pool[ax + pos]; }
		virtual void print()
		{ fprintf(fl, "%d : LOADABSSHIFT*\n", id); }
	};
	
	class loadabsfrom : public purepush {
	public:
		int pos;
		loadabsfrom(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			*top = ax;
			++top;
			ax = pool[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : LOADABSFROM %d\n", id, pos); }
	};

	class save : public instruction {
	public:
		save()
		{ }
		virtual void run()
		{
			top--;
			fp[ax] = *top;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVE\n", id); }
	};

	class savepop : public instruction {
	public:
		savepop()
		{ }
		virtual void run()
		{
			top -= 2;
			fp[ax] = *(top+1);
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVEPOP\n", id); }
	};

	class saveabs : public instruction {
	public:
		saveabs()
		{ }
		virtual void run()
		{
			top--;
			pool[ax] = *top;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVEABS\n", id); }
	};

	class saveabspop : public instruction {
	public:
		saveabspop()
		{ }
		virtual void run()
		{
			top -= 2;
			pool[ax] = *(top+1);
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVEABSPOP\n", id); }
	};

	class saveabsto : public instruction {
	public:
		int pos;
		saveabsto(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			pool[pos] = ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVEABSTO\n", id); }
	};

	class saveabstopop : public instruction {
	public:
		int pos;
		saveabstopop(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			pool[pos] = ax;
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVEABSTOPOP\n", id); }
	};
	
	class saveto : public instruction {
	public:
		int pos;
		saveto(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			fp[pos] = ax;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVETO %d\n", id, pos); }
	};

	class savetopop : public instruction {
	public:
		int pos;
		savetopop(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			fp[pos] = ax;
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : SAVETOPOP %d\n", id, pos); }
	};
	
	class addtopop : public instruction {
	public:
		int pos;
		addtopop(int pos)
		{ this->pos = pos; }
		virtual void run()
		{
			fp[pos] += ax;
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : ADDTOPOP %d\n", id, pos); }
	};

	
	class fptopool : public inplace {
	public:
		fptopool()
		{ }
		virtual void run()
		{
			ax = ax + (fp - pool);
		}
		virtual void print()
		{ fprintf(fl, "%d : FPTOPOOL*\n", id); }
	};
	
	class pushimm : public purepush {
	public:
		int value;
		pushimm(int value)
		{ this->value = value; }
		virtual void run()
		{
			*top = ax;
			++top;
			ax = value;
		}
		virtual void print()
		{ fprintf(fl, "%d : PUSHIMM %d\n", id, value); }
	};

	class pop : public instruction {
	public:
		pop()
		{}
		virtual void run()
		{
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : POP\n", id); }
	};

	class branch : public brancher {
	public:
		branch(instruction **tar)
		{ this->tar = tar; }
		virtual void run()
		{ pc = tar; }
		virtual void print()
		{ fprintf(fl, "%d : BRANCH %ld\n", id, tar - program); }
	};

	class bnez : public brancher {
	public:
		bnez(instruction **tar)
		{ this->tar = tar; }
		virtual void run()
		{
			if (ax) pc = tar;
		}
		virtual void print()
		{ fprintf(fl, "%d : BNEZ %ld\n", id, tar - program); }
	};

	class beqz : public brancher {
	public:
		beqz(instruction **tar)
		{ this->tar = tar; }
		virtual void run()
		{
			if (!ax) pc = tar;
		}
		virtual void print()
		{ fprintf(fl, "%d : BEQZ %ld\n", id, tar - program); }
	};

	class bnezpop : public brancher {
	public:
		bnezpop(instruction **tar)
		{ this->tar = tar; }
		virtual void run()
		{
			if (ax) pc = tar;
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : BNEZPOP %ld\n", id, tar - program); }
	};

	class beqzpop : public brancher {
	public:
		beqzpop(instruction **tar)
		{ this->tar = tar; }
		virtual void run()
		{
			if (!ax) pc = tar;
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : BEQZPOP %ld\n", id, tar - program); }
	};
	
	class alloc : public instruction {
	public:
		int size;
		instruction ** tar;
		alloc(int size, instruction **tar)
		{ this->size = size, this->tar = tar; }
		virtual void run()
		{
			// modify top
			*top = ax;
			top += 2;
			*top = top - fp;
			fp = top + 1;
			top += size;
			ax = *top;
			*sp = pc;
			sp++;
			pc = tar;
		}
		virtual void print()
		{ fprintf(fl, "%d : ALLOC %d, Branch to %ld\n", id, size, tar - program); }
	};

	// add 2
	class topmove : public instruction {
	public:
		topmove()
		{ }
		virtual void run()
		{ *top = ax; top += 2; }
		virtual void print()
		{ fprintf(fl, "%d : TOPMOVE\n", id); }
	};

	class topback : public instruction {
	public:
		topback()
		{ }
		virtual void run()
		{ top = fp - 1; }
		virtual void print()
		{ fprintf(fl, "%d : TOPBACK\n", id); }
	};

	class topadd : public instruction {
	public:
		int value;
		topadd(int value)
		{ this->value = value; }
		virtual void run()
		{ top += value; }
		virtual void print()
		{ fprintf(fl, "%d : TOPADD %d\n", id, value); }
	};
	
	class allocparm : public instruction {
	public:
		int parm, size;
		instruction ** tar;
		allocparm(int parm, int size, instruction ** tar)
		{
			this->parm = parm;
			this->size = size;
			this->tar = tar;
		}
		virtual void run()
		{
			*top = ax;
			top -= parm;
			*top = top - fp;
			fp = top + 1;
			top += size;
			ax = *top;
			*sp = pc;
			sp++;
			pc = tar;
		}
		virtual void print()
		{ fprintf(fl, "%d : ALLOCPARM (%d, %d), Branch to %ld\n", id, parm, size, tar - program); }
	};
	
	class freespc : public instruction {
	public:
		freespc()
		{}
		virtual void run()
		{
			// modify top
			top = fp - 2;
			ax = *top;
			fp--;
			fp -= *fp;
			// get pc status
			sp--;
			pc = *sp;
		}
		virtual void print()
		{ fprintf(fl, "%d : FREESPC\n", id); }
	};

	class affine : public pushstack {
	public:
		int pos, k, b;
		affine(int pos, int k, int b)
		{
			this->pos = pos;
			this->k = k;
			this->b = b;
		}
		virtual void run()
		{
			*top = ax;
			top++;
			(fp[pos] *= k) += b;
			ax = fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : AFFINE %d %d %d\n", id, pos, k, b); }
	};

	class addto : public pushstack {
	public:
		int pos, b;
		addto(int pos, int b)
		{
			this->pos = pos;
			this->b = b;
		}
		virtual void run()
		{
			*top = ax;
			top++;
			fp[pos] += b;
			ax = fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : ADDTO %d %d\n", id, pos, b); }
	};

	class multo : public pushstack {
	public:
		int pos, k;
		multo(int pos, int k)
		{
			this->pos = pos;
			this->k = k;
		}
		virtual void run()
		{
			*top = ax;
			top++;
			fp[pos] *= k;
			ax = fp[pos];
		}
		virtual void print()
		{ fprintf(fl, "%d : MULTO %d %d\n", id, pos, k); }
	};

	// affine pop instructions

	class affinepop : public instruction {
	public:
		int pos, k, b;
		affinepop(int pos, int k, int b)
		{
			this->pos = pos;
			this->k = k;
			this->b = b;
		}
		virtual void run()
		{
			*top = ax;
			(fp[pos] *= k) += b;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : AFFINEPOP %d %d %d\n", id, pos, k, b); }
	};

	class addbtopop : public instruction {
	public:
		int pos, b;
		addbtopop(int pos, int b)
		{
			this->pos = pos;
			this->b = b;
		}
		virtual void run()
		{
			*top = ax;
			fp[pos] += b;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : ADDBTOPOP %d %d\n", id, pos, b); }
	};

	class mulktopop : public pushstack {
	public:
		int pos, k;
		mulktopop(int pos, int k)
		{
			this->pos = pos;
			this->k = k;
		}
		virtual void run()
		{
			*top = ax;
			fp[pos] *= k;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : MULKTOPOP %d %d\n", id, pos, k); }
	};
	
	// read integer into pool[*top]
	class read : public instruction {
	public:
		read()
		{}
		virtual void run()
		{
			pool[ax] = sysIO->read();
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : READ\n", id); }
	};

	// write integer from *top
	class write : public instruction {
	public:
		write()
		{}
		virtual void run()
		{
			sysIO->print(ax);
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : WRITE\n", id); }
	};

	// putchar from *top
	class put : public instruction {
	public:
		put()
		{}
		virtual void run()
		{
			sysIO->put(ax);
			top--;
			ax = *top;
		}
		virtual void print()
		{ fprintf(fl, "%d : PUT\n", id); }
	};

#ifdef PERFORMANCE
	int *begin;
#endif
	
	void SimpleVMInit(const char *str, int globalSize)
	{
		int l = strlen(str);
		assert(l >= 4);
		string name = str;
		assert(name[l-1] == 'p');
		assert(name[l-2] == 'p');
		assert(name[l-3] == 'c');
		assert(name[l-4] == '.');
		fl = fopen( (name.substr(0, l-4) + ".q").c_str(), "w+" );
#ifdef DEBUGC
		fclose(fl);
		fl = stdout;
#endif
		top = fp = pool - 1;
		fp = pool + 2;
		top = fp + globalSize - 1;
#ifdef PERFORMANCE
		begin = top;
#endif
		ptop = program;
		sp = stp;
	}

	void SimpleVMClean()
	{
		for (instruction ** p = program ; p != ptop; p++)
			delete *p;
		fclose(fl);
	}

	void append(instruction *inc)
	{
		*ptop = inc;
		inc->id = (ptop++) - program;
	}

	void controlOptimize()
	{
		vector<brancher*> vec; 
		for (instruction ** p = program; p != ptop; p++) {
			brancher *bran = dynamic_cast<brancher*>(*p);
			if (bran != nullptr)
				vec.push_back(bran);
		}
		bool change;
		do {
			change = false;
			for (size_t i = 0, size = vec.size(); i < size; i++) {
				if (vec[i]->tar+1 != ptop && *(vec[i]->tar+1) != vec[i]) {
					branch *bran = dynamic_cast<branch*>(*(vec[i]->tar + 1));
					if (bran != nullptr) {
						vec[i]->tar = bran->tar;
						change = true;
					}
				}
			}
		} while (change);
	}
	
	void run()
	{
		controlOptimize();
#ifdef PERFORMANCE
		long long cycle = 0;
		double space = 0;
#endif
		
#ifdef DEBUG
		fl = stdout;
#endif		
		while (pc != ptop) {
#ifdef PERFORMANCE
			cycle++;
			space += (top - begin + 1) + (sp - stp + 1);
#endif
			
#ifdef DEBUGC
			cin.get();
			cout << "NOW pc = " << pc-program << ", top = " << top-pool << ", fp = " << fp-pool <<  ", ax = " << ax << enfp;
			for (int * p = pool; p != top + 1; p++)
				cout << *p << " ";
			cout << enfp;
			(*pc)->print();
			cout << enfp;
#endif
			(*pc)->run();
			pc++;
		}
#ifdef PERFORMANCE
		cerr << "TOT cycle : " << cycle << enfp;
		cerr << "AVERAGE SPACE : " << setprecision(2) << fixed << space / cycle << enfp;
#endif
	}

	void print()
	{
		for (instruction ** p = program; p != ptop; p++) {
			(*p)->print();
		}
	}
}
