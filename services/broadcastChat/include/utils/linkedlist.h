#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>

struct node
{
    int value;
    struct node* next;
};
typedef struct node node_t;


// Functions
void printlist (node_t *head);
node_t *create_new_node(int value);
node_t *insert_at_head(node_t **head, node_t *node_to_insert);
void insert_after_node(node_t *node_to_insert_after, node_t* newnode);
node_t *find_node(node_t *head, int value);

#endif