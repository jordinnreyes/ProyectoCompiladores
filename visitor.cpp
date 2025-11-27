#include <iostream>
#include "visitor.h"
#include "ast.h"

using namespace std;

// ==== helpers para tamaños de tipo ====
static bool es32Entero(Tipo t) {
    return t == T_INT || t == T_UNSIGNED || t == T_BOOL;
}
static bool es64Entero(Tipo t) {
    return t == T_LONG;
}
static bool esFlotante(Tipo t) {
    return t == T_FLOAT;   // SOLO float: 32 bits (SSE)
}

///////////////////////////////////////////////////////////////////////////////
//           MÉTODOS accept(...) DE LOS NODOS DEL AST
///////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Body::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int VarDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FunDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ReturnStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int CastExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

///////////////////////////////////////////////////////////////////////////////
//                     TYPECHECK VISITOR  (ANÁLISIS DE TIPOS)
///////////////////////////////////////////////////////////////////////////////

int TypeCheckVisitor::visit(Program* p) {
    // Cargar alias de tipos (type alias = ...)
    aliasMap = p->tdefs;

    // Variables globales
    for (auto vd : p->vdlist) {
        if (vd) vd->accept(this);
    }

    // Funciones
    for (auto fd : p->fdlist) {
        if (fd) fd->accept(this);
    }

    return 0;
}

int TypeCheckVisitor::visit(VarDec* vd) {
    if (!vd) return 0;

    Tipo tt = strToTipo(vd->type);

    if (!enFuncion) {
        // variables globales
        for (auto& v : vd->vars) {
            tipoGlobal[v] = tt;
        }
    } else {
        // variables locales
        for (auto& v : vd->vars) {
            tipoLocal[v] = tt;
        }
    }
    return 0;
}

int TypeCheckVisitor::visit(FunDec* fd) {
    if (!fd) return 0;

    // tipo de retorno de la función
    funRet[fd->nombre] = strToTipo(fd->tipo);

    enFuncion = true;
    funcionActual = fd->nombre;   // <--- IMPORTANTE
    tipoLocal.clear();            // limpiar entorno local

    // registrar parámetros en el entorno local
    for (size_t i = 0; i < fd->Pnombres.size(); ++i) {
        const string& pname = fd->Pnombres[i];
        const string& ptype = fd->Ptipos[i];
        Tipo t = strToTipo(ptype);
        tipoLocal[pname] = t;
    }

    if (fd->cuerpo) {
        fd->cuerpo->accept(this);
    }

    enFuncion = false;
    funcionActual.clear();        // <--- limpiar nombre
    return 0;
}

int TypeCheckVisitor::visit(Body* b) {
    if (!b) return 0;

    for (auto vd : b->declarations) {
        if (vd) vd->accept(this);
    }
    for (auto s : b->StmList) {
        if (s) s->accept(this);
    }

    return 0;
}

int TypeCheckVisitor::visit(AssignStm* s) {
    if (!s || !s->e) return 0;

    Tipo dst = T_INT;

    // Caso especial: asignación al nombre de la función (estilo Pascal)
    if (enFuncion && !funcionActual.empty() && s->id == funcionActual) {
        auto it = funRet.find(funcionActual);
        if (it != funRet.end()) {
            dst = it->second;  // tipo de retorno de la función
        }

        // Analizar expresión y castear al tipo de retorno
        s->e->accept(this);
        s->e = insertarCast(s->e, dst);

        // NO registramos 's->id' como variable local
        return 0;
    }

    // Asignación normal a variable local/global
    if (tipoLocal.count(s->id))       dst = tipoLocal[s->id];
    else if (tipoGlobal.count(s->id)) dst = tipoGlobal[s->id];
    else {
        // Si no está declarada, por defecto la tratamos como int
        dst = T_INT;
    }

    // Analizamos la expresión y la casteamos al tipo destino
    s->e->accept(this);
    s->e = insertarCast(s->e, dst);

    return 0;
}

