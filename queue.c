#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

/* Constants */
#define VISIT_SIZE (1 << 18)  // Size of visited states hash table (262,144 entries)
#define MAX_MOVES 60          // Maximum allowed moves before giving up

/* 
 * Battle-tested FNV-1a hash function for game states
 * Provides good distribution of hash values to minimize collisions
 */
size_t battle_hash(struct game_state state) {
    size_t hash = 2166136261u;  // FNV offset basis
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            hash = (hash ^ state.tiles[i][j]) * 16777619;  // FNV prime
        }
    }
    return hash ^ (state.empty_row << 8);  // Incorporate empty tile position
}

/*
 * Enqueue a game state into the queue
 * @param q: Pointer to the queue
 * @param state: Game state to enqueue
 */
void enqueue(struct queue *q, struct game_state state) {
    if (!q) return;  // Safety check
    
    // Create new node
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;  // Allocation check
    
    // Initialize node
    new_node->value = serialize(state);
    new_node->next = NULL;

    // Add to end of queue
    if (!q->data.head) {
        q->data.head = new_node;  // First element
    } else {
        // Traverse to end of queue
        struct list_node *current = q->data.head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;  // Append new node
    }
}

/*
 * Dequeue a game state from the queue
 * @param q: Pointer to the queue
 * @return: Dequeued game state, or empty state if queue is empty
 */
struct game_state dequeue(struct queue *q) {
    static struct game_state empty_state = {0};  // Returned for empty queue
    if (!q || !q->data.head) {
        return empty_state;
    }

    // Remove from front of queue
    struct list_node *front = q->data.head;
    struct game_state state = deserialize(front->value);
    q->data.head = front->next;
    free(front);
    
    return state;
}

/*
 * Check if a game state is solved
 * @param state: Game state to check
 * @return: 1 if solved, 0 otherwise
 */
int is_solved(struct game_state state) {
    unsigned char expected = 1;  // Expected tile value counter
    
    // Check each tile position
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == 3 && j == 3) {  // Bottom-right corner
                if (state.tiles[i][j] != 0) return 0;  // Must be empty
            } else {
                if (state.tiles[i][j] != expected++) return 0;  // Must be in order
            }
        }
    }
    return 1;  // All checks passed
}

/*
 * Calculate minimum number of moves to solve the puzzle
 * @param start: Initial game state
 * @return: Number of moves needed, or -1 if unsolvable
 */
int number_of_moves(struct game_state start) {
    // Initialize queue for BFS
    struct queue q = { .data = { .head = NULL } };
    
    // Visited states hash table
    int *visited = calloc(VISIT_SIZE, sizeof(int));
    if (!visited) return -1;  // Allocation failed
    
    int moves = 0;  // Move counter
    
    // Check if already solved
    if (is_solved(start)) {
        free(visited);
        return 0;
    }

    // Start BFS with initial state
    enqueue(&q, start);
    visited[battle_hash(start) % VISIT_SIZE] = 1;
    
    // BFS loop
    while (q.data.head && moves <= MAX_MOVES) {
        // Count nodes at current level
        int level_size = 0;
        for (struct list_node *n = q.data.head; n; n = n->next) {
            level_size++;
        }
        
        // Process all nodes at current level
        while (level_size--) {
            struct game_state curr = dequeue(&q);
            
            // Check if current state is solved
            if (is_solved(curr)) {
                // Cleanup before returning
                while (q.data.head) {
                    struct list_node *next = q.data.head->next;
                    free(q.data.head);
                    q.data.head = next;
                }
                free(visited);
                return moves;
            }
            
            // Generate all possible moves (Right, Down, Up, Left)
            int dirs[4][2] = {{0,1},{1,0},{-1,0},{0,-1}};
            for (int i = 0; i < 4; i++) {
                int r = curr.empty_row + dirs[i][0];
                int c = curr.empty_col + dirs[i][1];
                
                // Check if move is valid
                if (r >= 0 && r < 4 && c >= 0 && c < 4) {
                    // Create new state by swapping tiles
                    struct game_state next = curr;
                    next.tiles[curr.empty_row][curr.empty_col] = next.tiles[r][c];
                    next.tiles[r][c] = 0;
                    next.empty_row = r;
                    next.empty_col = c;
                    
                    // Check if state hasn't been visited
                    size_t h = battle_hash(next) % VISIT_SIZE;
                    if (!visited[h]) {
                        visited[h] = 1;
                        enqueue(&q, next);
                    }
                }
            }
        }
        moves++;  // Completed one level of BFS
    }
    
    // Cleanup if unsolved
    while (q.data.head) dequeue(&q);
    free(visited);
    return -1;  // No solution found within move limit
}
