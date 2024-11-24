#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"  // Для text_to_cstring
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(count_unique_words);

static char *to_lowercase(const char *str) {
    size_t len = strlen(str);
    char *lower = (char *) palloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        lower[i] = tolower((unsigned char)str[i]);
    }
    lower[len] = '\0';
    return lower;
}

static unsigned int hash_function(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + (unsigned char)*str++;
    }
    return hash;
}

Datum count_unique_words(PG_FUNCTION_ARGS) {
    text *input;
    char *input_str;
    char *token;
    char *saveptr;
    unsigned int *hash_table;
    size_t hash_table_size;
    size_t unique_count;

    if (PG_ARGISNULL(0)) {
        PG_RETURN_INT32(0);
    }

    input = PG_GETARG_TEXT_PP(0);
    input_str = text_to_cstring(input);

    if (strlen(input_str) == 0) {
        PG_RETURN_INT32(0);
    }

    hash_table = (unsigned int *)malloc(1024 * sizeof(unsigned int));
    if (!hash_table) {
        ereport(ERROR, (errmsg("Memory allocation failed")));
    }

    hash_table_size = 1024;
    unique_count = 0;
    memset(hash_table, 0, hash_table_size * sizeof(unsigned int));

    for (token = strtok_r(input_str, " ", &saveptr); token != NULL; token = strtok_r(NULL, " ", &saveptr)) {
        char *word_lower = to_lowercase(token);
        unsigned int hash = hash_function(word_lower);
        size_t index = hash % hash_table_size;

        if (hash_table[index] == 0) {
            hash_table[index] = hash;
            unique_count++;
        } else if (hash_table[index] != hash) {
            ereport(WARNING, (errmsg("Hash collision detected")));
        }

        pfree(word_lower);
    }

    free(hash_table);
    PG_RETURN_INT32((int32)unique_count);
}
