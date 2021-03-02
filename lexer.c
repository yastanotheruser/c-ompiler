#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include "cdefs.h"
#include "cmem.h"

typedef enum {
    BEGIN,
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    ID,
    PL,
    PR,
    OA,
    OR1,
    OR2,
    OR3,
    OR4,
    OR5,
    OL,
} LexerState;

Lexer *lexer_new(FILE *stream)
{
    Lexer *lex = cmalloc(sizeof(*lex));
    lex->stream = stream;
    lex->block = lexer_buffer_new(LEXER_BLOCK_SIZE);
    lex->block->bptr = lex->block->bend;
    lex->tbuf = lexer_buffer_new(LEXER_TBUF_SIZE);
    lex->error = NULL;
    lex->eof = FALSE;
    lex->line = 0;
    lex->column = 0;
    return lex;
}

void lexer_destroy(Lexer *lex) {
    lexer_buffer_destroy(lex->block);
    lexer_buffer_destroy(lex->tbuf);

    if (lex->error != NULL) {
        lexer_error_destroy(lex->error);
    }

    fclose(lex->stream);
    cfree(lex);
}

static int lexer_block_end(Lexer *lex)
{
    return lexer_buffer_is_end(lex->block);
}

static size_t lexer_read(Lexer *lex)
{
    size_t nread = fread(lex->block->data, 1, LEXER_BLOCK_SIZE, lex->stream);
    if (nread == 0) {
        if (feof(lex->stream)) {
            lex->eof = TRUE;
        } else if (ferror(lex->stream)) {
            perror("lexer read");
            lex->eof = TRUE;
        }
    }

    if (!lex->eof) {
        lex->block->bptr = lex->block->data;
        lex->block->bend = lex->block->data + nread;
    }

    return nread;
}

Token *lexer_next_token(Lexer *lex)
{
    LexerState state = BEGIN;
    TokenType type = UNKNOWN;
    char c;
    int s;
    int use_char;
    LexerErrorType error = NO_ERROR;
    lexer_buffer_seek(lex->tbuf, 0, LEXER_BUFFER_SET);

    while (TRUE) {
        if (lexer_block_end(lex)) {
            lexer_read(lex);
        }

        if (!lex->eof) {
            c = lexer_buffer_advance(lex->block);
            s = isspace(c);
        } else {
            s = TRUE;
        }

        use_char = TRUE;
        switch (state) {
        case BEGIN:
            if (lex->eof) {
                return NULL;
            } else if (s) {
                break;
            }

            if (isdigit(c)) {
                state = NUM1;
            } else if (c == '.') {
                state = NUM4;
            } else if (isalpha(c) || c == '_') {
                state = ID;
            } else if (c == '(') {
                state = PL;
            } else if (c == ')') {
                state = PR;
            } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                state = OA;
            } else if (c == '=') {
                state = OR1;
            } else if (c == '<') {
                state = OR2;
            } else if (c == '>') {
                state = OR4;
            } else if (c == '&' || c == '|') {
                state = OL;
            } else {
                error = INVALID_TOKEN;
            }

            break;
        case NUM1:
            if (s) {
                type = NUMBER;
            } else if (c == '.') {
                state = NUM2;
            } else if (!isdigit(c)) {
                type = NUMBER;
                use_char = FALSE;
            }

            break;
        case NUM2:
            if (s || !isdigit(c)) {
                type = NUMBER;
                use_char = FALSE;
            } else {
                state = NUM3;
            }

            break;
        case NUM3:
            if (s || !isdigit(c)) {
                type = NUMBER;
                use_char = FALSE;
            }

            break;
        case NUM4:
            if (s || !isdigit(c)) {
                use_char = FALSE;
                if (lex->eof) {
                    error = UNEXPECTED_EOF;
                } else {
                    error = INVALID_TOKEN;
                }
            } else {
                state = NUM5;
            }

            break;
        case NUM5:
            if (s || !isdigit(c)) {
                type = NUMBER;
                use_char = FALSE;
            }

            break;
        case ID:
            if (s || (!isalnum(c) && c != '_')) {
                type = IDENTIFIER;
                use_char = FALSE;
            }

            break;
        case PL:
            type = PAREN_LEFT;
            use_char = FALSE;
            break;
        case PR:
            type = PAREN_RIGHT;
            use_char = FALSE;
            break;
        case OA:
            type = OPERATOR_ARITHMETIC;
            use_char = FALSE;
            break;
        case OR1:
            type = OPERATOR_RELATIONAL;
            use_char = FALSE;
            break;
        case OR2:
            if (s || c != '=') {
                type = OPERATOR_RELATIONAL;
                use_char = FALSE;
            } else {
                state = OR3;
            }

            break;
        case OR3:
            type = OPERATOR_RELATIONAL;
            use_char = FALSE;
            break;
        case OR4:
            if (s || c != '=') {
                type = OPERATOR_RELATIONAL;
                use_char = FALSE;
            } else {
                state = OR5;
            }

            break;
        case OR5:
            type = OPERATOR_RELATIONAL;
            use_char = FALSE;
            break;
        case OL:
            type = OPERATOR_LOGICAL;
            use_char = FALSE;
            break;
        }

        if (!s) {
            if (use_char) {
                lexer_buffer_put(lex->tbuf, c);
            } else {
                lexer_buffer_unadvance(lex->block);
            }
        }

        if (error != NO_ERROR) {
            break;
        }

        if (!lex->eof && (s || use_char)) {
            if (c == '\r') {
                if (lexer_block_end(lex)) {
                    lexer_read(lex);
                }

                if (!lex->eof && lexer_buffer_advance(lex->block) != '\n') {
                    lexer_buffer_unadvance(lex->block);
                }
            }
            
            if (c == '\r' || c == '\n') {
                lex->line++;
                lex->column = 0;
            } else {
                lex->column++;
            }
        }

        if (type != UNKNOWN) {
            break;
        }
    }

    lexer_buffer_put(lex->tbuf, 0);
    if (error != NO_ERROR) {
        lex->error = lexer_error_new(lex, error);
        return NULL;
    }

    return lexer_token_new(type, lex->tbuf->data);
}

Token *lexer_token_new(TokenType type, const char *text)
{
    Token *t = cmalloc(sizeof(*t));
    char *dest = cmalloc(strlen(text) + 1);
    strcpy(dest, text);
    t->type = type;
    t->text = dest;
    return t;
}

void lexer_token_destroy(Token *t)
{
    cfree((void*) t->text);
    cfree(t);
}

LexerError *lexer_error_new(Lexer *lex, LexerErrorType type)
{
    LexerError *err = cmalloc(sizeof(*err));
    int line = lex->line;
    int column = lex->column;
    column -= lexer_buffer_offset(lex->tbuf) - 1;
    err->type = type;

    switch (type) {
    case INVALID_TOKEN:
        sprintf(err->text, "Token inválido `%s` en línea %d, columna %d",
                lex->tbuf->data, line + 1, column + 1);
        break;
    case UNEXPECTED_EOF:
        strcpy(err->text, "No se esperaba fin de archivo");
        break;
    default:
        strcpy(err->text, "Error léxico");
        break;
    }

    err->line = line;
    err->column = column;
    return err;
}

void lexer_error_destroy(LexerError *err)
{
    cfree(err);
}
