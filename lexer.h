#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // Keywords
    TOKEN_RECIPE,
    TOKEN_INGREDIENTS,
    TOKEN_STEPS,
    TOKEN_END,
    TOKEN_FOR,
    TOKEN_AT,

    // Actions
    TOKEN_CHOP,
    TOKEN_MIX,
    TOKEN_HEAT,
    TOKEN_BAKE,
    TOKEN_BOIL,
    TOKEN_SERVE,

    // Literals and Identifiers
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,

    // Separators
    TOKEN_COLON, // :

    // End of File and Errors
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

void init_lexer(const char *source);
Token scan_token();

#endif // LEXER_H
