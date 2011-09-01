/* error.h
 *
 * Copyright (C) 2010 Tamas Nepusz
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

#ifndef __ERROR_H__
#define __ERROR_H__

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

enum {
	PLFIT_SUCCESS  = 0,
	PLFIT_FAILURE  = 1,
	PLFIT_EINVAL   = 2,
	PLFIT_UNDRFLOW = 3,
	PLFIT_OVERFLOW = 4
};

#define PLFIT_ERROR(reason,plfit_errno) \
	do {\
		plfit_error (reason, __FILE__, __LINE__, plfit_errno) ; \
		return plfit_errno ; \
	} while (0)

typedef void plfit_error_handler_t(const char*, const char*, int, int);

extern plfit_error_handler_t plfit_error_handler_abort;
extern plfit_error_handler_t plfit_error_handler_ignore;
extern plfit_error_handler_t plfit_error_handler_printignore;

plfit_error_handler_t* plfit_set_error_handler(plfit_error_handler_t* new_handler);

void plfit_error(const char *reason, const char *file, int line, int plfit_errno);
const char* plfit_strerror(const int plfit_errno);

void plfit_error_handler_abort(const char *reason, const char *file, int line,
		int plfit_errno);
void plfit_error_handler_ignore(const char *reason, const char *file, int line,
		int plfit_errno);
void plfit_error_handler_printignore(const char *reason, const char *file, int line,
		int plfit_errno);

__END_DECLS

#endif /* __ERROR_H__ */
