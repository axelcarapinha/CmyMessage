#include "hash_table.h"

void hash_table_clean_fully(hash_table *p_ht)
{
    for (uint32_t i = 0; i < p_ht->size; i++)
    {
        while (p_ht->elements[i])
        {
            entry *p_temp = p_ht->elements[i];
            p_ht->elements[i] = p_ht->elements[i]->p_next;
            hash_table_free_element_with_ptr_to_ptr_of_entry(p_ht, &p_temp);
        }
    }
}

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

static size_t hash_table_calc_index_with_hash(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    size_t result = (p_ht->p_hash_func(p_key) % p_ht->size); // TODO check other ways of doing this
    return result;
}

//TODO discover why sometimes this does not find the username
bool hash_table_delete_element(hash_table *p_ht, ELEMENT_TYPE *p_key)
{
    // Table does not exist, or an invalid pointer was passed
    if (p_ht == NULL || p_key == NULL) {
        return false;
    }
    size_t index = hash_table_calc_index_with_hash(p_ht, p_key);

    // Traverse the TARGET linkedList to find the ELEMENT
    entry *p_temp = p_ht->elements[index];
    entry *p_prev = NULL;
    while (p_temp != NULL && strcmp(p_temp->p_key, p_key) != 0)
    {
        p_prev = p_temp;
        p_temp = p_temp->p_next;
    }


    // Element NOT found
    if (p_temp == NULL) {
        return false;
    }

    // DELETE...
    if (p_prev == NULL) {
        p_ht->elements[index] = p_temp->p_next; // ...the HEAD of the list
    }
    else {
        p_prev->p_next = p_temp->p_next; // ...other than the head
    }

    // Return the deleted content of the ELEMENT
    // hash_table_free_element_with_ptr_to_ptr_of_entry(p_ht, &p_temp); 

    return true;
}

static void hash_table_free_element_with_ptr_to_ptr_of_entry(hash_table *p_ht, entry **p_p_entry) {
    if (*p_p_entry == NULL) {
        return;  
    }

    if ((*p_p_entry)->p_key != NULL) {
        free((*p_p_entry)->p_key);
        (*p_p_entry)->p_key = NULL; 
    }

    if ((*p_p_entry)->p_object != NULL) {
        p_ht->p_cleanup_func(&(*p_p_entry)->p_object);
    }

    free(*p_p_entry);
    *p_p_entry = NULL;
}

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

bool hash_table_insert(hash_table *p_ht, ELEMENT_TYPE *p_key, OBJECT_TYPE *p_object)
{
    if (p_key == NULL || p_object == NULL) {
        return false; // the key is already present in the hash table
    }

    //TODO Sanitize the input
    // p_key[strlen(p_key)] = '\0';

    size_t index = hash_table_calc_index_with_hash(p_ht, p_key);

    // Avoid duplicates in the table
    if (hash_table_lookup(p_ht, p_key) != NULL) {
        return false;
    }

    // CREATE a new entry
    entry *p_entry = (entry *)malloc(sizeof(*p_entry));
    p_entry->p_object = p_object;
    p_entry->p_key = strdup(p_key);

    // INSERT the entry
    p_entry->p_next = p_ht->elements[index];
    p_ht->elements[index] = p_entry;
    return true;
}

void * hash_table_print(hash_table *p_ht)
{
    // Check if the hash table exists
    if (p_ht == NULL) {
        ERROR_VERBOSE_LOG("The provided hash table pointer is invalid. Probably the hash table does not exist");
        return NULL;
    }

    INFO_VERBOSE_LOG("Printing the hash table...\n");
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
    INFO_VERBOSE_LOG("Finished printing the hash table\n");
}

uint32_t murmur3_32_hash(ELEMENT_TYPE *p_key)
{
    // Allow a considerable amount of bytes for the username
    // being cautious because of computational power needed
    // even though this hash is efficient
    uint32_t len = MAX_USERNAME_LENGTH; 

    // MurmurHash3 algorithm values
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;

    // Fixed value, unlike with passwords
    uint32_t hash = USERNAMES_HASH_SEED; 

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

hash_table *hash_table_create(uint32_t size, hashFunc *p_hash_func, cleanObjFunc p_clean_obj_func)
{
    hash_table *p_ht = malloc(sizeof(*p_ht));
    if (p_ht == NULL) {
        ERROR_VERBOSE_LOG("Error allocating memory for the hash table failed");
        return NULL;
    }

    p_ht->size = size;
    //
    p_ht->p_hash_func = p_hash_func;
    p_ht->p_cleanup_func = (p_clean_obj_func) ? p_clean_obj_func : free; // free as the default cleanup function
    //
    p_ht->elements = calloc(sizeof(entry *), p_ht->size);

    return p_ht;
}