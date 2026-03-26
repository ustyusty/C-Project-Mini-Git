#include <stdio.h>
#include "../include/minigit.h"
int main(void){
    Commit *first_commit = init_repo();
    Commit *add_file_1_commit = add_file(first_commit, "file1", "stuf1");
    Commit *add_file_2_commit = add_file(add_file_1_commit, "file2", "stuf2");
    Commit *remove_file_1_commit = remove_file(add_file_2_commit, "file3");
    FileNode *cur = remove_file_1_commit->files;
    while (cur != NULL)
    {
        printf("%s\n", cur->name);
        cur = cur->next;
    }
    return 0;
}