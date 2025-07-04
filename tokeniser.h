// tokeniser.h

#ifndef TOKENISER_H
#define TOKENISER_H

enum TOKEN {
    FEOF, UNKNOWN, NUMBER, ID, STRINGCONST, RBRACKET, LBRACKET,
    RPARENT, LPARENT, COMMA, SEMICOLON, DOT, ADDOP, MULOP,
    RELOP, NOT, ASSIGN, MOTCLE, COLON,     DOUBLE_CONST_TOKEN,   // suffixe _TOKEN
    CHARCONST_TOKEN,
    DOUBLE_TYPE_TOKEN 
};

// Juste la **déclaration** de l'enum des mots-clés
enum MOTCLEVAL {
    IF_, THEN_, ELSE_, WHILE_, DO_, FOR_, TO_, BEGIN_, END_,
    DISPLAY_, VAR_, BOOLEAN_, INTEGER_, CHAR_KEYWORD_, DOUBLE_KEYWORD_, UNKNOWN_KEYWORD
};


// Juste la **déclaration** de la fonction :
int GetKeyword();


// Enumération des types
enum TYPES {
    UNSIGNED_INT,
    BOOLEAN,
    CHAR_TYPE,
    DOUBLE_TYPE,
    DOUBLE_CONST,
    CHARCONST
};






#endif
