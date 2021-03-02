#ifndef PARSER_H
#define PARSER_H

#include "stack.h"
#include "lexer.h"

typedef enum {
    PARSER_ERROR_LEXER_ERROR,
    PARSER_ERROR_UNEXPECTED_TOKEN,
    PARSER_ERROR_UNEXPECTED_EOF,
    PARSER_ERROR_INVALID_SEQUENCE,
} ParserErrorType;

typedef struct {
    ParserErrorType type;
    char text[128];
} ParserError;

typedef struct {
    Lexer *lex;
    Token *token;
    ParserStack *stack;
    ParserError *error;
} Parser;

ParserError *parser_error_new(Parser *p, ParserErrorType type);
void parser_error_destroy(ParserError *err);
Parser *parser_new(Lexer *lex);
void parser_destroy(Parser *p);
void parser_destroy_all(Parser *p);
void parser_parse(Parser *p);

#endif
