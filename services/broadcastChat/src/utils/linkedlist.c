
#include "linkedlist.h"

void printlist (node_t *head) {
    node_t *temporary = head;

    // Different from NULL because the last node points to the "NULL pointer"
    while (temporary != NULL) {
        printf("%d - ", temporary->value);
        temporary = temporary->next;
    }
    printf("\n");
}

node_t *create_new_node(int value) {
    node_t *result = malloc(sizeof(node_t));
    result->value = value;

    // "Just for safety"
    result->next = NULL;

    // Sending the address of the new created node 
    return result;
}

// Dereferencing the address of the pointer head to get it (the pointer, NOT the value)
node_t *insert_at_head(node_t **head, node_t *node_to_insert) {
    node_to_insert->next = *head;
    *head = node_to_insert;
    return node_to_insert;
}

void insert_after_node(node_t *node_to_insert_after, node_t* newnode) {
    newnode->next = node_to_insert_after->next;
    node_to_insert_after->next = newnode;
    // reversing that order would make us lose the pointer
    // to the last the tail in our 
    // (and it would create a loop)
}

node_t *find_node(node_t *head, int value) {
    node_t *tmp = head;
    while (tmp != NULL) {
        if(tmp->value == value) return tmp;
        tmp = tmp->next;
    }

    return NULL;
}