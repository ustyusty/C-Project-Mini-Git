#include <stdio.h>
#include "../include/minigit.h"
int main(void){
    Commit *first_commit = init_repo();
    Commit *new_commit = add_file(first_commit, "new_file", "pipapopa");
    printf("%s\n", first_commit->hash);
    printf("%s\n", new_commit->hash);
    return 0;
}