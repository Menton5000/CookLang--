#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

// Generates C code and compiles it
void generate_code(Node *program, const char *output_name);

#endif // CODEGEN_H
