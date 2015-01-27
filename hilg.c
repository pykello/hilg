#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <hilg.h>

struct hilg_state * hilg_init(struct hilg_game_info *game_info)
{
	struct hilg_state *state = NULL;
	char **board = NULL;
	int row = 0;

	int row_count = game_info->row_count;
	int col_count = game_info->col_count;

	/* allocate board */
	board = malloc(row_count * sizeof(char *));
	for (row = 0; row < row_count; row++)
		board[row] = malloc(col_count * sizeof(char));

	/* create state */
	state = malloc(sizeof(struct hilg_state));
	state->game_info = game_info;
	state->board = board;

	return state;
}

void hilg_run(struct hilg_state *state)
{
	struct hilg_game_info *game_info = state->game_info;
	clock_t previous_tick = 0;

	initscr();
	noecho();
	curs_set(FALSE);
	timeout(0);
	cbreak();
	keypad(stdscr, TRUE);

	while (!game_info->is_done_func(game_info->game_state))
	{
		int row = 0;
		int col = 0;

		game_info->update_board_func(game_info->game_state,
					     state->board,
					     game_info->row_count,
					     game_info->col_count);

		clear();

		for (row = 0; row < game_info->row_count; row++)
			for (col = 0; col < game_info->col_count; col++)
				mvprintw(row, col, "%c", state->board[row][col]);

		refresh();

		/* keyboard events */
		while (true) {
			struct hilg_event event;

			int keycode = getch();
			if (keycode == ERR)
				break;

			event.type = KEYPRESS;
			event.data.keycode = keycode;

			game_info->handle_event_func(game_info->game_state,
						     &event);
		}

		/* timer events */
		if (game_info->timer_interval != 0) {
			clock_t wait_limit =
				game_info->timer_interval * CLOCKS_PER_SEC;
			clock_t current_time = clock();

			if (current_time - previous_tick >= wait_limit) {
				struct hilg_event event = {
					.type = TIMER
				};
				game_info->handle_event_func(game_info->game_state,
							     &event);

				/* reset timer */
				previous_tick = current_time;
			}
		}

		/* simplest way to avoid 100% cpu */
		usleep(1000);
	}

	endwin();
}
