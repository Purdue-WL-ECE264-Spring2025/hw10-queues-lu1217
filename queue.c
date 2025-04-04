#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

#define VISIT_SIZE (1 << 22)  // Increased hash table size
#define MAX_MOVES 60          // Reasonable upper bound

// Enhanced hash function that better handles complex patterns
size_t enhanced_hash(struct game_state state) {
    size_t hash = 0;
    // Hash both tile values and their positions
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            hash = (hash << 5) | (state.tiles[i][j] + i*4 + j);
        }
    }
    // Incorporate empty tile position
    hash ^= (state.empty_row << 8) | state.empty_col;
    return hash;
}

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
    static struct game_state empty_state = {0};
    if (!q || !q->data.head) {
        return empty_state;
    }

    struct list_node *front = q->data.head;
    struct game_state state = deserialize(front->value);
    q->data.head = front->next;
    free(front);
    
    return state;
}

int is_solved(struct game_state state) {
    unsigned char expected = 1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == 3 && j == 3) {
                if (state.tiles[i][j] != 0) return 0;
            } else {
                if (state.tiles[i][j] != expected++) return 0;
            }
        }
    }
    return 1;
}

int number_of_moves(struct game_state start) {
    struct queue q = { .data = { .head = NULL } };
    int *visited = calloc(VISIT_SIZE, sizeof(int));
    if (!visited) return -1;
    
    int moves = 0;
    
    if (is_solved(start)) {
        free(visited);
        return 0;
    }

    enqueue(&q, start);
    visited[enhanced_hash(start) % VISIT_SIZE] = 1;
    
    while (q.data.head && moves <= MAX_MOVES) {
        int level_size = 0;
        struct list_node *current_node = q.data.head;
        while (current_node) {
            level_size++;
            current_node = current_node->next;
        }
        
        while (level_size--) {
            struct game_state curr = dequeue(&q);
            
            if (is_solved(curr)) {
                free(visited);
                while (q.data.head) dequeue(&q);
                return moves;
            }
            
            // Try moves in order: right, down, left, up (optimized for common cases)
            int directions[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
            for (int i = 0; i < 4; i++) {
                int new_r = curr.empty_row + directions[i][0];
                int new_c = curr.empty_col + directions[i][1];
                
                if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                    struct game_state next = curr;
                    // Perform tile swap
                    next.tiles[curr.empty_row][curr.empty_col] = next.tiles[new_r][new_c];
                    next.tiles[new_r][new_c] = 0;
                    next.empty_row = new_r;
                    next.empty_col = new_c;
                    
                    size_t hash = enhanced_hash(next) % VISIT_SIZE;
                    if (!visited[hash]) {
                        visited[hash] = 1;
                        enqueue(&q, next);
                    }
                }
            }
        }
        moves++;
    }
    
    free(visited);
    return -1;
}
