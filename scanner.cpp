#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s): input(s), first(0), current(0) { }

bool is_white_space(char c) {
    return c==' ' || c=='\n' || c=='\r' || c=='\t';
}

Token* Scanner::nextToken() {
    Token* token;

    // Saltar espacios en blanco
    while (current < input.length() && is_white_space(input[current]))
        current++;

    // Fin de archivo
    if (current >= input.length())
        return new Token(Token::END);

    char c = input[current];
    first = current;

    // =======================
    //   COMENTARIOS { ... }
    // =======================
    if (c == '{') {
        current++;
        while (current < input.length() && input[current] != '}')
            current++;

        if (current < input.length() && input[current] == '}')
            current++;
        return nextToken();
    }

    // =======================
    //     NÚMEROS
    // =======================
    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;

        if (current < input.length() && input[current] == '.') {
            current++;
            bool atleastone = false;

            while (current < input.length() && isdigit(input[current])) {
                current++;
                atleastone = true;
            }

            token = new Token(Token::FLOATNUM, input, first, current - first);
        } else {
            token = new Token(Token::NUM, input, first, current - first);
        }
    }

    // =======================
    // IDENTIFICADORES / KEYWORDS
    // =======================
    else if (isalpha(c)) {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current]=='_'))
            current++;

        string lexema = input.substr(first, current - first);

        // Palabras clave Pascal
        if      (lexema=="program")   return new Token(Token::PROGRAM,  input, first, current-first);
        else if (lexema=="begin")     return new Token(Token::BEGIN_KW, input, first, current-first);
        else if (lexema=="end")       return new Token(Token::END_KW,   input, first, current-first);
        else if (lexema=="var")       return new Token(Token::VAR,      input, first, current-first);

        else if (lexema=="if")        return new Token(Token::IF,       input, first, current-first);
        else if (lexema=="then")      return new Token(Token::THEN,     input, first, current-first);
        else if (lexema=="else")      return new Token(Token::ELSE,     input, first, current-first);

        else if (lexema=="while")     return new Token(Token::WHILE,    input, first, current-first);
        else if (lexema=="do")        return new Token(Token::DO,       input, first, current-first);

        else if (lexema=="for")       return new Token(Token::FOR,      input, first, current-first);
        else if (lexema=="to")        return new Token(Token::TO,       input, first, current-first);
        else if (lexema=="downto")    return new Token(Token::DOWNTO,   input, first, current-first);

        // Funciones / procedimientos
        else if (lexema=="function")  return new Token(Token::FUNCTION,  input, first, current-first);
        else if (lexema=="procedure") return new Token(Token::PROCEDURE, input, first, current-first);

        // I/O
        else if (lexema=="writeln")   return new Token(Token::WRITELN, input, first, current-first);
        else if (lexema=="readln")    return new Token(Token::READLN,  input, first, current-first);

        // Tipos
        else if (lexema=="integer")   return new Token(Token::INTEGER,  input, first, current-first);
        else if (lexema=="longint")   return new Token(Token::LONGINT,  input, first, current-first);
        else if (lexema=="real" || lexema=="float")
                                      return new Token(Token::FLOAT,    input, first, current-first);
        else if (lexema=="unsigned")  return new Token(Token::UNSIGNED, input, first, current-first);
        else if (lexema=="type")      return new Token(Token::TYPEKW,   input, first, current-first);

        // Operadores palabra
        else if (lexema=="div")       return new Token(Token::DIV, input, first, current-first);
        else if (lexema=="mod")       return new Token(Token::MOD, input, first, current-first);

        // Identificador
        else                          return new Token(Token::ID,  input, first, current-first);
    }

    // =======================
    // OPERADORES Y DELIMITADORES
    // =======================
    else if (c=='+' || c=='-' || c=='*' || c=='(' || c==')' ||
             c==';' || c=='=' || c=='<' || c=='>' || c==',' ||
             c==':' || c=='.' || c=='/' )
    {
        switch (c) {
            case '+': token = new Token(Token::PLUS,  c); current++; break;
            case '-': token = new Token(Token::MINUS, c); current++; break;
            case '*': token = new Token(Token::MUL,   c); current++; break;

            case '(':
                token = new Token(Token::LPAREN, c);
                current++;
                break;

            case ')':
                token = new Token(Token::RPAREN, c);
                current++;
                break;

            case ';':
                token = new Token(Token::SEMICOL, c);
                current++;
                break;

            case ',':
                token = new Token(Token::COMMA, c);
                current++;
                break;

            case '.':
                token = new Token(Token::DOT, c);
                current++;
                break;

            case ':':
                if (current+1 < (int)input.length() && input[current+1] == '=') {
                    token = new Token(Token::ASSIGN, input, first, 2); // ':='
                    current += 2;
                } else {
                    token = new Token(Token::COLON, c);
                    current++;
                }
                break;

            case '=':
                token = new Token(Token::EQ, c);
                current++;
                break;

            case '<':
                if (current+1 < (int)input.length() && input[current+1]=='=') {
                    token = new Token(Token::LE, input, first, 2);
                    current += 2;
                }
                else if (current+1 < (int)input.length() && input[current+1]=='>') {
                    token = new Token(Token::NEQ, input, first, 2);
                    current += 2;
                }
                else {
                    token = new Token(Token::LT, c);
                    current++;
                }
                break;

            case '>':
                if (current+1 < (int)input.length() && input[current+1]=='=') {
                    token = new Token(Token::GE, input, first, 2);
                    current += 2;
                }
                else {
                    token = new Token(Token::GT, c);
                    current++;
                }
                break;

            case '/':
                token = new Token(Token::REALDIV, c);
                current++;
                break;
        }
    }

    // =======================
    // CARÁCTER DESCONOCIDO
    // =======================
    else {
        token = new Token(Token::ERR, c);
        current++;
    }

    return token;
}

Scanner::~Scanner() { }

int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    string OutputFileName = InputFile;
    size_t pos = OutputFileName.find_last_of(".");
    if (pos != string::npos)
        OutputFileName = OutputFileName.substr(0, pos);

    OutputFileName += "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return 0;
    }

    outFile << "Scanner\n" << endl;

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << endl;
            delete tok;
            outFile << "\nScanner exitoso\n\n";
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido\n\nScanner no exitoso\n\n";
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