int TypeCheckVisitor::visit(PrintStm* s) {
    if (!s || !s->e) return 0;
    s->e->accept(this);
    return 0;
}

int TypeCheckVisitor::visit(WhileStm* s) {
    if (!s) return 0;

    if (s->condition) {
        s->condition->accept(this);
        // condición se fuerza a entero (0 / no 0)
        s->condition = insertarCast(s->condition, T_INT);
    }

    if (s->b) s->b->accept(this);

    return 0;
}

int TypeCheckVisitor::visit(IfStm* s) {
    if (!s) return 0;

    if (s->condition) {
        s->condition->accept(this);
        s->condition = insertarCast(s->condition, T_INT);
    }
    if (s->then) s->then->accept(this);
    if (s->els)  s->els->accept(this);

    return 0;
}

int TypeCheckVisitor::visit(ReturnStm* r) {
    if (!r) return 0;

    if (r->e) {
        r->e->accept(this);
    }

    return 0;
}

int TypeCheckVisitor::visit(NumberExp* e) {
    if (!e) return 0;

    if (e->isFloat) {
        e->tipoDato = T_FLOAT;
    } else {
        e->tipoDato = T_INT;
    }
    return 0;
}

int TypeCheckVisitor::visit(IdExp* e) {
    if (!e) return 0;

    if (tipoLocal.count(e->value))        e->tipoDato = tipoLocal[e->value];
    else if (tipoGlobal.count(e->value))  e->tipoDato = tipoGlobal[e->value];
    else                                  e->tipoDato = T_INT; // por defecto

    return 0;
}

int TypeCheckVisitor::visit(FcallExp* f) {
    if (!f) return 0;

    // analizar tipos de argumentos
    for (auto* arg : f->argumentos) {
        if (arg) arg->accept(this);
    }

    // tipo de retorno de la función
    auto it = funRet.find(f->nombre);
    if (it != funRet.end()) {
        f->tipoDato = it->second;
    } else {
        f->tipoDato = T_INT;  // por defecto
    }

    return 0;
}

int TypeCheckVisitor::visit(BinaryExp* e) {
    if (!e || !e->left || !e->right) return 0;

    e->left->accept(this);
    e->right->accept(this);

    if (esRelOp(e->op)) {
        // unificar tipos para comparación
        Tipo tgt = unificarBin(e->left->tipoDato, e->right->tipoDato);
        e->left  = insertarCast(e->left,  tgt);
        e->right = insertarCast(e->right, tgt);
        // resultado de una comparación: entero (0/1)
        e->tipoDato = T_INT;
    } else {
        // operación aritmética
        Tipo tgt = unificarBin(e->left->tipoDato, e->right->tipoDato);
        e->left  = insertarCast(e->left,  tgt);
        e->right = insertarCast(e->right, tgt);
        e->tipoDato = tgt;
    }

    return 0;
}

int TypeCheckVisitor::visit(CastExp* e) {
    if (!e || !e->expr) return 0;
    e->expr->accept(this);
    e->tipoDato = e->destino;
    return 0;
}

int TypeCheckVisitor::visit(TypeAlias* tdef) {
    if (!tdef) return 0;
    aliasMap[tdef->alias] = tdef->target;
    return 0;
}

int TypeCheckVisitor::visit(ExpStm* s) {
    if (s && s->e) s->e->accept(this);
    return 0;
}

// ----------- función auxiliar para insertar CastExp -----------

Exp* TypeCheckVisitor::insertarCast(Exp* e, Tipo dst) {
    if (!e) return nullptr;
    if (e->tipoDato == dst) return e;
    if (dst == T_LONG) {
        if (auto* num = dynamic_cast<NumberExp*>(e)) {
            num->tipoDato = T_LONG;   // hará que GenCode use movq $literal, %rax
            return e;
        }
    }

    auto* c = new CastExp(e, dst);
    c->tipoDato = dst;
    return c;
}

