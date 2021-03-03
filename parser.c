#include "parser.h"
#include <string.h>
#include "cdefs.h"
#include "cmem.h"

enum ParserStackSymbols {
    PARSER_SYMBOL_BOTTOM,           // Fondo de pila
    PARSER_SYMBOL_NONTERM_A,        // <A> (Axioma)
    PARSER_SYMBOL_NONTERM_AP,       // <A'>
    PARSER_SYMBOL_NONTERM_B,        // <B>
    PARSER_SYMBOL_NONTERM_BP,       // <B'>
    PARSER_SYMBOL_NONTERM_C,        // <C>
    PARSER_SYMBOL_NONTERM_CP,       // <C'>
    PARSER_SYMBOL_NONTERM_D,        // <D>
    PARSER_SYMBOL_NONTERM_DP,       // <D'>
    PARSER_SYMBOL_NONTERM_E,        // <E>
    PARSER_SYMBOL_NONTERM_EP,       // <E'>
    PARSER_SYMBOL_NONTERM_F,        // <F>
    PARSER_SYMBOL_TERM_RP,          // ")"
};

ParserError *parser_error_new(Parser *p, ParserErrorType type)
{
    ParserError *err = cmalloc(sizeof(*err));
    err->type = type;

    switch (type) {
    case PARSER_ERROR_LEXER_ERROR:
        sprintf(err->text, "Error léxico: %s", p->lex->error->text);
        break;
    case PARSER_ERROR_UNEXPECTED_TOKEN:
        sprintf(err->text, "No se esperaba token `%s` en línea %d, columna %d",
                p->token->text, p->token->line + 1, p->token->column + 1);
        break;
    case PARSER_ERROR_UNEXPECTED_EOF:
        strcpy(err->text, "No se esperaba fin de archivo");
        break;
    case PARSER_ERROR_INVALID_SEQUENCE:
        strcpy(err->text, "Secuencia inválida");
        break;
    }

    return err;
}

void parser_error_destroy(ParserError *err)
{
    cfree(err);
}

Parser *parser_new(Lexer *lex)
{
    Parser *p = cmalloc(sizeof(*p));
    p->lex = lex;
    p->stack = parser_stack_new();
    p->error = NULL;
    p->beforeproc = NULL;
    p->afterproc = NULL;
    return p;
}

void parser_destroy(Parser *p)
{
    parser_stack_destroy(p->stack);
    if (p->error != NULL) {
        parser_error_destroy(p->error);
    }

    cfree(p);
}

void parser_destroy_all(Parser *p)
{
    lexer_destroy(p->lex);
    parser_destroy(p);
}

static int parser_advance(Parser *p)
{
    if (p->token != NULL) {
        lexer_token_destroy(p->token);
    }

    p->token = lexer_next_token(p->lex);
    return p->token != NULL;
}

static void parser_reject(Parser *p)
{
    // Secuencia rechazada
    ParserError *err;
    if (p->token == NULL) {
        if (p->lex->error != NULL) {
            err = parser_error_new(p, PARSER_ERROR_LEXER_ERROR);
        } else {
            err = parser_error_new(p, PARSER_ERROR_UNEXPECTED_EOF);
        }
    } else {
        err = parser_error_new(p, PARSER_ERROR_UNEXPECTED_TOKEN);
    }

    p->error = err;
}

// <A> → <B><A'>
// Entrada: id | num | "("
PARSER_PROC(1,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_AP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_B))

// <A'> → <OL><B><A'>
// Entrada: "&" | "|"
PARSER_PROC(2,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_AP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_B);
    parser_advance(p))

// <A'> → ε
// Entrada: ")" | ¬
PARSER_PROC(3, parser_stack_pop(p->stack, NULL))

// <B> → <C><B'>
// Entrada: id | num | "("
PARSER_PROC(4,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_BP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_C))

// <B'> → <OR><C><B'>
// Entrada: "=" | "<" | "<=" | ">" | ">="
PARSER_PROC(5,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_BP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_C);
    parser_advance(p))

// <B'> → ε
// Entrada: <OL> | ")" | ¬
PARSER_PROC(6, parser_stack_pop(p->stack, NULL))

// <C> → <D><C'>
// Entrada: id | num | "("
PARSER_PROC(7,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_CP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_D))

// <C'> → <OA1><D><C'>
// Entrada: "+" | "-"
PARSER_PROC(8,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_CP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_D);
    parser_advance(p))

// <C'> → ε
// Entrada: <OR> | <OL> | ")" | ¬
PARSER_PROC(9, parser_stack_pop(p->stack, NULL))

// <D> → <E><D'>
// Entrada: id | num | "("
PARSER_PROC(10,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_DP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_E))

// <D'> → <OA2><E><D'>
// Entrada: "*" | "/"
PARSER_PROC(11,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_DP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_E);
    parser_advance(p))

// <D'> → ε
// Entrada: <OA1> | <OR> | <OL> | ")" | ¬
PARSER_PROC(12, parser_stack_pop(p->stack, NULL))

// <E> → <F><E'>
// Entrada: id | num | "("
PARSER_PROC(13,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_EP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_F))

// <E'> → <OA3><F><E'>
// Entrada: "^"
PARSER_PROC(14,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_EP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_F);
    parser_advance(p))

// <E'> → ε
// Entrada: <OA2> | <OA1> | <OR> | <OL> | ")" | ¬
PARSER_PROC(15, parser_stack_pop(p->stack, NULL))

