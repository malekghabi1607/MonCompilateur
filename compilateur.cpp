//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"

#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <cstring>
#include <FlexLexer.h>
#include "tokeniser.h"

using namespace std;


// === Déclarations globales ===
set<string> DeclaredVars;          // Variables déclarées
unsigned long tagID = 0;           // Pour les étiquettes Vrai/Suite
char lookedAhead;                  // Caractère look-ahead
int NLookedAhead = 0;              // Compteur look-ahead

TOKEN current;                     // Token courant
FlexLexer* lexer = new yyFlexLexer; // Lexer Flex++


// énumérations pour les opérateurs
enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR};
enum OPADD {ADD, SUB, OR, WTFA};
enum OPMUL {MUL, DIV, MOD, AND, WTFM};


// Vérifie si une variable est déclarée
bool VariableConnue(const string& nom) {
    return DeclaredVars.count(nom) != 0;
}

// Affiche une erreur personnalisée et arrête le programme
void Erreur(const string& msg) {
    cerr << "❌ Erreur ligne " << lexer->lineno() 
         << " : " << msg 
         << " (lu : '" << lexer->YYText() << "')" << endl;
    exit(1);
}





// Program := [DeclarationPart] StatementPart
// DeclarationPart := "[" Letter {"," Letter} "]"
// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement
// AssignementStatement := Letter "=" Expression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Letter | "(" Expression ")"| "!" Factor
// Number := Digit{Digit}

// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"



// --- Analyse syntaxique et génération d'assembleur ---

void Number() {
    cout << "\tpush $" << atoi(lexer->YYText()) << endl;
    current = (TOKEN) lexer->yylex();
}

void Identifier() {
    cout << "\tpush " << lexer->YYText() << endl;
    current = (TOKEN) lexer->yylex();
}

void Expression();                
OPADD AdditiveOperator(void); 
void IfStatement();
void WhileStatement();
void ForStatement();
void BlockStatement();







// Analyse un facteur d'une expression (nombre, variable, parenthèses)
void Factor(void) {
    if (current == RPARENT) {
        current = (TOKEN) lexer->yylex();
        Expression();
        if (current != LPARENT)
            Erreur("parenthèse fermante attendue");
        current = (TOKEN) lexer->yylex();
    } else if (current == NUMBER) {
        Number();
    } else if (current == ID) {
        Identifier();
    } else {
        Erreur("valeur attendue : identifiant, nombre ou parenthèse");
    }
}


// MultiplicativeOperator := "*" | "/" | "%" | "&&"

// Analyse les opérateurs de multiplication / division / modulo / et logique
OPMUL MultiplicativeOperator(void){
	OPMUL opmul;

	if(strcmp(lexer->YYText(),"*")==0)
		opmul=MUL;

	else if(strcmp(lexer->YYText(),"/")==0)
		opmul=DIV;

	else if(strcmp(lexer->YYText(),"%")==0)
		opmul=MOD;

	else if(strcmp(lexer->YYText(),"&&")==0)
		opmul=AND;

	else opmul=WTFM;
	current=(TOKEN) lexer->yylex();
	return opmul;

}
// Analyse un terme : une suite de facteurs liés par des opérateurs multiplicatifs
// Term := Factor {MultiplicativeOperator Factor}

void Term(void){
	OPMUL mulop;
	Factor();
	while(current==MULOP){
		mulop=MultiplicativeOperator();		// Save operator in local variable
		Factor();
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		switch(mulop){
			case AND:
				cout << "\tmulq	%rbx"<<endl;	// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# AND"<<endl;	// store result
				break;
			case MUL:
				cout << "\tmulq	%rbx"<<endl;	// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# MUL"<<endl;	// store result
				break;
			case DIV:
				cout << "\tmovq $0, %rdx"<<endl; 	// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;			// quotient goes to %rax
				cout << "\tpush %rax\t# DIV"<<endl;		// store result
				break;
			case MOD:
				cout << "\tmovq $0, %rdx"<<endl; 	// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;			// remainder goes to %rdx
				cout << "\tpush %rdx\t# MOD"<<endl;		// store result
				break;
			default:
				Erreur("opérateur multiplicatif attendu");
		}
	}

}


// SimpleExpression := Term {AdditiveOperator Term}


