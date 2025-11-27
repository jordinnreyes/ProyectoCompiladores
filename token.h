#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>
using namespace std;

class Token {
public:
    enum Type {

        // ---- Palabras clave Pascal ----
        PROGRAM,        // program
        VAR,            // var
        BEGIN_KW,       // begin
        END_KW,         // end
        FUNCTION,       // function
        PROCEDURE,      // procedure
        INTEGER,        // integer
        LONGINT,        // longint
        FLOAT,          // real / float
        UNSIGNED,       // unsigned
        TYPEKW,         // type
        IF,             // if
        THEN,           // then
        ELSE,           // else
        WHILE,          // while
        DO,             // do
        FOR,            // for
        TO,             // to
        DOWNTO,         // downto
        WRITELN,        // writeln
        READLN,         // readln

        // ---- Delimitadores ----
        SEMICOL,        // ;
        COLON,          // :
        COMMA,          // ,
        DOT,            // .
        LPAREN,         // (
        RPAREN,         // )

        // ---- Operadores ----
        ASSIGN,         // :=
        PLUS,           // +
        MINUS,          // -
        MUL,            // *
        DIV,            // div
        REALDIV,        // /
        MOD,            // mod

        // ---- Comparadores ----
        EQ,             // =
        LT,             // <
        LE,             // <=
        GT,             // >
        GE,             // >=
        NEQ,            // <>

        // ---- Literales ----
        NUM,            // entero
        FLOATNUM,       // real con punto
        ID,             // identificador

        // ---- Final / error ----
        END,            // fin del archivo
        ERR
    };

    Type type;
    string text;

    Token(Type t) : type(t), text("") {}
    Token(Type t, char c) : type(t), text(string(1,c)) {}
    Token(Type t, const string& src, int first, int len)
        : type(t), text(src.substr(first, len)) {}

    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif
