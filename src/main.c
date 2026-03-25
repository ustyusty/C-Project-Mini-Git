#include <stdio.h>
#include "../include/minigit.h"
int main(void){
    Commit *first_commit = init_repo();
    printf("%s\n", first_commit->hash);
    return 0;
}