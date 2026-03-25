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

Commit* add_file(Commit * parent, FileNode * file_dir, FileNode* file) {

    Commit *root = (Commit*)malloc(sizeof(Commit));

    root->name = strdup("Initial commit");
    root->timestamp = time(NULL);
    root->parent = NULL;
    root->files = NULL;
    compute_hash(root->name, root->hash);
    return root;

}