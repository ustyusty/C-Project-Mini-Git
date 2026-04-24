#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/minigit.h"
#include "../include/logging.h"
int main(int argc, char* argv[]){

    Commit *head = load_repo();
    Commit *staging = load_staging(head);  // Загружаем staging если это есть
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
        save_head(head);  // Сохраняем HEAD при инициализации
    }

    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: minigit add <file>\n");
        } else {
            char* content = read_file_from_disk(argv[2]);
            if(!content){
                LOG(ERROR, "Error: cannot read file %s\n", argv[2]);
            } else {
                char blob_hash[9];
                compute_hash(content, blob_hash);
                save_blob(content, blob_hash);
                staging = add_file(staging, argv[2], content);
                save_staging(staging);  // Сохраняем staging на диск
                printf("Added %s to staging\n", argv[2]);
                free(content);
            }
        }
    }

    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: minigit commit <message>\n");
        } else {
            Commit *new_head = commit(staging, argv[2]);
            
            if (new_head->parent != NULL) {
                LOG(DEBUG, "Linking new commit %s to parent %s", new_head->hash, new_head->parent->hash);
            } else {
                LOG(DEBUG, "New commit %s has NO parent (is it a root?)", new_head->hash);
            }

            head = new_head;
            save_commit(head);
            save_head(head);  // Сохраняем HEAD после коммита
            clear_staging();  // Очищаем staging после успешного commit
            printf("Changes committed with hash: %s\n", head->hash);
            staging = head;  // Сбрасываем staging на head
        }
    }
    else if (strcmp(argv[1], "log") == 0) {
        print_history(head);
    }
    else if (strcmp(argv[1], "status") == 0) {
        if (staging && staging != head) {
            printf("Staged files:\n");
            print_files(staging);
        } else {
            printf("No staged changes.\n");
        }
    }
    else if (strcmp(argv[1], "help") == 0) {
        printf("minigit - a simple version control system\n\n");
        printf("Available commands:\n");
        printf("  init                    Initialize a new repository\n");
        printf("  add <file>              Add a file to the staging area\n");
        printf("  commit <message>        Commit staged changes with a message\n");
        printf("  log                     Show commit history\n");
        printf("  status                  Show staged changes\n");
        printf("  help                    Show this help message\n");
        printf("\nUsage: minigit <command> [<args>]\n");
    }
    return 0;
}