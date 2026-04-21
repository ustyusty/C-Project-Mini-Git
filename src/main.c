#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/minigit.h"
#include "../include/logging.h"
int main(int argc, char* argv[]){

    Commit *head = load_repo();
    if (argc < 2) {
        printf("Usage: minigit <command> [<args>]\n");
        return 1;
    }
    
    if (strcmp(argv[1], "init") == 0) {
        if (head) {
            printf("Repository already initialized.\n");
            return 1;
        }
        head = init_repo();
        save_commit(head);
    }

    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: minigit add <file>\n");
        } else {
            char* content = read_file_from_disk(argv[2]);
            char blob_hash[9];
            compute_hash(content, blob_hash);
            save_blob(content, blob_hash);
            if(content){
                head = add_file(head, argv[2], content);
                free(content);
            } else {
                LOG(ERROR, "Error: cannot read file %s\n", argv[2]);
            }
        }
    }

    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: minigit commit <message>\n");
        } else {
            Commit *new_head = commit(head, argv[2]);
            
            // Проверка связи в памяти
            if (new_head->parent != NULL) {
                LOG(DEBUG, "Linking new commit %s to parent %s", new_head->hash, new_head->parent->hash);
            } else {
                LOG(DEBUG, "New commit %s has NO parent (is it a root?)", new_head->hash);
            }

            head = new_head;
            save_commit(head);
            printf("Changes committed with hash: %s\n", head->hash);
        }
    }
    else if (strcmp(argv[1], "log") == 0) {
        print_history(head);
    }
    return 0;
}