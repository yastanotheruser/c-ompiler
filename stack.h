#ifndef STACK_H
#define STACK_H

#include <stddef.h>

#define PARSER_STACK_MAX 256

typedef struct {
    int data[PARSER_STACK_MAX];
    size_t size;
} ParserStack;

ParserStack *parser_stack_new();
void parser_stack_destroy(ParserStack *stack);
int *parser_stack_top(ParserStack *stack);
int parser_stack_push(ParserStack *stack, int item);
int parser_stack_pop(ParserStack *stack, int *dest);

#endif
