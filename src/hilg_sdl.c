#include <SDL2/SDL.h>

#include <hilg.h>

void hilg_run(struct hilg_game_info *game_info)
{
	SDL_Window *window = NULL;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		640,                               // width, in pixels
		480,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags
	);

	if (window == NULL)
		return;

	SDL_Delay(2000);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
