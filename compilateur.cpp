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
#include <map>
#include <cstring>
#include <FlexLexer.h>
#include "tokeniser.h"

using namespace std;


// === Déclarations globales ===
map<string, TYPES> DeclaredVars;      // on a Changer le type de DeclaredVars
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
void TypeErreur(const string& msg) {
    cerr << "❌ Erreur ligne " << lexer->lineno() 
         << " : " << msg 
         << " (lu : '" << lexer->YYText() << "')" << endl;
    exit(1);
}

// Erreur classique (utilisée pour les erreurs de syntaxe, crochets, mots-clés...)
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


//  Number : retourne toujours le type UNSIGNED_INT
// Sert à vérifier que les valeurs numériques sont bien des entiers

TYPES Number() {
    cout << "\tpush $" << atoi(lexer->YYText()) << endl;
    current = (TOKEN) lexer->yylex();
    return UNSIGNED_INT;
}


//  Identifier : retourne le type de la variable déjà déclarée
// Actuellement, on suppose que toutes les variables sont de type UNSIGNED_INT

TYPES Identifier() {
    string nom = lexer->YYText();
    if (!VariableConnue(nom))
        Erreur("Variable non déclarée : " + nom);
    cout << "\tpush " << nom << endl;
    current = (TOKEN) lexer->yylex();
    return DeclaredVars[nom];
}


TYPES Expression();                
OPADD AdditiveOperator(void); 
void IfStatement();
void WhileStatement();
void ForStatement();
void BlockStatement();
void DisplayStatement();

void VarDeclaration(); 
TYPES Type();         

void VarDeclarationPart();  // Prototype de la fonction



//  Factor : appelle Number ou Identifier ou Expression récursivement
// Retourne le type qu’il a rencontré

