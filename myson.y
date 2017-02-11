%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

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
    | expression T_NEWLINE { printf("\tResult: %s\n", $1); /* write convert bignum to string here */ } 
    | T_QUIT T_NEWLINE { printf("bye!\n"); exit(0); }
;
expression: T_INT				
	  | expression T_PLUS expression	{ $$ = concat("Plus ",concat($1,$3));}
	  | expression T_MINUS expression	{ $$ = concat("-- ",concat($1,$3));}
	  | expression T_MULTIPLY expression	{ $$ = concat("prod ",concat($1,$3));}
	  | expression T_DIVIDE expression	{ $$ = concat("divs ",concat($1,$3));}
	  | T_LEFT expression T_RIGHT		{ $$ = concat("bracketed ",$2);}
	  | T_SQRT expression				{ $$ = concat("rooted ",$2);}
	  | T_LOG  expression   			{ $$ = concat("logged ",$2);}
	  | T_POW T_LEFT expression T_COMMA expression T_RIGHT  { $$ = concat("powered ",concat($3,$5));}

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


/*
expression: T_INT				{ $$ = $$; }
	  | expression T_PLUS expression	{ $$ = $$; }
	  | expression T_MINUS expression	{ $$ = $$; }
	  | expression T_MULTIPLY expression	{ $$ = $$; }
	  | T_LEFT expression T_RIGHT		{ $$ = $$; }
	  | T_SQRT expression				{ $$ = $$  ;}
	  | T_LOG  expression   			 {  $$ = $$ ;}
	  | T_POW T_LEFT expression T_COMMA expression T_RIGHT    { $$ = $$ ;}
;
*/