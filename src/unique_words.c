#include "postgres.h"           
#include "fmgr.h"               
#include "utils/builtins.h"     
#include <ctype.h>              
#include <stdlib.h>             
#include <string.h>             

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(count_unique_words);

// Прототип хэш-функции
static unsigned int hash_function(const char *str);

// Преобразует строку в нижний регистр
static char *to_lowercase(const char *str) {
    size_t len = strlen(str);
    char *lower = (char *) palloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        lower[i] = tolower((unsigned char)str[i]);
    }
    lower[len] = '\0';
    return lower;
}

// Удаляет пунктуацию из строки
static void clean_word(char *word) {
    char *src = word, *dst = word;
    while (*src) {
        if (isalnum((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

// Проверяет, существует ли слово в таблице
static bool word_exists(const char *word, char **hash_table, size_t hash_table_size) {
    unsigned int hash = hash_function(word);
    size_t index = hash % hash_table_size;

    while (hash_table[index] != NULL) {
        if (strcmp(hash_table[index], word) == 0) {
            return true;
        }
        index = (index + 1) % hash_table_size; // Линейное пробирование
    }
    return false;
}

// Добавляет слово в таблицу
static void add_word(const char *word, char **hash_table, size_t hash_table_size) {
    unsigned int hash = hash_function(word);
    size_t index = hash % hash_table_size;

    while (hash_table[index] != NULL) {
        index = (index + 1) % hash_table_size; // Линейное пробирование
    }
    hash_table[index] = pstrdup(word); // Копируем слово в таблицу
}

// Простая хэш-функция
static unsigned int hash_function(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + (unsigned char)*str++;
    }
    return hash;
}

// Основная функция для подсчета уникальных слов
Datum count_unique_words(PG_FUNCTION_ARGS) {
    text *input;
    char *input_str;
    char *token;
    char *saveptr;
    char **hash_table;
    size_t hash_table_size = 1024;
    size_t unique_count = 0;

    // Проверка на NULL
    if (PG_ARGISNULL(0)) {
        PG_RETURN_INT32(0);
    }

    input = PG_GETARG_TEXT_PP(0);
    input_str = text_to_cstring(input);

    if (strlen(input_str) == 0) {
        PG_RETURN_INT32(0);
    }

    // Инициализация таблицы хэшей
    hash_table = (char **)calloc(hash_table_size, sizeof(char *));
    if (!hash_table) {
        ereport(ERROR, (errmsg("Memory allocation failed")));
    }

    // Разбиение строки на слова
    token = strtok_r(input_str, " ", &saveptr); // Первая итерация
    while (token != NULL) {
        char *word_lower; // Объявляем здесь перед использованием

        clean_word(token); // Убираем пунктуацию
        if (strlen(token) == 0) {
            token = strtok_r(NULL, " ", &saveptr); // Пропускаем пустые слова
            continue;
        }

        word_lower = to_lowercase(token); // Преобразуем в нижний регистр
        if (!word_exists(word_lower, hash_table, hash_table_size)) {
            add_word(word_lower, hash_table, hash_table_size);
            unique_count++;
        }
        pfree(word_lower);

        token = strtok_r(NULL, " ", &saveptr); // Переход к следующему слову
    }

    // Освобождение памяти таблицы
    for (size_t i = 0; i < hash_table_size; i++) {
        if (hash_table[i] != NULL) {
            pfree(hash_table[i]);
        }
    }
    free(hash_table);

    PG_RETURN_INT32((int32)unique_count);
}
