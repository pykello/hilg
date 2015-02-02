AM_CFLAGS = -I$(top_srcdir)/include \
	    -Wall -W -Wno-unused-parameter -Wno-pointer-sign

hilg_ncurses_ldadd = $(top_builddir)/src/libhilg_ncurses.la
hilg_sdl_ldadd = $(top_builddir)/src/libhilg_sdl.la

bin_PROGRAMS = box_mover_ncurses snake_ncurses box_mover_sdl snake_sdl

box_mover_ncurses_SOURCES = box_mover.c
box_mover_ncurses_LDADD = $(hilg_ncurses_ldadd)

snake_ncurses_SOURCES = snake.c
snake_ncurses_LDADD = $(hilg_ncurses_ldadd)

box_mover_sdl_SOURCES = box_mover.c
box_mover_sdl_LDADD = $(hilg_sdl_ldadd)

snake_sdl_SOURCES = snake.c
snake_sdl_LDADD = $(hilg_sdl_ldadd)
