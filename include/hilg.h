
#ifndef HILG_H
#define HILG_H

/* include ncurses for KEY_UP, etc. */
#include <ncurses.h>

#define TITLE_LEN_MAX 20

enum event_type {
	KEYPRESS,
	TIMER
};

struct hilg_event {
	enum event_type type;
	union event_data {
		int keycode;
	} data;
};

struct hilg_game_info {
	int row_count;
	int col_count;
	int timer_interval;
	void *game_state;
	char title[TITLE_LEN_MAX + 1];

	void (*handle_event_func)(void *, struct hilg_event *);
	void (*update_board_func)(void *, char **, int, int);
	int (*is_done_func)(void *);
};

/* cell definitions */

struct hilg_cell {
	int row;
	int col;
};

#define cell_equals(c1, c2) ((c1).row == (c2).row && (c1).col == (c2).col)
#define cell_add(c1, c2) (struct hilg_cell) {(c1).row + (c2).row, (c1).col + (c2).col}
#define cell_on_border(c, rows, cols) ((c).row == 0 || (c).col == 0 ||\
				       (c).row == rows - 1 || (c).col == cols - 1)

const struct hilg_cell DIRECTION_UP = {.row = -1, .col = 0};
const struct hilg_cell DIRECTION_DOWN = {.row = 1, .col = 0};
const struct hilg_cell DIRECTION_LEFT = {.row = 0, .col = -1};
const struct hilg_cell DIRECTION_RIGHT = {.row = 0, .col = 1};

#define opposite_direction(a, b) ((a).row == -(b).row && (a).col == -(b).col)
#define key_to_direction(k) ((k == KEY_UP) ? DIRECTION_UP : (\
			     (k == KEY_DOWN) ? DIRECTION_DOWN : (\
			     (k == KEY_LEFT) ? DIRECTION_LEFT : (\
			     DIRECTION_RIGHT))))

/* key queue definitions */

#define KEY_QUEUE_SIZE_LIMIT 10

struct hilg_key_queue {
	int keycodes[KEY_QUEUE_SIZE_LIMIT];
	int head;
	int tail;
};

#define queue_next_head(q) (((q).head + 1) % KEY_QUEUE_SIZE_LIMIT)
#define queue_next_tail(q) (((q).tail + 1) % KEY_QUEUE_SIZE_LIMIT)
#define queue_empty(q) ((q).head == (q).tail)
#define queue_full(q) (queue_next_head(q) == (q).tail)
#define queue_push(q, v) ((q).keycodes[(q).head] = (v), (q).head = queue_next_head(q))
#define queue_peek(q) ((q).keycodes[(q).tail])
#define queue_pop(q) ((q).tail = queue_next_tail(q))

const struct hilg_key_queue KEY_QUEUE_EMPTY = {.head = 0, .tail = 0};

/* game loop function */

extern void hilg_run(struct hilg_game_info *game_info);

#endif
