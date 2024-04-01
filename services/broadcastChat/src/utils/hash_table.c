#include "hash_table.h"

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Creates a new hash table
 *
 * Allocates memory for a new hash table with the specified size.
 * Also assigns the provided hash function for calculating indices,
 * and a cleanup function for deallocating objects stored in the table.
 * If no cleanup function is provided, it defaults to 'free'.
 * 
 * If MUST be paid attention to using uncommenting the signaled part
 * of the 'hash_table_free_element' function if the value is a constant
 *
 * @param size The size of the hash table
 * @param p_hash_func Pointer to the hash function
 * @param p_clean_obj_func Pointer to the cleanup function for objects
 * @return Pointer to the newly created hash table
 */


hash_table *hash_table_create(uint32_t size, hashFunc *p_hash_func, cleanObjFunc p_clean_obj_func)
{
    hash_table *p_ht = malloc(sizeof(*p_ht));

    p_ht->size = size;
    //
    p_ht->p_hash_func = p_hash_func;
    p_ht->p_cleanup_func = (p_clean_obj_func) ? p_clean_obj_func : free; // free as the default cleanup function
    //
    p_ht->elements = calloc(sizeof(entry *), p_ht->size);

    return p_ht;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Looks up an element in the hash table by key
 *
 * Searches for an element with the given key in the provided hash table.
 * If either the hash table pointer or the key pointer is NULL, the function
 * returns NULL, indicating an invalid search. Otherwise, it calculates the
 * index for the given key and iterates through the LINKED LIST at that index
 * to find the desired element. If the element is found, its associated object
 * is returned. If the element is not found, NULL is returned.
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key to search for
 * @return Pointer to the object associated with the key if found, otherwise NULL
 */


void *hash_table_lookup(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    // Invalid arguments, impossible to search
    if (p_ht == NULL || p_key == NULL)
        return NULL;
    size_t index = hash_table_calc_index_with_hash(p_ht, p_key);

    // Try to find the desired element
    entry *p_temp = p_ht->elements[index];
    while (p_temp != NULL && strcmp(p_temp->p_key, p_key) != 0) {
        p_temp = p_temp->p_next;
    }

    // The element was not found
    if (p_temp == NULL) {
        return NULL;
    }

    return (OBJECT_TYPE *)p_temp->p_object;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Inserts a key-value pair into the hash table
 *
 * Inserts a new key-value pair into the provided hash table. If either
 * the key or the object is NULL, the function returns false, indicating
 * failure. If the key already exists in the hash table, the function also
 * returns false to AVOID DUPLICATES. Otherwise, a new entry is created
 * with the given key and object, and it is inserted into the hash table.
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key to be inserted
 * @param p_object Pointer to the object to be inserted
 * @return true if the insertion is successful, false otherwise
 */

bool hash_table_insert(hash_table *p_ht, ELEMENT_TYPE *p_key, OBJECT_TYPE *p_object)
{
    if (p_key == NULL || p_object == NULL)
        return false; // the key is already present in the hash table

    size_t index = hash_table_calc_index_with_hash(p_ht, p_key);

    // Avoid duplicates in the table
    if (hash_table_lookup(p_ht, p_key) != NULL)
        return false;

    // CREATE a new entry
    entry *p_entry = (entry *)malloc(sizeof(*p_entry));
    p_entry->p_object = p_object;
    p_entry->p_key = strdup(p_key);

    // INSERT the entry
    p_entry->p_next = p_ht->elements[index];
    p_ht->elements[index] = p_entry;
    return true;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Prints the contents of the hash table
 *
 * Prints the contents of the provided hash table to the standard output.
 * If the hash table pointer is NULL, an error message is printed and NULL
 * is returned. Otherwise, the elements of the hash table are iterated through,
 * and each element's memory address and associated object (key) are printed.
 *
 * @param p_ht Pointer to the hash table structure
 * @return NULL if the hash table pointer is NULL, otherwise void
 */

void * hash_table_print(hash_table *p_ht)
{
    // Check if the hash table exists
    if (p_ht == NULL) {
        perror("The provided hash table pointer is invalid. Probably the hash table does not exist");
        return NULL;
    }

    printf("Printing the hash table...\n");
    for (uint32_t i = 0; i < p_ht->size; i++)
    {
        if (p_ht->elements[i] != NULL)
        {
            // Find the element, if it exists
            entry *p_temp = p_ht->elements[i];
            while (p_temp != NULL)
            {
                //! Change the type of printing considering the type used in the struct
                // Print its memory address and its object
                printf("(%p) -- '%s'", p_temp->p_object, p_temp->p_key); 
                p_temp = p_temp->p_next;
            }
            printf("\n");
        }
    }
    printf("Finished printing the hash table\n");
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Cleans up all elements in the hash table
 *
 * Removes and deallocates all elements in the hash table,
 * freeing the memory associated with them. Also deallocates
 * memory for the elements array itself.
 *
 * @param p_ht Pointer to the hash table structure
 */

void hash_table_clean_fully(hash_table *p_ht)
{
    for (uint32_t i = 0; i < p_ht->size; i++)
    {
        while (p_ht->elements[i])
        {
            entry *p_temp = p_ht->elements[i];
            p_ht->elements[i] = p_ht->elements[i]->p_next;
            hash_table_free_element(p_ht, p_temp);
        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Deletes an element from the hash table
 *
 * Deletes the element associated with the specified key from the hash table,
 * if it exists. If the table or the key pointer is NULL, or if the element
 * is not found, returns NULL.
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key of the element to delete
 * @return Pointer to the deleted element's object, or NULL if not found
 */

void *hash_table_delete_element(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    // Table does not exist, or an invalid pointer was passed
    if (p_ht == NULL || p_key == NULL)
        return false;
    size_t index = hash_table_calc_index_with_hash(p_ht, p_key);

    // Traverse the linkedList to find the ELEMENT
    entry *p_temp = p_ht->elements[index];
    entry *p_prev = NULL;
    while (p_temp != NULL && strcmp(p_temp->p_key, p_key) != 0)
    {
        p_prev = p_temp;
        p_temp = p_temp->p_next;
    }

    // Element NOT found
    if (p_temp == NULL)
        return NULL;

    // DELETE...
    if (p_prev == NULL)
        p_ht->elements[index] = p_temp->p_next; // ...the HEAD of the list
    else
        p_prev->p_next = p_temp->p_next; // ...other than the head

    // Return the deleted content of the ELEMENT
    void *p_result = p_temp->p_object;
    hash_table_free_element(p_ht, p_temp);

    return p_result;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Destroys the hash table and sets its pointer to NULL
 *
 * Frees the memory allocated for the hash table structure and its elements array,
 * as well as cleaning up all elements in the hash table. Sets the pointer to NULL
 * to prevent using a dangling pointer.
 *
 * @param p_p_ht Pointer to the pointer to the hash table
 */

void hash_table_destroy_with_ptr_to_ptr(hash_table **p_p_ht) {
    hash_table *p_ht = *p_p_ht;

    if (p_ht == NULL)
        return;

    // Clean up ALL elements
    hash_table_clean_fully(p_ht);

    // Free the elements ARRAY (indexes of the linked lists)
    free(p_ht->elements);

    // Free the hash table structure ITSELF
    free(p_ht);

    // Signal this pointer for other functions
    // in order to avoid use a dangling pointer
    *p_p_ht = NULL;
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief Calculates the index for a given key in the hash table
 *
 * Uses the provided hash function to calculate the index for a given key
 * within the hash table. The result is the remainder of the hash function
 * value divided by the size of the hash table.
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_key Pointer to the key for which to calculate the index
 * @return The calculated index for the key in the hash table
 */
static size_t hash_table_calc_index_with_hash(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    size_t result = (p_ht->p_hash_func(p_key) % p_ht->size); // TODO ver outras formas
    return result;
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Frees memory associated with a hash table element
 *
 * Deallocates memory associated with the given hash table element.
 * If the element contains an object, it is cleaned up using the provided
 * cleanup function. Memory for the element itself is then deallocated.
 * If MUST be paid attention to using uncommenting the signaled part
 * if the value is a constant
 *
 * @param p_ht Pointer to the hash table structure
 * @param p_entry Pointer to the entry to be freed
 */

static void hash_table_free_element(hash_table *p_ht, entry *p_entry)
{
    //! Uncomment in case the type is not CONSTANT
    // if (p_entry->p_key)
    //     free(p_entry->p_key);

    if (p_entry->p_object)
        p_ht->p_cleanup_func(p_entry->p_object); // it's free by default

    free(p_entry);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 * @param a
 */

// Faster than the 64 version; greater likelihood of collision is considered
// (for now, before further testings) as less relevant
uint32_t murmur3_32_hash(ELEMENT_TYPE *p_key)
{
    // Allow a considerable amount of bytes for the username
    // being cautious because of computational power needed
    // even though this hash is efficient
    uint32_t len = 20; //TODO use MAX_USERNAME_LENGTH with dynamic libraries from broadcast chat service

    // MurmurHash3 algorithm values
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;

    // Fixed value, unlike with passwords
    uint32_t hash = 23; //TODO use USERNAMES_HASH_SEED with dynamic libraries from broadcast chat service

    const int nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *)p_key;
    int i;
    for (i = 0; i < nblocks; i++)
    {
        // Perform mixing operations on the block
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    // Process the parts that do NOT fit into a full block
    const uint8_t *tail = (const uint8_t *)(p_key + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    // Final mixing and adjustments
    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}