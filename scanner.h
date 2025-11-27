#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"

using namespace std;

class Scanner {
private:
    string input;   // Texto completo del archivo fuente
    int first;      // Índice de inicio del lexema actual
    int current;    // Índice de lectura actual

public:
    // Constructor: recibe el código fuente como C-string
    Scanner(const char* in_s);

    // Retorna el siguiente token
    Token* nextToken();

    // Destructor
    ~Scanner();
};

// Función auxiliar para probar el scanner y volcar los tokens a un archivo
int ejecutar_scanner(Scanner* scanner, const string& InputFile);

#endif // SCANNER_H
