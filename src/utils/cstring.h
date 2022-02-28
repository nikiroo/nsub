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

#ifndef CSTRING_H
#define CSTRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * This is a cstring. It contains a suite of characters terminated by a NUL byte
 * (or just a NUL byte), and a length.
 * It is advised NOT to modify either of those directly.
 * You can use cstring_convert to get a char*, though (in this case, the cstring
 * MUST NOT be used again, and you are responsible for freeing the said char*).
 */
typedef struct cstring_struct cstring;
typedef struct cstring_private_struct cstring_private;

struct cstring_struct {
	char CNAME[32];
	char *string;
	size_t length;
	cstring_private *priv;
};

/**
 * Instantiate a new cstring.
 *
 * Create (and allocate the memory for) a new cstring.
 * Do not forget to call cstring_free(cstring) when done.
 */
cstring *new_cstring();

/**
 * Free the given cstring.
 *
 * Free all the resources allocated for this cstring.
 *
 * @param self the cstring to free, which MUST NOT be used again afterward
 */
void free_cstring(cstring *self);

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
int cstring_grow(cstring *self, int min_extra);

/**
 * Grow the cstring to accommodate that many characters in total, if needed.
 *
 * @param self the string to grow
 * @param min the minimum number of characters it should be able to hold
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_grow_to(cstring *self, int min_buffer);

/**
 * Add a char at the end of the given cstring.
 *
 * @param self the cstring to work on
 * @param source the character to add
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_add_car(cstring *self, char source);

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
int cstring_add(cstring *self, const char source[]);

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
int cstring_addf(cstring *self, const char source[], size_t idx);

/**
 * Add a string (a sequence of char that MAY end with '\0') at the end of the
 * given cstring, up to N chars long.
 *
 * @param self the string to work on
 * @param source the string to add
 * @param n the maximum number of chars to add (excluding the NUL byte)
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addn(cstring *self, const char source[], size_t n);

/**
 * Add a string (a sequence of char that MAY end with '\0') at the end of the
 * given cstring, starting from index, up to N chars long.
 *
 * @param self the string to work on
 * @param source the string to add
 * @param idx the starting index at which to copy from the source
 * @param n the maximum number of chars to add (excluding the NUL byte)
 *
 * @return TRUE if success (FALSE means it was unable to grow due to memory
 * 		pressure)
 */
int cstring_addfn(cstring *self, const char source[], size_t idx, size_t n);

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
int cstring_addp(cstring *self, const char *fmt, ...);

/**
 * Cut the string at the given size if it is greater.
 * E.g.: it will have (at most) this many characters (without counting NUL) in it after.
 *
 * @param self the string to work on
 * @param size the size to cut at (the maximum size of the cstring after this operation, NUL excepted)
 */
void cstring_cut_at(cstring *self, size_t size);

/**
 * Create a substring of this cstring.
 *
 * @param self the cstring to work on
 * @param start the index to start at
 * @param length the number of characters to copy, 0 for 'up to the end'
 *
 * @return a newly allocated cstring
 */
cstring *cstring_substring(cstring *self, size_t start, size_t length);

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
 * Reverse the given cstring.
 *
 * @param self the cstring to work on
 */
void cstring_reverse(cstring *self);

/**
 * Replace all occurences of a string inside the given cstring by another.
 *
 * @param self the cstring to work on
 * @param from the string to replace
 * @param to the replacement string
 *
 * @return the number of occurences changed
 */
int cstring_replace(cstring *self, const char from[], const char to[]);

/**
 * Replace all occurences of a char inside the given cstring by another.
 *
 * @param self the cstring to work on
 * @param from the char to replace
 * @param to the replacement char
 *
 * @return the number of occurences changed
 */
int cstring_replace_car(cstring *self, char from, char to);

/**
 * Check if the cstring starts with the given pattern.
 *
 * @param self the cstring to work on
 * @param find the string to find
 * @param start_index the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_starts_with(cstring *self, const char find[], size_t start_index);

/**
 * Check if the string starts with the given pattern.
 *
 * @param self the string to work on
 * @param find the string to find
 * @param start_index the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_sstarts_with(const char string[], const char find[],
		size_t start_index);

/**
 * Check if the cstring ends with the given pattern.
 *
 * @param self the cstring to work on
 * @param find the string to find (if empty, will always be found)
 * @param start_index the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_ends_with(cstring *self, const char find[]);

/**
 * Check if the string ends with the given pattern.
 *
 * @param self the string to work on
 * @param find the string to find (if empty, will always be found)
 * @param start_index the index at which to start the comparison
 *
 * @return 1 if it does
 */
int cstring_sends_with(const char self[], const char find[]);

/**
 * Find the given substring in this one.
 *
 * @param self the cstring to work on
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
 * @param self the cstring to work on
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
long cstring_rfind(char self[], const char find[], size_t rstart_index);

/**
 * Check if the given string is contained by this one.
 *
 * @param self the string to work on
 * @param find the string to find
 * @param start_index the index at which to start the comparison
 *
 * @return the start index of the found string if found, or a negative value if not
 */
long long cstring_sfind(const char self[], const char find[],
		size_t start_index);