void SimpleExpression(void){


	OPADD adop;
	Term();

	while(current==ADDOP){
		adop=AdditiveOperator();		// Save operator in local variable
		Term();
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		switch(adop){
			case OR:
				cout << "\taddq	%rbx, %rax\t# OR"<<endl;// operand1 OR operand2
				break;			
			case ADD:
				cout << "\taddq	%rbx, %rax\t# ADD"<<endl;	// add both operands
				break;			
			case SUB:	
				cout << "\tsubq	%rbx, %rax\t# SUB"<<endl;	// substract both operands
				break;
			default:
				Erreur("opérateur additif inconnu");
		}
		cout << "\tpush %rax"<<endl;			// store result
	}



}



// DeclarationPart := "[" Letter {"," Letter} "]"
// Analyse la déclaration des variables entre [ ... ] et génère leur allocation mémoire
void DeclarationPart(void) {
    if (current != RBRACKET)
        Erreur("Il manque le crochet ouvrant '[' pour commencer la déclaration");

    // Section des données globales pour le programme
    cout << "\t.data" << endl;
    cout << "\t.align 8" << endl;

    current = (TOKEN) lexer->yylex();

    while (true) {
        if (current != ID)
            Erreur("Nom de variable attendu");

        string nomVar = lexer->YYText();

        if (DeclaredVars.count(nomVar))
            Erreur("La variable '" + nomVar + "' est déjà définie");

        DeclaredVars.insert(nomVar);
        cout << nomVar << ":\t.quad 0" << endl;

        current = (TOKEN) lexer->yylex();

        if (current != COMMA)
            break; // Fin de la liste des variables
        current = (TOKEN) lexer->yylex();
    }

    if (current != LBRACKET)
        Erreur("Il manque le crochet fermant ']' après les variables");

    current = (TOKEN) lexer->yylex();
}


// Détecte et interprète un opérateur de comparaison logique (==, !=, <, >, <=, >=)
OPREL RelationalOperator(void) {
    string symbole = lexer->YYText();  // On récupère le symbole actuel
    current = (TOKEN) lexer->yylex();  // On passe au prochain token

    if (symbole == "==") return EQU;
    if (symbole == "!=") return DIFF;
    if (symbole == "<")  return INF;
    if (symbole == ">")  return SUP;
    if (symbole == "<=") return INFE;
    if (symbole == ">=") return SUPE;

    return WTFR; // Si aucun des cas connus n'est reconnu
}


// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// Gère une expression complète : opération simple avec comparateur optionnel (==, <, etc.)
void Expression(void) {
    SimpleExpression(); // On commence par une expression simple (addition, multiplication...)

    if (current == RELOP) {
        OPREL oprel = RelationalOperator(); // On récupère le comparateur logique
        SimpleExpression(); // Deuxième partie de l'expression à comparer

        // Génération du code assembleur pour la comparaison
        cout << "\tpop %rax\n\tpop %rbx\n\tcmpq %rax, %rbx" << endl;

        string tag = to_string(++tagID); // Création d'une étiquette unique

        // Selon le comparateur, on saute vers le bloc 'Vrai'
        if      (oprel == EQU)  cout << "\tje Vrai" << tag << endl;
        else if (oprel == DIFF) cout << "\tjne Vrai" << tag << endl;
        else if (oprel == INF)  cout << "\tjb Vrai" << tag << endl;
        else if (oprel == SUP)  cout << "\tja Vrai" << tag << endl;
        else if (oprel == INFE) cout << "\tjbe Vrai" << tag << endl;
        else if (oprel == SUPE) cout << "\tjae Vrai" << tag << endl;
        else Erreur("comparateur non reconnu");

        // Faux : on pousse 0 (faux), puis on saute la partie 'vrai'
        cout << "\tpush $0\n\tjmp Suite" << tag << endl;

        // Vrai : on pousse -1 (vrai)
        cout << "Vrai" << tag << ":\tpush $-1\nSuite" << tag << ":" << endl;
    }
}




// AdditiveOperator := "+" | "-" | "||"


OPADD AdditiveOperator(void){
	OPADD opadd;
	if(strcmp(lexer->YYText(),"+")==0)
		opadd=ADD;

	else if(strcmp(lexer->YYText(),"-")==0)
		opadd=SUB;

	else if(strcmp(lexer->YYText(),"||")==0)
		opadd=OR;

	else opadd=WTFA;
	current=(TOKEN) lexer->yylex();
	return opadd;
}