///////////////////////////////////////////////////////////////////////////////
//                     GEN CODE VISITOR  (GENERACIÓN ASM)
///////////////////////////////////////////////////////////////////////////////

int GenCodeVisitor::generar(Program* program) {
    if (!program) return 0;
    return program->accept(this);
}

int GenCodeVisitor::visit(Program* program) {
    poolFloats.clear();

    // Sección de datos
    out << ".data\n";
    out << "print_fmt: .string \"%ld \\n\"\n";
    out << "printf_fmt_float: .string \"%f \\n\"\n";

    // alias de tipos (type alias = ...)
    aliasMap = program->tdefs;

    // variables globales -> rellenan tipoGlobal + memoriaGlobal
    for (auto dec : program->vdlist) {
        if (dec) dec->accept(this);
    }

    // Definiciones reales en .data según tipo
    for (auto &kv : tipoGlobal) {
        const string &name = kv.first;
        Tipo t = kv.second;

        if (esFlotante(t)) {
            // float 32 bits en memoria
            out << name << ": .float 0.0\n";
        } else if (es64Entero(t)) {
            // long 64 bits
            out << name << ": .quad 0\n";
        } else {
            // int / unsigned / bool (32 bits)
            out << name << ": .long 0\n";
        }
    }

    // Sección de código
    out << ".text\n";

    // Código de cada función
    for (auto dec : program->fdlist) {
        if (dec) dec->accept(this);
    }

    // Pool de constantes de punto flotante (float 32 bits)
    if (!poolFloats.empty()) {
        out << "\n# Constantes de punto flotante (float 32 bits)\n";
        for (size_t i = 0; i < poolFloats.size(); ++i) {
            out << "._CF" << i << ": .float " << poolFloats[i] << "\n";
        }
    }

    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}

int GenCodeVisitor::visit(TypeAlias* tdef) {
    if (!tdef) return 0;
    aliasMap[tdef->alias] = tdef->target;
    return 0;
}

string GenCodeVisitor::addFloatConst(double v) {
    for (size_t i = 0; i < poolFloats.size(); ++i) {
        if (poolFloats[i] == v) {
            return "._CF" + to_string(i);
        }
    }
    poolFloats.push_back(v);
    return "._CF" + to_string(poolFloats.size() - 1);
}

int GenCodeVisitor::visit(VarDec* vd) {
    if (!vd) return 0;

    Tipo tt = mapStr(vd->type);

    for (auto& var : vd->vars) {
        if (!entornoFuncion) {
            // ---- variable global ----
            memoriaGlobal[var] = true;
            tipoGlobal[var]    = tt;
        } else {
            // ---- variable local en función ----
            // long -> 8 bytes, todo lo demás (int, unsigned, float, bool) -> 4 bytes
            int tam = es64Entero(tt) ? 8 : 4;

            offset -= tam;
            memoria[var]   = offset;
            tipoLocal[var] = tt;
        }
    }
    return 0;
}


