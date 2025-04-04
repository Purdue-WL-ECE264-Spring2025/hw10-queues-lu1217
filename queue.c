#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>
#include <string.h> // For memcmp

// Helper structure to track moves
typedef struct {
    struct game_state state;
    int moves;
} queue_item;

void enqueue(struct queue *q, struct game_state state, int moves) {
    if (!q) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    // Store both state and moves in the node
    new_node->value = serialize(state);
    new_node->moves = moves;
    new_node->next = NULL;

    if (!q->data.head) {
        q->data.head = new_node;
    } else {
        struct list_node *current = q->data.head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
}

struct game_state dequeue(struct queue *q, int *moves) {
    if (!q || !q->data.head) {
        return (struct game_state){0};
    }

    struct list_node *front = q->data.head;
    struct game_state state = deserialize(front->value);
    *moves = front->moves;
    q->data.head = front->next;
    free(front);
    
    return state;
}

int number_of_moves(struct game_state start) {
    struct queue q = { .data = { .head = NULL } };
    int visited[1 << 16] = {0}; // Visited tracking
    
    const uint8_t solved[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 0}
    };
    
    enqueue(&q, start, 0);
    visited[serialize(start)] = 1;
    
    while (q.data.head) {
        int current_moves;
        struct game_state current = dequeue(&q, &current_moves);
        
        // Check if solved
        if (memcmp(current.tiles, solved, sizeof(solved)) == 0) {
            // Clean up queue
            while (q.data.head) {
                int dummy;
                dequeue(&q, &dummy);
            }
            return current_moves;
        }
        
        // Generate moves
        int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int i = 0; i < 4; i++) {
            int new_r = current.empty_row + directions[i][0];
            int new_c = current.empty_col + directions[i][1];
            
            if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                struct game_state next = current;
                // Swap tiles
                uint8_t temp = next.tiles[current.empty_row][current.empty_col];
                next.tiles[current.empty_row][current.empty_col] = next.tiles[new_r][new_c];
                next.tiles[new_r][new_c] = temp;
                
                next.empty_row = new_r;
                next.empty_col = new_c;
                
                size_t hash = serialize(next);
                if (!visited[hash]) {
                    visited[hash] = 1;
                    enqueue(&q, next, current_moves + 1);
                }
            }
        }
    }
    
    return -1; // No solution
}
