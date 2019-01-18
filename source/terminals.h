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
