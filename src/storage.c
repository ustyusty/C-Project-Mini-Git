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

char* get_file_content(Commit* commit, const char *path){
    if (!commit) return NULL;
    FileNode * current_file = commit->files;
    while(current_file!=NULL){
        if (strcmp(current_file->name, path) == 0){
            return current_file->content;
        }
        current_file = current_file->next;
    }
    return NULL;
}

bool get_file_exists(Commit* commit, const char *path){
    if (!commit) return false;
    FileNode * current_file = commit->files;
    while(current_file!=NULL){
        if (strcmp(current_file->name, path) == 0){
            return true;
        }
        current_file = current_file->next;
    }
    return false;
}
