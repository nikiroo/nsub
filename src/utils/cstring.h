/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2013 Niki Roo
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
 * @file cstring.h
 * @author Niki
 * @date 2013 - 2022
 *
 * @brief Some string utility functions
 *
 * This file implements some basic functions of a string, most often by working
 * directly with <tt>char *</tt> (but when needed with the provided
 * <tt>cstring</tt> object).
 */

#ifndef CSTRING_H
#define CSTRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * @brief A NUL-byte terminated string, with a known length
 *
 * The structure contains a suite of characters terminated by a NUL byte
 * (or just a NUL byte), and a length.
 * It is advised NOT to modify either of those directly.
 * You can use <tt>cstring_convert</tt> to get a <tt>char *</tt>, though
 * (in this case, the cstring <b>MUST NOT</b> be used again, and you are
 * responsible for freeing said <tt>char *</tt>).
 *
 * @see cstring_convert
 */
typedef struct {
	char CNAME[10];
	char *string;
	size_t length;
	void *priv;
} cstring_t;

/**
 * Instantiate a new cstring.
 *
 * Create (and allocate the memory for) a new cstring.
 * Do not forget to call cstring_free(cstring) when done.
 */
cstring_t *new_cstring();

/**
 * Free the given cstring.
 *
 * Free all the resources allocated for this cstring.
 *
 * @param self the cstring to free, which MUST NOT be used again afterward
 */
void free_cstring(cstring_t *self);

/**
 * Grow the cstring to accommodate that many characters in addition to those
 * already held, if needed.
 *
 * @param self the string to grow
 * @param min the minimum number of extra characters it should be able to hold
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_grow(cstring_t *self, int min_extra);

/**
 * Grow the cstring to accommodate that many characters in total, if needed.
 *
 * @param self the string to grow
 * @param min the minimum number of characters it should be able to hold
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_grow_to(cstring_t *self, int min_buffer);

/**
 * Compact the memory used by this string.
 *
 * @param self the string to work on
 */
void cstring_compact(cstring_t *self);

/**
 * Add a char at the end of the given cstring.
 *
 * @param self the cstring to work on
 * @param source the character to add
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_add_car(cstring_t *self, char source);

/**
 * Add a string (a sequence of char that MUST end with '\0') at the end of the
 * given cstring.
 *
 * @param self the string to work on
 * @param source the string to add
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_add(cstring_t *self, const char source[]);

/**
 * Add a string (a sequence of char that MUST end with '\0') at the end of the
 * given cstring, starting from index.
 *
 * @param self the cstring to work on
 * @param source the string to add
 * @param idx the starting index at which to copy from the source
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addf(cstring_t *self, const char source[], size_t idx);

/**
 * Add a string (a sequence of char that MAY end with '\0') at the end of the
 * given string, up to N chars long.
 *
 * @param self the string to work on
 * @param source the string to add
 * @param n the maximum number of chars to add (excluding the NUL byte), or 0
 * 		to add the whole source (which MUST then end with a '\0')
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addn(cstring_t *self, const char source[], size_t n);

/**
 * Add a string (a sequence of char that MAY end with '\0') at the end of the
 * given cstring, starting from index, up to N chars long.
 *
 * @param self the string to work on
 * @param source the string to add
 * @param idx the starting index at which to copy from the source
 * @param n the maximum number of chars to add (excluding the NUL byte) or 0
 * 		to add the whole source (which MUST then end with a '\0')
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addfn(cstring_t *self, const char source[], size_t idx, size_t n);

/**
 * Add a string via the usual <tt>printf</tt> formatters.
 *
 * @param self the string to add to
 * @param fmt the required format specifiers (@{see printf})
 * @param ... the printf-format parameters
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addp(cstring_t *self, const char *fmt, ...);

/**
 * Cut the string at the given size if it is greater.
 *
 * E.g.: it will have (at most) this many characters (without counting NULL) in
 * it after.
 *
 * @param self the string to work on
 * @param size the size to cut at (the maximum size of the cstring after this
 * 		operation, NUL excepted)
 */
void cstring_cut_at(cstring_t *self, size_t size);

/**
 * Create a substring of this one.
 *
 * @param self the string to work on
 * @param start the index to start at
 * @param length the number of characters to copy, 0 for 'up to the end'
 *
 * @return a newly allocated cstring
 */
cstring_t *cstring_substring(const char self[], size_t start, size_t length);

/**
 * Split a cstring into "smaller" cstrings every time the given separator is found.
 * Will also allow empty fields, ie: "a,b,,c" will return 4 cstrings, the third being empty).
 *
 * @param self the cstring to work on
 * @param delim the separator, which can be longer than one character
 *
 * @return a list of cstring
 */
//TODO: use a []
//clist *cstring_split(cstring *self, cstring *delim, cstring *quote);
/**
 * Split a cstring into "smaller" cstrings every time the given separator (which MUST end in \0) is found.
 * Will also allow empty fields, ie: "a,b,,c" will return 4 cstrings, the third being empty).
 *
 * @param self the cstring to work on
 * @param delim the separator, which can be longer than one character and MUST end with \0
 *
 * @return a list of cstring
 */
//TODO: use a []
//clist *cstring_splits(cstring *self, const char delim[], const char quote[]);
/**
 * Split a cstring into "smaller" cstrings every time the given separator is found.
 * Will also allow empty fields, ie: "a,b,,c" will return 4 cstrings, the third being empty).
 *
 * @param self the cstring to work on
 * @param delim the separator
 *
 * @return a list of cstring
 */
//TODO: use a []
//clist *cstring_splitc(cstring *self, char delim, char quote);
/**
 * Reverse the given string.
 *
 * @param self the string to work on
 */
void cstring_reverse(char *self);

