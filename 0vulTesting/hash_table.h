#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ELEMENT_TYPE const char

typedef uint16_t(hashFunction)(ELEMENT_TYPE);
typedef struct _hash_table hash_table; // hide details from the implemetation for the user

hash_table *hash_table_create(uint32_t size, hashFunction *hf);
    bool hash_table_insert(hash_table *ht, ELEMENT_TYPE *key);
    void hash_table_lookup(hash_table *ht, void *key);
void hash_table_clean(hash_table *ht);
void hash_table_delete(hast_table *ht, ELEMENT_TYPE *key);

#endif