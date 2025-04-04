#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

void enqueue(struct queue *q, struct game_state state) {
    if (!q) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    new_node->value = serialize(state);
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

struct game_state dequeue(struct queue *q) {
    if (!q || !q->data.head) {
        return (struct game_state){0};
    }

    struct list_node *front = q->data.head;
    struct game_state state = deserialize(front->value);
    q->data.head = front->next;
    free(front);
    
    return state;
}

int number_of_moves(struct game_state start) {
    struct queue q = { .data = { .head = NULL } };
    struct queue move_q = { .data = { .head = NULL } }; // Parallel queue for moves
    int visited[1 << 16] = {0};
    
    // Initialize queues
    enqueue(&q, start);
    enqueue(&move_q, (struct game_state){0}); // Use dummy state to store move count
    
    while (q.data.head) {
        struct game_state current = dequeue(&q);
        struct game_state moves = dequeue(&move_q);
        int current_moves = moves.tiles[0][0]; // Store moves in dummy state
        
        // Check solved state (hardcoded comparison)
        if (current.tiles[0][0] == 1 && current.tiles[0][1] == 2 &&
            current.tiles[0][2] == 3 && current.tiles[0][3] == 4 &&
            current.tiles[1][0] == 5 && current.tiles[1][1] == 6 &&
            current.tiles[1][2] == 7 && current.tiles[1][3] == 8 &&
            current.tiles[2][0] == 9 && current.tiles[2][1] == 10 &&
            current.tiles[2][2] == 11 && current.tiles[2][3] == 12 &&
            current.tiles[3][0] == 13 && current.tiles[3][1] == 14 &&
            current.tiles[3][2] == 15 && current.tiles[3][3] == 0) {
            // Cleanup
            while (q.data.head) dequeue(&q);
            while (move_q.data.head) dequeue(&move_q);
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
                unsigned char temp = next.tiles[current.empty_row][current.empty_col];
                next.tiles[current.empty_row][current.empty_col] = next.tiles[new_r][new_c];
                next.tiles[new_r][new_c] = temp;
                
                next.empty_row = new_r;
                next.empty_col = new_c;
                
                size_t hash = serialize(next);
                if (!visited[hash]) {
                    visited[hash] = 1;
                    enqueue(&q, next);
                    // Store move count in parallel queue
                    struct game_state next_moves = {0};
                    next_moves.tiles[0][0] = current_moves + 1;
                    enqueue(&move_q, next_moves);
                }
            }
        }
    }
    
    return -1; // No solution
}
