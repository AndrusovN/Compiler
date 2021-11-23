%option noyywrap bison-bridge
%{
	#include "grammar.tab.hh"
	#include <string>
	#define YY_TERMINATE return 
%}
%x comment

wc	[ \t\r]
num	([1-9][0-9]*)|"0"
var	[a-zA-Z_]+

%%

"//"		BEGIN(comment);
<comment>[^\n]*
<comment>\n	BEGIN(INITIAL);

{wc}+
"/"		return yy::parser::token::TOK_DIV;
"%"		return yy::parser::token::TOK_MOD;
"*"		return yy::parser::token::TOK_MULT;
"-"		return yy::parser::token::TOK_MINUS;
"+"		return yy::parser::token::TOK_PLUS;
"("		return yy::parser::token::TOK_LPAR;
")"		return yy::parser::token::TOK_RPAR;
{num}	{
		*yylval = std::stoi(yytext);
		return yy::parser::token::TOK_NUM;
	}
";"		return yy::parser::token::TOK_SPLIT;
"return"	return yy::parser::token::TOK_RETURN;
"="		return yy::parser::token::TOK_ASSIGN;
{var}	{
		if (variable_id.find(yytext) == variable_id.end()) {
			variable_id[yytext] = variables.size();
			*yylval = variables.size();
			variables.push_back(0);
		} else {
			*yylval = variable_id[yytext];
		}
		return yy::parser::token::TOK_VAR;
	}
.	throw yy::parser::syntax_error("invalid character: " + std::string(yytext));
%%
