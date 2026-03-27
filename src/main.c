#include <stdio.h>
#include <time.h>
#include "../include/minigit.h"
int main(void){
    Commit *current_commit = init_repo();
    current_commit = add_file(current_commit, "hello.txt", "world");
    current_commit = commit(current_commit, "add hello.txt");
    current_commit = add_file(current_commit, "world.txt", "hello");
    current_commit = commit(current_commit, "add world.txt");
    print_history(current_commit);
    return 0;
}