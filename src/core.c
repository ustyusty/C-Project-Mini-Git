#include <stdlib.h>
#include <string.h>
#include "../include/minigit.h"

Commit* init_repo() {

    Commit *root = (Commit*)malloc(sizeof(Commit));
    if (!root) return NULL;

    root->name = strdup("Initial commit");
    root->timestamp = time(NULL);
    root->parent = NULL;
    root->files = NULL;

    compute_hash(root->name, root->hash);

    return root;
}