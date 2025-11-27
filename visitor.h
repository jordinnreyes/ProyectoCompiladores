#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class BinaryExp;
class NumberExp;
class IdExp;
class Program;
class PrintStm;
class WhileStm;
class IfStm;
class AssignStm;
class Body;
class VarDec;
class FcallExp;
class ReturnStm;
class FunDec;
class CastExp;
class TypeAlias;
class ExpStm;

// --------------------------------------
// Visitor base (patrón Visitor clásico)
// --------------------------------------
class Visitor {
public:
    virtual ~Visitor() = default;

    virtual int visit(BinaryExp* exp)   = 0;
    virtual int visit(NumberExp* exp)   = 0;
    virtual int visit(IdExp* exp)       = 0;
    virtual int visit(Program* p)       = 0;
    virtual int visit(PrintStm* stm)    = 0;
    virtual int visit(WhileStm* stm)    = 0;
    virtual int visit(IfStm* stm)       = 0;
    virtual int visit(AssignStm* stm)   = 0;
    virtual int visit(Body* body)       = 0;
    virtual int visit(VarDec* vd)       = 0;
    virtual int visit(FcallExp* fcall)  = 0;
    virtual int visit(ReturnStm* r)     = 0;
    virtual int visit(FunDec* fd)       = 0;
    virtual int visit(CastExp* exp)     = 0;
    virtual int visit(TypeAlias* ta)    = 0;
    virtual int visit(ExpStm* s)        = 0;
};

// --------------------------------------
// TYPECHECK VISITOR (Pascal-like types)
// --------------------------------------
class TypeCheckVisitor : public Visitor {
public:
    // Tabla de tipos globales y locales (variables)
    unordered_map<string, Tipo> tipoGlobal;
    unordered_map<string, Tipo> tipoLocal;

    // alias type x = y;
    unordered_map<string, string> aliasMap;

    string funcionActual;

    // Tipo de retorno de funciones: funRet["f"] = T_INT / T_FLOAT / ...
    unordered_map<string, Tipo> funRet;

    bool enFuncion = false;

    int analizar(Program* p) { return p->accept(this); }

    // Convierte strings Pascal a Tipo interno
    Tipo strToTipo(const string& s) {
        if (s == "integer" || s == "int")      return T_INT;
        if (s == "longint" || s == "long")     return T_LONG;
        if (s == "unsigned")                   return T_UNSIGNED;
        if (s == "real" || s == "float" ||
            s == "double")                     return T_FLOAT;

        auto it = aliasMap.find(s);
        if (it != aliasMap.end()) {
            return strToTipo(it->second);
        }
        // Por defecto, si no se reconoce, lo tratamos como int
        return T_INT;
    }

    // Regla de promoción para binarios
    static Tipo unificarBin(Tipo a, Tipo b) {
        if (a == T_FLOAT  || b == T_FLOAT)     return T_FLOAT;
        if (a == T_LONG   || b == T_LONG)      return T_LONG;
        if (a == T_UNSIGNED || b == T_UNSIGNED)return T_UNSIGNED;
        return T_INT;
    }

    // ¿Es operador relacional?
    static bool esRelOp(BinaryOp op) {
        return op==LT_OP || op==LE_OP ||
               op==GT_OP || op==GE_OP ||
               op==EQ_OP || op==NEQ_OP;
    }

    // Inserta un CastExp si hace falta
    static Exp* insertarCast(Exp* e, Tipo dst);

    // Implementaciones de Visitor
    int visit(Program* p)      override;
    int visit(VarDec* vd)      override;
    int visit(FunDec* fd)      override;
    int visit(Body* b)         override;
    int visit(AssignStm* s)    override;
    int visit(PrintStm* s)     override;
    int visit(WhileStm* s)     override;
    int visit(IfStm* s)        override;
    int visit(ReturnStm* r)    override;
    int visit(BinaryExp* e)    override;
    int visit(NumberExp* e)    override;
    int visit(IdExp* e)        override;
    int visit(FcallExp* f)     override;
    int visit(CastExp* e)      override;
    int visit(TypeAlias* tdef) override;
    int visit(ExpStm* s)       override;
};

// --------------------------------------
// CODEGEN VISITOR (x86-64 + printf)
// --------------------------------------
class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;

public:
    GenCodeVisitor(std::ostream& out) : out(out) {}

    int generar(Program* program);

    // Layout de memoria y tipos
    unordered_map<string, int>  memoria;       // offset local (%rbp)
    unordered_map<string, bool> memoriaGlobal; // true si es global
    unordered_map<string, Tipo> tipoGlobal;
    unordered_map<string, Tipo> tipoLocal;
    unordered_map<string, string> aliasMap;

    int    offset       = -8;
    int    labelcont    = 0;
    bool   entornoFuncion = false;
    string nombreFuncion;

    // Pool de constantes de punto flotante
    vector<double> poolFloats;
    string addFloatConst(double v);

    Tipo mapStr(const string& s) const {
        if (s == "integer" || s == "int")      return T_INT;
        if (s == "longint" || s == "long")     return T_LONG;
        if (s == "unsigned")                   return T_UNSIGNED;
        if (s == "real" || s == "float" ||
            s == "double")                     return T_FLOAT;

        auto it = aliasMap.find(s);
        if (it != aliasMap.end())
            return mapStr(it->second);

        return T_INT;
    }

    // Implementaciones de Visitor
    int visit(Program* p)      override;
    int visit(VarDec* vd)      override;
    int visit(FunDec* fd)      override;
    int visit(Body* body)      override;
    int visit(AssignStm* stm)  override;
    int visit(PrintStm* stm)   override;
    int visit(WhileStm* stm)   override;
    int visit(IfStm* stm)      override;
    int visit(ReturnStm* r)    override;
    int visit(BinaryExp* exp)  override;
    int visit(NumberExp* exp)  override;
    int visit(IdExp* exp)      override;
    int visit(FcallExp* fcall) override;
    int visit(CastExp* e)      override;
    int visit(TypeAlias* tdef) override;
    int visit(ExpStm* s)       override;
};

#endif // VISITOR_H
