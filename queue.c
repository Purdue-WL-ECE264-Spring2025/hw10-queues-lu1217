#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>  // Added for malloc/free

void enqueue(struct queue *q, struct game_state state) {
    size_t serialized = serialize(state);
    struct list_node *new_node = malloc(sizeof(struct list_node));
    new_node->value = serialized;
    new_node->next = NULL;
    
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
    
    start.moves = 0;
    enqueue(&q, start);
    visited[serialize(start)] = 1;
    
    while (q.data.head != NULL) {
        struct game_state current = dequeue(&q);
        
        if (current.board[0][0] == 1 && 
            current.board[0][1] == 2 && 
            current.board[0][2] == 3 && 
            current.board[0][3] == 4 &&
            current.board[1][0] == 5 &&
            current.board[1][1] == 6 &&
            current.board[1][2] == 7 &&
            current.board[1][3] == 8 &&
            current.board[2][0] == 9 &&
            current.board[2][1] == 10 &&
            current.board[2][2] == 11 &&
            current.board[2][3] == 12 &&
            current.board[3][0] == 13 &&
            current.board[3][1] == 14 &&
            current.board[3][2] == 15 &&
            current.board[3][3] == 0) {
            // Clean up remaining queue
            while (q.data.head != NULL) {
                struct list_node *next = q.data.head->next;
                free(q.data.head);
                q.data.head = next;
            }
            return current.moves;
        }
        
        // Generate possible moves
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int i = 0; i < 4; i++) {
            int new_r = current.empty_row + dirs[i][0];
            int new_c = current.empty_col + dirs[i][1];
            
            if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                struct game_state next = current;
                // Swap tiles
                next.board[current.empty_row][current.empty_col] = 
                    next.board[new_r][new_c];
                next.board[new_r][new_c] = 0;
                next.empty_row = new_r;
                next.empty_col = new_c;
                next.moves++;
                
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