// <F> → id | num
PARSER_PROC(16,
    parser_stack_pop(p->stack, NULL);
    parser_advance(p))

// <F> → "(" <A> ")"
PARSER_PROC(17,
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_TERM_RP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_A);
    parser_advance(p))

// Símbolo de pila: "("
PARSER_PROC(18,
    parser_stack_pop(p->stack, NULL);
    parser_advance(p));

/*
 * Gramática:
 * <A>   → <B><A'>
 * <A'>  → <OL><B><A'>
 * <A'>  → ε
 * <OL>  → "&" | "|"
 * <B>   → <C><B'>
 * <B'>  → <OR><C>
 * <B'>  → ε
 * <OR>  → "=" | "<" | "<=" | ">" | ">="
 * <C>   → <D><C'>
 * <C'>  → <OA1><D><C'>
 * <C'>  → ε
 * <OA1> → "+" | "-"
 * <D>   → <E><D'>
 * <D'>  → <OA2><E><D'>
 * <D'>  → ε
 * <OA2> → "*" | "/"
 * <E>   → <F><E'>
 * <E'>  → <OA3><F><E'>
 * <E'>  → ε
 * <OA3> → "^"
 * <F>   → id
 * <F>   → num
 * <F>   → "(" <A> ")"
 */
void parser_parse(Parser *p)
{
    parser_stack_push(p->stack, PARSER_SYMBOL_BOTTOM);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_A);
    parser_advance(p);

    while (TRUE) {
        if (p->lex->error != NULL) {
            p->error = parser_error_new(p, PARSER_ERROR_LEXER_ERROR);
            return;
        }

        int stack_top = *parser_stack_top(p->stack);
        switch (stack_top) {
        case PARSER_SYMBOL_BOTTOM:
            // Rechazar si aún queda un token
            if (p->token != NULL) {
                parser_reject(p);
            }

            // Finalizar análisis sintáctico
            return;
        case PARSER_SYMBOL_NONTERM_A:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
            case PAREN_LEFT:
                parser_proc1(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_AP:
            if (p->token == NULL) {
                parser_proc3(p);
                break;
            }

            switch (p->token->type) {
            case OPERATOR_LOGICAL:
                parser_proc2(p);
                break;
            case PAREN_RIGHT:
                parser_proc3(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_B:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
            case PAREN_LEFT:
                parser_proc4(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_BP:
            if (p->token == NULL) {
                parser_proc6(p);
                break;
            }

            switch (p->token->type) {
            case OPERATOR_RELATIONAL:
                parser_proc5(p);
                break;
            case OPERATOR_LOGICAL:
            case PAREN_RIGHT:
                parser_proc6(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_C:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
            case PAREN_LEFT:
                parser_proc7(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_CP:
            if (p->token == NULL) {
                parser_proc9(p);
                break;
            }

            switch (p->token->type) {
            case OPERATOR_ARITHMETIC:
                if (strcmp(p->token->text, "+") == 0 ||
                    strcmp(p->token->text, "-") == 0)
                {
                    parser_proc8(p);
                } else {
                    parser_reject(p);
                    return;
                }

                break;
            case OPERATOR_RELATIONAL:
            case OPERATOR_LOGICAL:
            case PAREN_RIGHT:
                parser_proc9(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_D:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
            case PAREN_LEFT:
                parser_proc10(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_DP:
            if (p->token == NULL) {
                parser_proc12(p);
                break;
            }

            switch (p->token->type) {
            case OPERATOR_ARITHMETIC:
                if (strcmp(p->token->text, "*") == 0 ||
                    strcmp(p->token->text, "/") == 0)
                {
                    parser_proc11(p);
                } else if (strcmp(p->token->text, "+") == 0 ||
                           strcmp(p->token->text, "-") == 0)
                {
                    parser_proc12(p);
                } else {
                    parser_reject(p);
                    return;
                }

                break;
            case OPERATOR_RELATIONAL:
            case OPERATOR_LOGICAL:
            case PAREN_RIGHT:
                parser_proc12(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_E:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
            case PAREN_LEFT:
                parser_proc13(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_EP:
            if (p->token == NULL) {
                parser_proc15(p);
                break;
            }

            switch (p->token->type) {
            case OPERATOR_ARITHMETIC:
                if (strcmp(p->token->text, "^") == 0) {
                    parser_proc14(p);
                } else if (strcmp(p->token->text, "+") == 0 ||
                           strcmp(p->token->text, "-") == 0 ||
                           strcmp(p->token->text, "*") == 0 ||
                           strcmp(p->token->text, "/") == 0)
                {
                    parser_proc15(p);
                } else {
                    parser_reject(p);
                    return;
                }

                break;
            case OPERATOR_RELATIONAL:
            case OPERATOR_LOGICAL:
            case PAREN_RIGHT:
                parser_proc15(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_NONTERM_F:
            if (p->token == NULL) {
                parser_reject(p);
                return;
            }

            switch (p->token->type) {
            case IDENTIFIER:
            case NUMBER:
                parser_proc16(p);
                break;
            case PAREN_LEFT:
                parser_proc17(p);
                break;
            default:
                parser_reject(p);
                return;
            }

            break;
        case PARSER_SYMBOL_TERM_RP:
            if (p->token != NULL && p->token->type == PAREN_RIGHT) {
                parser_proc18(p);
            } else {
                parser_reject(p);
                return;
            }

            break;
        }
    }
}
