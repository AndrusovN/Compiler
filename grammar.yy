%language "c++"
%require "3.2"

%define api.token.raw

%code requires {
	#include "ast.h"
}	

%define api.value.type { INode* }
%parse-param {INode*& ast}

%code provides {
	#define YYSTYPE yy::parser::semantic_type
}

%code {
	// Give Flex the prototype of yylex we want ...
	#define YY_DECL \
		yy::parser::token::yytokentype yylex(yy::parser::semantic_type *yylval)
	// ... and declare it for the parser's sake.
	YY_DECL;
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
	LBR
	RBR
	LOGL
	LOGR
	FUNCTION
	CONDITIONAL
	AND
	OR
	XOR
	NOT
	EQUALS
	SPL
%nterm expr

%left SPLIT
%right ASSIGN
%left PLUS
%left MINUS
%left MULT
%left DIV
%left MOD

%start program
%%
program: expr SPLIT { ast = new Scope($1); }
;

expr:	NUM { $$ = $1; }
    |   FUNCTION VAR LPAR named_tuple RPAR scope {$$ = new Function($6, $2, $4); }
    |   FUNCTION VAR LPAR RPAR scope {$$ = new Function($5, $2, nullptr); }
    |	RETURN LPAR expr RPAR {$$ = new Return($3); }
    |	LOGL expr LOGR {$$ = new Log($2); } 
    |   VAR LPAR tuple RPAR {$$ = new FunctionCall($3, $1); }
    |   VAR LPAR RPAR {$$ = new FunctionCall(nullptr, $1); }
    |	VAR { $$ = new Variable($1); }
    |	LPAR expr RPAR { $$ = $2; }
    |	expr SPLIT expr { $$ = new Splitted($1, $3); }
    |	expr PLUS expr { $$ = new Plus($1, $3); }
    |	expr MINUS expr { $$ = new Minus($1, $3); }
    |	expr ASSIGN expr { $$ = new Assign((Variable*)$1, $3); }
    |   scope { $$ = $1; }
    |   CONDITIONAL LPAR expr RPAR scope { $$ = new ConditionalBehaviour($3, $5); }
    |	expr AND expr { $$ = new BooleanOp($1, $3, _and); }
    |	expr OR expr { $$ = new BooleanOp($1, $3, _or); }
    |	expr XOR expr { $$ = new BooleanOp($1, $3, _xor); }
    |	expr EQUALS expr { $$ = new Equals($1, $3); }
    |	NOT expr { $$ = new InversedBoolean($2); }
;

scope: 	LBR expr SPLIT RBR { $$ = new Scope($2); };

named_tuple: VAR SPL named_tuple {$$ = new Tuple($1, $3); }
	   | VAR {$$ = new Tuple($1, nullptr); }
;

tuple: expr SPL tuple {$$ = new Tuple($1, $3); }
     | expr {$$ = new Tuple($1, nullptr); }
;
%%

void yy::parser::error(const std::string &err_message) {
	std::cerr << "Error: " << err_message << std::endl;
}
