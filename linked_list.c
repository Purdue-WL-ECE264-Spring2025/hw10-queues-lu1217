#include "linked_list.h"
#include <stdlib.h>

// Creates a new node with given value
struct list_node *new_node(size_t value) {
    struct list_node *node = malloc(sizeof(struct list_node));
    if (!node) return NULL;
    
    node->value = value;
    node->next = NULL;
    return node;
}

// Adds new node at beginning of list
void insert_at_head(struct linked_list *list, size_t value) {
    struct list_node *node = new_node(value);
    if (!node) return;
    
    node->next = list->head;
    list->head = node;
}

// Adds new node at end of list
void insert_at_tail(struct linked_list *list, size_t value) {
    struct list_node *node = new_node(value);
    if (!node) return;
    
    if (!list->head) {
        list->head = node;
    } else {
        struct list_node *current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = node;
    }
}

// Removes and returns first node's value
size_t remove_from_head(struct linked_list *list) {
    if (!list->head) return 0;
    
    struct list_node *old_head = list->head;
    size_t value = old_head->value;
    
    list->head = old_head->next;
    free(old_head);
    return value;
}

// Removes and returns last node's value
size_t remove_from_tail(struct linked_list *list) {
    if (!list->head) return 0;
    
    size_t value;
    
    if (!list->head->next) {
        value = list->head->value;
        free(list->head);
        list->head = NULL;
    } else {
        struct list_node *current = list->head;
        while (current->next->next) {
            current = current->next;
        }
        value = current->next->value;
        free(current->next);
        current->next = NULL;
    }
    
    return value;
}

// Frees all nodes in the list
void free_list(struct linked_list list) {
    struct list_node *current = list.head;
    while (current) {
        struct list_node *next = current->next;
        free(current);
        current = next;
    }
}
