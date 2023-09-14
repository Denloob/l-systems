#include "stack.h"
#include "utils.h"
#include <stdlib.h>

typedef struct StackNode
{
    void *data;
    struct StackNode *next;
} StackNode;

StackNode *stack_node_create(void *data)
{
    StackNode *node = xmalloc(sizeof(*node));

    node->next = NULL;
    node->data = data;

    return node;
}

void stack_push(StackNode **stack_ptr, void *data)
{
    StackNode *node = stack_node_create(data);

    node->next = *stack_ptr;
    *stack_ptr = node;
}

void *stack_pop(StackNode **stack_ptr)
{
    if (*stack_ptr == NULL)
        return NULL;

    StackNode *node = *stack_ptr;

    *stack_ptr = node->next;
    void *data = node->data;

    free(node);

    return data;
}
