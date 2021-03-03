#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cdefs.h"
#include "cmem.h"
#include "parser.h"
#include "stack.h"

#define TEXT_DELIM "--------------------------------\n"

static const char *filename;

static void pstos(char *dest, ParserStack *stack)
{
    *dest++ = '{';
    for (int i = stack->size - 1; i >= 0; i--) {
        dest += sprintf(dest, "%d", stack->data[i]);
        if (i > 0) {
            dest += sprintf(dest, ", ");
        }
    }

    *dest++ = '}';
    *dest = 0;
}

static void beforeproc_func(const char *name, Parser *p)
{
    char stack_str[1024];
    pstos(stack_str, p->stack);
    fprintf(stderr, "%s: Inicio procedimiento %s\n", filename, name);
    fprintf(stderr, "%s: Pila -> %s\n", filename, stack_str);

    if (p->token != NULL) {
        fprintf(stderr, "%s: Token -> `%s` (tipo %d)\n",
                filename, p->token->text, p->token->type);
    } else {
        fprintf(stderr, "%s: Fin de secuencia\n", filename);
    }
}

static void afterproc_func(const char *name, Parser *p)
{
    char stack_str[1024];
    pstos(stack_str, p->stack);
    fprintf(stderr, "%s: Pila -> %s\n", filename, stack_str);
    fprintf(stderr, "%s: Fin procedimiento %s\n", filename, name);
    fprintf(stderr, TEXT_DELIM);
}

static void parse_file()
{
    FILE *f;
    int is_stdin = strcmp(filename, "-") == 0;

    if (!is_stdin) {
        f = fopen(filename, "rb");
        if (f == NULL) {
            fprintf(stderr, "c-ompiler: ");
            perror(filename);
            return;
        }
    } else {
        f = stdin;
        filename = "STDIN";
    }

    printf("Analizando %s\n", filename);
    printf(TEXT_DELIM);
    Lexer *lex = lexer_new(f);
    Parser *p = parser_new(lex);
    p->beforeproc = &beforeproc_func;
    p->afterproc = &afterproc_func;
    parser_parse(p);

    if (p->error != NULL) {
        printf("%s: Error: %s\n", filename, p->error->text);
        printf("%s: Secuencia rechazada\n", filename);
    } else {
        printf("%s: Secuencia aceptada\n", filename);
    }

    printf(TEXT_DELIM);
    parser_destroy_all(p);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        for (char **f = argv + 1; *f != NULL; f++) {
            filename = *f;
            parse_file();
        }
    } else {
        filename = "-";
        parse_file();
    }

    return 0;
}
