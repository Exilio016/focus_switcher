#include "bfutils_vector.h"
#include <string.h>
#include <stdio.h>
#include <sys/param.h>

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

