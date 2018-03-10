## plfit/test/Makefile.am -- AutoMake metafile
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

check_PROGRAMS = \
	test_continuous \
	test_discrete \
	test_gss \
	test_kolmogorov \
	test_real \
	test_sampling \
	test_underflow_handling

TESTS = $(check_PROGRAMS)

AM_CFLAGS = \
	-DDATADIR=\"$(abs_top_srcdir)/data\" \
	-I$(top_srcdir)/src

LDADD = \
	$(top_builddir)/src/libplfit.la \
	$(LIBM)

test_continuous_SOURCES = test_continuous.c test_common.c
test_discrete_SOURCES = test_discrete.c test_common.c
test_gss_SOURCES = test_gss.c test_common.c
test_kolmogorov_SOURCES = test_kolmogorov.c test_common.c
test_real_SOURCES = test_real.c test_common.c
test_sampling_SOURCES = test_sampling.c test_common.c
test_underflow_handling_SOURCES = test_underflow_handling.c test_common.c

noinst_HEADERS = test_common.h

##
## eos
