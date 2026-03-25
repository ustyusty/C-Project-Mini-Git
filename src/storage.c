#include <stdio.h>
#include <string.h>
#include "../include/minigit.h"

void compute_hash(const char *data, char *out_hash){
    unsigned long hash = 5381;
    int c;

    while ((c = *data++)) {
        hash = ((hash << 5) + hash) + c; 
    }

    sprintf(out_hash, "%08lx", hash);
}