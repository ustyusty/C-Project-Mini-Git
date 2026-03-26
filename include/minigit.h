#ifndef MINIGIT_H
#define MINIGIT_H

#include <time.h>
// структура хранения файла
typedef struct FileNode {
    char *name;
    char *content;
    char hash[41];
    struct FileNode *next;
} FileNode;

// структура коммита
typedef struct Commit{
    char hash[41];
    char *name;
    time_t timestamp;

    struct FileNode *files;
    struct Commit *parent;
} Commit;

Commit* init_repo();
void compute_hash(const char *data, char *out_hash);

FileNode* cpy_files(FileNode *file);
Commit* add_file(Commit* old_commit, const char *path, const char *content);

#endif