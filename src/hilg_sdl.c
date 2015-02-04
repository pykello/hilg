#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <hilg.h>

#define DEFAULT_TILE_SIZE 48

int color_map [][3] = {
	[' '] = {0, 0, 0},
	['#'] = {0, 128, 0},
	['*'] = {128, 0, 0},
	['@'] = {200, 200, 200}
};

/* forward declarations */
static char ** allocate_board(int row_count, int col_count);
static void free_board(char **board, int row_count);
static SDL_Window * create_window(struct hilg_game_info *game_info);
static void draw_board(SDL_Window *window, char **board,
		       int row_count, int col_count);
static void dispatch_keyboard_events(struct hilg_game_info *game_info);
static void dispatch_timer_events(struct hilg_game_info *game_info,
				  struct timespec *previous_tick);
static int diff_timespec_ms(struct timespec end, struct timespec begin);

void hilg_run(struct hilg_game_info *game_info)
{
	struct timespec previous_tick = {0, 0};
	SDL_Window *window = NULL;

	int row_count = game_info->row_count;
	int col_count = game_info->col_count;
	char **board = allocate_board(row_count, col_count);

	SDL_Init(SDL_INIT_VIDEO);

	window = create_window(game_info);
	if (window == NULL)
		return;

	while (!game_info->is_done_func(game_info->game_state))
	{
		game_info->update_board_func(game_info->game_state,
					     board, row_count, col_count);

		draw_board(window, board, row_count, col_count);

		dispatch_keyboard_events(game_info);
		dispatch_timer_events(game_info, &previous_tick);

		/* avoid 100% cpu */
		usleep(5000);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
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

static SDL_Window * create_window(struct hilg_game_info *game_info)
{
	SDL_Window *window = NULL;

	int width = game_info->col_count * DEFAULT_TILE_SIZE + 20;
	int height = game_info->row_count * DEFAULT_TILE_SIZE + 20;

	window = SDL_CreateWindow(game_info->title, SDL_WINDOWPOS_UNDEFINED,
				  SDL_WINDOWPOS_UNDEFINED, width, height,
				  SDL_WINDOW_OPENGL);

	return window;
}

static void draw_board(SDL_Window *window, char **board,
		       int row_count, int col_count)
{
	int row = 0;
	int col = 0;

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	for (row = 0; row < row_count; row++)
		for (col = 0; col < col_count; col++) {
			SDL_Rect rect = {
				.x = col * DEFAULT_TILE_SIZE + 12,
				.y = row * DEFAULT_TILE_SIZE + 12,
				.w = DEFAULT_TILE_SIZE - 4,
				.h = DEFAULT_TILE_SIZE - 4
			};

			int code = board[row][col];
			int color = SDL_MapRGB(screen->format,
					       color_map[code][0],
					       color_map[code][1],
					       color_map[code][2]);

			SDL_FillRect(screen, &rect, color);
		}

	SDL_UpdateWindowSurface(window);
}

static void dispatch_keyboard_events(struct hilg_game_info *game_info)
{
	SDL_Event sdl_event;
	struct hilg_event event;
	int keycode = 0;

	while (SDL_PollEvent(&sdl_event)) {
		if (sdl_event.type != SDL_KEYDOWN)
			continue;

		switch (sdl_event.key.keysym.sym) {
		case SDLK_UP:
			keycode = KEY_UP;
			break;
		case SDLK_DOWN:
			keycode = KEY_DOWN;
			break;
		case SDLK_LEFT:
			keycode = KEY_LEFT;
			break;
		case SDLK_RIGHT:
			keycode = KEY_RIGHT;
			break;
		default:
			keycode = sdl_event.key.keysym.sym;
			break;
		}

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
