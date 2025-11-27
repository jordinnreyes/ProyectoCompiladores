#include "ast.h"
#include "visitor.h"
#include <iostream>
#include <cmath>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:   return "+";
        case MINUS_OP:  return "-";
        case MUL_OP:    return "*";
        case DIV_OP:    return "/";
        case POW_OP:    return "^";
        case LT_OP:     return "<";
        case LE_OP:     return "<=";
        case GE_OP:     return ">=";
        case GT_OP:     return ">";
        case EQ_OP:     return "==";
        case NEQ_OP:    return "!=";
        case MOD_OP:    return "mod";
        default:        return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

BinaryExp::BinaryExp(Exp* l, BinaryOp o, Exp* r)
    : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
}

// ------------------ NumberExp ------------------
NumberExp::NumberExp(long long v)
    : ivalue(v), fvalue(0.0), isFloat(false) {
    tipoDato = T_INT;
}

NumberExp::NumberExp(double fv)
    : ivalue(0), fvalue(fv), isFloat(true) {
    tipoDato = T_FLOAT;
}

NumberExp::~NumberExp() {}

// ------------------ IdExp ------------------
IdExp::IdExp(string v) : value(v) {}
IdExp::~IdExp() {}

// ------------------ CastExp ------------------
CastExp::~CastExp() {
}

// ------------------ Stm y derivados ------------------
Stm::~Stm() {}

PrintStm::~PrintStm() {}
AssignStm::~AssignStm() {}

IfStm::IfStm(Exp* c, Body* t, Body* e)
    : condition(c), then(t), els(e) {}

WhileStm::WhileStm(Exp* c, Body* t)
    : condition(c), b(t) {}

PrintStm::PrintStm(Exp* expresion) {
    e = expresion;
}

AssignStm::AssignStm(string variable, Exp* expresion)
    : id(variable), e(expresion) {}

// ------------------ VarDec y Body ------------------
VarDec::VarDec() {}
VarDec::~VarDec() {}

Body::Body() {
    declarations = list<VarDec*>();
    StmList      = list<Stm*>();
}

Body::~Body() {
}

// ------------------ Program ------------------
Program::~Program() {
    for (auto* v : vdlist)  delete v;
    for (auto* f : fdlist)  delete f;
    tdefs.clear();
}

int Program::accept(Visitor* v) { return v->visit(this); }

// ------------------ TypeAlias ------------------
int TypeAlias::accept(Visitor* v) { return v->visit(this); }

// ------------------ ExpStm ------------------
int ExpStm::accept(Visitor* v) { return v->visit(this); }

// -----------------------------------------------------
// OPTIMIZACIÓN 1: CONSTANT FOLDING (plegado de constantes)
// -----------------------------------------------------
static Exp* foldConstants(Exp* e) {
    if (!e) return nullptr;

    auto bin = dynamic_cast<BinaryExp*>(e);
    if (!bin) return e;

    bin->left  = foldConstants(bin->left);
    bin->right = foldConstants(bin->right);

    auto lnum = dynamic_cast<NumberExp*>(bin->left);
    auto rnum = dynamic_cast<NumberExp*>(bin->right);

    if (!lnum || !rnum) return e;

    double lv = lnum->isFloat ? lnum->fvalue : static_cast<double>(lnum->ivalue);
    double rv = rnum->isFloat ? rnum->fvalue : static_cast<double>(rnum->ivalue);
    double result = 0.0;

    switch (bin->op) {
        case PLUS_OP:  result = lv + rv; break;
        case MINUS_OP: result = lv - rv; break;
        case MUL_OP:   result = lv * rv; break;

        case DIV_OP:
            if (rv != 0.0) result = lv / rv;
            else return e;
            break;

        case POW_OP:   result = pow(lv, rv); break;

        case LT_OP:    result = (lv <  rv); break;
        case LE_OP:    result = (lv <= rv); break;
        case GT_OP:    result = (lv >  rv); break;
        case GE_OP:    result = (lv >= rv); break;
        case EQ_OP:    result = (lv == rv); break;
        case NEQ_OP:   result = (lv != rv); break;

        case MOD_OP:
            if (rv != 0.0) {
                long long li = static_cast<long long>(lv);
                long long ri = static_cast<long long>(rv);
                if (ri != 0) result = li % ri;
                else return e;
            } else {
                return e;
            }
            break;

        default:
            return e;
    }

    NumberExp* ne = nullptr;
    if (bin->tipoDato == T_FLOAT) {
        ne = new NumberExp(result);
        ne->tipoDato = T_FLOAT;
        ne->isFloat  = true;
    } else {
        long long iv = static_cast<long long>(result);
        ne = new NumberExp(iv);
        ne->tipoDato = bin->tipoDato;
        ne->isFloat  = false;
    }

    return ne;
}

// -----------------------------------------------------
// OPTIMIZACIÓN 2: DEAD CODE ELIMINATION
// -----------------------------------------------------
static Stm* removeDeadCode(Stm* stm) {
    if (!stm) return nullptr;

    // -------- WHILE --------
    if (auto wh = dynamic_cast<WhileStm*>(stm)) {
        wh->condition = foldConstants(wh->condition);

        if (auto num = dynamic_cast<NumberExp*>(wh->condition)) {
            double v = num->isFloat ? num->fvalue
                                    : static_cast<double>(num->ivalue);
            if (v == 0.0) {
                // while(0) => eliminar completamente
                return nullptr;
            }
        }
        return wh;
    }

    // -------- IF --------
    if (auto ifs = dynamic_cast<IfStm*>(stm)) {
        ifs->condition = foldConstants(ifs->condition);

        if (auto num = dynamic_cast<NumberExp*>(ifs->condition)) {
            double v = num->isFloat ? num->fvalue
                                    : static_cast<double>(num->ivalue);

            if (v == 0.0) {
                if (ifs->els) {
                    ifs->then = ifs->els;
                    ifs->els  = nullptr;
                    ifs->condition = new NumberExp(1LL);
                } else {
                    return nullptr;
                }
            } else {
                ifs->els = nullptr;
                ifs->condition = new NumberExp(1LL);
            }
        }
        return ifs;
    }

    return stm;
}

// -----------------------------------------------------
// FUNCIÓN PRINCIPAL DE OPTIMIZACIÓN GLOBAL
// -----------------------------------------------------
void optimizeAST(Program* prog) {
    if (!prog) return;

    // Recorre cada función del programa
    for (auto& f : prog->fdlist) {
        if (!f->cuerpo) continue;

        // Elimina código muerto (Dead Code)
        for (auto& s : f->cuerpo->StmList) {
            s = removeDeadCode(s);
        }

        // Plegado de constantes (Constant Folding)
        for (auto& s : f->cuerpo->StmList) {
            if (!s) continue;

            if (auto a = dynamic_cast<AssignStm*>(s)) {
                a->e = foldConstants(a->e);
            }
            else if (auto p = dynamic_cast<PrintStm*>(s)) {
                p->e = foldConstants(p->e);
            }
            else if (auto i = dynamic_cast<IfStm*>(s)) {
                i->condition = foldConstants(i->condition);
            }
            else if (auto w = dynamic_cast<WhileStm*>(s)) {
                w->condition = foldConstants(w->condition);
            }
        }

        // Limpieza: eliminar sentencias nulas del Body
        f->cuerpo->StmList.remove(nullptr);
    }

    std::cout << "Optimizaciones aplicadas correctamente." << std::endl;
}