int GenCodeVisitor::visit(NumberExp* exp) {
    if (!exp) return 0;

    if (exp->isFloat || exp->tipoDato == T_FLOAT) {
        // Cargar constante float (32 bits) en %xmm0
        std::string lbl = addFloatConst(exp->isFloat ? exp->fvalue
                                                     : (double)exp->ivalue);
        out << " movss " << lbl << "(%rip), %xmm0\n";
    } else {
        // enteros
        if (es64Entero(exp->tipoDato)) {
            out << " movq $" << exp->ivalue << ", %rax\n";  // long 64 bits
        } else {
            // int / unsigned / bool -> 32 bits
            out << " movl $" << (int)exp->ivalue << ", %eax\n";
        }
    }
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (!exp) return 0;

    bool esGlobalVar = memoriaGlobal.count(exp->value);
    Tipo t = T_INT;

    if (tipoLocal.count(exp->value))       t = tipoLocal[exp->value];
    else if (tipoGlobal.count(exp->value)) t = tipoGlobal[exp->value];

    if (esFlotante(t)) {
        // float 32 bits
        if (esGlobalVar)
            out << " movss " << exp->value << "(%rip), %xmm0\n";
        else
            out << " movss " << memoria[exp->value] << "(%rbp), %xmm0\n";
    } else if (es64Entero(t)) {
        // long -> 64 bits
        if (esGlobalVar)
            out << " movq " << exp->value << "(%rip), %rax\n";
        else
            out << " movq " << memoria[exp->value] << "(%rbp), %rax\n";
    } else {
        // int / unsigned / bool -> 32 bits
        if (esGlobalVar)
            out << " movl " << exp->value << "(%rip), %eax\n";
        else
            out << " movl " << memoria[exp->value] << "(%rbp), %eax\n";
        // escribir en %eax pone en cero la parte alta de %rax
    }

    return 0;
}

int GenCodeVisitor::visit(BinaryExp* e) {
    if (!e || !e->left || !e->right) return 0;

    // --------- CASO FLOAT (aritmético seguro en funciones) -------------
    if (e->tipoDato == T_FLOAT && !TypeCheckVisitor::esRelOp(e->op)) {

        // Evaluamos left → %xmm0
        e->left->accept(this);
        int temp1 = offset - 4;     // reservar local float (4 bytes)
        out << " movss %xmm0, " << temp1 << "(%rbp)\n";

        // Evaluamos right → %xmm0
        e->right->accept(this);
        int temp2 = temp1 - 4;      // otro local float
        out << " movss %xmm0, " << temp2 << "(%rbp)\n";

        // Recuperar valores
        out << " movss " << temp1 << "(%rbp), %xmm1\n";
        out << " movss " << temp2 << "(%rbp), %xmm0\n";

        // Operación
        switch (e->op) {
            case PLUS_OP:
                out << " addss %xmm0, %xmm1\n";
                out << " movss %xmm1, %xmm0\n";
                break;
            case MINUS_OP:
                out << " subss %xmm0, %xmm1\n";
                out << " movss %xmm1, %xmm0\n";
                break;
            case MUL_OP:
                out << " mulss %xmm0, %xmm1\n";
                out << " movss %xmm1, %xmm0\n";
                break;
            case DIV_OP:
                out << " divss %xmm0, %xmm1\n";
                out << " movss %xmm1, %xmm0\n";
                break;
        }

        offset = temp2;   // registrar consumo de locals temporales
        return 0;
    }

    // --------- COMPARACIONES CON FLOAT -------------
    if (e->left->tipoDato == T_FLOAT || e->right->tipoDato == T_FLOAT) {
        e->left->accept(this);
        out << " subq $8, %rsp\n";
        out << " movss %xmm0, (%rsp)\n";

        e->right->accept(this);
        out << " movss (%rsp), %xmm1\n";
        out << " addq $8, %rsp\n";

        out << " ucomiss %xmm0, %xmm1\n";

        out << " movl $0, %eax\n";
        switch (e->op) {
            case LT_OP:  out << " setb %al\n";  break;
            case LE_OP:  out << " setbe %al\n"; break;
            case GT_OP:  out << " seta %al\n";  break;
            case GE_OP:  out << " setae %al\n"; break;
            case EQ_OP:  out << " sete %al\n";  break;
            case NEQ_OP: out << " setne %al\n"; break;
            default:     break;
        }
        out << " movzbq %al, %rax\n";   // resultado 0/1 en %rax
        return 0;
    }

    // --------- CASO ENTERO / LONG / UNSIGNED ---------
    bool esLong = (e->left->tipoDato  == T_LONG ||
                   e->right->tipoDato == T_LONG);

    // Evaluar left
    e->left->accept(this);
    out << " pushq %rax\n";
    // Evaluar right
    e->right->accept(this);

    if (esLong) {
        // ======= 64 BITS: long =======
        out << " movq %rax, %rcx\n";
        out << " popq %rax\n";

        switch (e->op) {
            case PLUS_OP:
                out << " addq %rcx, %rax\n";
                break;
            case MINUS_OP:
                out << " subq %rcx, %rax\n";
                break;
            case MUL_OP:
                out << " imulq %rcx, %rax\n";
                break;
            case DIV_OP:
                out << " cqto\n";
                out << " idivq %rcx\n";
                break;
            case MOD_OP:
                out << " cqto\n";
                out << " idivq %rcx\n";
                out << " movq %rdx, %rax\n";
                break;

            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
            case EQ_OP:
            case NEQ_OP:
                out << " cmpq %rcx, %rax\n";
                out << " movl $0, %eax\n";
                switch (e->op) {
                    case LT_OP:  out << " setl %al\n";  break;
                    case LE_OP:  out << " setle %al\n"; break;
                    case GT_OP:  out << " setg %al\n";  break;
                    case GE_OP:  out << " setge %al\n"; break;
                    case EQ_OP:  out << " sete %al\n";  break;
                    case NEQ_OP: out << " setne %al\n"; break;
                    default:     break;
                }
                out << " movzbq %al, %rax\n";
                break;

            default:
                break;
        }
    } else {
        // ======= 32 BITS: int / unsigned / bool =======
        out << " movl %eax, %ecx\n";
        out << " popq %rax\n";

        switch (e->op) {
            case PLUS_OP:
                out << " addl %ecx, %eax\n";
                break;
            case MINUS_OP:
                out << " subl %ecx, %eax\n";
                break;
            case MUL_OP:
                out << " imull %ecx, %eax\n";
                break;
            case DIV_OP:
                out << " cltd\n";
                out << " idivl %ecx\n";
                break;
            case MOD_OP:
                out << " cltd\n";
                out << " idivl %ecx\n";
                out << " movl %edx, %eax\n";
                break;

            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
            case EQ_OP:
            case NEQ_OP:
                out << " cmpl %ecx, %eax\n";
                out << " movl $0, %eax\n";
                switch (e->op) {
                    case LT_OP:  out << " setl %al\n";  break;
                    case LE_OP:  out << " setle %al\n"; break;
                    case GT_OP:  out << " setg %al\n";  break;
                    case GE_OP:  out << " setge %al\n"; break;
                    case EQ_OP:  out << " sete %al\n";  break;
                    case NEQ_OP: out << " setne %al\n"; break;
                    default:     break;
                }
                out << " movzbq %al, %rax\n";
                break;

            default:
                break;
        }
    }

    return 0;
}

