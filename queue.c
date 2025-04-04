#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

#define VISIT_SIZE (1 << 22)  // Increased size
#define MAX_MOVES 50

size_t pattern_hash(struct game_state s) {
    size_t h = 0;
    for (int i = 0; i < 16; i++) 
        h = (h << 4) | s.tiles[i/4][i%4];
    return h ^ (s.empty_row << 8) ^ s.empty_col;
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
    visited[pattern_hash(start) % VISIT_SIZE] = 1;
    
    while (q.data.head && moves <= MAX_MOVES) {
        int level_size = 0;
        for (struct list_node *n = q.data.head; n; n = n->next) 
            level_size++;
        
        while (level_size--) {
            struct game_state curr = dequeue(&q);
            if (is_solved(curr)) {
                free(visited);
                while (q.data.head) dequeue(&q);
                return moves;
            }

            // Priority moves: right, down, left, up
            int dirs[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
            for (int i = 0; i < 4; i++) {
                int r = curr.empty_row + dirs[i][0];
                int c = curr.empty_col + dirs[i][1];
                if (r >= 0 && r < 4 && c >= 0 && c < 4) {
                    struct game_state next = curr;
                    next.tiles[curr.empty_row][curr.empty_col] = next.tiles[r][c];
                    next.tiles[r][c] = 0;
                    next.empty_row = r;
                    next.empty_col = c;
                    
                    size_t h = pattern_hash(next) % VISIT_SIZE;
                    if (!visited[h]) {
                        visited[h] = 1;
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
