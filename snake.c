#include <hilg.h>
#include <stdlib.h>

/* types and definitions */
#define ROWS_MAX 20
#define COLS_MAX 20
#define SNAKE_LEN_MAX 20
#define TIMER_INTERVAL 300

struct snake_state {
	int done;
	struct hilg_cell snake[SNAKE_LEN_MAX];
	int snake_len;
	int row_count;
	int col_count;
	struct hilg_cell direction;
	struct hilg_cell food;
};


/* forward declarations */
static void snake_handle_event(void *state, struct hilg_event *event);
static void snake_handle_keypress_event(struct snake_state *state,
					struct hilg_event *event);
static void snake_handle_timer_event(struct snake_state *state);
static void extend_snake(struct snake_state *state);
static void generate_food(struct snake_state *state);
static void snake_update_board(void *state, char **board,
			       int row_count, int col_count);
static int snake_is_done(void *state);


/* main function */
int main(void)
{
	struct snake_state game_state = {
		.done = 0,
		.snake_len = 1,
		.snake = {
			{.row = 1, .col = 1}
		},
		.row_count = ROWS_MAX,
		.col_count = COLS_MAX,
		.direction = DIRECTION_RIGHT,
		.food = {.row = ROWS_MAX / 2, .col = COLS_MAX / 2}
	};

	struct hilg_game_info game_info = {
		.row_count = game_state.row_count,
		.col_count = game_state.col_count,
		.timer_interval = TIMER_INTERVAL,
		.game_state = &game_state,
		.handle_event_func = snake_handle_event,
		.update_board_func = snake_update_board,
		.is_done_func = snake_is_done
	};

	hilg_run(&game_info);

	return 0;
}

static void snake_handle_event(void *gstate, struct hilg_event *event)
{
	struct snake_state *state = gstate;

	switch (event->type)
	{
	case KEYPRESS:
		snake_handle_keypress_event(state, event);
		break;
	case TIMER:
		snake_handle_timer_event(state);
		break;
	}
}

static void snake_handle_keypress_event(struct snake_state *state,
					struct hilg_event *event)
{
	switch (event->data.keycode)
	{
	case 'q':
		state->done = 1;
		break;
	case KEY_UP:
		state->direction = DIRECTION_UP;
		break;
	case KEY_DOWN:
		state->direction = DIRECTION_DOWN;
		break;
	case KEY_LEFT:
		state->direction = DIRECTION_LEFT;
		break;
	case KEY_RIGHT:
		state->direction = DIRECTION_RIGHT;
		break;
	}
}

static void snake_handle_timer_event(struct snake_state *state)
{
	int cell_index = 0;
	struct hilg_cell *snake = state->snake;
	int snake_len = state->snake_len;
	int last_cell = snake_len - 1;

	for (cell_index = 0; cell_index < last_cell; cell_index++)
		snake[cell_index] = snake[cell_index + 1];

	snake[last_cell].row += state->direction.row;
	snake[last_cell].col += state->direction.col;

	/* game over ? */
	if (snake[last_cell].row == 0 ||
	    snake[last_cell].row == state->row_count - 1 ||
	    snake[last_cell].col == 0 ||
	    snake[last_cell].col == state->col_count - 1) {
		state->done = 1;
	}

	/* food ? */
	if (snake[last_cell].row == state->food.row &&
	    snake[last_cell].col == state->food.col) {
		extend_snake(state);
		generate_food(state);
	}
}

static void extend_snake(struct snake_state *state)
{
	int cell_index = 0;
	state->snake_len += 1;

	for (cell_index = state->snake_len - 1; cell_index > 0; cell_index--)
		state->snake[cell_index] = state->snake[cell_index - 1];
}

static void generate_food(struct snake_state *state)
{
	while (true) {
		int cell_index = 0;
		int collides_snake = 0;

		state->food = (struct hilg_cell) {
			.row = rand() % (state->row_count - 2) + 1,
			.col = rand() % (state->col_count - 2) + 1
		};

		for (cell_index = 0; cell_index < state->snake_len; cell_index++)
			if (state->food.row == state->snake[cell_index].row &&
			    state->food.col == state->snake[cell_index].col)
				collides_snake = 1;

		if (!collides_snake)
			break;
	}
}

static void snake_update_board(void *gstate, char **board,
			       int row_count, int col_count)
{
	struct snake_state *state = gstate;
	int row = 0;
	int col = 0;
	int cell_index = 0;

	/* empty board */
	for (row = 0; row < row_count; row++)
		for (col = 0; col < col_count; col++)
			board[row][col] = ' ';

	/* draw borders */
	for (row = 0; row < row_count; row++) {
		board[row][0] = '#';
		board[row][col_count - 1] = '#';
	}

	for (col = 0; col < col_count; col++) {
		board[0][col] = '#';
		board[row_count - 1][col] = '#';
	}

	/* draw snake */
	for (cell_index = 0; cell_index < state->snake_len; cell_index++) {
		struct hilg_cell cell = state->snake[cell_index];
		board[cell.row][cell.col] = '*';
	}

	/* draw food */
	board[state->food.row][state->food.col] = '@';
}

static int snake_is_done(void *gstate)
{
	struct snake_state *state = gstate;
	return state->done;
}
