#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

void enqueue(struct queue *q, struct game_state state) {
    if (!q) return;
    insert_at_tail(&q->data, serialize(state));
}

struct game_state dequeue(struct queue *q) {
    if (!q || !q->data.head) {
        return (struct game_state){0};
    }
    size_t val = remove_from_head(&q->data);
    return deserialize(val);
}

int number_of_moves(struct game_state start) {
    struct queue q = { .data = { .head = NULL } };
    // Note: This visited array only works for very small board spaces.
    // For real-world use, you'd want a better hash or dynamic set.
    int visited[1 << 20] = {0}; // Slightly larger, still limited

    enqueue(&q, start);
    visited[serialize(start)] = 1;

    while (q.data.head) {
        struct game_state curr = dequeue(&q);

        // Check if solved
        int solved = 1;
        for (int i = 0, val = 1; i < 4 && solved; i++) {
            for (int j = 0; j < 4 && solved; j++) {
                if (i == 3 && j == 3) {
                    if (curr.tiles[i][j] != 0) solved = 0;
                } else if (curr.tiles[i][j] != val++) {
                    solved = 0;
                }
            }
        }

        if (solved) {
            free_list(q.data);
            return curr.number_of_moves;
        }

        // Move directions: up, down, left, right
        int directions[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int i = 0; i < 4; i++) {
            int new_r = curr.empty_row + directions[i][0];
            int new_c = curr.empty_col + directions[i][1];

            if (new_r >= 0 && new_r < 4 && new_c >= 0 && new_c < 4) {
                struct game_state next = curr;

                // Swap the tile
                next.tiles[curr.empty_row][curr.empty_col] = next.tiles[new_r][new_c];
                next.tiles[new_r][new_c] = 0;

                next.empty_row = new_r;
                next.empty_col = new_c;
                next.number_of_moves = curr.number_of_moves + 1;

                size_t hash = serialize(next);
                if (!visited[hash]) {
                    visited[hash] = 1;
                    enqueue(&q, next);
                }
            }
        }
    }

    free_list(q.data);
    return -1; // No solution
}
