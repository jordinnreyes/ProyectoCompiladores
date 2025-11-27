#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "ast.h"
#include <list>

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;

    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();

    // Bloque begin ... end
    Body* parseBody();
    void parseVarBlock(std::list<VarDec*>& outList);

public:
    Parser(Scanner* scanner);

    Program* parseProgram();

    // Declaraciones
    VarDec*    parseVarDec();     // (versión simple: var x,y: tipo;)
    TypeAlias* parseTypeAlias();  // alias = tipo;
    FunDec*    parseFunDec();     // function f(...): tipo; [var ...;] begin ... end;

    // Sentencias
    Stm* parseStm();

    // Expresiones
    Exp* parseCE();  // Comparaciones: <, <=, >, >=, =, <>
    Exp* parseBE();  // Suma/resta
    Exp* parseE();   // *, div, /, mod
    Exp* parseT();   // unarios +/- 
    Exp* parseF();   // primarias: NUM, FLOATNUM, (expr), id, llamada f(...), casts tipo(expr)
};

#endif // PARSER_H
