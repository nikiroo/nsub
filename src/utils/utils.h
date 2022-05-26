/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2020 Niki Roo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file utils.h
 * @author Niki
 * @date 2020 - 2022
 *
 * @brief Include all the other .h as well as C99-compatible
 * <tt>strdup</tt>/<tt>strnlen</tt> functions if they are not already defined
 */
#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cstring.h"
#include "array.h"
#include "desktop.h"
#include "print.h"
#include "timing.h"

/* Helps with C99 compatibility for code that is not */

#if _POSIX_C_SOURCE < 200809L && _XOPEN_SOURCE < 500
#ifndef _GNU_SOURCE
/**
 * The strnlen() function returns the number of bytes in the string pointed to
 * by s, excluding the terminating null byte ('\0'), but at most maxlen.
 * In doing this, strnlen() looks only at the first maxlen characters in the
 * string pointed to by s and never beyond s[maxlen-1].
 *
 * @return The strnlen() function returns strlen(s), if that is less than
 * 		maxlen, or maxlen if there is no null terminating ('\0') among the first
 * 		maxlen characters pointed to by s
 */
size_t strnlen(const char *s, size_t maxlen);
#endif
#endif
#if _POSIX_C_SOURCE < 200809L && _XOPEN_SOURCE < 500
/**
 * The strdup() function returns a pointer to a new string which is a
 * duplicate of the string s.  Memory for the new string is obtained with
 * malloc(3), and can be freed with free(3).
 *
 *
 * @return On success, the strdup() function returns a pointer to the duplicated
 *     string.  It returns NULL if insufficient memory was available, with
 *     errno set to indicate the error.
 */
char *strdup(const char *source);
#endif

/* */

#endif // UTILS_H

#ifdef __cplusplus
}
#endif

