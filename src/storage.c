#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

char* read_file_from_disk(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0';
    }
    fclose(f);
    return buffer;
}

void save_blob(const char *content, const char *hash) {
    char path[256];
    sprintf(path, ".minigit/objects/%s", hash);
    
    FILE *f = fopen(path, "wb");
    if (!f) return;
    if (fputs(content, f) == EOF) {
        return;
    }

    fclose(f);
}

void save_commit(struct Commit *c) {
    FILE *f = fopen(".minigit/history.dat", "ab");
    if (!c || !f) return;
    fwrite(c->hash, sizeof(char), 41, f);
    char empty_hash[41] = {0};
    if (c->parent) {
        fwrite(c->parent->hash, sizeof(char), 41, f);
    } else {
        fwrite(empty_hash, sizeof(char), 41, f);
    }
    fwrite(&(c->timestamp), sizeof(time_t), 1, f);
    int name_len = c->name ? strlen(c->name) : 0;
    fwrite(&name_len, sizeof(int), 1, f);
    if (name_len > 0) {
        fwrite(c->name, sizeof(char), name_len, f);
    }

    FileNode *cur = c->files;
    while (cur != NULL) {
        fwrite(cur->hash, sizeof(char), 41, f);

        // Пишем длину имени файла и само имя
        int path_len = strlen(cur->name);
        fwrite(&path_len, sizeof(int), 1, f);
        fwrite(cur->name, sizeof(char), path_len, f);

        cur = cur->next; // Продвигаемся по списку!
    }

    fwrite(empty_hash, sizeof(char), 41, f);
    fclose(f);
}

Commit* load_commit(FILE *f, char *temp_parent_hash) {
    Commit *c = malloc(sizeof(Commit));
    if (!c) return NULL;

    if (fread(c->hash, 1, 41, f) < 41) {
        free(c);
        return NULL;
    }

    fread(temp_parent_hash, 1, 41, f);
    
    c->parent = NULL; 

    fread(&(c->timestamp), sizeof(time_t), 1, f);

    int name_len;
    fread(&name_len, sizeof(int), 1, f);
    if (name_len > 0) {
        c->name = malloc(name_len + 1);
        fread(c->name, sizeof(char), name_len, f);
        c->name[name_len] = '\0';
    } else {
        c->name = NULL;
    }

    c->files = NULL;
    while (1) {
        char file_hash[41];
        if (fread(file_hash, sizeof(char), 41, f) < 41 || file_hash[0] == '\0') {
            break;
        }

        FileNode *fn = malloc(sizeof(FileNode));
        strcpy(fn->hash, file_hash);

        int path_len;
        fread(&path_len, sizeof(int), 1, f);
        fn->name = malloc(path_len + 1);
        fread(fn->name, sizeof(char), path_len, f);
        fn->name[path_len] = '\0';

        fn->next = c->files;
        c->files = fn;
    }

    return c;

}

Commit* all_commits[1000]; 
int total_commits = 0;

Commit* load_repo() {
    FILE *f = fopen(".minigit/history.dat", "rb");
    if (!f) return NULL;

    // Массив для временного хранения хешей родителей
    char temp_hashes[1000][41]; 
    total_commits = 0;

    while (total_commits < 1000) {
        Commit *c = load_commit(f, temp_hashes[total_commits]); 
        if (!c) break;
        all_commits[total_commits++] = c;
    }
    fclose(f);

    // Теперь связываем указатели, используя наш временный список хешей
    for (int i = 0; i < total_commits; i++) {
        // Если хеш родителя не пустой
        if (temp_hashes[i][0] != '\0') {
            for (int j = 0; j < total_commits; j++) {
                if (strcmp(temp_hashes[i], all_commits[j]->hash) == 0) {
                    all_commits[i]->parent = all_commits[j];
                    break;
                }
            }
        }
    }
    return (total_commits > 0) ? all_commits[total_commits - 1] : NULL;
}