#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

static Token current_token;
static Token previous_token;

static void advance_parser() {
    previous_token = current_token;
    current_token = scan_token();
    // Skip comments or irrelevant tokens if any, but we don't have them in lexer
}

static void consume(TokenType type, const char *message) {
    if (current_token.type == type) {
        advance_parser();
        return;
    }
    fprintf(stderr, "Error at line %d: %s. Got token type %d\n", current_token.line, message, current_token.type);
    exit(1);
}

static int match_token(TokenType type) {
    if (current_token.type == type) {
        advance_parser();
        return 1;
    }
    return 0;
}

static char* copy_string(const char *start, int length) {
    char *str = (char*)malloc(length + 1);
    memcpy(str, start, length);
    str[length] = '\0';
    return str;
}

// Helpers for Node creation
static Node* create_node(NodeType type) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->type = type;
    return node;
}

// Parsing functions
static Node* parse_ingredient() {
    Node *node = create_node(NODE_INGREDIENT);
    
    // <number> <unit/identifier> <ingredient/identifier>
    // e.g. 2 units eggs
    // or 1 pinch salt
    
    // Quantity
    if (!match_token(TOKEN_NUMBER)) {
        fprintf(stderr, "Error: Expected quantity (number) for ingredient.\n");
        exit(1);
    }
    // We need to parse the number string to double
    // Lexer stores ptr to start and len
    char *num_str = copy_string(previous_token.start, previous_token.length);
    node->data.ingredient.quantity = atof(num_str);
    free(num_str);

    // Unit (Identifier)
    if (!match_token(TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected unit for ingredient.\n");
        exit(1);
    }
    node->data.ingredient.unit = copy_string(previous_token.start, previous_token.length);

    // Ingredient Name (Identifier)
    if (!match_token(TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected ingredient name.\n");
        exit(1);
    }
    node->data.ingredient.name = copy_string(previous_token.start, previous_token.length);

    return node;
}

static Node* parse_step() {
    Node *node = create_node(NODE_STEP);
    node->data.step.has_time = 0;
    node->data.step.has_temp = 0;

    // Action (mix, chop, etc.) - These are keywords in Lexer but we treat them as action verbs here
    // In grammar: <action> ::= CHOP | MIX ...
    // Check if current token is one of the action tokens
    TokenType t = current_token.type;
    if (t >= TOKEN_CHOP && t <= TOKEN_SERVE) {
        advance_parser();
        node->data.step.action = copy_string(previous_token.start, previous_token.length);
    } else {
        fprintf(stderr, "Error: Expected action (mix, chop, etc) at line %d\n", current_token.line);
        exit(1);
    }

    // Object (Identifier) - e.g. "eggs", "mixture", "dish"
    // The grammar says <identifier>, but it could be "eggs" which is an identifier.
    // NOTE: "mixture" or "dish" might be arbitrary identifiers.
    if (!match_token(TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected object for action.\n");
        exit(1);
    }
    node->data.step.object = copy_string(previous_token.start, previous_token.length);

    // Options: [for <time>] [at <temp>]
    // Order is not strictly enforced in example, but grammar said: <time_opt> <temp_opt>
    // Let's allow arbitrary order or check specifically.
    
    while (1) {
        if (match_token(TOKEN_FOR)) {
             // Time: <number> <identifier>
             if (!match_token(TOKEN_NUMBER)) {
                 fprintf(stderr, "Error: Expected number after 'for'.\n");
                 exit(1);
             }
             char *num_str = copy_string(previous_token.start, previous_token.length);
             node->data.step.time_val = atof(num_str);
             free(num_str);
             node->data.step.has_time = 1;

             // Unit might be attached or separate? Lexer splits numbers and identifiers.
             // "30s" -> NUMBER(30) IDENTIFIER(s)
             if (!match_token(TOKEN_IDENTIFIER)) {
                 fprintf(stderr, "Error: Expected time unit.\n");
                 exit(1);
             }
             node->data.step.time_unit = copy_string(previous_token.start, previous_token.length);

        } else if (match_token(TOKEN_AT)) {
            // Temp: <number> <identifier>
            if (!match_token(TOKEN_NUMBER)) {
                 fprintf(stderr, "Error: Expected number after 'at'.\n");
                 exit(1);
             }
             char *num_str = copy_string(previous_token.start, previous_token.length);
             node->data.step.temp_val = atof(num_str);
             free(num_str);
             node->data.step.has_temp = 1;

             if (!match_token(TOKEN_IDENTIFIER)) { // e.g. "C" or "F"
                 fprintf(stderr, "Error: Expected temperature unit.\n");
                 exit(1);
             }
             node->data.step.temp_unit = copy_string(previous_token.start, previous_token.length);
        } else {
            break;
        }
    }

    return node;
}

Node* parse_program() {
    advance_parser(); // Prime the pump
    
    Node *program = create_node(NODE_PROGRAM);
    program->data.program.ingredients = NULL;
    program->data.program.ingredient_count = 0;
    program->data.program.steps = NULL;
    program->data.program.step_count = 0;

    // RECIPE <name>
    consume(TOKEN_RECIPE, "Expected 'RECIPE' at start");
    consume(TOKEN_IDENTIFIER, "Expected recipe name");
    program->data.program.name = copy_string(previous_token.start, previous_token.length);

    // INGREDIENTS:
    consume(TOKEN_INGREDIENTS, "Expected 'INGREDIENTS' section");
    if (match_token(TOKEN_COLON)) { /* Optional colon handled */ }

    // Parse ingredients until we hit STEPS
    // How do we allocate? Dynamic array realloc for simplicity.
    int i_cap = 10;
    program->data.program.ingredients = malloc(sizeof(Node*) * i_cap);
    
    while (current_token.type != TOKEN_STEPS && current_token.type != TOKEN_EOF) {
        if (program->data.program.ingredient_count >= i_cap) {
            i_cap *= 2;
            program->data.program.ingredients = realloc(program->data.program.ingredients, sizeof(Node*) * i_cap);
        }
        program->data.program.ingredients[program->data.program.ingredient_count++] = parse_ingredient();
    }

    // STEPS:
    consume(TOKEN_STEPS, "Expected 'STEPS' section");
    if (match_token(TOKEN_COLON)) { /* Optional colon */ }

    int s_cap = 10;
    program->data.program.steps = malloc(sizeof(Node*) * s_cap);

    while (current_token.type != TOKEN_END && current_token.type != TOKEN_EOF) {
        if (program->data.program.step_count >= s_cap) {
            s_cap *= 2;
            program->data.program.steps = realloc(program->data.program.steps, sizeof(Node*) * s_cap);
        }
        program->data.program.steps[program->data.program.step_count++] = parse_step();
    }

    consume(TOKEN_END, "Expected 'END' at the end of recipe");
    
    return program;
}

void free_ast(Node *node) {
    if (!node) return;
    if (node->type == NODE_PROGRAM) {
        free(node->data.program.name);
        for (int i=0; i < node->data.program.ingredient_count; i++) free_ast(node->data.program.ingredients[i]);
        free(node->data.program.ingredients);
        for (int i=0; i < node->data.program.step_count; i++) free_ast(node->data.program.steps[i]);
        free(node->data.program.steps);
    } else if (node->type == NODE_INGREDIENT) {
        free(node->data.ingredient.unit);
        free(node->data.ingredient.name);
    } else if (node->type == NODE_STEP) {
        free(node->data.step.action);
        free(node->data.step.object);
        if (node->data.step.has_time) free(node->data.step.time_unit);
        if (node->data.step.has_temp) free(node->data.step.temp_unit);
    }
    free(node);
}
