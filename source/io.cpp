
class SystemIO {

public:
	virtual int read() = 0;
	virtual int put(int c) = 0;
	virtual int print(int d) = 0;
	virtual ~SystemIO()
	{}
} ;

SystemIO *sysIO;

class Uoj98IO : public SystemIO {

	vector<int> vec;
	size_t pos;

public:
	Uoj98IO()
	{
		int n, d;
		pos = 0;
		scanf("%d", &n);
		for (int i = 1; i <= n; i++) {
			scanf("%d", &d);
			vec.push_back(d);
		}
	}

	virtual ~Uoj98IO()
	{}

	virtual int read()
	{
		if (pos == vec.size()) return 0;
		return vec[pos++];
	}

	virtual int put(int c)
	{
		putchar(c);
		if (c == '\n')
			fflush(stdout);
		return c;
	}

	virtual int print(int d)
	{
		printf("%d", d);
		return d;
	}
} ;

class StdIO : public SystemIO {

public:

	virtual ~StdIO()
	{}
	
	virtual int read()
	{
		int a = 0, c;
		do c = getchar(); while (!isdigit(c));
		while (isdigit(c)) {
			a = a*10+c-'0';
			c = getchar();
		}
		return a;
	}

	virtual int put(int c)
	{
		putchar(c);
		return c;
	}

	virtual int print(int d)
	{
		printf("%d", d);
		return d;
	}
	
} ;
