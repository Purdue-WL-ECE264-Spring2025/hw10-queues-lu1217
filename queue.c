#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

void enqueue(struct queue *q, struct game_state state) {
    if (!q) return;
    
    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node) return;
    
    // Store both state and moves in the serialized value
    // Using bit manipulation to pack moves into upper bits
    size_t serialized = serialize(state);
    new_node->value = serialized;
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
    // Create parallel queue for move counts
    struct queue state_queue = { .data = { .head = NULL } };
    struct queue move_queue = { .data = { .head = NULL } };
    
    // Track visited states (simplified)
    int visited[65536] = {0};
    
    // Initial state
    enqueue(&state_queue, start);
    size_t initial_move = 0;
    enqueue(&move_queue, (struct game_state){ .tiles = {{0}} }); // Dummy state with moves in tiles[0][0]
    ((unsigned char*)&move_queue.data.head->value)[0] = 0; // Store move count
    
    while (state_queue.data.head) {
        struct game_state current = dequeue(&state_queue);
        int current_move;
        
        // Retrieve move count from parallel queue
        struct game_state move_state = dequeue(&move_queue);
        current_move = move_state.tiles[0][0];
        
        // Check solved condition
        int solved = 1;
        unsigned char target = 1;
        for (int i = 0; i < 4 && solved; i++) {
            for (int j = 0; j < 4 && solved; j++) {
                if (i == 3 && j == 3) {
                    if (current.tiles[i][j] != 0) solved = 0;
                } else {
                    if (current.tiles[i][j] != target++) solved = 0;
                }
            }
        }
        if (solved) {
            // Cleanup
            while (state_queue.data.head) dequeue(&state_queue);
            while (move_queue.data.head) dequeue(&move_queue);
            return current_move;
        }
        
        // Generate moves
        int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int d = 0; d < 4; d++) {
            int new_r = current.empty_row + directions[d][0];
            int new_c = current.empty_col + directions[d][1];
            
            if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                struct game_state next = current;
                // Swap tiles
                next.tiles[current.empty_row][current.empty_col] = next.tiles[new_r][new_c];
                next.tiles[new_r][new_c] = 0;
                next.empty_row = new_r;
                next.empty_col = new_c;
                
                // Check visited using simple hash
                size_t hash = 0;
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        hash = (hash * 31 + next.tiles[i][j]) % 65536;
                    }
                }
                
                if (!visited[hash]) {
                    visited[hash] = 1;
                    enqueue(&state_queue, next);
                    
                    // Store move count in parallel queue
                    struct game_state next_move = { .tiles = {{0}} };
                    next_move.tiles[0][0] = current_move + 1;
                    enqueue(&move_queue, next_move);
                }
            }
        }
    }
    
    return -1; // No solution found
}
