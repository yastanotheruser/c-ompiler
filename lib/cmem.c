#include "cmem.h"
#include <stdio.h>

static void _memalloc_fatal()
{
    fputs("FATAL: Failed to allocate memory.", stderr);
    exit(EXIT_FAILURE);
}

void *cmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        _memalloc_fatal();
    }

    return ptr;
}

void cfree(void *ptr)
{
    free(ptr);
}

void *ccalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        _memalloc_fatal();
    }

    return ptr;
}

void *crealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
        _memalloc_fatal();
    }

    return ptr;
}
