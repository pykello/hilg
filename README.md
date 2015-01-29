# hilg

hilg is game engine which makes development & testing of the games which can be represented as grids extremely easy.

## The Philosophy

When beginners want to create games in C, dealing with how to do input and output might be too confusing for them.
Hilg handles the game input/output, and all it requires from the programmer is to implement the game logic.
We beleive this separation has several benefits:

* Makes developing simple games extremely easy for absolute beginner C programmers,
* Makes automated testing of these games very easy,
* ...

## Installing and Running Examples

hilg requires ncurses to compile and run:

```
# Fedora, Redhat, CentOS
sudo yum install ncurses-devel
```

Now, you can compile the library and the examples by running make:

```
make
```

Currently hilg comes with two examples. To run them:

```
# Run the box mover example
./box_mover

# Run the snake example
./snake
```

## The Interface
To create a game in hilg, you need to fill-in a ```hilg_game_info``` structure, and pass it to the ```hilg_run()``` function:

```C
struct hilg_game_info game_info = {
    .row_count = /* game board's row count */,
    .col_count = /* game board's column count */,
    .timer_interval = /* how often (in milliseconds) to generate events. 0 for never. */,
    .game_state = /* a pointer to game state, which will be passed to callback functions */,
    .handle_event_func = /* pointer to a function which handles events and updates game state */,
    .update_board_func = /* pointer to a function which converts game state to a board */,
    .is_done_func = /* pointer to a function which tells if game is finished or not */
}

hilg_run(&game_info);
```

Your program should implement three functions which are passed as ```.handle_event_func```,
```.update_board_func```, and ```.is_done_func``` pointers.

The signature of ```.handle_event_func``` should be similar to:

```C
void handle_event(void *gstate, struct hilg_event *event)
{
  ...
}
```

This function receives as the first argument the ```.game_state``` that you passed as part of ```game_info```,
and an event. Its goal is to update the ```gstate``` based on the ```event```. ```struct hilg_event``` is defined as:

```C
struct hilg_event {
	enum event_type type; /* KEYPRESS, TIMER */
	union event_data {
		int keycode;
	} data;
};
```

The signature of ```.update_event_func``` should be similar to:

```C
void update_board(void *gstate, char **board, int row_count, int col_count)
{
  ...
}
```

This function receives as the first argument the ```.game_state``` that you passed as part of ```game_info```,
a board, and its dimensions. Its goal is to update the already allocated ```board``` based on ```gstate```.

The signature of ```.is_done_func``` should be similar to:

```C
int is_done(void *gstate)
{
  ...
}
```

This function receives as the first argument the ```.game_state``` that you passed as part of ```game_info```,
and returns 1 if is game is finished, and 0 otherwise.
