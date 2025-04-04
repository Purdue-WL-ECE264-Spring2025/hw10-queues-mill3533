#include "queue.h"
#include "tile_game.h"
#include <stdbool.h>

void enqueue(struct queue *q, struct game_state state) {
    uint64_t serialized = serialize(state);
    insert_at_tail(&q->data, serialized);
}

struct game_state dequeue(struct queue *q) {
    uint64_t serialized = remove_from_head(&q->data);
    return deserialize(serialized);
}

int number_of_moves(struct game_state start) {
    // Check if already solved
    bool solved = true;
    for (int i = 0; i < 4 && solved; i++) {
        for (int j = 0; j < 4 && solved; j++) {
            if (i == 3 && j == 3) {
                if (start.tiles[i][j] != 0) solved = false;
            } else {
                if (start.tiles[i][j] != i * 4 + j + 1) solved = false;
            }
        }
    }
    if (solved) return 0;

    struct queue q = {0};
    enqueue(&q, start);

    while (1) {
        if (q.data.head == NULL) {
            free_list(q.data);
            return -1; // No solution found (shouldn't happen for valid inputs)
        }

        struct game_state current = dequeue(&q);

        // Check if current state is solved
        solved = true;
        for (int i = 0; i < 4 && solved; i++) {
            for (int j = 0; j < 4 && solved; j++) {
                if (i == 3 && j == 3) {
                    if (current.tiles[i][j] != 0) solved = false;
                } else {
                    if (current.tiles[i][j] != i * 4 + j + 1) solved = false;
                }
            }
        }
        if (solved) {
            int moves = current.num_steps;
            free_list(q.data);
            return moves;
        }

        // Generate possible moves
        if (current.empty_row > 0) { // Can move down
            struct game_state down = current;
            move_down(&down);
            enqueue(&q, down);
        }
        if (current.empty_row < 3) { // Can move up
            struct game_state up = current;
            move_up(&up);
            enqueue(&q, up);
        }
        if (current.empty_col > 0) { // Can move right
            struct game_state right = current;
            move_right(&right);
            enqueue(&q, right);
        }
        if (current.empty_col < 3) { // Can move left
            struct game_state left = current;
            move_left(&left);
            enqueue(&q, left);
        }
    }
}