/**
 * Replace all occurrences of a string inside the given cstring by another.
 *
 * @param self the string to work on
 * @param from the string to replace
 * @param to the replacement string
 *
 * @return the number of occurrences changed
 */
int cstring_replace(cstring_t *self, const char from[], const char to[]);

/**
 * Replace all occurrences of a char inside the given string by another.
 *
 * @param self the string to work on
 * @param from the char to replace
 * @param to the replacement char
 *
 * @return the number of occurrences changed
 */
int cstring_replace_car(char *self, char from, char to);

/**
 * Check if the string starts with the given pattern.
 *
 * @param self the string to work on
 * @param find the string to find
 * @param start_idx the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_starts_with(const char self[], const char find[], size_t start_idx);

/**
 * Check if the string ends with the given pattern.
 *
 * @param self the string to work on
 * @param find the string to find (if empty, will always be found)
 * @param start_index the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_ends_with(const char self[], const char find[]);

/**
 * Find the given substring in this one.
 *
 * @param self the string to work on
 * @param find the string to find
 * @param start_index the index at which to start the search
 *
 * @return the start index of the found string if found, or a negative value
 * if not
 */
long cstring_find(const char self[], const char find[], size_t rstart_index);

/**
 * Find the given substring in this one, but search in the reverse direction.
 *
 * @param self the string to work on
 * @param find the string to find
 * @param rstart_index the index at which to start the search, or 0 for
 *        "end of string" (remember that it is reverse, you would then never
 *        find anything with a real rstart_index of 0), or a negative value
 *        to count from the end of the string (-2 means 2 character before the
 *        end)
 *
 * @return the start index of the found string if found, or a negative value
 * if not
 */
long cstring_rfind(char self[], const char find[], long rstart_index);

/**
 * Clear (truncate its size to 0) the given string.
 *
 * @param self the string to work on
 */
void cstring_clear(cstring_t *self);

/**
 * Convert this cstring into a string
 * This means that you MUST NOT call cstring_free nor use the cstring anymore.
 * NULL will return NULL.
 *
 * @param self the cstring to work on
 */
char *cstring_convert(cstring_t *self);

/**
 * Clone this string.
 * NULL will return NULL.
 *
 * @param self the string to clone
 */
cstring_t *cstring_clone(const char self[]);

/**
 * Trim this cstring of all trailing 'car' instances.
 *
 * @param self the cstring to work on
 * @param car the character to trim (right only)
 *
 * @return a right trimmed cstring
 */
void cstring_rtrim(cstring_t *self, char car);

/**
 * Trim this cstring of all 'car' instances from the start and/or the
 * end of the string.
 * 
 * @param self the cstring to work on
 * @param car the character to trim
 * 
 * @return a trimmed cstring
 */
void cstring_trim(cstring_t *self, char car);

/**
 * Remove the \r and \n sequence (or one OR the other) at the end of the string.
 *
 * @param self the string to change
 *
 * @return the new length of the string
 */
size_t cstring_remove_crlf(char *self);

/**
 * Change the case to upper-case (UTF-8 compatible, but the string MUST be
 * whole).
 *
 * @note: if LC_ALL is not set or is set to C and a viable $LANG exists, it will
 * 		set LC_ALL to $LANG
 *
 * @param self the cstring to work on
 */
void cstring_toupper(cstring_t *self);

/**
 * Change the case to lower-case (UTF-8 compatible, but the string MUST be
 * whole).
 *
 * @note: if LC_ALL is not set or is set to C and a viable $LANG exists, it will
 * 		set LC_ALL to $LANG
 *
 * @param self the cstring to work on
 */
void cstring_tolower(cstring_t *self);

/**
 * Read a whole line (CR, LN or CR+LN terminated) from the given file stream.
 *
 * @param self the cstring to read into
 * @param file the file to read
 *
 * @return 1 if a line was read, 0 if not
 */
int cstring_readline(cstring_t *self, FILE *file);

/**
 * Add a path to the given cstring (if it is currently empty, it
 * will result in a root path).
 *
 * Will be separated by a forward '/' except on non-standard systems
 * that uses reverse slash (i.e., Windows).
 *
 * @param self the base cstring (empty for a root path)
 * @param subpath the path component to add
 */
void cstring_add_path(cstring_t *self, const char subpath[]);

/**
 * Remove the <tt>how_many</tt> components of the path described by this
 * cstring. Will ignore extra path separators and always trim it from the final
 * result (i.e., <tt>some//path/</tt> is identical to <tt>some/path</tt>).
 *
 * @note popping "0" path will simply make sure the string does not end in "/"
 *
 * @param how_many how many path components to remove (for instance, to go from
 * 		<tt>/some/path/to/file</tt> to <tt>/some/path</tt> you would need 2)
 */
int cstring_pop_path(cstring_t *self, int how_many);

/**
 * Return the basename component of this path (for instance,
 * '/home/user/file.ext' becomes 'file.ext').
 *
 * @param path the path to get the dir of (it can be a dir itself)
 * @param ext the extension to remove if any (can be empty or NULL for none)
 *
 * @note the extension should include the "." if any
 *
 * @return a new string representing the parent directory
 */
char *cstring_basename(const char path[], const char ext[]);

/**
 * Return the dirname of this path (for instance,
 * '/home/user/file.ext' becomes '/home/user').
 *
 * @param path the path to get the dir of (it can be a dir itself)
 *
 * @return a new string representing the parent directory
 */
char *cstring_dirname(const char path[]);

/**
 * Check if the string is a correct and whole UTF-8 string (i.e., it is indeed
 * an UTF-8 string and doesn't contain incomplete UTF-8 sequences).
 * 
 * @return TRUE if it is UTF-8
 */
int cstring_is_utf8(cstring_t *self);

#endif

#ifdef __cplusplus
}
#endif

