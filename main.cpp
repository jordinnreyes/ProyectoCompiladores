#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"

using namespace std;

// Declaración de la función definida en ast.cpp
void optimizeAST(Program* prog);

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner y Parser
    Scanner scanner1(input.c_str());
    Parser parser(&scanner1);

    // Parsear y generar AST
    Program* program = parser.parseProgram();
    if (!program) {
        cerr << "[ERROR] Parser falló: AST nulo.\n";
        return 2;
    }

    // Determinar nombre del archivo de salida (.s)
    string inputFile(argv[1]);
    size_t dotPos = inputFile.find_last_of('.');
    string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
    string outputFilename = baseName + ".s";

    ofstream outfile(outputFilename);
    if (!outfile.is_open()) {
        cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
        return 1;
    }

    cout << "\n=== DEBUG AST ===\n";
    cout << "Funciones en el AST: " << program->fdlist.size() << endl;
    for (auto* f : program->fdlist) {
        cout << " - " << f->nombre
            << " (" << f->Ptipos.size() << " tipos, "
            << f->Pnombres.size() << " nombres, cuerpo "
            << (f->cuerpo ? "OK" : "NULL") << ")\n";
    }
    cout << "=================\n";

    //Analizador de tipos
    TypeCheckVisitor typer;
    typer.analizar(program);

    //Aplicar optimizaciones
    optimizeAST(program);

    //Generar código ensamblador
    cout << "Generando codigo ensamblador en " << outputFilename << endl;
    GenCodeVisitor codigo(outfile);
    codigo.tipoGlobal = typer.tipoGlobal;
    codigo.tipoLocal  = typer.tipoLocal;
    codigo.generar(program);

    outfile.close();
    cout << "Compilación y optimización completadas con éxito." << endl;

    return 0;
}