int GenCodeVisitor::visit(AssignStm* s) {
    if (!s || !s->e) return 0;

    // Caso especial: asignación al nombre de la función => valor de retorno
    if (entornoFuncion && s->id == nombreFuncion) {
        // Evaluamos la expresión; deja el resultado en:
        // - %rax / %eax para enteros / long / unsigned
        // - %xmm0 para float
        s->e->accept(this);
        // No almacenamos en memoria: al hacer 'ret' ese será el valor de retorno.
        return 0;
    }

    // Asignación normal a variable
    s->e->accept(this);  // resultado en %rax o %xmm0

    bool esGlobalVar = memoriaGlobal.count(s->id);
    Tipo t = T_INT;

    if (tipoLocal.count(s->id))       t = tipoLocal[s->id];
    else if (tipoGlobal.count(s->id)) t = tipoGlobal[s->id];

    if (esFlotante(t)) {
        if (esGlobalVar)
            out << " movss %xmm0, " << s->id << "(%rip)\n";
        else
            out << " movss %xmm0, " << memoria[s->id] << "(%rbp)\n";

    } else if (es64Entero(t)) {
        if (esGlobalVar)
            out << " movq %rax, " << s->id << "(%rip)\n";
        else
            out << " movq %rax, " << memoria[s->id] << "(%rbp)\n";

    } else {
        if (esGlobalVar)
            out << " movl %eax, " << s->id << "(%rip)\n";
        else
            out << " movl %eax, " << memoria[s->id] << "(%rbp)\n";
    }

    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    if (!stm || !stm->e) return 0;

    stm->e->accept(this);

    if (stm->e->tipoDato == T_FLOAT) {
        // Tenemos float en %xmm0 -> convertir a double en %xmm0 para printf("%f")
        out << " cvtss2sd %xmm0, %xmm0\n";
        out << " leaq printf_fmt_float(%rip), %rdi\n";
        out << " movl $1, %eax\n";
        out << " call printf@PLT\n";
    } else {
        // int / long / unsigned -> valor en %rax
        out << " movq %rax, %rsi\n";
        out << " leaq print_fmt(%rip), %rdi\n";
        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";
    }
    return 0;
}

