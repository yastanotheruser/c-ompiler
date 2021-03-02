#include <stdio.h>
#include <string.h>
#include "cdefs.h"
#include "cmem.h"
#include "lexer.h"

int main(int argc, char *argv[]) {
    FILE **files;
    if (argc > 1) {
        int n = argc - 1;
        int did_stdio = FALSE;
        files = ccalloc(n + 1, sizeof(*files));

        for (int i = 0, j = 0; i < n; i++) {
            const char *fname = argv[i + 1];
            FILE *f;

            if (strcmp(fname, "-") == 0) {
                if (did_stdio) {
                    continue;
                }

                f = stdin;
                did_stdio = TRUE;
            } else {
                f = fopen(fname, "rb");
                if (f == NULL) {
                    fprintf(stderr, "c-ompiler: ");
                    perror(fname);
                    continue;
                }
            }

            files[j++] = f;
        }
    } else {
        files = ccalloc(2, sizeof(*files));
        files[0] = stdin;
    }

    for (FILE **f = files; *f != NULL; f++) {
        Lexer *l = lexer_new(*f);
        Token *t;

        while ((t = lexer_next_token(l)) != NULL) {
            printf("type = %d ; text = %s\n", t->type, t->text);
            lexer_token_destroy(t);
        }

        if (l->error != NULL) {
            fprintf(stderr, "%s\n", l->error->text);
        }

        lexer_destroy(l);
    }

    return 0;
}
