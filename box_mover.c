#include <hilg.h>
#include <ncurses.h>
#include <stdlib.h>

/* types */
struct bmover_state {
	int done;
	int current_row;
	int current_col;
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
int main(int argc, char *argv[])
{
	struct hilg_game_info game_info;
	struct hilg_state *hilg_token = NULL;

	struct bmover_state game_state = {
		.done = 0,
		.current_row = 1,
		.current_col = 1,
		.row_count = 10,
		.col_count = 10
	};

	game_info = (struct hilg_game_info) {
		.row_count = 10,
		.col_count = 10,
		.timer_interval = 0,
		.game_state = &game_state,
		.handle_event_func = bmover_handle_event,
		.update_board_func = bmover_update_board,
		.is_done_func = bmover_is_done
	};

	hilg_token = hilg_init(&game_info);
	hilg_run(hilg_token);

	return 0;
}


static void bmover_handle_event(void *gstate, struct hilg_event *event)
{
	struct bmover_state *state = gstate;
	int delta_row = 0;
	int delta_col = 0;

	if (event->type != KEYPRESS)
		return;

	switch (event->data.keycode)
	{
	case 'q':
		state->done = 1;
		break;
	case KEY_UP:
		delta_row = -1;
		delta_col = 0;
		break;
	case KEY_DOWN:
		delta_row = 1;
		delta_col = 0;
		break;
	case KEY_LEFT:
		delta_row = 0;
		delta_col = -1;
		break;
	case KEY_RIGHT:
		delta_row = 0;
		delta_col = 1;
		break;
	}

	if (delta_row != 0 || delta_col != 0) {
		int new_row = state->current_row + delta_row;
		int new_col = state->current_col + delta_col;

		if (new_row > 0 && new_row < state->row_count - 1 &&
		    new_col > 0 && new_col < state->col_count - 1) {

			state->current_row = new_row;
			state->current_col = new_col;
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
			/* borders */
			if (row == 0 || row == row_count - 1 ||
			    col == 0 || col == col_count - 1) {
				board[row][col] = '*';
			}
			/* box */
			else if (row == state->current_row &&
				 col == state->current_col) {
				board[row][col] = '#';
			}
			/* empty space */
			else {
				board[row][col] = ' ';
			}
		}
}

static int bmover_is_done(void *gstate)
{
	struct bmover_state *state = gstate;
	return state->done;
}

