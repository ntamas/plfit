## plfit/src/Makefile.am -- AutoMake metafile
## plfit - fitting power-law distributions to empirical data
## Copyright (C) 2018 Tamas Nepusz
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
##

LT_INFORMATION = -release $(PLFIT_LT_RELEASE)
#LT_INFORMATION = -version-info $(PLFIT_LT_VERSION)

lib_LTLIBRARIES = libplfit.la

bin_PROGRAMS = plfit plgen

include_HEADERS = \
	plfit.h \
	plfit_mt.h

noinst_HEADERS = \
	arithmetic_ansi.h \
	arithmetic_sse_double.h \
	arithmetic_sse_float.h \
	error.h \
	gss.h \
	hzeta.h \
	kolmogorov.h \
	lbfgs.h \
	platform.h \
	sampling.h \
	getopt.h

libplfit_la_SOURCES = \
	error.c \
	gss.c \
	kolmogorov.c \
	lbfgs.c \
	mt.c \
	platform.c \
	plfit.c \
	options.c \
	rbinom.c \
	sampling.c \
	stats.c \
	hzeta.c \
	version.c \
	libversion.c

libplfit_la_LDFLAGS = \
	$(LT_INFORMATION)

libplfit_la_LIBADD = \
	$(LIBM)

plfit_SOURCES = \
	main.c

plfit_LDADD = \
	libplfit.la \
	$(LIBM)

plgen_SOURCES = \
	plgen.c

plgen_LDADD = \
	libplfit.la \
	$(LIBM)

##
## eos
