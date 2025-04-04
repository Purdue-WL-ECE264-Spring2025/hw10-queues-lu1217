#include "tile_game.h"
#include <stdlib.h>

void enqueue(struct queue *q, struct game_state state) {
    // Serialize the state to an integer
    size_t serialized = serialize(state);
    
    // Create new node
    struct list_node *new_node = malloc(sizeof(struct list_node));
    new_node->value = serialized;
    new_node->next = NULL;
    
    // Add to queue (using only head pointer)
    if (q->data.head == NULL) {
        q->data.head = new_node;
    } else {
        struct list_node *current = q->data.head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

struct game_state dequeue(struct queue *q) {
    if (q->data.head == NULL) {
        return (struct game_state){0};
    }
    
    struct list_node *front = q->data.head;
    size_t serialized = front->value;
    struct game_state state = deserialize(serialized);
    
    q->data.head = front->next;
    free(front);
    
    return state;
}

int number_of_moves(struct game_state start) {
    struct queue q = { .data = { .head = NULL } };
    int visited[1 << 16] = {0};
    
    // Initialize starting state
    struct game_state initial = start;
    // Note: Assuming move count is tracked elsewhere since struct doesn't have moves field
    
    enqueue(&q, initial);
    visited[serialize(initial)] = 1;
    
    while (q.data.head != NULL) {
        struct game_state current = dequeue(&q);
        
        // Check if solved (using serialized value comparison)
        if (serialize(current) == serialize((struct game_state){
            // Solved board configuration
            // Note: This assumes the serialize function can handle this
            .tiles = {
                {1, 2, 3, 4},
                {5, 6, 7, 8},
                {9, 10, 11, 12},
                {13, 14, 15, 0}
            },
            .empty_row = 3,
            .empty_col = 3
        })) {
            // Clean up queue
            while (q.data.head != NULL) {
                struct list_node *next = q.data.head->next;
                free(q.data.head);
                q.data.head = next;
            }
            // Return move count - need to track this separately
            // Since the struct doesn't have moves field, we'll need to:
            // Either: 1) Return a fixed value if we can't track moves
            // Or: 2) Implement move counting differently
            return 0; // Placeholder - needs proper implementation
        }
        
        // Generate possible moves
        int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (int i = 0; i < 4; i++) {
            int new_row = current.empty_row + directions[i][0];
            int new_col = current.empty_col + directions[i][1];
            
            if (new_row >= 0 && new_row < 4 && new_col >= 0 && new_col < 4) {
                // Create new state by swapping tiles
                struct game_state next = current;
                
                // Swap tiles
                uint8_t temp = next.tiles[current.empty_row][current.empty_col];
                next.tiles[current.empty_row][current.empty_col] = next.tiles[new_row][new_col];
                next.tiles[new_row][new_col] = temp;
                
                next.empty_row = new_row;
                next.empty_col = new_col;
                
                // Check if we've seen this state before
                size_t serialized = serialize(next);
                if (!visited[serialized]) {
                    visited[serialized] = 1;
                    enqueue(&q, next);
                }
            }
        }
    }
    
    return -1; // No solution found
}

