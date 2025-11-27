#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Visitor;
class VarDec;
class TypeAlias;

// ========================
//       Tipos
// ========================
enum Tipo {
    T_INT,
    T_FLOAT,
    T_LONG,
    T_UNSIGNED,
    T_BOOL
};

// ========================
//   Operaciones binarias
// ========================
enum BinaryOp {
    PLUS_OP,    // +
    MINUS_OP,   // -
    MUL_OP,     // *
    DIV_OP,     // div / /
    POW_OP,     // potencia (si la usas)
    LT_OP,      // <
    LE_OP,      // <=
    GE_OP,      // >=
    GT_OP,      // >
    EQ_OP,      // =
    NEQ_OP,     // <>
    MOD_OP      // mod
};

// ========================
//       Expresión base
// ========================
class Exp {
public:
    Tipo tipoDato = T_INT;
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};

// ========================
//     Binary Expression
// ========================
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;

    BinaryExp(Exp* l, BinaryOp o, Exp* r);
    BinaryExp(Exp* l, Exp* r, BinaryOp o);

    int accept(Visitor* v) override;
    ~BinaryExp();
};

// ========================
//     Number Expression
// ========================
class NumberExp : public Exp {
public:
    long long ivalue = 0;
    double    fvalue = 0.0;
    bool      isFloat = false;

    NumberExp(long long v);
    NumberExp(double fv);

    int accept(Visitor* visitor);
    ~NumberExp();
};

// ========================
//       Identifier
// ========================
class IdExp : public Exp {
public:
    string value;
    IdExp(string v);
    int accept(Visitor* visitor);
    ~IdExp();
};

// ========================
//     Statements base
// ========================
class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

// ========================
//     Variable Decl
// ========================
class VarDec {
public:
    string type;
    list<string> vars;

    VarDec();
    int accept(Visitor* visitor);
    ~VarDec();
};

// ========================
//    Body (BEGIN..END)
// ========================
class Body {
public:
    list<Stm*>   StmList;
    list<VarDec*> declarations;

    Body();
    int accept(Visitor* visitor);
    ~Body();
};

// ========================
//          If
// ========================
class IfStm : public Stm {
public:
    Exp*  condition;
    Body* then;
    Body* els;

    IfStm(Exp* condition, Body* thenBody, Body* elseBody);
    int accept(Visitor* visitor);
    ~IfStm() {};
};

// ========================
//         While
// ========================
class WhileStm : public Stm {
public:
    Exp*  condition;
    Body* b;

    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStm() {};
};

// ========================
//       Assignment
// ========================
class AssignStm : public Stm {
public:
    string id;
    Exp*   e;

    AssignStm(string id, Exp* e);
    int accept(Visitor* visitor);
    ~AssignStm();
};

// ========================
//       writeln(Exp)
//   (internamente PrintStm)
// ========================
class PrintStm : public Stm {
public:
    Exp* e;

    PrintStm(Exp* e);
    int accept(Visitor* visitor);
    ~PrintStm();
};

// ========================
//   Return (si lo usas)
//  (en Pascal real se suele
//   usar asignando al nombre
//   de la función, pero lo
//   mantenemos interno)
// ========================
class ReturnStm : public Stm {
public:
    Exp* e;
    ReturnStm() : e(nullptr) {}
    ~ReturnStm() {}
    int accept(Visitor* visitor);
};

// ========================
//   Function Call Exp
// ========================
class FcallExp : public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;

    FcallExp() {}
    int accept(Visitor* visitor);
    ~FcallExp() {}
};

// ========================
//      Function Decl
// ========================
class FunDec {
public:
    string nombre;             // nombre de la función
    string tipo;               // tipo de retorno ("integer", etc.)
    Body*  cuerpo;             // cuerpo begin..end
    vector<string> Ptipos;     // tipos de parámetros
    vector<string> Pnombres;   // nombres de parámetros

    FunDec() : cuerpo(nullptr) {}
    int accept(Visitor* visitor);
    ~FunDec() {}
};

// ========================
//        Program
// ========================
class Program {
public:
    list<VarDec*> vdlist;   // variables globales
    list<FunDec*> fdlist;   // funciones (incluida "main" sintética)
    unordered_map<string,string> tdefs; // type alias

    int accept(Visitor* v);
    ~Program();
};

// ========================
//        Cast Exp
// ========================
class CastExp : public Exp {
public:
    Exp* expr;
    Tipo destino;

    CastExp(Exp* e, Tipo d) : expr(e), destino(d) {
        this->tipoDato = d;
    }
    int accept(Visitor* visitor);
    ~CastExp();
};

// ========================
//       Type Alias
// ========================
class TypeAlias {
public:
    string alias;
    string target;

    TypeAlias(const string& a, const string& t) : alias(a), target(t) {}
    int accept(Visitor* v);
};

// ========================
//     Exp como sentencia
// ========================
struct ExpStm : Stm {
    Exp* e;
    ExpStm(Exp* _e) : e(_e) {}
    virtual int accept(Visitor* v);
};

#endif // AST_H
