#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
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

FileNode* cpy_files(FileNode *files) {
    FileNode *old_current = files;
    FileNode *new_head = NULL;
    FileNode *current = new_head;

    while (old_current != NULL) {
        FileNode* new_file = (FileNode*)malloc(sizeof(FileNode));

        new_file->name = old_current->name;
        new_file->content = old_current->content;
        memcpy(new_file->hash, old_current->hash, sizeof(old_current->hash));
        if (new_head == NULL) {
            new_head = new_file;
        } else {
            current->next = new_file;
        }
        new_file->next = NULL;
        current = new_file;
        old_current = old_current->next;
        
    }
    return new_head;
}

Commit* add_file(Commit* old_commit, const char *path, const char *content) {
    Commit * new_commit = (Commit *)malloc(sizeof(Commit));
    new_commit->parent = old_commit;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "add file %s", path);
    new_commit->name = strdup(buffer);
    // new_commit->timestamp = time();
    new_commit->files = cpy_files(old_commit->files);

    FileNode *current_file = new_commit->files;
    while (current_file != NULL){
        if (strcmp(current_file->name, path) == 0){
            break;
        }
        current_file = current_file->next;
    }
    if (current_file == NULL){
        current_file = (FileNode*)malloc(sizeof(FileNode));
        current_file->name = strdup(path);
        current_file->content = strdup(content);

        current_file->next = new_commit->files;
        new_commit->files = current_file;

    } else {
        current_file->content = strdup(content);
    }
    compute_hash(current_file->content, current_file->hash);
    compute_hash(new_commit->name, new_commit->hash);
    return new_commit;
}