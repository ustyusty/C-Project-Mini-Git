#ifndef MINIGIT_H
#define MINIGIT_H

#include <time.h>
// структура хранкния файла
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

#endif