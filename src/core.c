#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/minigit.h"
#include "../include/logging.h"

/**
 * @brief Инициализирует репозиторий, создавая начальный пустой коммит.
 * * @return `Commit*` Указатель на выделенный корневой коммит.
 */
Commit* init_repo() {
    Commit *root = (Commit*)malloc(sizeof(Commit));
    if (!root) { 
        LOG(ERROR, "Failed to allocate memory for root commit"); 
        return NULL; 
    }

    root->name = strdup("Initial commit");
    root->timestamp = time(NULL);
    root->parent = NULL;
    root->files = NULL;
    
    compute_hash(root->name, root->hash);
    LOG(INFO, "Initialized repository. Root hash: %s", root->hash); 
    return root;
}

/**
 * @brief Добавляет или обновляет файл, создавая новую версию репозитория.
 * Реализует структурное разделение: неизмененные файлы копируются "по ссылке".
 * * @param old_commit Текущее состояние.
 * @param path Путь к файлу.
 * @param content Новое содержимое.
 * @return `Commit*` Новый промежуточный коммит (staging).
 */
Commit* add_file(Commit* old_commit, const char *path, const char *content) {
    if (!old_commit || !path || !content) return NULL;

    Commit* new_commit = (Commit*)malloc(sizeof(Commit));
    if (!new_commit) { LOG(ERROR, "Memory allocation failed for commit"); return NULL; }
    
    // Новое состояние ссылается на старое как на родителя
    new_commit->parent = old_commit;
    new_commit->name = NULL;
    new_commit->timestamp = time(NULL);
    new_commit->files = NULL;

    FileNode *curr = old_commit->files;
    FileNode *new_list_head = NULL;
    FileNode *last_node = NULL;
    bool found = false;

    // Проходим по списку файлов старого коммита
    while (curr) {
        FileNode *node_to_add;
        if (strcmp(curr->name, path) == 0) {
            // Файл найден — создаем обновленный узел
            node_to_add = (FileNode*)malloc(sizeof(FileNode));
            node_to_add->name = strdup(path);
            node_to_add->content = strdup(content);
            compute_hash(content, node_to_add->hash); // Хешируем контент 
            node_to_add->next = NULL;
            found = true;
            LOG(DEBUG, "Updating existing file: %s", path);
        } else {
            node_to_add = (FileNode*)malloc(sizeof(FileNode));
            node_to_add->name = curr->name;       // Ссылка на ту же строку
            node_to_add->content = curr->content; // Ссылка на тот же контент
            memcpy(node_to_add->hash, curr->hash, sizeof(curr->hash));
            node_to_add->next = NULL;
            LOG(DEBUG, "Sharing unchanged file: %s", curr->name);
        }

        if (!new_list_head) new_list_head = node_to_add;
        else last_node->next = node_to_add;
        last_node = node_to_add;
        
        curr = curr->next;
    }

    // Если файла не было в списке, добавляем его в конец
    if (!found) {
        FileNode *new_file = (FileNode*)malloc(sizeof(FileNode));
        new_file->name = strdup(path);
        new_file->content = strdup(content);
        compute_hash(content, new_file->hash);
        new_file->next = NULL;
        
        if (!new_list_head) new_list_head = new_file;
        else last_node->next = new_file;
        LOG(INFO, "Added new file to repository: %s", path);
    }

    new_commit->files = new_list_head;
    return new_commit;
}

/**
 * @brief Удаляет файл из новой версии репозитория.
 * * @return `Commit*` Новый коммит без указанного файла или старый, если файл не найден.
 */
Commit * remove_file(Commit* old_commit, const char *path) {
    if (!old_commit) return NULL;

    LOG(DEBUG, "Attempting to remove file: %s", path);

    Commit* new_commit = (Commit*)malloc(sizeof(Commit));
    if (!new_commit) { LOG(ERROR, "Memory allocation failed for commit"); return NULL; }
    
    new_commit->parent = old_commit;
    new_commit->name = NULL;
    new_commit->timestamp = time(NULL);
    new_commit->files = NULL;

    FileNode *new_list_head = NULL;
    FileNode *last_node = NULL;
    FileNode *curr = old_commit->files;

    while (curr) {
        if (strcmp(curr->name, path) != 0) {
            FileNode *node_to_add = (FileNode*)malloc(sizeof(FileNode));
            node_to_add->name = curr->name;
            node_to_add->content = curr->content;
            memcpy(node_to_add->hash, curr->hash, sizeof(curr->hash));
            node_to_add->next = NULL;

            if (!new_list_head) new_list_head = node_to_add;
            else last_node->next = node_to_add;
            last_node = node_to_add;
        } else {
            LOG(DEBUG, "Removing file: %s", path);
        }
        curr = curr->next;
    }

    new_commit->files = new_list_head;
    LOG(INFO, "File %s removed in new commit state", path);
    return new_commit; 
}

/**
 * @brief Фиксирует изменения, превращая staging в финальный коммит.
 * * @param staging_commit Промежуточный коммит.
 * @param msg Сообщение коммита. 
 * @return `Commit*` Финализированный коммит с хешем.
 */
Commit* commit(Commit* staging_head, const char *msg) {
    Commit* new_commit = malloc(sizeof(Commit));
    if (!new_commit) return NULL;
    memset(new_commit, 0, sizeof(Commit));

    new_commit->parent = staging_head;

    new_commit->files = staging_head ? staging_head->files : NULL;

    new_commit->name = strdup(msg);
    new_commit->timestamp = time(NULL);
    
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s%ld%s", 
             msg, 
             (long)new_commit->timestamp, 
             new_commit->parent ? new_commit->parent->hash : "root");
    
    compute_hash(buffer, new_commit->hash);

    LOG(INFO, "New commit created: [%s] -> Parent: [%s]", 
        new_commit->hash, 
        new_commit->parent ? new_commit->parent->hash : "None");

    return new_commit;
}
/**
 * @brief Выводит историю коммитов от текущего до самого первого.
 */
void print_history(Commit* current) {
    LOG(DEBUG, "Printing history log...");
    while (current != NULL) {
        print_commit(current);
        current = current->parent;
    }
}