#ifndef MINIGIT_H
#define MINIGIT_H

#include <time.h>
/* 
Cтруктура хранения файла:

`char` *name - название/путь
`char` *content - содержимое
`char` hash[41]

`FileNode` *next
*/
typedef struct FileNode {
    char *name;
    char *content;
    char hash[41];
    struct FileNode *next;
} FileNode;
/*
Cтруктура коммита:

`char` hash[41]
`char` *name - название
`time_t` timestamp - время создания

`FileNode` *files
`Commit` *parent - предыдущий коммит
*/
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
Commit* remove_file(Commit * old_commit, const char *path);

#endif