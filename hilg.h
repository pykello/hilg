
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

extern void hilg_run(struct hilg_game_info *game_info);

#endif
