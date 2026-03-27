#include <stdio.h>
#include "../include/minigit.h"
int main(void){
    Commit *first_commit = init_repo();
    Commit *commit = add_file(first_commit, "file1", "stuf1");
    commit = add_file(commit, "file2", "stuf2");
    commit = remove_file(commit, "file2");
    FileNode *cur = commit->files;
    while (cur != NULL)
    {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
    return 0;
}