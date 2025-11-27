#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Utilidad local
// =============================
static inline void expectOrThrow(bool ok, const char* msg) {
    if (!ok) throw runtime_error(msg);
}

// =============================
// Constructor y utilidades
// =============================
Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current  = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* tmp = current;
        if (previous) delete previous;
        current  = scanner->nextToken();
        previous = tmp;

        if (check(Token::ERR)) {
            throw runtime_error("Error léxico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Program* Parser::parseProgram() {
    Program* p = new Program();

    if (match(Token::PROGRAM)) {
        expectOrThrow(match(Token::ID), "Se esperaba nombre del programa tras 'program'");
        expectOrThrow(match(Token::SEMICOL), "Se esperaba ';' tras cabecera de 'program'");
    }

    while (check(Token::TYPEKW)) {
        expectOrThrow(match(Token::TYPEKW), "Se esperaba 'type'");

        while (check(Token::ID)) {
            TypeAlias* ta = parseTypeAlias();
            p->tdefs[ta->alias] = ta->target;
            delete ta;
            expectOrThrow(match(Token::SEMICOL), "Se esperaba ';' tras definición 'type'");
        }
    }
    while (check(Token::VAR)) {
        parseVarBlock(p->vdlist);
    }

    // ====== Declaraciones de funciones ======
    while (check(Token::FUNCTION)) {
        FunDec* f = parseFunDec();
        p->fdlist.push_back(f);
    }

    // ====== Bloque principal begin ... end. ======
    Body* mainBody = parseBody();  // consume BEGIN_KW ... END_KW

    // Punto final del programa
    expectOrThrow(match(Token::DOT), "Se esperaba '.' al final del programa Pascal");

    // Convertimos el bloque principal en una función 'main'
    FunDec* mainFun = new FunDec();
    mainFun->nombre = "main";
    mainFun->tipo   = "integer";
    mainFun->cuerpo = mainBody;
    p->fdlist.push_back(mainFun);

    cout << "Parser exitoso" << endl;
    return p;
}

TypeAlias* Parser::parseTypeAlias() {
    expectOrThrow(match(Token::ID), "Se esperaba nombre del alias tras 'type'");
    string alias = previous->text;

    // '=' → token EQ
    expectOrThrow(match(Token::EQ), "Se esperaba '=' en definición de alias");

    string target;
    if (match(Token::INTEGER))       target = "integer";
    else if (match(Token::FLOAT))    target = "float";
    else if (match(Token::LONGINT))  target = "longint";
    else if (match(Token::UNSIGNED)) target = "unsigned";
    else if (match(Token::ID))       target = previous->text;
    else throw runtime_error("Tipo destino inválido en 'type alias = ...'");

    return new TypeAlias(alias, target);
}

void Parser::parseVarBlock(std::list<VarDec*>& outList) {
    expectOrThrow(match(Token::VAR), "Se esperaba 'var'");

    while (check(Token::ID)) {
        VarDec* vd = new VarDec();

        expectOrThrow(match(Token::ID), "Se esperaba identificador en declaración 'var'");
        vd->vars.push_back(previous->text);

        while (match(Token::COMMA)) {
            expectOrThrow(match(Token::ID), "Se esperaba identificador en lista de variables");
            vd->vars.push_back(previous->text);
        }

        expectOrThrow(match(Token::COLON), "Se esperaba ':' en declaración 'var'");

        if (match(Token::INTEGER))           vd->type = "integer";
        else if (match(Token::FLOAT))        vd->type = "float";
        else if (match(Token::LONGINT))      vd->type = "longint";
        else if (match(Token::UNSIGNED))     vd->type = "unsigned";
        else if (match(Token::ID))           vd->type = previous->text;
        else throw runtime_error("Tipo inválido en declaración Pascal");

        outList.push_back(vd);
        expectOrThrow(match(Token::SEMICOL), "Se esperaba ';' tras declaración 'var'");
    }
}

VarDec* Parser::parseVarDec() {
    VarDec* vd = new VarDec();
    expectOrThrow(match(Token::VAR), "Se esperaba 'var'");

    expectOrThrow(match(Token::ID), "Se esperaba identificador en declaración 'var'");
    vd->vars.push_back(previous->text);

    while (match(Token::COMMA)) {
        expectOrThrow(match(Token::ID), "Se esperaba identificador en lista de variables");
        vd->vars.push_back(previous->text);
    }

    expectOrThrow(match(Token::COLON), "Se esperaba ':' en declaración 'var'");

    if (match(Token::INTEGER))           vd->type = "integer";
    else if (match(Token::FLOAT))        vd->type = "float";
    else if (match(Token::LONGINT))      vd->type = "longint";
    else if (match(Token::UNSIGNED))     vd->type = "unsigned";
    else if (match(Token::ID))           vd->type = previous->text;
    else throw runtime_error("Tipo inválido en declaración Pascal");

    return vd;
}

Body* Parser::parseBody() {
    Body* b = new Body();

    while (check(Token::VAR)) {
        parseVarBlock(b->declarations);
    }

    // begin ... end
    expectOrThrow(match(Token::BEGIN_KW), "Se esperaba 'begin' para iniciar un bloque");

    while (!check(Token::END_KW) && !isAtEnd()) {
        Stm* s = parseStm();
        b->StmList.push_back(s);
        if (check(Token::SEMICOL)) match(Token::SEMICOL);
    }

    expectOrThrow(match(Token::END_KW), "Se esperaba 'end' para cerrar el bloque");

    return b;
}

FunDec* Parser::parseFunDec() {
    FunDec* fd = new FunDec();

    expectOrThrow(match(Token::FUNCTION), "Se esperaba 'function'");

    expectOrThrow(match(Token::ID), "Se esperaba nombre de función");
    fd->nombre = previous->text;

    expectOrThrow(match(Token::LPAREN), "Se esperaba '(' en parámetros de función");

    if (!check(Token::RPAREN)) {
        while (true) {
            std::vector<std::string> paramNames;
            expectOrThrow(match(Token::ID), "Se esperaba identificador de parámetro");
            paramNames.push_back(previous->text);

            while (match(Token::COMMA)) {
                expectOrThrow(match(Token::ID), "Se esperaba identificador de parámetro");
                paramNames.push_back(previous->text);
            }

            expectOrThrow(match(Token::COLON), "Se esperaba ':' tras nombres de parámetros");

            std::string ptype;
            if (match(Token::INTEGER))      ptype = "integer";
            else if (match(Token::FLOAT))   ptype = "float";
            else if (match(Token::LONGINT)) ptype = "longint";
            else if (match(Token::UNSIGNED))ptype = "unsigned";
            else if (match(Token::ID))      ptype = previous->text;
            else throw runtime_error("Tipo de parámetro inválido en function");

            for (auto &pn : paramNames) {
                fd->Pnombres.push_back(pn);
                fd->Ptipos.push_back(ptype);
            }

            if (!match(Token::SEMICOL))
                break;
        }
    }

    expectOrThrow(match(Token::RPAREN), "Se esperaba ')' al cerrar parámetros");

    expectOrThrow(match(Token::COLON), "Se esperaba ':' antes del tipo de retorno en function");

    if (match(Token::INTEGER))      fd->tipo = "integer";
    else if (match(Token::FLOAT))   fd->tipo = "float";
    else if (match(Token::LONGINT)) fd->tipo = "longint";
    else if (match(Token::UNSIGNED))fd->tipo = "unsigned";
    else if (match(Token::ID))      fd->tipo = previous->text;
    else throw runtime_error("Tipo de retorno inválido en function");

    expectOrThrow(match(Token::SEMICOL), "Se esperaba ';' tras cabecera de function");

    fd->cuerpo = parseBody();

    expectOrThrow(match(Token::SEMICOL), "Se esperaba ';' tras 'end' de function");

    return fd;
}

static Body* makeSingleStmBody(Stm* s) {
    Body* b = new Body();
    if (s) b->StmList.push_back(s);
    return b;
}

// =============================
// Sentencias
// =============================
Stm* Parser::parseStm() {
    while (match(Token::SEMICOL));

    Stm* a = nullptr;
    Exp* e = nullptr;
    string nombre;

    if (match(Token::ID)) {
        nombre = previous->text;

        if (check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nombre;

            if (!check(Token::RPAREN)) {
                fcall->argumentos.push_back(parseCE());
                while (match(Token::COMMA)) {
                    fcall->argumentos.push_back(parseCE());
                }
            }
            expectOrThrow(match(Token::RPAREN), "Se esperaba ')' al cerrar llamada de función/procedimiento");
            a = new ExpStm(fcall);
        } else {
            expectOrThrow(match(Token::ASSIGN), "Se esperaba ':=' en asignación");
            e = parseCE();
            a = new AssignStm(nombre, e);
        }
    }

    // 2) writeln( CE );
    else if (match(Token::WRITELN)) {
        expectOrThrow(match(Token::LPAREN), "Se esperaba '(' en writeln");
        e = parseCE();
        expectOrThrow(match(Token::RPAREN), "Se esperaba ')' en writeln");
        a = new PrintStm(e);
    }

    // 3) readln( ... );
    else if (match(Token::READLN)) {
        expectOrThrow(match(Token::LPAREN), "Se esperaba '(' en readln");
        FcallExp* fcall = new FcallExp();
        fcall->nombre = "readln";

        if (!check(Token::RPAREN)) {
            fcall->argumentos.push_back(parseCE());
            while (match(Token::COMMA)) {
                fcall->argumentos.push_back(parseCE());
            }
        }
        expectOrThrow(match(Token::RPAREN), "Se esperaba ')' en readln");
        a = new ExpStm(fcall);
    }

    // 4) if CE then bloque | sentencia   [else bloque | sentencia]
    else if (match(Token::IF)) {
        e = parseCE();
        expectOrThrow(match(Token::THEN), "Se esperaba 'then' después de la condición del if");

        Body* tb = nullptr;
        Body* fb = nullptr;

        // THEN: o bien 'begin ... end' o una sola sentencia
        if (check(Token::BEGIN_KW)) {
            tb = parseBody();  // begin ... end
        } else {
            Stm* sThen = parseStm();  // una sola sentencia
            tb = makeSingleStmBody(sThen);
        }

        // ELSE opcional: igual, bloque o sentencia simple
        if (match(Token::ELSE)) {
            if (check(Token::BEGIN_KW)) {
                fb = parseBody();   // else begin ... end
            } else {
                Stm* sElse = parseStm(); // else <sentencia>
                fb = makeSingleStmBody(sElse);
            }
        }

        a = new IfStm(e, tb, fb);
    }

    // 5) while CE do bloque | sentencia
    else if (match(Token::WHILE)) {
        e = parseCE();
        expectOrThrow(match(Token::DO), "Se esperaba 'do' después de la condición del while");

        Body* bb = nullptr;

        // DO: o 'begin ... end' o sentencia simple
        if (check(Token::BEGIN_KW)) {
            bb = parseBody();  // while ... do begin ... end
        } else {
            Stm* sBody = parseStm(); // while ... do <sentencia>
            bb = makeSingleStmBody(sBody);
        }

        a = new WhileStm(e, bb);
    }

    else {
        throw runtime_error("Error sintáctico en sentencia");
    }

    if (check(Token::SEMICOL)) match(Token::SEMICOL);
    return a;
}

// =============================
// Expresiones
// =============================
Exp* Parser::parseCE() {
    Exp* l = parseBE();
    if (match(Token::LT) || match(Token::LE) || match(Token::GT) ||
        match(Token::GE) || match(Token::EQ) || match(Token::NEQ)) {

        BinaryOp op;
        switch (previous->type) {
            case Token::LT:  op = LT_OP;  break;
            case Token::LE:  op = LE_OP;  break;
            case Token::GT:  op = GT_OP;  break;
            case Token::GE:  op = GE_OP;  break;
            case Token::EQ:  op = EQ_OP;  break;
            case Token::NEQ: op = NEQ_OP; break;
            default: throw runtime_error("Operador relacional inesperado");
        }
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// BE: suma / resta
Exp* Parser::parseBE() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// E: multiplicativos (*, div, /, mod)
Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV) || match(Token::REALDIV) || match(Token::MOD)) {
        BinaryOp op;
        switch (previous->type) {
            case Token::MUL:     op = MUL_OP;  break;
            case Token::DIV:     // div entero
            case Token::REALDIV: // / real
                op = DIV_OP;     break;
            case Token::MOD:     op = MOD_OP;  break;
            default:
                throw runtime_error("Operador multiplicativo inesperado");
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// T: unarios +/- y delega a F
Exp* Parser::parseT() {
    if (match(Token::PLUS)) {
        return parseT();
    }
    if (match(Token::MINUS)) {
        Exp* e = parseT();
        return new BinaryExp(new NumberExp((long long)0), e, MINUS_OP);
    }
    return parseF();
}

// F: primarias + CASTS tipo(expr)
Exp* Parser::parseF() {
    Exp* e;
    string nom;

    // ---- Casts explícitos estilo Pascal: float(expr), integer(expr), longint(expr), unsigned(expr) ----
    if (match(Token::FLOAT) || match(Token::INTEGER) || match(Token::LONGINT) || match(Token::UNSIGNED)) {
        Token::Type t = previous->type;
        expectOrThrow(match(Token::LPAREN), "Se esperaba '(' después del tipo en cast");

        Exp* inner = parseCE();
        expectOrThrow(match(Token::RPAREN), "Se esperaba ')' para cerrar el cast");

        Tipo dst;
        switch (t) {
            case Token::FLOAT:   dst = T_FLOAT;    break;
            case Token::INTEGER: dst = T_INT;      break;
            case Token::LONGINT: dst = T_LONG;     break;
            case Token::UNSIGNED:dst = T_UNSIGNED; break;
            default:             dst = T_INT;      break;
        }

        CastExp* c = new CastExp(inner, dst);
        c->tipoDato = dst;
        return c;
    }

    // ---- Números ----
    if (match(Token::NUM)) {
        return new NumberExp((long long)stoll(previous->text));
    }
    else if (match(Token::FLOATNUM)) {
        return new NumberExp(stod(previous->text));
    }

    // ---- (expr) ----
    else if (match(Token::LPAREN)) {
        e = parseCE();
        expectOrThrow(match(Token::RPAREN), "Se esperaba ')' para cerrar la expresión");
        return e;
    }

    // ---- id o llamada f(...) ----
    else if (match(Token::ID)) {
        nom = previous->text;
        if (check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nom;
            if (!check(Token::RPAREN)) {
                fcall->argumentos.push_back(parseCE());
                while (match(Token::COMMA)) {
                    fcall->argumentos.push_back(parseCE());
                }
            }
            expectOrThrow(match(Token::RPAREN), "Se esperaba ')' al cerrar llamada de función");
            return fcall;
        } else {
            return new IdExp(nom);
        }
    }

    throw runtime_error("Error sintáctico en expresión");
}