// Analyse un facteur d'une expression (nombre, variable, parenthèses)
TYPES Factor() {
    TYPES t;
    if (current == RPARENT) {
        current = (TOKEN) lexer->yylex();
        t = Expression();
        if (current != LPARENT)
            Erreur("Parenthèse fermante attendue");
        current = (TOKEN) lexer->yylex();
    } else if (current == NUMBER) {
        t = Number();
    } else if (current == ID) {
        t = Identifier();
    } else {
        Erreur("valeur attendue : identifiant, nombre ou parenthèse");
    }
    return t;
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


TYPES Term() {
    TYPES t1 = Factor();
    while (current == MULOP) {
        OPMUL op = MultiplicativeOperator();
        TYPES t2 = Factor();
        if (t1 != t2) TypeErreur("types incompatibles dans Term");
        cout << "\tpop %rbx" << endl;
        cout << "\tpop %rax" << endl;
        switch(op) {
            case AND:
                cout << "\tmulq\t%rbx" << endl;
                cout << "\tpush %rax\t# AND" << endl;
                break;
            case MUL:
                cout << "\tmulq\t%rbx" << endl;
                cout << "\tpush %rax\t# MUL" << endl;
                break;
            case DIV:
                cout << "\tmovq $0, %rdx" << endl;
                cout << "\tdiv %rbx" << endl;
                cout << "\tpush %rax\t# DIV" << endl;
                break;
            case MOD:
                cout << "\tmovq $0, %rdx" << endl;
                cout << "\tdiv %rbx" << endl;
                cout << "\tpush %rdx\t# MOD" << endl;
                break;
            default:
                Erreur("opérateur multiplicatif attendu");
        }
    }
    return t1;

}

// SimpleExpression := Term {AdditiveOperator Term}
//  SimpleExpression : gère les +, -, ||
// Vérifie les types des Term, retourne le type si tous identiques

TYPES SimpleExpression() {
    TYPES t1 = Term();
    while (current == ADDOP) {
        OPADD op = AdditiveOperator();
        TYPES t2 = Term();
        if (t1 != t2) TypeErreur("types incompatibles dans SimpleExpression");
        cout << "\tpop %rbx" << endl;
        cout << "\tpop %rax" << endl;
        switch(op) {
            case OR:
                cout << "\taddq\t%rbx, %rax\t# OR" << endl;
                break;
            case ADD:
                cout << "\taddq\t%rbx, %rax\t# ADD" << endl;
                break;
            case SUB:
                cout << "\tsubq\t%rbx, %rax\t# SUB" << endl;
                break;
            default:
                Erreur("opérateur additif inconnu");
        }
        cout << "\tpush %rax" << endl;
    }
    return t1;
}







// DeclarationPart := "[" Letter {"," Letter} "]"
// Analyse la déclaration des variables entre [ ... ] et génère leur allocation mémoire


// Déclaration d'une ligne de variables typées : a,b,c : BOOLEAN
void VarDeclaration() {
    set<string> variables;

    if (current != ID)
        Erreur("Nom de variable attendu");

    variables.insert(lexer->YYText());
    current = (TOKEN) lexer->yylex();

    while (current == COMMA) {
        current = (TOKEN) lexer->yylex();
        if (current != ID)
            Erreur("Nom de variable attendu après ','");

        variables.insert(lexer->YYText());
        current = (TOKEN) lexer->yylex();
    }

    if (current != COLON)
        Erreur("':' attendu");

    current = (TOKEN) lexer->yylex();
    TYPES type = Type();

    for (auto& nom : variables) {
        if (DeclaredVars.count(nom))
            Erreur("Variable déjà déclarée : " + nom);

        DeclaredVars[nom] = type;

        // Affiche la bonne allocation mémoire en assembleur suivant le type
        switch(type) {
            case BOOLEAN:
            case UNSIGNED_INT:
                cout << nom << ":\t.quad 0" << endl;
                break;
            // tu peux gérer d'autres types ici plus tard (CHAR, DOUBLE)
            default:
                Erreur("Type non géré en allocation mémoire");
        }
    }
}

// Gestion complète de la déclaration VAR ...
void VarDeclarationPart() {
    if (GetKeyword() != VAR_)
        Erreur("'VAR' attendu");

    current = (TOKEN) lexer->yylex(); // Passe 'VAR'

    VarDeclaration();

    while (current == SEMICOLON) {
        current = (TOKEN) lexer->yylex();
        VarDeclaration();
    }

    if (current != DOT)
        Erreur("'.' attendu à la fin de la déclaration de variables");

    current = (TOKEN) lexer->yylex(); // Passe '.'
}


TYPES Type() {
    if (current != MOTCLE)
        Erreur("Type attendu");

    string t = lexer->YYText();

    if (t == "BOOLEAN") {
        current = (TOKEN) lexer->yylex();
        return BOOLEAN;
    } else if (t == "INTEGER") {
        current = (TOKEN) lexer->yylex();
        return UNSIGNED_INT;
    } else {
        Erreur("Type non reconnu : " + t);
        return BOOLEAN; // juste pour éviter warnings
    }
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



//  Expression : gère éventuellement une comparaison entre deux expressions arithmétiques
// Si une comparaison est faite (==, !=, <, etc.), retourne BOOLEAN
// Sinon retourne le type de la SimpleExpression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// Gère une expression complète : opération simple avec comparateur optionnel (==, <, etc.)
TYPES Expression() {
    TYPES t1 = SimpleExpression(); // On commence par une expression simple (addition, multiplication...)

        if (current == RELOP) {
        OPREL oprel = RelationalOperator();
        TYPES t2 = SimpleExpression();
        if (t1 != t2) TypeErreur("types incompatibles pour la comparaison");

        cout << "\tpop %rax\n\tpop %rbx\n\tcmpq %rax, %rbx" << endl;
        string tag = to_string(++tagID);

        if      (oprel == EQU)  cout << "\tje Vrai" << tag << endl;
        else if (oprel == DIFF) cout << "\tjne Vrai" << tag << endl;
        else if (oprel == INF)  cout << "\tjb Vrai" << tag << endl;
        else if (oprel == SUP)  cout << "\tja Vrai" << tag << endl;
        else if (oprel == INFE) cout << "\tjbe Vrai" << tag << endl;
        else if (oprel == SUPE) cout << "\tjae Vrai" << tag << endl;
        else Erreur("comparateur non reconnu");

        cout << "\tpush $0\n\tjmp Suite" << tag << endl;
        cout << "Vrai" << tag << ":\tpush $-1\nSuite" << tag << ":" << endl;

        return BOOLEAN;  // ✅ ici uniquement si on fait une comparaison
    }

    return t1;  // ✅ sinon, on renvoie le type de la SimpleExpression (ex: a+1 => UNSIGNED_INT)

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





//  Affectation : vérifie que le type de la variable et celui de l'expression sont identiques
// Sinon déclenche une erreur de type

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
    if (kw == "DISPLAY") return DISPLAY_;
    if (kw == "VAR") return VAR_;
    if (kw == "BOOLEAN") return BOOLEAN_;
    if (kw == "INTEGER") return INTEGER_;
    if (kw == "CHAR") return CHAR_;
    if (kw == "DOUBLE") return DOUBLE_;

    return UNKNOWN_KEYWORD;
}
// Ajoute la prise en compte de VAR dans Statement()
void Statement() {
    if (current == ID) {
        AssignementStatement();
    } else if (current == MOTCLE) {
        int kw = GetKeyword();
        switch(kw) {
            case VAR_:
                VarDeclarationPart();
                break;
            case IF_:
                IfStatement();
                break;
            case WHILE_:
                WhileStatement();
                break;
            case FOR_:
                ForStatement();
                break;
            case BEGIN_:
                BlockStatement();
                break;
            case DISPLAY_:
                DisplayStatement();
                break;
            default:
                Erreur("Mot-clé inattendu");
        }
    } else {
        Erreur("Instruction inconnue");
    }
}



//  Conditionnelle IF ou boucle WHILE : vérifie que l'expression conditionnelle est bien de type BOOLEAN
// Sinon génère une erreur de type



// Gère une instruction conditionnelle IF avec option ELSE
// Syntaxe : IF <expression> THEN <instruction> [ELSE <instruction>]
void IfStatement() {
    unsigned long numTag = ++tagID;

    // Vérifie le mot-clé IF
    if (current != MOTCLE || GetKeyword() != IF_)
        Erreur("Mot-clé 'IF' attendu");

    current = (TOKEN) lexer->yylex();  // Passe IF

    // Évalue la condition

    TYPES tcond = Expression();
    if (tcond != BOOLEAN) TypeErreur("La condition d’un IF doit être booléenne");


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

    TYPES tcond = Expression();
    if (tcond != BOOLEAN) TypeErreur("La condition d’un WHILE doit être booléenne");

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



    TYPES tmax = Expression();
    if (tmax != UNSIGNED_INT) TypeErreur("La borne du FOR doit être un entier non signé");

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
void Program() {
    if (current == MOTCLE && GetKeyword() == VAR_) {
        VarDeclarationPart();  // On traite la section VAR avant les instructions
    }
    StatementPart();
}


void DisplayStatement() {
    current = (TOKEN) lexer->yylex();
    TYPES t = Expression();

    unsigned long localTag = ++tagID;  // Incrémente ici pour avoir un tag unique

    if (t == UNSIGNED_INT) {
        cout << "\tpop %rsi" << endl;                     
        cout << "\tmovq $FormatString1, %rdi" << endl;    
        cout << "\tmovl $0, %eax" << endl;                 
        cout << "\tcall printf@PLT" << endl;
    }
    else if (t == BOOLEAN) {
        cout << "\tpop %rdx\t# valeur booléenne à afficher" << endl;
        cout << "\tcmpq $0, %rdx" << endl;
        cout << "\tje BoolFalse" << localTag << endl;
        cout << "\tmovq $TrueString, %rsi" << endl;
        cout << "\tjmp BoolEnd" << localTag << endl;
        cout << "BoolFalse" << localTag << ":" << endl;
        cout << "\tmovq $FalseString, %rsi" << endl;
        cout << "BoolEnd" << localTag << ":" << endl;
        cout << "\tmovl $1, %edi" << endl;
        cout << "\tmovl $0, %eax" << endl;
        cout << "\tcall printf@PLT" << endl;
    }
    else {
        TypeErreur("DISPLAY ne fonctionne qu'avec des entiers non signés ou booléens");
    }
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
    
    cout << "DisplayMsg:\t.string \"Résultat : %llu\\n\"\t# affichage entier 64 bits" << endl;
    cout << "FormatString1:\t.string \"%llu\\n\"\t# affichage brut sans message" << endl;

    cout << "TrueString:\t.string \"TRUE\\n\"" << endl;
    cout << "FalseString:\t.string \"FALSE\\n\"" << endl;



    // Section standard GNU
    cout << "\t.section .note.GNU-stack,\"\",@progbits" << endl;

    if (current != FEOF)
        Erreur("Il reste du contenu après la fin du programme.");

    return 0;
}
