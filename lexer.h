#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "buffer.h"

#ifndef LEXER_BLOCK_SIZE
#define LEXER_BLOCK_SIZE 512
#endif

#ifndef LEXER_TBUF_SIZE
#define LEXER_TBUF_SIZE 64
#endif

typedef enum {
    LEXER_ERROR_NO_ERROR,
    LEXER_ERROR_INVALID_TOKEN,
    LEXER_ERROR_UNEXPECTED_EOF,
} LexerErrorType;

typedef struct {
    LexerErrorType type;
    char text[128];
    int line;
    int column;
} LexerError;

typedef struct {
    FILE *stream;
    LexerBuffer *block;
    LexerBuffer *tbuf;
    LexerError *error;
    int eof;
    int line;
    int column;
} Lexer;

typedef enum {
    UNKNOWN,
    NUMBER,
    IDENTIFIER,
    PAREN_LEFT,
    PAREN_RIGHT,
    OPERATOR_ARITHMETIC,
    OPERATOR_RELATIONAL,
    OPERATOR_LOGICAL,
} TokenType;

typedef struct {
    TokenType type;
    const char *text;
    int line;
    int column;
} Token;

Lexer *lexer_new(FILE *stream);
void lexer_destroy(Lexer *lex);
Token *lexer_next_token(Lexer *lex);
Token *lexer_token_new(TokenType type, const char *text, int line, int column);
void lexer_token_destroy(Token *t);
LexerError *lexer_error_new(Lexer *lex, LexerErrorType type);
void lexer_error_destroy(LexerError *err);

#endif
