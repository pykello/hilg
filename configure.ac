#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([maj_ver], [0])
m4_define([med_ver], [1])
m4_define([min_ver], [0])
m4_define([so_version], [0:1:0])
m4_define([hilg_version], [maj_ver.med_ver.min_ver])

AC_PREREQ([2.69])
AC_INIT([hilg], [hilg_version], [hadi@moshayedi.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([ncurses], [initscr])
AC_CHECK_LIB([SDL2], [SDL_Init])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime])

AC_CONFIG_FILES([Makefile src/Makefile examples/Makefile])
AC_OUTPUT
