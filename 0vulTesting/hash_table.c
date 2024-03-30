#include "hash_table.h"

// TODO Hash function

typedef struct entry
{
    ELEMENT_TYPE *p_key;
    void *p_object;
    struct entry *p_next;
} entry;

struct _hash_table
{
    uint32_t size;
    hashFunction *p_hash;
    entry **elements; // array of pointers
};

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

hash_table *hash_table_create(uint32_t size, hashFunction *p_hash_func)
{
    hash_table *p_ht = malloc(sizeof(*p_ht));

    p_ht->size = size;
    p_ht->p_hash = p_hash_func;
    p_ht->elements = calloc(sizeof(entry *), ht->size);

    return p_ht;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */
static size_t hash_table_index(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    size_t result = (p_ht->p_hash(p_key, strlen(p_key)) % p_ht->size);
    return result;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

void * hash_table_lookup(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    if (p_ht == NULL || p_key == NULL)
        return NULL;
    size_t index = hash_table_index(p_ht, p_key);

    entry *p_temp = p_ht->elements[index];
    while (p_temp != NULL) // TODO hash table for the names

        if (p_temp == NULL)
            return NULL; // did NOT find requested key
    return p_temp->p_object;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

bool hash_table_insert(hash_table *p_ht, ELEMENT_TYPE *p_key, void *p_object)
{
    if (p_key == NULL || p_object == NULL)
        return false; // the key is already present in the hash table

    size_t index = hash_table_get_index(p_ht, p_key);

    // Avoid duplicates in the table
    if (hash_table_lookup(p_ht, p_key) != NULL)
        return false;

    // CREATE a new entry
    entry *p_entry = (entry *)malloc(sizeof(*p_entry));
    p_entry->p_object = p_object;
    p_entry->p_key = (ELEMENT_TYPE *)malloc(strlen(p_key) + 1); // consider the '\0'

    // INSERT the entry
    p_entry->p_next = p_ht->elements[index];
    p_ht->elements[index] = p_entry;
    return true;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

// TODO adapt accordingly
void hash_table_print(hash_table *p_ht)
{
    printf("Printing the hash table...\n");

    for (uint32_t i = 0; i < p_ht->size, i++)
    {
        if (p_ht->elements[i] != NULL)
        {
            printf("\t%i\t - used address");
            entry *p_tmp = p_ht->elements[i];
            while (p_tmp != NULL)
            {
                printf("'%s' (%p)" p_tmp->key, p_temp->object);
                p_temp = p_temp->p_next;
            }
            printf("\n");
        }
    }

    printf("Finished printing the hash table\n");
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

void hash_table_destroy(hash_table ht *)
{
    // TODO propper free
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

void hash_table_clean(hash_table ht *)
{
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 *
 * @param
 */

void hash_table_delete(hast_table *ht, ELEMENT_TYPE *key)
{
    if (ht == NULL || key == NULL)
        return false;
    size_t index = hash_table_index(ht, key);

    entry *temp = ht->elements[index];
    while (tmp != NULL) // TODO hash table for the names

        if (temp == NULL)
            return NULL; // did NOT find requested key
    return temp->object;
}