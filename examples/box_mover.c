#include <hilg.h>
#include <stdlib.h>

#define CELL_BORDER '*'
#define CELL_BOX '#'
#define CELL_EMPTY ' '

/* types */
struct bmover_state {
	int done;
	struct hilg_cell current_cell;
	int row_count;
	int col_count;
};

/* forward declarations */
static void bmover_handle_event(void *state,
				struct hilg_event *event);
static void bmover_update_board(void *state, char **board,
				int row_count, int col_count);
static int bmover_is_done(void *state);


/* main function */
int main(void)
{
	struct bmover_state game_state = {
		.done = 0,
		.current_cell = {.row = 1, .col = 1},
		.row_count = 10,
		.col_count = 10
	};

	struct hilg_game_info game_info = {
		.row_count = 10,
		.col_count = 10,
		.timer_interval = 0,
		.game_state = &game_state,
		.handle_event_func = bmover_handle_event,
		.update_board_func = bmover_update_board,
		.is_done_func = bmover_is_done
	};

	hilg_run(&game_info);

	return 0;
}


static void bmover_handle_event(void *gstate, struct hilg_event *event)
{
	struct bmover_state *state = gstate;

	if (event->type != KEYPRESS)
		return;

	switch (event->data.keycode)
	{
	case 'q':
		state->done = 1;
		break;

	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
	{
		struct hilg_cell direction = key_to_direction(event->data.keycode);
		struct hilg_cell target_cell = cell_add(state->current_cell, direction);

		if (!cell_on_border(target_cell, state->row_count, state->col_count))
			state->current_cell = target_cell;

		break;
	}
	}
}

static void bmover_update_board(void *gstate, char **board,
				int row_count, int col_count)
{
	struct bmover_state *state = gstate;

	int row = 0;
	int col = 0;

	for (row = 0; row < row_count; row++)
		for (col = 0; col < col_count; col++) {
			struct hilg_cell cell = {.row = row, .col = col};

			if (cell_on_border(cell, row_count, col_count)) {
				board[row][col] = CELL_BORDER;
			}
			else if (cell_equals(cell, state->current_cell)) {
				board[row][col] = CELL_BOX;
			}
			else {
				board[row][col] = CELL_EMPTY;
			}
		}
}

static int bmover_is_done(void *gstate)
{
	struct bmover_state *state = gstate;
	return state->done;
}
