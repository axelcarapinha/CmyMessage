#ifndef HASH_TABLE_H // implemented with chaining to handle collisions
#define HASH_TABLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "clients.h"

#define ELEMENT_TYPE const char //! RECOMMENDED (does not involve changing code)
#define OBJECT_TYPE ClientInfo_t

typedef struct _hash_table hash_table;            // underscore to hide details from the implemetation for the user
typedef uint32_t(hashFunc)(ELEMENT_TYPE * p_key); // prepared for murmur3_32 hash
typedef void cleanObjFunc(void **); 
// A pointer to a pointer allows to set the first pointed one to be NULL,
// in order to mark it as freed, avoid freeing memory more than once (and consequent exception)

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


/**
 * @brief Cleans up all elements in the hash table
 *
 * Removes and deallocates all elements in the hash table,
 * freeing the memory associated with them. Also deallocates
 * memory for the elements array itself.
 *
 * @param p_ht Pointer to the hash table structure
 */
void hash_table_clean_fully(hash_table *p_ht);

/**
 * @brief Destroys the hash table and sets its pointer to NULL
 * @param p_p_ht Pointer to the pointer to the hash table
 */
void hash_table_destroy_with_ptr_to_ptr(hash_table **p_p_ht);

/**
 * @brief Calculates the index of the linked list
 * within the hash table's array based on the hash value
 * of the given key, using the modulus operation.
 * 
 * @param p_ht 
 * @param p_key 
 * @return size_t 
 */
static size_t hash_table_calc_index_with_hash(hash_table *p_ht, ELEMENT_TYPE *p_key);

/**
 * @brief Deletes a give element from the hash
 * by calculating its hash value,
 * searching in the Linked List located at that index,
 * and then freeing up the memory
 * 
 * @param p_ht 
 * @param p_key 
 * @return true (if the element was deleted from the hash table)
 * @return false (if the element was NOT found)
 */
bool hash_table_delete_element(hash_table *p_ht, ELEMENT_TYPE *p_key);

/**
 * @brief Frees the memory associated with an 
 * element of the hash table
 * 
 * @param p_ht 
 * @param p_p_entry 
 */
static void hash_table_free_element_with_ptr_to_ptr_of_entry(hash_table *p_ht, entry **p_p_entry);

/**
 * @brief Looks up an element in the hash table by key
 * 
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key to search for
 * @return Pointer to the object associated with the key if found, otherwise NULL
 */
void *hash_table_lookup(hash_table *p_ht, ELEMENT_TYPE *p_key);

/**
 * @brief Inserts a key-value pair into the hash table
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key to be inserted
 * @param p_object Pointer to the object to be inserted
 * @return true if the insertion is successful, 
 * false otherwise (the element is already present)
 */
bool hash_table_insert(hash_table *p_ht, ELEMENT_TYPE *p_key, OBJECT_TYPE *p_object);

/**
 * @brief Prints the contents of the hash table
 *
 * @param p_ht Pointer to the hash table structure
 * @return NULL if the hash table pointer is NULL, otherwise void
 */
void *hash_table_print(hash_table *p_ht);

/**
 * @brief Hash function for the usernames, 
 * faster than the 64 version; 
 * greater likelihood of collision is considered
 * (for now, before further testings) as less relevant
 * 
 * @param p_key 
 * @return uint32_t 
 */
uint32_t murmur3_32_hash(ELEMENT_TYPE *p_key);

/**
 * @brief Creates the hash table
 * with the customized hash and cleanup functions
 * 
 * @param size 
 * @param p_hash_func 
 * @param p_clean_obj_func 
 * @return hash_table* 
 */
hash_table *hash_table_create(uint32_t size, hashFunc *p_hash_func, cleanObjFunc p_clean_obj_func);

#endif