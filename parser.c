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

static void parser_accept(Parser *p)
{
    // Secuencia aceptada
    puts("Secuencia aceptada");
}

static void parser_reject(Parser *p)
{
    // Secuencia rechazada
    p->error = parser_error_new(p, PARSER_ERROR_INVALID_SEQUENCE);
    puts("Secuencia rechazada");
}

static void parser_proc1(Parser *p)
{
    // <A> → <B><A'>
    // Entrada: id | num | "("
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_AP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_B);
}

static void parser_proc2(Parser *p)
{
    // <A'> → <OL><B><A'>
    // Entrada: "&" | "|"
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_AP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_B);
    parser_advance(p);
}

static void parser_proc3(Parser *p)
{
    // <A'> → ε
    // Entrada: ")" | ¬
    parser_stack_pop(p->stack, NULL);
}

static void parser_proc4(Parser *p)
{
    // <B> → <C><B'>
    // Entrada: id | num | "("
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_BP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_C);
}

static void parser_proc5(Parser *p)
{
    // <B'> → <OR><C><B'>
    // Entrada: "=" | "<" | "<=" | ">" | ">="
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_BP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_C);
    parser_advance(p);
}

static void parser_proc6(Parser *p)
{
    // <B'> → ε
    // Entrada: <OL> | ")" | ¬
    parser_stack_pop(p->stack, NULL);
}

static void parser_proc7(Parser *p)
{
    // <C> → <D><C'>
    // Entrada: id | num | "("
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_CP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_D);
}

static void parser_proc8(Parser *p)
{
    // <C'> → <OA1><D><C'>
    // Entrada: "+" | "-"
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_CP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_D);
    parser_advance(p);
}

static void parser_proc9(Parser *p)
{
    // <C'> → ε
    // Entrada: <OR> | <OL> | ")" | ¬
    parser_stack_pop(p->stack, NULL);
}

static void parser_proc10(Parser *p)
{
    // <D> → <E><D'>
    // Entrada: id | num | "("
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_DP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_E);
}

static void parser_proc11(Parser *p)
{
    // <D'> → <OA2><E><D'>
    // Entrada: "*" | "/"
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_DP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_E);
    parser_advance(p);
}

static void parser_proc12(Parser *p)
{
    // <D'> → ε
    // Entrada: <OA1> | <OR> | <OL> | ")" | ¬
    parser_stack_pop(p->stack, NULL);
}

static void parser_proc13(Parser *p)
{
    // <E> → <F><E'>
    // Entrada: id | num | "("
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_EP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_F);
}

static void parser_proc14(Parser *p)
{
    // <E'> → <OA3><F><E'>
    // Entrada: "^"
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_EP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_F);
    parser_advance(p);
}

static void parser_proc15(Parser *p)
{
    // <E'> → ε
    // Entrada: <OA2> | <OA1> | <OR> | <OL> | ")" | ¬
    parser_stack_pop(p->stack, NULL);
}

static void parser_proc16(Parser *p)
{
    // <F> → id | num
    parser_stack_pop(p->stack, NULL);
    parser_advance(p);
}

static void parser_proc17(Parser *p)
{
    // <F> → "(" <A> ")"
    parser_stack_pop(p->stack, NULL);
    parser_stack_push(p->stack, PARSER_SYMBOL_TERM_RP);
    parser_stack_push(p->stack, PARSER_SYMBOL_NONTERM_A);
    parser_advance(p);
}

static void parser_proc18(Parser *p)
{
    // Símbolo de pila: "("
    parser_stack_pop(p->stack, NULL);
    parser_advance(p);
}

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
            if (p->token == NULL) {
                parser_accept(p);
            } else {
                parser_reject(p);
            }

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
                break;
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
                break;
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
