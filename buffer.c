#include "buffer.h"
#include "cdefs.h"
#include "cmem.h"

LexerBuffer *lexer_buffer_new(size_t size)
{
    LexerBuffer *buf = cmalloc(sizeof(*buf));
    buf->size = size;
    buf->data = ccalloc(size, 1);
    buf->bend = buf->data + size;
    buf->bptr = buf->data;
    return buf;
}

void lexer_buffer_destroy(LexerBuffer *buf)
{
    cfree(buf->data);
    cfree(buf);
}

int lexer_buffer_is_start(LexerBuffer *buf)
{
    return buf->bptr == buf->data;
}

int lexer_buffer_is_end(LexerBuffer *buf)
{
    return buf->bptr == buf->bend;
}

char lexer_buffer_advance(LexerBuffer *buf)
{
    return *buf->bptr++;
}

void lexer_buffer_unadvance(LexerBuffer *buf)
{
    --buf->bptr;
}

void lexer_buffer_put(LexerBuffer *buf, char c)
{
    if (lexer_buffer_is_end(buf)) {
        ptrdiff_t d = buf->bptr - buf->data;
        buf->size *= 2;
        buf->data = crealloc(buf->data, buf->size);
        buf->bend = buf->data + buf->size;
        buf->bptr = buf->data + d;
    }

    *buf->bptr++ = c;
}

void lexer_buffer_seek(LexerBuffer *buf, size_t offset, int whence)
{
    if (whence == LEXER_BUFFER_END) {
        buf->bptr = buf->bend + offset;
    } else if (whence == LEXER_BUFFER_CUR) {
        buf->bptr += offset;
    } else {
        buf->bptr = buf->data + offset;
    }
}

ptrdiff_t lexer_buffer_offset(LexerBuffer *buf)
{
    return buf->bptr - buf->data;
}
