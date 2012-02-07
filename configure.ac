#                                               -*- Autoconf -*-
# Copyright (c) 2012 Jess VanDerwalker <washu@sonic.net>
#
# Basic outline created with autoscan
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([xtoq], [0.1])
AC_CONFIG_SRCDIR([src/xtoq.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_SEARCH_LIBS([xcb_connect], [xcb])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 xtoq/xtoq/Makefile])
AC_OUTPUT
