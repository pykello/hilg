#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <hilg.h>

/* forward declarations */
static char ** allocate_board(int row_count, int col_count);
static void free_board(char **board, int row_count);
static void init_ncurses(void);
static WINDOW * create_main_window(struct hilg_game_info *game_info);
static WINDOW * create_board_window(struct hilg_game_info *game_info);
static WINDOW * create_sidebar_window(struct hilg_game_info *game_info);
static void draw_board(WINDOW *window, char **board,
		       int row_count, int col_count);
static void update_sidebar(WINDOW *window, struct hilg_game_info *game_info);
static void dispatch_keyboard_events(WINDOW *board_window,
				     struct hilg_game_info *game_info);
static void dispatch_timer_events(struct hilg_game_info *game_info,
				  struct timespec *previous_tick);
static int diff_timespec_ms(struct timespec end, struct timespec begin);

void hilg_run(struct hilg_game_info *game_info)
{
	struct timespec previous_tick = {0, 0};
	WINDOW *main_window = NULL;
	WINDOW *board_window = NULL;
	WINDOW *sidebar_window = NULL;

	int row_count = game_info->row_count;
	int col_count = game_info->col_count;
	char **board = allocate_board(row_count, col_count);

	init_ncurses();
	main_window = create_main_window(game_info);
	board_window = create_board_window(game_info);
	sidebar_window = create_sidebar_window(game_info);

	while (!game_info->is_done_func(game_info->game_state))
	{
		game_info->update_board_func(game_info->game_state,
					     board, row_count, col_count);

		draw_board(board_window, board, row_count, col_count);
		update_sidebar(sidebar_window, game_info);

		dispatch_keyboard_events(board_window, game_info);
		dispatch_timer_events(game_info, &previous_tick);

		doupdate();

		/* avoid 100% cpu */
		usleep(5000);
	}

	delwin(sidebar_window);
	delwin(board_window);
	delwin(main_window);
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

static WINDOW * create_main_window(struct hilg_game_info *game_info)
{
	WINDOW *window = NULL;
	int title_col = 0;

	int game_row_count = game_info->row_count;
	int game_col_count = game_info->col_count;

	/*
	 * 2 rows for borders + 1 row for title + 1 row for title separator,
	 * 2 cols for borders.
	 */
	int win_row_count = game_row_count + 4;
	int win_col_count = game_col_count + 2;

	if (game_info->sidebar_fields != 0)
		win_col_count += 1 + game_info->sidebar_length;

	window = newwin(win_row_count, win_col_count, 0, 0);

	/* draw borders and title separator */
	box(window, 0, 0);
	mvwhline(window, 2, 1, 0, win_col_count - 2);
	mvwaddch(window, 2, 0, ACS_LTEE);
	mvwaddch(window, 2, win_col_count - 1, ACS_RTEE);

	if (game_info->sidebar_fields != 0) {
		mvwvline(window, 3, game_col_count + 1, 0, game_col_count);
		mvwaddch(window, 2, game_col_count + 1, ACS_TTEE);
		mvwaddch(window, 3 + game_row_count, game_col_count + 1, ACS_BTEE);
	}

	/* write game info */
	title_col = 1 + (game_col_count - strlen(game_info->title)) / 2;
	mvwprintw(window, 1, title_col, "%s", game_info->title);

	wrefresh(window);

	return window;
}

static WINDOW * create_board_window(struct hilg_game_info *game_info)
{
	WINDOW *window = NULL;

	window = newwin(game_info->row_count, game_info->col_count, 3, 1);

	/* set input options */
	wtimeout(window, 0);
	keypad(window, TRUE);

	return window;
}

static WINDOW * create_sidebar_window(struct hilg_game_info *game_info)
{
	WINDOW *window = NULL;

	window = newwin(game_info->row_count, game_info->sidebar_length,
			3, game_info->col_count + 2);

	return window;
}

static void draw_board(WINDOW *window, char **board,
		       int row_count, int col_count)
{
	int row = 0;
	int col = 0;

	for (row = 0; row < row_count; row++)
		for (col = 0; col < col_count; col++)
			mvwaddch(window, row, col, board[row][col]);

	wnoutrefresh(window);
}

static void update_sidebar(WINDOW *window, struct hilg_game_info *game_info)
{
	int field = 0;
	char *field_text = NULL;

	if (game_info->sidebar_fields == 0)
		return;

	field_text = malloc(game_info->sidebar_length + 1);
	werase(window);

	for (field = 0; field < game_info->sidebar_fields; field++) {
		game_info->update_sidebar_func(game_info->game_state,
					       field, field_text);
		mvwprintw(window, field, 0, field_text);
	}

	wnoutrefresh(window);
	free(field_text);
}

static void dispatch_keyboard_events(WINDOW *board_window,
				     struct hilg_game_info *game_info)
{
	while (true) {
		struct hilg_event event;

		int keycode = wgetch(board_window);
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
