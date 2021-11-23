%language "c++"
%require "3.2"

%define api.token.raw
%define api.value.type { int }
%parse-param {int &val}
%code provides {
	#define YYSTYPE yy::parser::semantic_type
	#include <map>
	#include <vector>
	#include <string>

	extern std::vector<int> variables;
	extern std::map<std::string, int> variable_id;
}

%code {
	std::vector<int> variables;
	std::map<std::string, int> variable_id;
	// Give Flex the prototype of yylex we want ...
	#define YY_DECL \
		yy::parser::token::yytokentype yylex(yy::parser::semantic_type *yylval)
	// ... and declare it for the parser's sake.
	YY_DECL;
	#define MATH_ABS(x) (((x) < 0) ? (-(x)) : (x))
}

%define parse.error verbose
%define parse.lac full


%define api.token.prefix {TOK_}
%token
	RETURN
	SPLIT
	VAR
	ASSIGN
	PLUS
	MINUS
	MULT
	DIV
	MOD
	LPAR
	RPAR
	NUM
%nterm expr
%nterm preparation

%left SPLIT
%left PLUS
%left MINUS
%left MULT
%left DIV
%left MOD

%start program
%%
program: preparation SPLIT RETURN expr	{ val = $4; }
       |	RETURN expr {val = $2; }
;

preparation: preparation SPLIT preparation {}
	|	VAR ASSIGN expr { variables[$1] = $3; }
;

expr:	expr PLUS expr	{ $$ = $1 + $3; }
    |   VAR { $$ = variables[$1]; }
    |   MINUS expr { $$ = -1 * $2; } 
    |	LPAR expr RPAR	{ $$ = $2; }
    |	NUM		{ $$ = $1; }
    |   expr MINUS expr { $$ = $1 - $3; }
    |   expr MULT expr { $$ = $1 * $3; }
    |   expr DIV expr { $$ = $1 / $3; }
    |   expr MOD expr { if ($1 < 0) {
				std::cout << "there\n";
				$$ = (MATH_ABS($3) - ((-$1) % MATH_ABS($3))) % MATH_ABS($3);
			} else {
				$$ = $1 % MATH_ABS($3);
			}
		}
;

%%

void yy::parser::error(const std::string &err_message) {
	std::cerr << "Error: " << err_message << std::endl;
}
