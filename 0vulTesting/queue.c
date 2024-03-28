#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node
{
    struct node *next;
    int *number;
};
typedef struct node node_t;

// Functions
int *dequeue();
void enqueue(int *number);
bool isEmptyQueue();

node_t *head = NULL;
node_t *tail = NULL;

bool isEmptyQueue()
{
    return head == NULL;
}

void enqueue(int *number)
{
    node_t *new_node = malloc(sizeof(node_t));
    new_node->number = number;
    new_node->next = NULL;
    if (tail == NULL)
    {
        head = new_node;
    }
    else
    {
        tail->next = new_node;
    }
    tail = new_node;
}

int *dequeue()
{
    if (isEmptyQueue())
        return NULL;

    int *result = head->number;

    // Advance to the next memory address
    node_t *temp = head;
    head = head->next;

    if (isEmptyQueue())
        tail = NULL;

    free(temp);
    return result;
}

int main(int argc, char *argv[])
{
    int *number = (int *)malloc(sizeof(int));
    *number = 404;
    enqueue(number);

    printf("Dequeue = %d\n", *dequeue());
    if (isEmptyQueue())
    {
        printf("It's empty\n");
    }

    int *anotherOne = (int *)malloc(sizeof(int));
    *anotherOne = 3;
    enqueue(anotherOne);
    // printf("Dequeue = %d\n", *dequeue());
    if (isEmptyQueue())
    {
        printf("It's empty\n");
    }

    puts("Hello world");
    return 0;
}