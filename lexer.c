#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

typedef struct {
    const char *start;
    const char *current;
    int line;
} Lexer;

Lexer lexer;

void init_lexer(const char *source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

static int is_at_end() {
    return *lexer.current == '\0';
}

static char advance() {
    lexer.current++;
    return lexer.current[-1];
}

static char peek() {
    return *lexer.current;
}

static char peek_next() {
    if (is_at_end()) return '\0';
    return lexer.current[1];
}

static int match(char expected) {
    if (is_at_end()) return 0;
    if (*lexer.current != expected) return 0;
    lexer.current++;
    return 1;
}

static Token make_token(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    return token;
}

static Token error_token(const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer.line;
    return token;
}

static void skip_whitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                lexer.line++;
                advance();
                break;
            default:
                return;
        }
    }
}

static TokenType check_keyword(int start, int length, const char *rest, TokenType type) {
    if (lexer.current - lexer.start == start + length &&
        memcmp(lexer.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type() {
    // Basic Trie / Switch for keywords
    switch (lexer.start[0]) {
        case 'a': return check_keyword(1, 1, "t", TOKEN_AT);
        case 'b':
            if (lexer.current - lexer.start > 1) {
                switch (lexer.start[1]) {
                    case 'a': return check_keyword(2, 2, "ke", TOKEN_BAKE);
                    case 'o': return check_keyword(2, 2, "il", TOKEN_BOIL);
                }
            }
            break;
        case 'c': return check_keyword(1, 3, "hop", TOKEN_CHOP);
        // END
        case 'E': return check_keyword(1, 2, "ND", TOKEN_END);
        case 'f': return check_keyword(1, 2, "or", TOKEN_FOR);
        case 'h': return check_keyword(1, 3, "eat", TOKEN_HEAT);
        // INGREDIENTS
        case 'I': return check_keyword(1, 10, "NGREDIENTS", TOKEN_INGREDIENTS);
        case 'm': return check_keyword(1, 2, "ix", TOKEN_MIX);
        case 'R': return check_keyword(1, 5, "ECIPE", TOKEN_RECIPE);
        case 'S':
            if (lexer.current - lexer.start > 1) {
               switch(lexer.start[1]) {
                   case 'T': return check_keyword(2, 3, "EPS", TOKEN_STEPS);
                   case 'E': return check_keyword(2, 3, "RVE", TOKEN_SERVE); // Wait, SERVE vs STEPS?
                   // SERVE -> S E R V E
                   // STEPS -> S T E P S
               } 
            }
            if (memcmp(lexer.start, "serve", 5) == 0 && (lexer.current - lexer.start == 5)) return TOKEN_SERVE; // Simplify
            break;
        case 's': return check_keyword(1, 4, "erve", TOKEN_SERVE); 
    }
    
    // Fallback for case-insensitivity or simple check
    // The spec uses uppercase for structure (RECIPE) and lowercase for actions (chop).
    // Let's being strict?
    // User example: RECIPE Tortilla (UPPER)
    // mix (lower)
    
    // My switch above handles mixed cases a bit loosely. Let's strictify.
    // Uppercase Keywords: RECIPE, INGREDIENTS, STEPS, END
    // Lowercase Keywords/Actions: chop, mix, heat, bake, boil, serve, for, at
    
    char c = lexer.start[0];
    if (c == 'R') return check_keyword(1, 5, "ECIPE", TOKEN_RECIPE);
    if (c == 'I') return check_keyword(1, 10, "NGREDIENTS", TOKEN_INGREDIENTS);
    if (c == 'S') return check_keyword(1, 4, "TEPS", TOKEN_STEPS); // STEPS
    if (c == 'E') return check_keyword(1, 2, "ND", TOKEN_END);
    
    if (c == 'f') return check_keyword(1, 2, "or", TOKEN_FOR);
    if (c == 'a') return check_keyword(1, 1, "t", TOKEN_AT);
    
    if (c == 'c') return check_keyword(1, 3, "hop", TOKEN_CHOP);
    if (c == 'm') return check_keyword(1, 2, "ix", TOKEN_MIX);
    if (c == 'h') return check_keyword(1, 3, "eat", TOKEN_HEAT);
    if (c == 'b') {
        if (lexer.current - lexer.start > 1 && lexer.start[1] == 'a') return check_keyword(2, 2, "ke", TOKEN_BAKE);
        if (lexer.current - lexer.start > 1 && lexer.start[1] == 'o') return check_keyword(2, 2, "il", TOKEN_BOIL);
    }
    if (c == 's') return check_keyword(1, 4, "erve", TOKEN_SERVE);

    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (is_alpha(peek()) || is_digit(peek())) advance();
    return make_token(identifier_type());
}

static Token number() {
    while (is_digit(peek())) advance();
    // Allow decimal
    if (peek() == '.' && is_digit(peek_next())) {
        advance();
        while (is_digit(peek())) advance();
    }
    return make_token(TOKEN_NUMBER);
}

Token scan_token() {
    skip_whitespace();
    lexer.start = lexer.current;

    if (is_at_end()) return make_token(TOKEN_EOF);

    char c = advance();

    if (is_alpha(c)) return identifier();
    if (is_digit(c)) return number();

    switch (c) {
        case ':': return make_token(TOKEN_COLON);
    }

    return error_token("Unexpected character.");
}
