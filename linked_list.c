#include "linked_list.h"

#include <stdlib.h>

// Creates a new node with given value
struct list_node *new_node(size_t value) {
    struct list_node *node = malloc(sizeof(struct list_node));
    if (!node) return NULL;  // Handle malloc failure
    node->value = value;
    node->next = NULL;
    return node;
}

// Inserts a new node at the head of the list
void insert_at_head(struct linked_list *list, size_t value) {
    struct list_node *node = new_node(value);
    if (!node) return;  // Allocation failed
    
    node->next = list->head;
    list->head = node;
    
    // If list was empty, tail is also the new node
    if (!list->tail) {
        list->tail = node;
    }
}

// Inserts a new node at the tail of the list
void insert_at_tail(struct linked_list *list, size_t value) {
    struct list_node *node = new_node(value);
    if (!node) return;  // Allocation failed
    
    if (!list->tail) {
        // List is empty
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

// Removes and returns the value at the head of the list
size_t remove_from_head(struct linked_list *list) {
    if (!list->head) return 0;  // List is empty
    
    struct list_node *old_head = list->head;
    size_t value = old_head->value;
    
    list->head = old_head->next;
    free(old_head);
    
    // If we removed the last node, update tail
    if (!list->head) {
        list->tail = NULL;
    }
    
    return value;
}

// Removes and returns the value at the tail of the list
size_t remove_from_tail(struct linked_list *list) {
    if (!list->head) return 0;  // List is empty
    
    size_t value;
    
    if (list->head == list->tail) {
        // Only one node in list
        value = list->head->value;
        free(list->head);
        list->head = list->tail = NULL;
    } else {
        // Find the node before tail
        struct list_node *prev = list->head;
        while (prev->next != list->tail) {
            prev = prev->next;
        }
        
        value = list->tail->value;
        free(list->tail);
        list->tail = prev;
        prev->next = NULL;
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

// Utility function to help you debugging, do not modify
void dump_list(FILE *fp, struct linked_list list) {
  fprintf(fp, "[ ");
  for (struct list_node *cur = list.head; cur != NULL; cur = cur->next) {
    fprintf(fp, "%zu ", cur->value);
  }
  fprintf(fp, "]\n");
}
