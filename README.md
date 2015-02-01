# hilg

hilg is game engine which makes development & testing of the games which can be represented as grids extremely easy.

## Philosophy

When beginners want to create games in C, dealing with how to do input and output might be too confusing for them.
Hilg handles the game input/output, and all it requires from the programmer is to implement the game logic.

## Installing and Running Examples

hilg requires ncurses to compile and run:

```
# Fedora, Redhat, CentOS
sudo yum install ncurses-devel
```

Now, you can compile and install the library:

```
./autogen.sh
./configure
make
sudo make install
sudo ldconfig
```

Currently hilg comes with two examples. To run them:

```
# Run the box mover example
box_mover_ncurses

# Run the snake example
snake_ncurses
```

## Interface
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

## Tutorials?
If you are looking for a tutorial, we recommend that you study the source code of the examples that come with hilg:
* [box mover](examples/box_mover.c)
* [snake](examples/snake.c)

## Utility types, macros, etc.

**struct hilg_cell** can be used to represent a cell in a 2d grid, or a 2d direction. To define and initialize
a ```hilg_cell``, you can do:

```C
struct hilg_cell some_cell = {.row = 1, .col = 1};
```

hilg comes with a few constants of type ```hilg_cell```:

```C
const struct hilg_cell DIRECTION_UP = {.row = -1, .col = 0};
const struct hilg_cell DIRECTION_DOWN = {.row = 1, .col = 0};
const struct hilg_cell DIRECTION_LEFT = {.row = 0, .col = -1};
const struct hilg_cell DIRECTION_RIGHT = {.row = 0, .col = 1};
```

and a few useful macros:

* ```cell_equals(c1, c2)``` which tells if c1 is equal to c2,
* ```cell_add(c1, c2)``` which adds the two cells together. This is most useful when c2 represents a direction.
* ```cell_on_border(c, rows, cols)``` which tells if c is on border of a rows*cols grid or not.
* ```opposite_direction(c1, c2)``` which tells if c1 and c2 are opposite directions.
* ```key_to_direction(k)``` which converts ```KEY_UP```, ```KEY_DOWN```, ```KEY_LEFT```, and ```KEY_RIGHT``` to the
  corresponding direction values.
 
**struct hilg_key_queue** can be used if you need to keep a queue of keycodes. You can initialize it with
the constant ```KEY_QUEUE_EMPTY```.

It also comes with the operations of a standard queue:
* ```queue_empty(q)```
* ```queue_full(q)```
* ```queue_push(q, keycode)```
* ```queue_peek(q)```
* ```queue_pop(q)```


