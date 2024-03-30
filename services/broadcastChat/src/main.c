#include "service_utils.h"
#include "broadcast_chat_service.h"


//TODO change
uint16_t simple_hash(const char *str) {
    uint32_t hash = 0;
    while (*str) {
        hash += *str;
        str++;
    }
    return hash;
}

int main(int argc, char *argv[])
{
    printf(YELLOW "Powering up the service %s!\n" RESET, SERVICE_NAME);

    // Broadcasting service
    // int exit_status;
    // if ((exit_status = start_service(DEFAULT_PORT, prepare_client_for_broadcast_and_start)) < 0) {
    //     perror("Failed to start the broadcasting service");
    // }
    


    //!
    //! Testing the hash table 
    //!

    //! Data to create and insert
    uint32_t size = __FD_SETSIZE;

    hash_table *p_usernames_hash_table = hash_table_create(size, simple_hash, NULL);

    const char *p_key1 = "Axek";
    int *p_value1 = malloc(sizeof(int));
    *p_value1 = 1;
    hash_table_insert(p_usernames_hash_table, p_key1, p_value1);

    const char *p_key2 = "User 2";
    int *p_value2 = malloc(sizeof(int));
    *p_value2 = 2;
    hash_table_insert(p_usernames_hash_table, p_key2, p_value2);

    const char *p_key3 = "esadfiasndfouasbd+fyasvdfivsapdfasd";
    int *p_value3 = malloc(sizeof(int));
    *p_value3 = 3;
    hash_table_insert(p_usernames_hash_table, p_key3, p_value3);

    //! Inserting, deleting and printing
    // puts("------");
    // hash_table_print(p_usernames_hash_table);
    // hash_table_delete_element(p_usernames_hash_table, p_key1);
    // puts("------");
    // hash_table_print(p_usernames_hash_table);

    //! LookUp
    // int *testing = (int *)hash_table_lookup(p_usernames_hash_table, p_key3);
    // if (testing == NULL) {
    //     puts("Element not found");
    // }
    // else {
    //     printf("testing val = %d\n", *testing);
    // }

    //! Getting index
    // int val1 = hash_table_calc_index_with_hash(p_usernames_hash_table, p_key1);
    // int val2 = hash_table_calc_index_with_hash(p_usernames_hash_table, p_key2);
    // printf("index of FIRST = %d\n", val1);
    // printf("index of two = %d\n", val2);
 
    // Clean the table and print again (or destroy the hash table)
    hash_table_print(p_usernames_hash_table);
    puts("-------------");
    // hash_table_clean_fully(p_usernames_hash_table);
    hash_table_destroy_with_ptr_to_ptr(&p_usernames_hash_table);
    // p_usernames_hash_table = NULL;
    void * result;
    if ((result = hash_table_print(p_usernames_hash_table)) == NULL)
        puts("Invalid pointer detected by the function");





    
    // Connect to the database
    //TODO

    return 0;
}