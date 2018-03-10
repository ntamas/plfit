/* vim:set ts=4 sw=4 sts=4 et: */
/* libversion.c
 *
 * Copyright (C) 2018 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "plfit.h"

extern
void plfit_libversion_get_numbers(int* major, int* minor, int* micro) {
	if (major) *major=PLFIT_VERSION_MAJOR;
	if (minor) *minor=PLFIT_VERSION_MINOR;
	if (micro) *micro=PLFIT_VERSION_MICRO;
    return ;
}

/*  return 0 for incompatible library
 *  return 1 for compatible and equal library
 *  return 2 for compatible but not equal library
 */
extern
int plfit_libversion_check_numbers(int major, int minor, int micro) {
	int status=0;

#if 0
	if (major < PLFIT_VERSION_MAJOR) {
		status=1;
		}
	else
#endif
	if (major == PLFIT_VERSION_MAJOR) {
		if (minor < PLFIT_VERSION_MINOR) {
			status=2;
			}
		else if (minor == PLFIT_VERSION_MINOR) {
			if (micro == PLFIT_VERSION_MICRO) {
				status=1;
				}
			else {
				status=2;
				}
			}
		}

	return (status); }

