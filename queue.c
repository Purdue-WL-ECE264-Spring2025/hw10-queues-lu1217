#include "queue.h"
#include "tile_game.h"

void enqueue(struct queue *q, struct game_state state) {
    // Convert game state to integer representation
    size_t serialized = serialize(state);
    
    // Create new node
    struct list_node *new = malloc(sizeof(struct list_node));
    new->value = serialized;
    new->next = NULL;
    
    // Add to end of queue
    if (q->list.tail) {
        q->list.tail->next = new;
        q->list.tail = new;
    } else {
        q->list.head = q->list.tail = new;
    }
}

struct game_state dequeue(struct queue *q) {
    // Remove from front of queue
    struct list_node *front = q->list.head;
    size_t serialized = front->value;
    
    q->list.head = front->next;
    if (!q->list.head) q->list.tail = NULL;
    free(front);
    
    return deserialize(serialized);
}

int number_of_moves(struct game_state start) {
    struct queue q = { .list = { .head = NULL, .tail = NULL } };
    enqueue(&q, start);
    
    while (q.list.head) {
        struct game_state current = dequeue(&q);
        
        if (is_solved(current)) {
            // Clean up remaining nodes
            while (q.list.head) {
                struct list_node *next = q.list.head->next;
                free(q.list.head);
                q.list.head = next;
            }
            return current.number_of_moves;
        }
        
        // Generate possible moves
        int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
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
                next.number_of_moves++;
                
                enqueue(&q, next);
            }
        }
    }
    
    return -1; // No solution found
}
