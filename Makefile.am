## plfit/Makefile.am -- AutoMake master metafile
## Process this file with automake to produce Makefile.in
## (in this top directory and all subdirectories).
##
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

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = \
	gnu \
	dist-xz \
	dist-bzip2 \
	dist-zip \
	foreign

SUBDIRS = src test

DIST_SUBDIRS = $(SUBDIRS) data doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = plfit.pc

EXTRA_DIST = README.rst

export TAR_OPTIONS = --owner=0 --group=0 --numeric-owner --sort=name

##
## eos
