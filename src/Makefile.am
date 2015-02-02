libhilg_common_cflags = -I$(top_srcdir)/include \
			-Wall -W -Wno-unused-parameter -Wno-pointer-sign

lib_LTLIBRARIES = libhilg_ncurses.la libhilg_sdl.la

libhilg_ncurses_la_SOURCES = hilg_ncurses.c
libhilg_ncurses_la_CFLAGS = $(libhilg_common_cflags)

libhilg_sdl_la_SOURCES = hilg_sdl.c
libhilg_sdl_la_CFLAGS = $(libhilg_common_cflags)

include_HEADERS = $(top_srcdir)/include/hilg.h
