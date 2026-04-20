#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/minigit.h"
int main(int argc, char* argv[]){

    Commit *head = load_repo();
    if (argc < 2) {
        printf("Usage: minigit <command> [<args>]\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        head = init_repo();
        save_commit(head);
    }

    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: minigit add <file>\n");
        } else {
            char* content = read_file_from_disk(argv[2]);
            if(content){
                head = add_file(head, argv[2], content);
                free(content);
                printf("File %s staged\n", argv[2]);
            } else {
                printf("Error: cannot read file %s\n", argv[2]);
            }
        }
    }

    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: minigit commit <message>\n");
        } else {
            head = commit(head, argv[2]);
            save_commit(head);
            printf("Changes committed with hash: %s\n", head->hash);
        }
    }
    else if (strcmp(argv[1], "log") == 0) {
        print_history(head);
    }
    return 0;
}