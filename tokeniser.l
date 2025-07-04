%{
// This is our Lexical tokeniser 
// It should be compiled into cpp with :
// flex++ -d -otokeniser.cpp tokeniser.l 
// And then compiled into object with
// g++ -c tokeniser.cpp
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

#include "tokeniser.h"
#include <iostream>

using namespace std;

%}

%option noyywrap
%option c++
%option yylineno

stringconst  \"[^\n"]+\"
ws      [ \t\n\r]+
alpha   [A-Za-z]
digit   [0-9]
number  {digit}+
id	{alpha}({alpha}|{digit})*
addop	(\+|\-|\|\|)
mulop	(\*|\/|%|\&\&)
relop	(\<|\>|"=="|\<=|\>=|!=)
unknown [^\"A-Za-z0-9 \n\r\t\(\)\<\>\=\!\%\&\|\}\-\;\.]+

charconst    \'[^\']\'            { return CHARCONST; }
doubleconst  [0-9]+\.[0-9]+       { return DOUBLE_CONST; }


%%

{addop}		return ADDOP;
{mulop}		return MULOP;
{relop}		return RELOP;
{number}	return NUMBER;

"IF"        { return MOTCLE; }
"THEN"      { return MOTCLE; }
"ELSE"      { return MOTCLE; }
"WHILE"     { return MOTCLE; }
"FOR"       { return MOTCLE; }
"DO"        { return MOTCLE; }
"TO"        { return MOTCLE; }
"BEGIN"     { return MOTCLE; }
"END"       { return MOTCLE; }
"VAR"       { return MOTCLE; }
"BOOLEAN"   { return MOTCLE; }
"INTEGER"   { return MOTCLE; }
"CHAR"      { return MOTCLE; }
"DOUBLE"    { return MOTCLE; }
"DISPLAY"   { return MOTCLE; }

{id}		return ID;

{stringconst}	return STRINGCONST;
"["		return RBRACKET;
"]"		return LBRACKET;
","		return COMMA;
";"		return SEMICOLON;
"."		return DOT;
":="		return ASSIGN;
"("		return LPARENT;
")"		return RPARENT;
"!"		return NOT;

":"     { return COLON; }
"."		return DOT;



<<EOF>>		return FEOF;
{ws}    {/* skip blanks and tabs */};
"(*"    { /* Skip comments between '(*' and '*)' */
		int c;
		while((c = yyinput()) != 0){
     			if(c == '*'){
     	    			if((c = yyinput()) == ')')
    	        			break;
    	     			else
  	          			unput(c);
  	   		}	
		}
	};

{unknown}	return UNKNOWN;

%%
