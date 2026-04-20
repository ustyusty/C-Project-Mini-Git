#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/minigit.h"
#include "../include/logging.h"


Commit* init_repo() {

    Commit *root = (Commit*)malloc(sizeof(Commit));
    if (!root) return NULL;

    root->name = strdup("Initial commit");
    root->timestamp = time(NULL);
    root->parent = NULL;
    root->files = NULL;
    
    compute_hash(root->name, root->hash);
    LOG(LOG_INFO, "Initialized empty MiniGit repository with initial commit hash: %s", root->hash);
    return root;
}

Commit* add_file(Commit* old_commit, const char *path, const char *content) {
    Commit* new_commit = (Commit*)malloc(sizeof(Commit));
    new_commit->parent = old_commit->parent ? old_commit->parent: old_commit;
    new_commit->files = old_commit->files;
    new_commit->name = NULL;
    new_commit->timestamp = time(NULL);

    FileNode *new_head = NULL;
    FileNode *prev_file = NULL;
    FileNode *current_file = new_commit->files;
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s%ld", path, (long)new_commit->timestamp);
    while (current_file != NULL){
        if (strcmp(current_file->name, path) == 0){
            FileNode *updated_file = (FileNode*)malloc(sizeof(FileNode));
            updated_file->name = strdup(path);
            updated_file->content = strdup(content);
            updated_file->next = current_file->next;
            if (new_head == NULL) new_head = updated_file;
            else prev_file->next = updated_file;
            break;

        } else {
            FileNode *copy = (FileNode*)malloc(sizeof(FileNode));
            copy->name = current_file->name;
            copy->content = current_file->content;
            memcpy(copy->hash, current_file->hash, sizeof(current_file->hash));
            copy->next = NULL;
            
            if (new_head == NULL) {
                new_head = copy;
                prev_file = copy;

            } else {
                prev_file->next = copy;
                prev_file = copy;
            }
        }
        current_file = current_file->next;
    }

    if (current_file == NULL){
        current_file = (FileNode*)malloc(sizeof(FileNode));
        current_file->name = strdup(path);
        current_file->content = strdup(content);
        current_file->next = new_commit->files;
        new_commit->files = current_file;

    }
    compute_hash(buffer, current_file->hash);
    save_blob(current_file->content, current_file->hash);
    return new_commit;
}

Commit * remove_file(Commit* old_commit, const char *path){
    FileNode *check_file = old_commit->files;
    int find = 0;
    while (check_file != NULL){
        if (strcmp(check_file->name, path) == 0){
            find = 1;
            break;
        }
        check_file = check_file->next;
    }
    if (!find) {
        return old_commit; 
    }

    Commit* new_commit = (Commit*)malloc(sizeof(Commit));
    new_commit->parent = old_commit;
    new_commit->name = NULL;
    new_commit->timestamp = time(NULL);

    
    FileNode *current_file = old_commit->files;
    FileNode *new_head = NULL;
    FileNode *prev_file = NULL;
    while (current_file != NULL){
        if (strcmp(current_file->name, path) == 0){
            if (new_head == NULL) {
                new_head = current_file->next;
            } else {
                prev_file = current_file->next;
            }
            break;
        } else {
            FileNode *copy = (FileNode*)malloc(sizeof(FileNode));
            copy->name = current_file->name;
            copy->content = current_file->content;
            memcpy(copy->hash, current_file->hash, sizeof(current_file->hash));
            copy->next = NULL;
            
            if (new_head == NULL) {
                new_head = copy;
                prev_file = copy;

            } else {
                prev_file->next = copy;
                prev_file = copy;
            }
        }
        current_file = current_file->next;
    }
    new_commit->files = new_head;
    return new_commit;
}

Commit* commit(Commit* staging_commit, const char *msg) {
    if (!staging_commit) return NULL;

    if (staging_commit->name) free(staging_commit->name); 

    staging_commit->name = strdup(msg);
    
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s%ld", msg, (long)staging_commit->timestamp);
    compute_hash(buffer, staging_commit->hash);

    return staging_commit;
}

void print_commit(Commit* staging_commit){
    if (!staging_commit) return;
    struct tm *tm = localtime(&staging_commit->timestamp);  

    printf("\033[33mcommit %s\033[0m\n", staging_commit->hash);

    if (staging_commit->parent!=NULL){
        printf("\033[32mparent: %s\033[0m\n", staging_commit->parent->hash);
    }

    printf("Date: %s\n", asctime(tm));
    printf("    %s\n\n" , staging_commit->name ? staging_commit->name : "(no commit message / staging)");
    print_files(staging_commit);
}

void print_history(Commit* staging_commit){
    Commit* current_comm = staging_commit;
    while (current_comm!=NULL){
        print_commit(current_comm);
        current_comm = current_comm->parent;
    }
}

void print_files(Commit* staging_commit){
    FileNode*current_file = staging_commit->files;
    while (current_file!=NULL){
        printf("  - %s [%s]\n", current_file->name, current_file->hash);
        current_file = current_file->next;
    }
    printf("\n");
    
}