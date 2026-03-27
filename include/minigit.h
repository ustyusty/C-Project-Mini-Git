#ifndef MINIGIT_H
#define MINIGIT_H

#include <time.h>
#include <stdbool.h>
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

Commit* add_file(Commit* old_commit, const char *path, const char *content);
Commit* remove_file(Commit * old_commit, const char *path);

Commit * commit(Commit* staging_commit, const char *msg);
char * get_file_content(Commit* commit, const char *path);
bool get_file_exists(Commit* commit, const char *path);

void print_commit(Commit* staging_commit);
void print_history(Commit* staging_commit);
void print_files(Commit* staging_commit);

#endif