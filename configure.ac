#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([svarog], [0.0.6], [pawel.f.biernacki@gmail.com],[],[www.perkun.org])
AC_CONFIG_SRCDIR([src/kuna.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC
LT_INIT

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

AC_CONFIG_MACRO_DIRS([m4])


# Checks for libraries.
AC_SUBST([LIBREADLINE], ["-lreadline -lncurses"])
AC_DEFINE([HAVE_LIBREADLINE], [1], [Define if you have libreadline])


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile 
	inc/Makefile 
	etc/Makefile 
	examples/Makefile 
	perl/Makefile 
	src/Makefile 
	libsvarog.pc])
AC_OUTPUT