int GenCodeVisitor::visit(CastExp* e) {
    if (!e || !e->expr) return 0;

    e->expr->accept(this);
    Tipo src = e->expr->tipoDato;
    Tipo dst = e->destino;

    if (src == dst) return 0;

    // entero/long/unsigned -> float
    if (dst == T_FLOAT && (src == T_INT || src == T_LONG || src == T_UNSIGNED)) {
        out << " cvtsi2ss %rax, %xmm0\n";
        return 0;
    }

    // float -> entero (int/long/unsigned)
    if ((dst == T_INT || dst == T_LONG || dst == T_UNSIGNED) && src == T_FLOAT) {
        out << " cvttss2si %xmm0, %rax\n";
        return 0;
    }

    // int <-> long no necesitan instrucción extra: ya están en %rax
    return 0;
}

int GenCodeVisitor::visit(Body* b) {
    if (!b) return 0;

    for (auto dec : b->declarations) {
        if (dec) dec->accept(this);
    }
    for (auto s : b->StmList) {
        if (s) s->accept(this);
    }
    return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    if (!stm || !stm->condition) return 0;

    int label = labelcont++;

    stm->condition->accept(this);
    out << " cmpq $0, %rax\n";
    out << " je else_" << label << "\n";

    if (stm->then) stm->then->accept(this);
    out << " jmp endif_" << label << "\n";

    out << "else_" << label << ":\n";
    if (stm->els) stm->els->accept(this);

    out << "endif_" << label << ":\n";
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    if (!stm || !stm->condition) return 0;

    int label = labelcont++;

    out << "while_" << label << ":\n";
    stm->condition->accept(this);
    out << " cmpq $0, %rax\n";
    out << " je endwhile_" << label << "\n";

    if (stm->b && !stm->b->StmList.empty())
        stm->b->accept(this);

    out << " jmp while_" << label << "\n";
    out << "endwhile_" << label << ":\n";
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (!stm || !stm->e) return 0;
    stm->e->accept(this);
    out << " jmp .end_" << nombreFuncion << "\n";
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    if (!f) return 0;

    entornoFuncion = true;
    memoria.clear();
    tipoLocal.clear();
    offset = -8;
    nombreFuncion = f->nombre;

    // coherencia parámetros (debug)
    if (f->Pnombres.size() != f->Ptipos.size()) {
        std::cerr << "[GenCodeVisitor] Error: en función '" << f->nombre
                  << "' la cantidad de nombres de parámetros ("
                  << f->Pnombres.size() << ") difiere de la cantidad de tipos ("
                  << f->Ptipos.size() << ").\n";

        out << ".globl " << f->nombre << "\n";
        out << f->nombre << ":\n";
        out << " pushq %rbp\n";
        out << " movq %rsp, %rbp\n";
        out << ".end_" << f->nombre << ":\n";
        out << " leave\n";
        out << " ret\n";
        entornoFuncion = false;
        return 0;
    }

    std::vector<std::string> intRegs   = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    std::vector<std::string> floatRegs = {"%xmm0","%xmm1","%xmm2","%xmm3","%xmm4","%xmm5"};
    int iInt = 0, iFlt = 0;

    out << ".globl " << f->nombre << "\n";
    out << f->nombre << ":\n";
    out << " pushq %rbp\n";
    out << " movq %rsp, %rbp\n";

    // Guardar parámetros en la pila (frame) según su tipo
    for (size_t i = 0; i < f->Pnombres.size(); ++i) {
        const std::string& pname = f->Pnombres[i];
        Tipo tt = mapStr(f->Ptipos[i]);

        memoria[pname]   = offset;
        tipoLocal[pname] = tt;

        if (tt == T_FLOAT) {
            if (iFlt < (int)floatRegs.size()) {
                out << " movss " << floatRegs[iFlt++] << ", " << offset << "(%rbp)\n";
            } else {
                std::cerr
                    << "[GenCodeVisitor] Advertencia: demasiados parámetros float en '"
                    << f->nombre << "'.\n";
            }
        } else {
            if (iInt < (int)intRegs.size()) {
                out << " movq "  << intRegs[iInt++]   << ", " << offset << "(%rbp)\n";
            } else {
                std::cerr
                    << "[GenCodeVisitor] Advertencia: demasiados parámetros enteros en '"
                    << f->nombre << "'.\n";
            }
        }
        offset -= 8; // slot de 8 bytes para cada parámetro (aunque float use solo 4)
    }

    // Variables locales del cuerpo
    if (f->cuerpo) {
        for (auto vd : f->cuerpo->declarations) {
            vd->accept(this);
        }
    }

    int reserva = -offset - 8;
    if (reserva % 16 != 0)
        reserva += 8;

    if (reserva > 0)
        out << " subq $" << reserva << ", %rsp\n";

    // Sentencias
    if (f->cuerpo) {
        for (auto s : f->cuerpo->StmList) {
            if (s) s->accept(this);
        }
    } else {
        std::cerr << "[GenCodeVisitor] Advertencia: cuerpo nulo en función '"
                  << f->nombre << "'.\n";
    }

    out << ".end_" << f->nombre << ":\n";
    out << " leave\n";
    out << " ret\n";

    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    if (!exp) return 0;

    vector<string> intRegs   = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
    vector<string> floatRegs = {"%xmm0","%xmm1","%xmm2","%xmm3","%xmm4","%xmm5"};

    struct TempArg {
        bool isFloat;
        int  offset;
    };

    vector<TempArg> temps;

    // 1) Evaluar argumentos de izquierda a derecha
    for (size_t i = 0; i < exp->argumentos.size(); ++i) {
        Exp* arg = exp->argumentos[i];
        arg->accept(this);

        if (arg->tipoDato == T_FLOAT) {
            offset -= 8;
            out << " movss %xmm0, " << offset << "(%rbp)\n";
            temps.push_back({true, offset});
        } else {
            offset -= 8;
            out << " movq %rax, " << offset << "(%rbp)\n";
            temps.push_back({false, offset});
        }
    }

    // 2) Asignar argumentos a los registros ABI en orden
    int iInt = 0;
    int iFlt = 0;

    for (size_t i = 0; i < temps.size(); ++i) {
        if (temps[i].isFloat) {
            out << " movss " << temps[i].offset << "(%rbp), " 
                << floatRegs[iFlt++] << "\n";
        } else {
            out << " movq " << temps[i].offset << "(%rbp), " 
                << intRegs[iInt++] << "\n";
        }
    }

    out << " call " << exp->nombre << "\n";
    return 0;
}

int GenCodeVisitor::visit(ExpStm* s) {
    if (s && s->e) s->e->accept(this);
    return 0;
}
