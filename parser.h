#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST Node Types
typedef enum {
    NODE_PROGRAM,
    NODE_INGREDIENT,
    NODE_STEP
} NodeType;

// Forward declaration
struct Node;

typedef struct {
    double quantity;
    char *unit;
    char *name;
} IngredientData;

typedef struct {
    char *action;
    char *object;
    // Time
    int has_time;
    double time_val;
    char *time_unit;
    // Temp
    int has_temp;
    double temp_val;
    char *temp_unit;
} StepData;

typedef struct Node {
    NodeType type;
    union {
        struct { // NODE_PROGRAM
            char *name; // Recipe name
            struct Node **ingredients;
            int ingredient_count;
            struct Node **steps;
            int step_count;
        } program;
        
        IngredientData ingredient; // NODE_INGREDIENT
        StepData step;             // NODE_STEP
    } data;
} Node;

Node *parse_program();
void free_ast(Node *node);

#endif // PARSER_H
