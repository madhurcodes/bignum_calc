%{
#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);
%}

%union {
	char* ival;
}

%token<ival> T_INT
%token T_PLUS T_MINUS T_MULTIPLY T_DIVIDE T_LEFT T_RIGHT T_SQRT T_LOG T_POW T_COMMA
%token T_NEWLINE T_QUIT
%left T_PLUS T_MINUS
%left T_MULTIPLY T_DIVIDE 
%right T_SQRT T_LOG T_POW 


%left T_RIGHT
%right T_LEFT
%type<ival> expression

%start calculation

%%

calculation: 
	   | calculation line
;

line: T_NEWLINE
    | expression T_NEWLINE { printf("\tResult: %s\n", $1); // write convert bignum to string here } 
    | T_QUIT T_NEWLINE { printf("bye!\n"); exit(0); }
;
expression: T_INT				{ $$ = $$; //write convert string to big_num here}
	  | expression T_PLUS expression	{ $$ = $$; }
	  | expression T_MINUS expression	{ $$ = $$; }
	  | expression T_MULTIPLY expression	{ $$ = $$; }
	  | T_LEFT expression T_RIGHT		{ $$ = $$; }
	  | T_SQRT expression				{ $$ = $$  ;}
	  | T_LOG  expression   			 {  $$ = $$ ;}
	  | T_POW T_LEFT expression T_COMMA expression T_RIGHT    { $$ = $$ ;}
;

%%
int main() {
	yyin = stdin;
	do { 
		yyparse();
	} while(!feof(yyin));
	return 0;
}
void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}