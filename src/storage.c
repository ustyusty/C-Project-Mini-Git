#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/minigit.h"
#include "../include/logging.h"


/**
 * @brief Генерирует хеш-строку на основе данных (алгоритм djb2).
 * @param data Строка для хеширования.
 * @param out_hash Буфер для сохранения результата (минимум 9 байт для 8 символов + \0).
 */
void compute_hash(const char *data, char *out_hash){
    uint32_t hash = 5381;
    int c;

    while ((c = *data++)) {
        hash = ((hash << 5) + hash) + c; 
    }

    sprintf(out_hash, "%08x", hash);
    LOG(DEBUG, "Computed hash: %s", out_hash);
}

/**
 * @brief Ищет содержимое файла в конкретном коммите.
 * @return Указатель на строку контента или NULL, если файл не найден.
 */
char* get_file_content(Commit* commit, const char *path){
    if (!commit) return NULL;
    FileNode * current_file = commit->files;
    while(current_file != NULL){
        if (strcmp(current_file->name, path) == 0){
            return current_file->content;
        }
        current_file = current_file->next;
    }
    return NULL;
}

/**
 * @brief Проверяет наличие файла в коммите.
 */
bool get_file_exists(Commit* commit, const char *path){
    if (!commit) return false;
    FileNode * current_file = commit->files;
    while(current_file != NULL){
        if (strcmp(current_file->name, path) == 0){
            return true;
        }
        current_file = current_file->next;
    }
    return false;
}

/**
 * @brief Считывает файл целиком с диска в динамическую память.
 */
char* read_file_from_disk(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        LOG(ERROR, "Failed to open file for reading: %s", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0';
    }
    fclose(f);
    return buffer;
}

/**
 * @brief Сохраняет содержимое файла (blob) в хранилище объектов.
 */
void save_blob(const char *content, const char *hash) {
    char path[256];
    sprintf(path, ".minigit/objects/%s", hash);
    
    FILE *f = fopen(path, "wb");
    if (!f) {
        LOG(ERROR, "Could not save blob to %s", path);
        return;
    }
    
    if (fputs(content, f) == EOF) {
        LOG(ERROR, "Failed writing content to blob %s", hash);
        fclose(f);
        return;
    }

    LOG(DEBUG, "Blob saved: %s", hash);
    fclose(f);
}

/**
 * @brief Сериализует структуру коммита и записывает её в history.dat.
 */
void save_commit(struct Commit *c) {
    FILE *f = fopen(".minigit/history.dat", "ab");
    if (!c || !f) {
        LOG(ERROR, "Failed to open history.dat for writing");
        return;
    }

    // Записываем хеш текущего коммита
    fwrite(c->hash, sizeof(char), 9, f);
    
    // Записываем хеш родителя или пустую строку
    char empty_hash[9] = {0};
    if (c->parent) {
        fwrite(c->parent->hash, sizeof(char), 9, f);
    } else {
        fwrite(empty_hash, sizeof(char), 9, f);
    }

    // Записываем временную метку
    fwrite(&(c->timestamp), sizeof(time_t), 1, f);

    // Записываем длину
    int name_len = c->name ? strlen(c->name) : 0;
    fwrite(&name_len, sizeof(int), 1, f);

    // Записываем имя/сообщение коммита
    if (name_len > 0) fwrite(c->name, sizeof(char), name_len, f);

    // Итерация по списку файлов коммита
    FileNode *cur = c->files;
    while (cur != NULL) {
        fwrite(cur->hash, sizeof(char), 9, f);

        int path_len = strlen(cur->name);
        fwrite(&path_len, sizeof(int), 1, f);
        fwrite(cur->name, sizeof(char), path_len, f);

        cur = cur->next;
    }

    // Маркер конца списка файлов для данного коммита
    fwrite(empty_hash, sizeof(char), 9, f);
    
    LOG(DEBUG, "Commit %s saved to history", c->hash);
    fclose(f);
}

/**
 * @brief Считывает один коммит из файла истории.
 */
Commit* load_commit(FILE *f, char *temp_parent_hash) {
    Commit *c = malloc(sizeof(Commit));
    if (!c) return NULL;

    // Читаем основной хеш
    if (fread(c->hash, 1, 9, f) < 9) {
        free(c);
        return NULL;
    }

    // Временно сохраняем хеш родителя для последующей связки
    fread(temp_parent_hash, 1, 9, f);
    c->parent = NULL;

    // Читаем временную метку
    fread(&(c->timestamp), sizeof(time_t), 1, f);

    // Читаем сообщение коммита
    int name_len;
    fread(&name_len, sizeof(int), 1, f);
    if (name_len > 0) {
        c->name = malloc(name_len + 1);
        fread(c->name, sizeof(char), name_len, f);
        c->name[name_len] = '\0';
    } else {
        c->name = NULL;
    }

    // Читаем список файлов коммита до встречи с пустым хешем
    c->files = NULL;
    while (1) {
        char file_hash[41];
        if (fread(file_hash, sizeof(char), 9, f) < 9 || file_hash[0] == '\0') {
            break;
        }

        FileNode *fn = malloc(sizeof(FileNode));
        strcpy(fn->hash, file_hash);

        int path_len;
        fread(&path_len, sizeof(int), 1, f);
        fn->name = malloc(path_len + 1);
        fread(fn->name, sizeof(char), path_len, f);
        fn->name[path_len] = '\0';

        fn->next = c->files;
        c->files = fn;
    }

    return c;
}


/**
 * @brief Загружает все коммиты из истории и восстанавливает дерево (связи parent).
 */
Commit* load_repo() {
    FILE *f = fopen(".minigit/history.dat", "rb");
    if (!f) {
        LOG(DEBUG, "No history file found.");
        return NULL;
    }

    Commit* all_commits[1000]; 
    int total_commits = 0;
    char temp_hashes[1000][9];
    memset(temp_hashes, 0, sizeof(temp_hashes));

    // 1. Сначала загружаем все коммиты "плоским" списком
    while (total_commits < 1000) {
        Commit *c = load_commit(f, temp_hashes[total_commits]);
        if (!c) break;
        all_commits[total_commits++] = c;
    }
    fclose(f);

    // 2. Связываем указатели родительских коммитов по сохраненным хешам
    for (int i = 0; i < total_commits; i++) {
        if (temp_hashes[i][0] != '\0') {
            bool found = false;
            for (int j = 0; j < total_commits; j++) {
                if (strcmp(temp_hashes[i], all_commits[j]->hash) == 0) {
                    all_commits[i]->parent = all_commits[j];
                    LOG(DEBUG, "Linked commit %s to parent %s", all_commits[i]->hash, all_commits[j]->hash);
                    found = true;
                    break;
                }
            }
            if (!found) {
                LOG(ERROR, "Parent hash %s for commit %s not found in history!", temp_hashes[i], all_commits[i]->hash);
            }
        }
    }

    if (total_commits > 0) {
        Commit* last = all_commits[total_commits - 1];
        LOG(DEBUG, "Loaded repository. Latest commit: %s. Parent: %s", last->hash, last->parent ? last->parent->hash : "None");
        return last;
    }

    return NULL;
}