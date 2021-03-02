#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

#define LEXER_BUFFER_SET 0
#define LEXER_BUFFER_END 1
#define LEXER_BUFFER_CUR 2

typedef struct {
    size_t size;
    char *data;
    char *bend;
    char *bptr;
} LexerBuffer;

LexerBuffer *lexer_buffer_new(size_t size);
void lexer_buffer_destroy(LexerBuffer *buf);
int lexer_buffer_is_start(LexerBuffer *buf);
int lexer_buffer_is_end(LexerBuffer *buf);
char lexer_buffer_advance(LexerBuffer *buf);
void lexer_buffer_unadvance(LexerBuffer *buf);
void lexer_buffer_put(LexerBuffer *buf, char c);
void lexer_buffer_seek(LexerBuffer *buf, size_t offset, int whence);
ptrdiff_t lexer_buffer_offset(LexerBuffer *buf);

#endif
