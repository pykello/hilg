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
				  struct timespec *previous_tick);
static int diff_timespec_ms(struct timespec end, struct timespec begin);

void hilg_run(struct hilg_game_info *game_info)
{
	struct timespec previous_tick = {0, 0};

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
				  struct timespec *previous_tick)
{
	struct timespec current_time = {0, 0};
	int time_passed_ms = 0;

	if (game_info->timer_interval == 0)
		return;

	clock_gettime(CLOCK_REALTIME, &current_time);

	time_passed_ms = diff_timespec_ms(current_time, *previous_tick);
	if (time_passed_ms >= game_info->timer_interval) {
		struct hilg_event event = {
			.type = TIMER
		};

		game_info->handle_event_func(game_info->game_state, &event);

		/* reset timer */
		*previous_tick = current_time;
	}
}

static int diff_timespec_ms(struct timespec end, struct timespec begin)
{
	int diff = 0;

	diff += (end.tv_sec - begin.tv_sec) * 1000;
	diff += (end.tv_nsec - begin.tv_nsec) / 1000000;

	return diff;
}