// Analyse une instruction d'affectation : une variable reçoit une valeur (ex : x := 5+2)
void AssignementStatement(void) {
    if (current != ID)
        Erreur("Une variable était attendue ici");

    string nomVar = lexer->YYText();

    if (!VariableConnue(nomVar))
        Erreur("La variable '" + nomVar + "' n’a pas été déclarée");

    current = (TOKEN) lexer->yylex();

    if (current != ASSIGN)
        Erreur("Il manque l’opérateur ':=' dans l’affectation");

    current = (TOKEN) lexer->yylex();

    Expression();

    cout << "\tpop " << nomVar << endl;
}



int GetKeyword() {
    string kw = lexer->YYText();
    if (kw == "IF") return IF_;
    if (kw == "THEN") return THEN_;
    if (kw == "ELSE") return ELSE_;
    if (kw == "WHILE") return WHILE_;
    if (kw == "DO") return DO_;
    if (kw == "FOR") return FOR_;
    if (kw == "TO") return TO_;
    if (kw == "BEGIN") return BEGIN_;
    if (kw == "END") return END_;
    return UNKNOWN_KEYWORD;
}

// Traite une instruction simple (actuellement : affectation uniquement)
void Statement(void) {
    if (current == ID) {
        AssignementStatement();
    } else if (current == MOTCLE) {
        switch(GetKeyword()) {
            case IF_:     IfStatement(); break;
            case WHILE_:  WhileStatement(); break;
            case FOR_:    ForStatement(); break;
            case BEGIN_:  BlockStatement(); break;
            default: Erreur("Mot-clé inattendu");
        }
    } else {
        Erreur("Instruction inconnue");
    }
}
// Gère une instruction conditionnelle IF avec option ELSE
// Syntaxe : IF <expression> THEN <instruction> [ELSE <instruction>]
void IfStatement() {
    unsigned long numTag = ++tagID;

    // Vérifie le mot-clé IF
    if (current != MOTCLE || GetKeyword() != IF_)
        Erreur("Mot-clé 'IF' attendu");

    current = (TOKEN) lexer->yylex();  // Passe IF

    // Évalue la condition
    Expression();

    // Génère le code pour vérifier si la condition est fausse (== 0)
    cout << "\tpop %rax\n\tcmpq $0, %rax\n";
    cout << "\tje ELSE" << numTag << endl;

    // Vérifie et passe THEN
    if (current != MOTCLE || GetKeyword() != THEN_)
        Erreur("'THEN' attendu après IF");

    current = (TOKEN) lexer->yylex();  // Passe THEN

    // Partie exécutée si condition vraie
    Statement();
    cout << "\tjmp FINIF" << numTag << endl;

    // Partie exécutée si condition fausse
    cout << "ELSE" << numTag << ":" << endl;

    if (current == MOTCLE && GetKeyword() == ELSE_) {
        current = (TOKEN) lexer->yylex();  // Passe ELSE
        Statement();
    }

    // Fin de l'instruction IF
    cout << "FINIF" << numTag << ":" << endl;
}



// Gère une boucle conditionnelle WHILE
// Syntaxe : WHILE <expression> DO <instruction>
void WhileStatement() {
    unsigned long tag = ++tagID;

    if (GetKeyword() != WHILE_) Erreur("Mot-clé 'WHILE' attendu");
    current = (TOKEN) lexer->yylex();

    // Début de la boucle
    cout << "DEBUTWHILE" << tag << ":" << endl;

    // Évaluation de la condition
    Expression();
    cout << "\tpop %rax\n\tcmpq $0, %rax" << endl;
    cout << "\tje FINWHILE" << tag << endl;

    if (current != MOTCLE || GetKeyword() != DO_) Erreur("'DO' attendu après WHILE");
    current = (TOKEN) lexer->yylex();

    // Corps de la boucle
    Statement();
    cout << "\tjmp DEBUTWHILE" << tag << endl;

    // Fin de boucle
    cout << "FINWHILE" << tag << ":" << endl;
}


