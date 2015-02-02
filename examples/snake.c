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
	int score;
	struct hilg_cell direction;
	struct hilg_cell food;
	struct hilg_key_queue key_queue;
};

/* forward declarations */
static void snake_update_board(void *state, char **board,
			       int row_count, int col_count);
static void snake_update_sidebar(void *state, int id, char *field_str);
static int snake_is_done(void *state);
static void snake_handle_event(void *state, struct hilg_event *event);
static void snake_handle_keypress_event(struct snake_state *state,
					struct hilg_event *event);
static void snake_handle_timer_event(struct snake_state *state);
static void extend_snake(struct snake_state *state);
static void generate_food(struct snake_state *state);

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
		.food = {.row = ROWS_MAX / 2, .col = COLS_MAX / 2},
		.key_queue = KEY_QUEUE_EMPTY
	};

	struct hilg_game_info game_info = {
		.title = "Snake",
		.row_count = game_state.row_count,
		.col_count = game_state.col_count,
		.timer_interval = TIMER_INTERVAL,
		.sidebar_fields = 1,
		.sidebar_length = 10,
		.game_state = &game_state,
		.handle_event_func = snake_handle_event,
		.update_board_func = snake_update_board,
		.is_done_func = snake_is_done,
		.update_sidebar_func = snake_update_sidebar
	};

	hilg_run(&game_info);

	return 0;
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

static void snake_update_sidebar(void *gstate, int id, char *field_str)
{
	struct snake_state *state = gstate;
	sprintf(field_str, "score: %d", state->score);
}

static int snake_is_done(void *gstate)
{
	struct snake_state *state = gstate;
	return state->done;
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
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
		queue_push(state->key_queue, event->data.keycode);
		break;
	}
}

static void snake_handle_timer_event(struct snake_state *state)
{
	int cell_index = 0;
	struct hilg_cell *snake = state->snake;
	int snake_len = state->snake_len;
	int last_cell = snake_len - 1;

	while (!queue_empty(state->key_queue)) {
		struct hilg_cell new_direction;

		int keycode = queue_peek(state->key_queue);
		queue_pop(state->key_queue);

		new_direction = key_to_direction(keycode);

		if (!opposite_direction(state->direction, new_direction)) {
			state->direction = new_direction;
			break;
		}
	}

	for (cell_index = 0; cell_index < last_cell; cell_index++)
		snake[cell_index] = snake[cell_index + 1];

	snake[last_cell] = cell_add(snake[last_cell], state->direction);

	if (cell_on_border(snake[last_cell], state->row_count, state->col_count))
		state->done = 1;

	if (cell_equals(snake[last_cell], state->food)) {
		extend_snake(state);
		generate_food(state);
		state->score += 1;
	}
}

static void extend_snake(struct snake_state *state)
{
	int cell_index = 0;

	if (state->snake_len == SNAKE_LEN_MAX)
		return;

	state->snake_len += 1;

	for (cell_index = state->snake_len - 1; cell_index > 0; cell_index--)
		state->snake[cell_index] = state->snake[cell_index - 1];
}

static void generate_food(struct snake_state *state)
{
	int collides_snake = 0;

	do {
		int cell_index = 0;

		state->food = (struct hilg_cell) {
			.row = rand() % (state->row_count - 2) + 1,
			.col = rand() % (state->col_count - 2) + 1
		};

		collides_snake = 0;
		for (cell_index = 0; cell_index < state->snake_len; cell_index++)
			if (cell_equals(state->food, state->snake[cell_index]))
				collides_snake = 1;

	} while (collides_snake);
}

