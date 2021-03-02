#ifndef LIB_CMEM_H
#define LIB_CMEM_H

#include <stdlib.h>

void *cmalloc(size_t size);
void cfree(void *ptr);
void *ccalloc(size_t nmemb, size_t size);
void *crealloc(void *ptr, size_t size);

#endif
