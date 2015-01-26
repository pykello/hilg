
#ifndef HILG_H
#define HILG_H

enum event_type {
	KEYPRESS,
	TIMER
};

struct hilg_event {
	enum event_type type;
	union event_data {
		int keycode;
	} data;
};

struct hilg_game_info {
	int row_count;
	int col_count;
	int timer_interval;
	void *game_state;

	void (*handle_event_func)(void *, struct hilg_event *);
	void (*update_board_func)(void *, char **, int, int);
	int (*is_done_func)(void *);
};

struct hilg_state {
	struct hilg_game_info *game_info;
	char **board;
};

extern struct hilg_state * hilg_init(struct hilg_game_info *game_info);
extern void hilg_run(struct hilg_state *state);

#define hilg_init_and_run(game_info_ptr) hilg_run(hilg_init(game_info_ptr))

#endif
