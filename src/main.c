#include <stdio.h>
#include <time.h>
#include "../include/minigit.h"
int main(void){
    Commit *current_commit = init_repo();
    current_commit = add_file(current_commit, "hello.txt", "world");
    current_commit = commit(current_commit, "add hello.txt");
    FileNode *cur = current_commit->files;
    struct tm *tm = localtime(&current_commit->timestamp);
    printf("commit %s at %s\n", current_commit->name, asctime(tm));
    while (cur != NULL)
    {
        printf("%s -> %s\n", cur->name, cur->content);
        cur = cur->next;
    }
    return 0;
}