/**
 * Check if any of the given characters (in a char* which MUST end with '\0') is found.
 *
 * @param self the cstring to work on
 * @param find the characters to find, which MUST be an array of char that ends with '\0'
 * @param start_index the index at which to start the comparison
 *
 * @return the start index of the first found character if found, or a negative value if not
 */
long long cstring_find_any(cstring *self, const char find[], size_t start_index);

/**
 * Clear (truncate its size to 0) the given cstring.
 *
 * @param self the cstring to work on
 */
void cstring_clear(cstring *self);

/**
 * Convert this cstring into a string
 * This means that you MUST NOT call cstring_free nor use the cstring anymore.
 * NULL will return NULL.
 *
 * @param self the cstring to work on
 */
char *cstring_convert(cstring *self);

/**
 * Clone this cstring.
 * NULL will return NULL.
 *
 * @param self the cstring to clone
 */
cstring *cstring_clone(cstring *self);

/**
 * Clone this string into a new cstring.
 * NULL will return NULL.
 *
 * @param self the string to clone
 */
cstring *cstring_clones(const char self[]);

/**
 * Clone this cstring into a new string.
 * NULL will return NULL.
 *
 * @param self the cstring to clone
 */
char *cstring_sclone(cstring *self);

/**
 * Clone this string into a new string.
 * NULL will return NULL.
 *
 * @param self the string to clone
 */
char *cstring_sclones(const char self[]);

/**
 * Encode the string to BASE64.
 *
 * @param self the cstring to encode
 */
cstring *cstring_to_b64(cstring *self);

/**
 * Encode the string to BASE64.
 *
 * @param self the cstring to encode
 */
char *cstring_to_sb64(cstring *self);

/**
 * Encode the string to BASE64.
 *
 * @param self the string to encode
 * @param size the size of the data (e.g., strlen(self))
 */
char *cstring_to_sb64s(char *self, size_t size);

/**
 * Decode the string to BASE64.
 *
 * @param self the cstring to decode
 */
cstring *cstring_from_b64(cstring *self);

/**
 * Decode the string to BASE64.
 *
 * @param self the cstring to decode
 */
char *cstring_from_sb64(cstring *self);

/**
 * Decode the string to BASE64.
 *
 * @param self the string to decode
 */
char *cstring_from_sb64s(char *self);

/**
 * Trim this cstring of all trailing 'car' instances.
 *
 * @param self the cstring to work on
 * @param car the character to trim (right only)
 *
 * @return a right trimmed cstring
 */
void cstring_rtrim(cstring *self, char car);


/**
 * Trim this cstring of all 'car' instances from the start and/or the
 * end of the string.
 * 
 * @param self the cstring to work on
 * @param car the character to trim
 * 
 * @return a trimmed cstring
 */
void cstring_trim(cstring *self, char car);

/**
 * Compact the memory used by this cstring.
 *
 * @param self the cstring to work on
 */
void cstring_compact(cstring *self);

/**
 * Change the case to upper-case (UTF-8 compatible, but the string MUST be
 * whole).
 *
 * @param self the cstring to work on
 */
void cstring_toupper(cstring *self);

/**
 * Change the case to lower-case (UTF-8 compatible, but the string MUST be
 * whole).
 *
 * @param self the cstring to work on
 */
void cstring_tolower(cstring *self);

/**
 * Read a whole line (CR, LN or CR+LN terminated) from the given file stream.
 *
 * @param self the cstring to read into
 * @param file the file to read
 *
 * @return 1 if a line was read, 0 if not
 */
int cstring_readline(cstring *self, FILE *file);

/**
 * Read a whole line (CR, LN or CR+LN terminated) from the given socket.
 *
 * @param self the cstring to read into
 * @param fd the socket to read from
 *
 * @return 1 if a line was read, 0 if not
 */
int cstring_readnet(cstring *self, int fd);

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
void cstring_add_path(cstring *self, const char subpath[]);

//TODO: desc
int cstring_pop_path(cstring *self, int how_many);

/**
 * Return the basename component of this path (for instance,
 * '/home/user/file.ext' becomes 'file.ext').
 *
 * @param path the path to get the dir of (it can be a dir itself)
 *
 * @return a new string representing the parent directory
 */
char *cstring_basename(const char path[]);

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
 * Return the latest path component of this path (usually a FILE).
 *
 * @param path the path to get the basename of (it can be a dir itself)
 *
 * @return a new cstring representing the latest path component
 */
cstring *cstring_getfile(cstring *path);

/**
 * Return the latest path component of this path (usually a FILE).
 *
 * @param path the path to get the basename of (it can be a dir itself)
 *
 * @return a new string representing the latest path component
 */
cstring *cstring_getfiles(const char path[]);

/**
 * Remove all the \r and \n at the end of the given cstring.
 *
 * @param self the cstring to change
 *
 * @return how many removed characters
 */
size_t cstring_remove_crlf(cstring *self);

/**
 * Remove all the \r and \n at the end of the given string.
 * 
 * @param self the string to change
 * @param n the size of the string
 *
 * @return how many removed characters
 */
size_t cstring_sremove_crlf(char *self, size_t n);

/**
 * Check if the string is whole (i.e., it doesn't contain incomplete UTF-8
 * sequences).
 * 
 * @return TRUE if it is whole
 */
int cstring_is_whole(cstring *self);

#endif

#ifdef __cplusplus
}
#endif

