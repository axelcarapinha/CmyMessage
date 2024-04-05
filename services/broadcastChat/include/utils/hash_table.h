#ifndef HASH_TABLE_H // based on a Linked List
#define HASH_TABLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "clients.h"

#define ELEMENT_TYPE const char // RECOMMENDED (does not involve changing code)
#define OBJECT_TYPE ClientInfo_t


//TODO use dynamic libraries to avoid circular dependencies
//TODO and be able to use values defined in the broadcat chat service
//! HARDCODED 

typedef struct _hash_table hash_table; // hide details from the implemetation for the user
typedef uint32_t(hashFunc)(ELEMENT_TYPE * p_key); // prepared for murmur3_32 hash
typedef void cleanObjFunc(void **); // TODO consider removing the typedef
// A pointer to a pointer allows to set the first pointed one to be NULL,
// in order to mark it as freed

typedef struct entry
{
    ELEMENT_TYPE *p_key;
    void *p_object;
    struct entry *p_next;
} entry;

struct _hash_table
{
    uint32_t size;
    entry **elements; // array of pointers

    // Functions
    hashFunc *p_hash_func;
    cleanObjFunc *p_cleanup_func;
};

// Function prototypes
hash_table *hash_table_create(uint32_t size, hashFunc *p_hash_func, cleanObjFunc p_clean_obj_func);
void *hash_table_lookup(hash_table *p_ht, ELEMENT_TYPE *p_key);
bool hash_table_insert(hash_table *p_ht, ELEMENT_TYPE *p_key, OBJECT_TYPE *p_object);
void *hash_table_print(hash_table *p_ht);
void hash_table_clean_fully(hash_table *p_ht);
bool hash_table_delete_element(hash_table *p_ht, ELEMENT_TYPE *p_key);
void hash_table_destroy_with_ptr_to_ptr(hash_table **p_p_ht);
static size_t hash_table_calc_index_with_hash(hash_table *p_ht, ELEMENT_TYPE *p_key);
static void hash_table_free_element(hash_table *p_ht, entry *p_entry);
uint32_t murmur3_32_hash(ELEMENT_TYPE *p_key);

#endif