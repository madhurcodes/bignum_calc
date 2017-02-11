%option noyywrap

%{
#include <stdio.h>


#include "myson.tab.h"

%}

%%

[ \t]	; // ignore all whitespace
[0-9]+\.?[0-9]+ 	{yylval.ival = strdup(yytext); return T_INT;}
\n		{return T_NEWLINE;}
"+"		{return T_PLUS;}
"-"		{return T_MINUS;}
"*"		{return T_MULTIPLY;}
"/"		{return T_DIVIDE;}
","		{return T_COMMA;}
"( "		{return T_LEFT;}
" )"		{return T_RIGHT;}
"SQRT"        {return T_SQRT;}
"LOG"        {return T_LOG;}
"POW"        {return T_POW;}
"exit"		{return T_QUIT;}
"quit"		{return T_QUIT;}

%%