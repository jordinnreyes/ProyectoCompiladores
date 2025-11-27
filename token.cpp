#include <iostream>
#include "token.h"

using namespace std;

ostream& operator<<(ostream& outs, const Token& tok) {

    switch (tok.type) {

        // Palabras clave
        case Token::PROGRAM:     outs << "TOKEN(PROGRAM, \""   << tok.text << "\")"; break;
        case Token::VAR:         outs << "TOKEN(VAR, \""       << tok.text << "\")"; break;
        case Token::BEGIN_KW:    outs << "TOKEN(BEGIN, \""     << tok.text << "\")"; break;
        case Token::END_KW:      outs << "TOKEN(END, \""       << tok.text << "\")"; break;
        case Token::FUNCTION:    outs << "TOKEN(FUNCTION, \""  << tok.text << "\")"; break;
        case Token::PROCEDURE:   outs << "TOKEN(PROCEDURE, \"" << tok.text << "\")"; break;
        case Token::INTEGER:     outs << "TOKEN(INTEGER, \""   << tok.text << "\")"; break;
        case Token::LONGINT:     outs << "TOKEN(LONGINT, \""   << tok.text << "\")"; break;
        case Token::FLOAT:       outs << "TOKEN(FLOAT, \""     << tok.text << "\")"; break;
        case Token::UNSIGNED:    outs << "TOKEN(UNSIGNED, \""  << tok.text << "\")"; break;
        case Token::TYPEKW:      outs << "TOKEN(TYPE, \""      << tok.text << "\")"; break;
        case Token::IF:          outs << "TOKEN(IF, \""        << tok.text << "\")"; break;
        case Token::THEN:        outs << "TOKEN(THEN, \""      << tok.text << "\")"; break;
        case Token::ELSE:        outs << "TOKEN(ELSE, \""      << tok.text << "\")"; break;
        case Token::WHILE:       outs << "TOKEN(WHILE, \""     << tok.text << "\")"; break;
        case Token::DO:          outs << "TOKEN(DO, \""        << tok.text << "\")"; break;
        case Token::FOR:         outs << "TOKEN(FOR, \""       << tok.text << "\")"; break;
        case Token::TO:          outs << "TOKEN(TO, \""        << tok.text << "\")"; break;
        case Token::DOWNTO:      outs << "TOKEN(DOWNTO, \""    << tok.text << "\")"; break;
        case Token::WRITELN:     outs << "TOKEN(WRITELN, \""   << tok.text << "\")"; break;
        case Token::READLN:      outs << "TOKEN(READLN, \""    << tok.text << "\")"; break;

        // Delimitadores
        case Token::SEMICOL:     outs << "TOKEN(SEMICOL, \""   << tok.text << "\")"; break;
        case Token::COLON:       outs << "TOKEN(COLON, \""     << tok.text << "\")"; break;
        case Token::COMMA:       outs << "TOKEN(COMMA, \""     << tok.text << "\")"; break;
        case Token::DOT:         outs << "TOKEN(DOT, \""       << tok.text << "\")"; break;
        case Token::LPAREN:      outs << "TOKEN(LPAREN, \""    << tok.text << "\")"; break;
        case Token::RPAREN:      outs << "TOKEN(RPAREN, \""    << tok.text << "\")"; break;

        // Operadores
        case Token::ASSIGN:      outs << "TOKEN(ASSIGN, \""    << tok.text << "\")"; break;
        case Token::PLUS:        outs << "TOKEN(PLUS, \""      << tok.text << "\")"; break;
        case Token::MINUS:       outs << "TOKEN(MINUS, \""     << tok.text << "\")"; break;
        case Token::MUL:         outs << "TOKEN(MUL, \""       << tok.text << "\")"; break;
        case Token::DIV:         outs << "TOKEN(DIV, \""       << tok.text << "\")"; break;
        case Token::REALDIV:     outs << "TOKEN(REALDIV, \""   << tok.text << "\")"; break;
        case Token::MOD:         outs << "TOKEN(MOD, \""       << tok.text << "\")"; break;

        // Comparadores
        case Token::EQ:          outs << "TOKEN(EQ, \""        << tok.text << "\")"; break;
        case Token::LT:          outs << "TOKEN(LT, \""        << tok.text << "\")"; break;
        case Token::LE:          outs << "TOKEN(LE, \""        << tok.text << "\")"; break;
        case Token::GT:          outs << "TOKEN(GT, \""        << tok.text << "\")"; break;
        case Token::GE:          outs << "TOKEN(GE, \""        << tok.text << "\")"; break;
        case Token::NEQ:         outs << "TOKEN(NEQ, \""       << tok.text << "\")"; break;

        // Literales
        case Token::NUM:         outs << "TOKEN(NUM, \""       << tok.text << "\")"; break;
        case Token::FLOATNUM:    outs << "TOKEN(FLOATNUM, \""  << tok.text << "\")"; break;
        case Token::ID:          outs << "TOKEN(ID, \""        << tok.text << "\")"; break;

        // Fin / error
        case Token::END:         outs << "TOKEN(END)"; break;
        case Token::ERR:         outs << "TOKEN(ERR, \""       << tok.text << "\")"; break;
    }

    return outs;
}

ostream& operator<<(ostream& outs, const Token* tok) {
    if (!tok) return outs << "TOKEN(NULL)";
    return outs << *tok;
}
