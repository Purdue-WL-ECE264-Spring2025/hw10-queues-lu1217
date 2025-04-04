#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>
#include <stdio.h>

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
    int visited[1 << 16] = {0};  // Track visited states
    int moves = 0;
    
    enqueue(&q, start);
    visited[serialize(start)] = 1;
    
    while (q.data.head) {
        int level_size = 0;
        struct list_node *current_node = q.data.head;
        while (current_node) {
            level_size++;
            current_node = current_node->next;
        }
        
        while (level_size--) {
            struct game_state curr_state = dequeue(&q);
            
            //Check if puzzle is solved
            int solved = 1;
            for (int i = 0, val = 1; i < 4 && solved; i++) {
                for (int j = 0; j < 4 && solved; j++) {
                    if (i == 3 && j == 3) continue;
                    if (curr_state.tiles[i][j] != val++) solved = 0;
                }
            }
            if (solved) {
                while (q.data.head) dequeue(&q);  // Clean up queue
                return moves;
            }
            
            //Generate possible moves
            int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
            for (int i = 0; i < 4; i++) {
                int new_r = curr_state.empty_row + directions[i][0];
                int new_c = curr_state.empty_col + directions[i][1];
                
                if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                    struct game_state next = curr_state;
                    // Swap tiles
                    unsigned char temp = next.tiles[curr_state.empty_row][curr_state.empty_col];
                    next.tiles[curr_state.empty_row][curr_state.empty_col] = next.tiles[new_r][new_c];
                    next.tiles[new_r][new_c] = temp;
                    
                    next.empty_row = new_r;
                    next.empty_col = new_c;
                    
                    size_t hash = serialize(next);
                    if (!visited[hash]) {
                        visited[hash] = 1;
                        enqueue(&q, next);
                    }
                }
            }
        }
        moves++;  //Move counter
    }
    
    return -1; //No solution case
}
