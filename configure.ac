dnl Process this file with autoconf to produce a configure script.
AC_COPYRIGHT([
Copyright (C) 2010-2018 Tamas Nepusz

This file is part of plfit project.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
])
dnl
AC_PREREQ([2.69])
AC_INIT([plfit],
	m4_esyscmd_s([sed -ne 's|.*PLFIT_VERSION_STRING.*"\(.*\)"|\1|p' src/plfit.h]),
	[https://github.com/ntamas/plfit/issues],
	[],
	[https://github.com/ntamas/plfit])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/plfit.c])
AC_CONFIG_HEADERS([src/config.h:src/plfit_config.h.in])
AC_USE_SYSTEM_EXTENSIONS
AC_DISABLE_STATIC([])
AM_INIT_AUTOMAKE([gnu])
AM_MAINTAINER_MODE

dnl Package versionning:
PLFIT_VERSION_MAJOR=${PACKAGE_VERSION%-*}.0.0.0
PLFIT_VERSION_MINOR=${PLFIT_VERSION_MAJOR#*.}
PLFIT_VERSION_MICRO=${PLFIT_VERSION_MINOR#*.}
PLFIT_VERSION_MAJOR=${PLFIT_VERSION_MAJOR%%.*}
PLFIT_VERSION_MINOR=${PLFIT_VERSION_MINOR%%.*}
PLFIT_VERSION_MICRO=${PLFIT_VERSION_MICRO%%.*}

dnl Library versionning:
##PLFIT_LT_CURRENT=0
##PLFIT_LT_REVISION=0
##PLFIT_LT_AGE=0
##PLFIT_LT_VERSION=${PLFIT_LT_CURRENT}:${PLFIT_LT_REVISION}:${PLFIT_LT_AGE}
PLFIT_LT_RELEASE=${PLFIT_VERSION_MAJOR}.${PLFIT_VERSION_MINOR}.${PLFIT_VERSION_MICRO}
##AC_SUBST(PLFIT_LT_VERSION)
AC_SUBST(PLFIT_LT_RELEASE)

dnl Checks for programs:
AC_PROG_CC

dnl LibTool setup:
LT_INIT

dnl Checks for linker script support:
gl_LD_VERSION_SCRIPT

dnl Checks for libraries:
dnl Math library
LT_LIB_M

dnl Checks for header files:
AC_CHECK_HEADERS([float.h limits.h memory.h stdint.h stdlib.h string.h])
AC_CHECK_HEADERS([emmintrin.h malloc.h])

dnl Checks for typedefs, structures, and compiler characteristics:
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

dnl Checks for library functions:
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor memmove memset pow sqrt strchr])

dnl Checks compliler options:
AX_APPEND_COMPILE_FLAGS(-std=c11)
##AX_APPEND_COMPILE_FLAGS(-Werror)
AX_APPEND_COMPILE_FLAGS(-Wall)
AX_APPEND_COMPILE_FLAGS(-W)
AX_APPEND_COMPILE_FLAGS(-Wno-unused-function)
AX_APPEND_COMPILE_FLAGS(-Wno-unused-parameter)
AX_APPEND_COMPILE_FLAGS(-Wno-unused-value)
#AX_APPEND_COMPILE_FLAGS(-g)
##AX_APPEND_COMPILE_FLAGS(-O2)
AX_APPEND_COMPILE_FLAGS(-O3)

AC_CONFIG_FILES([
	plfit.pc
	Makefile
	src/Makefile
	test/Makefile
	data/Makefile
	doc/Makefile
	])

AC_OUTPUT

dnl eos
