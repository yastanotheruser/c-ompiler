#ifndef PARSER_H
#define PARSER_H

#include "stack.h"
#include "lexer.h"

// Estructura general de un procedimiento del parser
#define PARSER_PROC(name, definition) \
    static void parser_proc##name(Parser *p) \
    { \
        if (p->beforeproc != NULL) { \
            p->beforeproc(#name, p); \
        } \
        definition; \
        if (p->afterproc != NULL) { \
            p->afterproc(#name, p); \
        } \
    }

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

struct Parser;
typedef void (*ParserProcCallback)(const char*, struct Parser*);
typedef struct Parser {
    Lexer *lex;
    Token *token;
    ParserStack *stack;
    ParserError *error;
    ParserProcCallback beforeproc;
    ParserProcCallback afterproc;
} Parser;

ParserError *parser_error_new(Parser *p, ParserErrorType type);
void parser_error_destroy(ParserError *err);
Parser *parser_new(Lexer *lex);
void parser_destroy(Parser *p);
void parser_destroy_all(Parser *p);
void parser_parse(Parser *p);

#endif
