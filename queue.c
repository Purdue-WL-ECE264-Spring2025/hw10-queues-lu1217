#include "queue.h"
#include "tile_game.h"

void enqueue(struct queue *q, struct game_state state) {
    // Implementation added
    size_t serialized = serialize(state);
    struct list_node *new = malloc(sizeof(struct list_node));
    new->value = serialized;
    new->next = NULL;
    
    if (q->list.tail) {
        q->list.tail->next = new;
        q->list.tail = new;
    } else {
        q->list.head = q->list.tail = new;
    }
}

struct game_state dequeue(struct queue *q) {
    // Implementation added
    if (!q->list.head) return (struct game_state){0};
    
    struct list_node *front = q->list.head;
    size_t serialized = front->value;
    struct game_state state = deserialize(serialized);
    
    q->list.head = front->next;
    if (!q->list.head) q->list.tail = NULL;
    free(front);
    
    return state;
}

int number_of_moves(struct game_state start) {
    // Implementation added
    struct queue q = { .list = { .head = NULL, .tail = NULL } };
    int visited[1 << 16] = {0};
    
    start.number_of_moves = 0;
    enqueue(&q, start);
    visited[serialize(start)] = 1;
    
    while (q.list.head) {
        struct game_state current = dequeue(&q);
        
        if (is_solved(current)) {
            while (q.list.head) {
                struct list_node *next = q.list.head->next;
                free(q.list.head);
                q.list.head = next;
            }
            return current.number_of_moves;
        }
        
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int i = 0; i < 4; i++) {
            int new_r = current.empty_row + dirs[i][0];
            int new_c = current.empty_col + dirs[i][1];
            
            if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                struct game_state next = current;
                next.board[current.empty_row][current.empty_col] = 
                    next.board[new_r][new_c];
                next.board[new_r][new_c] = 0;
                next.empty_row = new_r;
                next.empty_col = new_c;
                next.number_of_moves++;
                
                size_t serialized = serialize(next);
                if (!visited[serialized]) {
                    visited[serialized] = 1;
                    enqueue(&q, next);
                }
            }
        }
    }
    
    return -1;
}
