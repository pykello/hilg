
#ifndef HILG_H
#define HILG_H

/* include ncurses for KEY_UP, etc. */
#include <ncurses.h>

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

	void (*handle_event_func)(void *, struct hilg_event *);
	void (*update_board_func)(void *, char **, int, int);
	int (*is_done_func)(void *);
};

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

extern void hilg_run(struct hilg_game_info *game_info);

#endif
