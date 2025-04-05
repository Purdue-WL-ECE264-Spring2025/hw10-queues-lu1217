#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

// Constants
#define VISIT_SIZE (1 << 18)  // Size of visited states table
#define MAX_MOVES 60          // Max moves allowed

// Hash function for game states
size_t battle_hash(struct game_state state) {
    size_t hash = 2166136261u;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            hash = (hash ^ state.tiles[i][j]) * 16777619;
        }
    }
    return hash ^ (state.empty_row << 8);
}

// Add state to queue
void enqueue(struct queue *q, struct game_state state) {
    if (!q) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    new_node->value = serialize(state);
    new_node->next = NULL;

    if (!q->data.head) {
        q->data.head = new_node;  // First element
    } else {
        struct list_node *current = q->data.head;
        while (current->next) current = current->next;
        current->next = new_node;  // Add to end
    }
}

// Remove and return state from queue
struct game_state dequeue(struct queue *q) {
    static struct game_state empty_state = {0};
    if (!q || !q->data.head) return empty_state;

    struct list_node *front = q->data.head;
    struct game_state state = deserialize(front->value);
    q->data.head = front->next;
    free(front);
    
    return state;
}

// Check if puzzle is solved
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

// BFS to find minimum moves to solve
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
    visited[battle_hash(start) % VISIT_SIZE] = 1;
    
    while (q.data.head && moves <= MAX_MOVES) {
        int level_size = 0;
        for (struct list_node *n = q.data.head; n; n = n->next) {
            level_size++;
        }
        
        while (level_size--) {
            struct game_state curr = dequeue(&q);
            
            if (is_solved(curr)) {
                while (q.data.head) dequeue(&q);
                free(visited);
                return moves;
            }
            
            // Try all 4 possible moves
            int dirs[4][2] = {{0,1},{1,0},{-1,0},{0,-1}};
            for (int i = 0; i < 4; i++) {
                int r = curr.empty_row + dirs[i][0];
                int c = curr.empty_col + dirs[i][1];
                
                if (r >= 0 && r < 4 && c >= 0 && c < 4) {
                    struct game_state next = curr;
                    next.tiles[curr.empty_row][curr.empty_col] = next.tiles[r][c];
                    next.tiles[r][c] = 0;
                    next.empty_row = r;
                    next.empty_col = c;
                    
                    size_t h = battle_hash(next) % VISIT_SIZE;
                    if (!visited[h]) {
                        visited[h] = 1;
                        enqueue(&q, next);
                    }
                }
            }
        }
        moves++;
    }
    
    while (q.data.head) dequeue(&q);
    free(visited);
    return -1;  // No solution found
}