// Gère une boucle FOR à incrémentation
// Syntaxe : FOR <assignation> TO <expression> DO <instruction>
void ForStatement() {
    unsigned long tag = ++tagID;

    if (GetKeyword() != FOR_) Erreur("'FOR' attendu");
    current = (TOKEN) lexer->yylex();

    string var = lexer->YYText(); // i
    AssignementStatement();       // i := 0

    if (current != MOTCLE || GetKeyword() != TO_) Erreur("'TO' attendu après FOR");
    current = (TOKEN) lexer->yylex();

    cout << "DEBUTFOR" << tag << ":" << endl;

    Expression(); // évalue TO
    cout << "\tpop %rax" << endl;                    // TO -> rax
    cout << "\tcmpq %rax, " << var << endl;         // compare i > TO ?
    cout << "\tja FINFOR" << tag << endl;

    if (current != MOTCLE || GetKeyword() != DO_) Erreur("'DO' attendu après TO");
    current = (TOKEN) lexer->yylex();

    Statement();

    cout << "\tincq " << var << "\n\tjmp DEBUTFOR" << tag << endl;
    cout << "FINFOR" << tag << ":" << endl;
}



// Gère un bloc BEGIN ... END contenant plusieurs instructions
// Syntaxe : BEGIN <instruction> { ; <instruction> } END
void BlockStatement() {
    if (GetKeyword() != BEGIN_) Erreur("'BEGIN' attendu");
    current = (TOKEN) lexer->yylex();

    // Première instruction
    Statement();

    // Instructions séparées par ;
    while (current == SEMICOLON) {
        current = (TOKEN) lexer->yylex();
        Statement();
    }

    // Fin du bloc
    if (current != MOTCLE || GetKeyword() != END_) Erreur("'END' attendu pour fermer le bloc");
    current = (TOKEN) lexer->yylex();
}





// Partie exécutable du programme : enchaînement d’instructions terminées par un point
void StatementPart(void) {
    cout << "\t.text\n\t.globl main\nmain:\n\tmovq %rsp, %rbp" << endl;

    Statement();

    while (current == SEMICOLON) {
        current = (TOKEN) lexer->yylex();
        Statement();
    }

    if (current != DOT)
        Erreur("Fin du programme attendue (manque le '.')");

    current = (TOKEN) lexer->yylex();
}

// Lance l'analyse complète : déclaration + instructions
void Program(void) {
    if (current == RBRACKET)
        DeclarationPart();

    StatementPart();
}
// Point d'entrée principal du compilateur

int main(void) {
    // Entête du code assembleur
    cout << "\t\t\t# Code généré automatiquement par MonCompilateur" << endl;
    cout << "\t.extern printf" << endl; // appel à printf

    current = (TOKEN) lexer->yylex();
        Program();

    // Affichage de la variable a
    cout << "\tmovq a, %rsi" << endl;
    cout << "\tleaq msg_a(%rip), %rdi" << endl;
    cout << "\txor %rax, %rax" << endl;
    cout << "\tcall printf" << endl;

    // Affichage de la variable b
    cout << "\tmovq b, %rsi" << endl;
    cout << "\tleaq msg_b(%rip), %rdi" << endl;
    cout << "\txor %rax, %rax" << endl;
    cout << "\tcall printf" << endl;



    // Affichage de la variable c
    cout << "\tmovq c, %rsi" << endl;
    cout << "\tleaq msg_c(%rip), %rdi" << endl;
    cout << "\txor %rax, %rax" << endl;
    cout << "\tcall printf" << endl;

    // Affichage de la variable z
    cout << "\tmovq z, %rsi" << endl;
    cout << "\tleaq msg_z(%rip), %rdi" << endl;
    cout << "\txor %rax, %rax" << endl;
    cout << "\tcall printf" << endl;


    // Épilogue du programme assembleur
    cout << "\tmovq %rbp, %rsp\n\tret" << endl;

    // Section de données en lecture seule pour le message
    cout << "\t.section .rodata" << endl;
    cout << "msg_a:\t.string \"Valeur de a : %ld\\n\"" << endl;
    cout << "msg_b:\t.string \"Valeur de b : %ld\\n\"" << endl;
    cout << "msg_c:\t.string \"Valeur de c : %ld\\n\"" << endl;
    cout << "msg_z:\t.string \"Valeur de z : %ld\\n\"" << endl;


    // Section standard GNU
    cout << "\t.section .note.GNU-stack,\"\",@progbits" << endl;

    if (current != FEOF)
        Erreur("Il reste du contenu après la fin du programme.");

    return 0;
}
