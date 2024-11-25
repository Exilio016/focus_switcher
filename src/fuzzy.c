#include "bfutils_vector.h"
#include "fuzzy.h"
#include <string.h>
#include <stdio.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int levenshtein_distance(const char *a, const char *b) {
    int *v0 = NULL;
    int *v1 = NULL;
    vector_ensure_capacity(v0, strlen(b) + 1);
    vector_ensure_capacity(v1, strlen(b) + 1);

    for (int i = 0; i <= strlen(b); i++) {
        v0[i] = i;
    }

    for (int i = 0; i < strlen(a); i++) {
        v1[0] = i + 1;
        for (int j = 0; j < strlen(b) ; j++) {
            int deletionCost = v0[j+1] + 1;
            int insertionCost = v1[j] + 1;
            int substititutionCost = v0[j] + 1;
            if (a[i] == b[j]) {
                substititutionCost = v0[j];
            }
            v1[j+1] = MIN(deletionCost, MIN(insertionCost, substititutionCost));
        }
        int *t = v0;
        v0 = v1;
        v1 = t;
    }
    int res = v0[strlen(b)];

    vector_free(v0);
    vector_free(v1);
    return res;
}

static const char *s;
int windowcmp (const void *a, const void *b) {
    WmWindow *wa = (WmWindow*) a;
    WmWindow *wb = (WmWindow*) b;

    int wa_dist = levenshtein_distance(s, wa->title);
    int wb_dist = levenshtein_distance(s, wb->title);
    return wa_dist - wb_dist;
}

void search_window(WmWindow *w, const char* search_text) {
    if (search_text != NULL && strlen(search_text) > 0) {
        s = search_text;
        qsort(w, vector_length(w), sizeof(WmWindow), windowcmp);
    }
}
