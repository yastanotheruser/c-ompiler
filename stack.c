#include "stack.h"
#include "cdefs.h"
#include "cmem.h"

ParserStack *parser_stack_new()
{
    ParserStack *stack = cmalloc(sizeof(*stack));
    stack->size = 0;
    return stack;
}

void parser_stack_destroy(ParserStack *stack)
{
    cfree(stack);
}

int *parser_stack_top(ParserStack *stack)
{
    return stack->size > 0 ? &stack->data[stack->size - 1] : NULL;
}

int parser_stack_push(ParserStack *stack, int item)
{
    stack->data[stack->size++] = item;
    return stack->size;
}

int parser_stack_pop(ParserStack *stack, int *dest)
{
    if (stack->size == 0) {
        return FALSE;
    }

    --stack->size;
    if (dest != NULL) {
        *dest = stack->data[stack->size];
    }

    return TRUE;
}
