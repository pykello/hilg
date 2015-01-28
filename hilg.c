#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <hilg.h>

/* forward declarations */
static char ** allocate_board(int row_count, int col_count);
static void free_board(char **board, int row_count);
static void init_ncurses(void);
static void draw_board(char **board, int row_count, int col_count);
static void dispatch_keyboard_events(struct hilg_game_info *game_info);
static void dispatch_timer_events(struct hilg_game_info *game_info,
				  clock_t *previous_tick);


void hilg_run(struct hilg_game_info *game_info)
{
	clock_t previous_tick = 0;

	int row_count = game_info->row_count;
	int col_count = game_info->col_count;
	char **board = allocate_board(row_count, col_count);

	init_ncurses();

	while (!game_info->is_done_func(game_info->game_state))
	{
		game_info->update_board_func(game_info->game_state,
					     board, row_count, col_count);

		draw_board(board, row_count, col_count);

		dispatch_keyboard_events(game_info);
		dispatch_timer_events(game_info, &previous_tick);

		/* avoid 100% cpu */
		usleep(1000);
	}

	endwin();
	free_board(board, row_count);
}

static char ** allocate_board(int row_count, int col_count)
{
	char **board = NULL;
	int row = 0;

	board = malloc(row_count * sizeof(char *));
	for (row = 0; row < row_count; row++)
		board[row] = malloc(col_count * sizeof(char));

	return board;
}

static void free_board(char **board, int row_count)
{
	int row = 0;
	for (row = 0; row < row_count; row++)
		free(board[row]);

	free(board);
}

static void init_ncurses(void)
{
	initscr();
	noecho();
	curs_set(FALSE);
	timeout(0);
	cbreak();
	keypad(stdscr, TRUE);	
}

static void draw_board(char **board, int row_count, int col_count)
{
	int row = 0;
	int col = 0;

	clear();

	for (row = 0; row < row_count; row++)
		for (col = 0; col < col_count; col++)
			mvprintw(row, col, "%c", board[row][col]);

	refresh();	
}

static void dispatch_keyboard_events(struct hilg_game_info *game_info)
{
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
}

static void dispatch_timer_events(struct hilg_game_info *game_info,
				  clock_t *previous_tick)
{
	clock_t wait_limit = 0;
	clock_t current_time = 0;

	if (game_info->timer_interval == 0)
		return;

	wait_limit = (game_info->timer_interval * CLOCKS_PER_SEC) / 1000;
	current_time = clock();

	if (current_time - *previous_tick >= wait_limit) {
		struct hilg_event event = {
			.type = TIMER
		};

		game_info->handle_event_func(game_info->game_state, &event);

		/* reset timer */
		*previous_tick = current_time;
	}
}
