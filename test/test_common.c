/* test_common.c
 *
 * Copyright (C) 2011 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include "test_common.h"

size_t test_read_file(const char* fname, double* data, size_t max_n) {
	size_t n = 0;
	const char* prefixes[] = {
		"./",
		"../data/",
		"../../data/",
#ifdef DATADIR
		DATADIR "/" ,
#endif
		0 };
	const char** prefix_ptr;
	char fname_with_path[4096];
	FILE* f = 0;

	for (prefix_ptr = prefixes; *prefix_ptr && f == 0; prefix_ptr++) {
		snprintf(fname_with_path, sizeof(fname_with_path), "%s%s",
				*prefix_ptr, fname);
		f = fopen(fname_with_path, "r");
	}

	if (!f) {
		fprintf(stderr, "error: cannot find data file %s\n", fname);
		exit(1);
	}

	while (!feof(f) && n < max_n) {
		if (fscanf(f, "%lf", data+n)) {
			n++;
		}
	}

	fclose(f);
	return n;
}
