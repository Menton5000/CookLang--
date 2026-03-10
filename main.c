#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "codegen.h"

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void get_output_name(const char *input_path, char *output_buffer)
{
    // tortilla.cooklang++ -> tortilla.exe
    const char *dot = strrchr(input_path, '.');
    int len = 0;
    if (!dot)
    {
        len = strlen(input_path);
    }
    else
    {
        len = dot - input_path;
    }
    memcpy(output_buffer, input_path, len);
    output_buffer[len] = '\0';
    strcat(output_buffer, ".exe"); // Force .exe for Windows/compat
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: cook <file.cooklang++>\n");
        return 1;
    }

    char *source = read_file(argv[1]);

    // 1. Lexer Initialization
    init_lexer(source);

    // 2. Parsing
    Node *program = parse_program();

    // 3. Semantic Analysis
    if (check_semantics(program) != 0)
    {
        fprintf(stderr, "Compilation aborted due to semantic errors.\n");
        exit(1);
    }

    // 4. Code Generation
    char output_name[256];
    get_output_name(argv[1], output_name);
    generate_code(program, output_name);

    free_ast(program);
    free(source);

    return 0;
}
