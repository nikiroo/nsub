/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2012 Niki Roo
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

/*
 Name:        cstring.c
 Copyright:   niki (cc-by-nc) 2011
 Author:      niki
 Date:        2011-06-16
 Description: cstring is a collection of helper functions to manipulate string of text
 */

#include "cstring.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 81
#endif

#ifdef WIN32
#define CSTRING_SEP '\\'
#else
#define CSTRING_SEP '/'
#endif

//start of private prototypes

typedef struct {
	size_t buffer_length;
} priv_t;

/** Swap the data */
static void cstring_swap(cstring *a, cstring *b);
/** Change the case to upper -or- lower case (UTF8-compatible) */
static void cstring_change_case(cstring *self, int up);

//end of private prototypes

cstring *new_cstring() {
	cstring *string;

	string = malloc(sizeof(cstring));
	strcpy(string->CNAME, "[CString]");
	string->priv = malloc(sizeof(priv_t));
	string->length = 0;
	((priv_t *) string->priv)->buffer_length = BUFFER_SIZE;
	string->string = malloc(sizeof(char) * BUFFER_SIZE);
	string->string[0] = '\0';

	return string;
}

void free_cstring(cstring *string) {
	if (!string)
		return;

	free(string->priv);
	free(string->string);

	free(string);
}

void cstring_swap(cstring *a, cstring *b) {
	void *tmp_p;
	char *tmp_s;
	size_t tmp_l;

	tmp_s = a->string;
	tmp_l = a->length;
	tmp_p = a->priv;

	a->string = b->string;
	a->length = b->length;
	a->priv = b->priv;

	b->string = tmp_s;
	b->length = tmp_l;
	b->priv = tmp_p;
}

int cstring_grow(cstring *self, int min_extra) {
	priv_t *priv = ((priv_t *) self->priv);

	size_t sz = priv->buffer_length;
	size_t req = self->length + min_extra;

	if (req > sz) {
		sz += BUFFER_SIZE;
		if (req > sz)
			sz = req;

		return cstring_grow_to(self, sz);
	}

	return 1;
}

int cstring_grow_to(cstring *self, int min_buffer) {
	priv_t *priv = ((priv_t *) self->priv);

	if (min_buffer > priv->buffer_length) {
		priv->buffer_length = min_buffer;
		void *mem = realloc(self->string, sizeof(char) * priv->buffer_length);

		if (mem)
			self->string = (char *) mem;
		else
			return 0;
	}

	return 1;
}

void cstring_compact(cstring *self) {
	if (self != NULL) {
		priv_t *priv = ((priv_t *) self->priv);

		priv->buffer_length = self->length + 1;
		self->string = (char *) realloc(self->string, self->length + 1);
	}
}

int cstring_add_car(cstring *self, char source) {
	if (!cstring_grow(self, 1))
		return 0;

	self->string[self->length] = source;
	self->length++;
	self->string[self->length] = '\0';

	return 1;
}

int cstring_add(cstring *self, const char source[]) {
	return cstring_addf(self, source, 0);
}

int cstring_addf(cstring *self, const char source[], size_t idx) {
	return cstring_addfn(self, source, idx, 0);
}

int cstring_addn(cstring *self, const char source[], size_t n) {
	return cstring_addfn(self, source, 0, n);
}

int cstring_addfn(cstring *self, const char source[], size_t idx, size_t n) {
	size_t ss;

	ss = strlen(source);
	if (source && ss > idx && idx >= 0) {
		ss -= idx;

		if (n && n < ss)
			ss = n;

		if (ss) {
			// "+1" for the added '\0'
			if (!cstring_grow(self, ss + 1))
				return 0;

			memcpy(self->string + self->length, source + idx, ss);
			self->length += ss;
			self->string[self->length] = '\0';
		}
	}

	return 1;
}

int cstring_addp(cstring *self, const char *fmt, ...) {
	va_list ap;
	char empty = '\0';

	va_start(ap, fmt);
	int sz = vsnprintf(&empty, 0, fmt, ap);
	va_end(ap);

	char *tmp = malloc((sz + 1) * sizeof(char));
	if (!tmp)
		return 0;

	va_start(ap, fmt);
	sz = vsnprintf(tmp, sz + 1, fmt, ap);
	va_end(ap);

	int ok = cstring_add(self, tmp);
	free(tmp);

	return ok;
}

void cstring_cut_at(cstring *self, size_t size) {
	if (self->length > size) {
		self->string[size] = '\0';
		self->length = size;
	}
}

cstring *cstring_substring(const char self[], size_t start, size_t length) {
	size_t sz = strlen(self);
	cstring * sub = new_cstring();

	if (start <= sz) {
		const char *source = (self + start);

		if (!length)
			cstring_add(sub, source);
		else
			cstring_addn(sub, source, length);
	}

	return sub;
}

/*
 clist *cstring_splitc(cstring *self, char delim, char quote) {
 clist *list;
 cstring *d, *q;

 d = new_cstring();
 q = new_cstring();

 if (delim)
 cstring_add_car(d, delim);
 if (quote)
 cstring_add_car(q, quote);

 list = cstring_split(self, d, q);

 free_cstring(d);
 free_cstring(q);

 return list;
 }

 clist *cstring_splits(cstring *self, const char delim[], const char quote[]) {
 clist *list;
 cstring *d, *q;

 d = new_cstring();
 q = new_cstring();

 if (delim)
 cstring_add(d, delim);
 if (quote)
 cstring_add(q, quote);

 list = cstring_split(self, d, q);

 free_cstring(d);
 free_cstring(q);

 return list;
 }
 clist *cstring_split(cstring *self, cstring *delim, cstring *quote) {
 clist *list;
 cstring *elem;
 clist_node *node;
 size_t i;
 int in_quote;
 int hasdelim;

 hasdelim = delim && delim->length > 0;

 list = clist_new();
 in_quote = 0;
 elem = NULL;
 for (i = 0; i < self->length; i++) {
 if (quote->length > 0 && cstring_starts_with(self, quote, i)) {
 in_quote = !in_quote;
 i += quote->length - 1;
 } else {
 if (elem == NULL) {
 elem = new_cstring();
 node = clist_node_new();
 node->data = elem;
 node->free_data = free_cstring;
 clist_add(list, node);
 }
 if (!in_quote && hasdelim && cstring_starts_with(self, delim, i)) {
 elem = new_cstring();
 node = clist_node_new();
 node->data = elem;
 node->free_data = free_cstring;
 clist_add(list, node);
 i += delim->length - 1;
 } else {
 cstring_add_car(elem, self->string[i]);
 }
 }
 }

 return list;
 }
 */

void cstring_reverse(char *self) {
	size_t i;
	size_t last;
	char tmp;

	size_t sz = strlen(self);
	if (sz) {
		last = sz - 1;
		for (i = 0; i <= (last / 2); i++) {
			tmp = self[i];
			self[i] = self[last - i];
			self[last - i] = tmp;
		}
	}
}

int cstring_replace(cstring *self, const char from[], const char to[]) {
	cstring *buffer;
	size_t i;
	size_t step;
	char *swap;
	int occur;

	// easy optimization:
	if (!from || !from[0])
		return 0;
	if (from && to && from[0] && to[0] && !from[1] && !to[1])
		return cstring_replace_car(self->string, from[0], to[0]);

	// optimize for same-size strings?

	step = strlen(from) - 1;
	buffer = new_cstring();
	occur = 0;
	for (i = 0; i < self->length; i++) {
		if (cstring_starts_with(self->string, from, i)) {
			cstring_add(buffer, to);
			i += step;
			occur++;
		} else {
			cstring_add_car(buffer, self->string[i]);
		}
	}

	// not clean, but quicker:
	swap = self->string;
	self->string = buffer->string;
	buffer->string = swap;
	self->length = buffer->length;

	free_cstring(buffer);
	return occur;
}

int cstring_replace_car(char *self, char from, char to) {
	size_t i;
	int occur = 0;

	for (i = 0; self[i]; i++) {
		if (self[i] == from) {
			self[i] = to;
			occur++;
		}
	}

	return occur;
}

int cstring_starts_with(const char string[], const char find[],
		size_t start_idx) {
	size_t i;

	for (i = 0;
			string[start_idx + i] == find[i] && string[start_idx + i] != '\0'
					&& find[i] != '\0'; i++)
		;

	return find[i] == '\0';
}

int cstring_ends_with(const char self[], const char find[]) {
	size_t sz = strlen(self);
	size_t sz_needle = strlen(find);
	if (sz_needle <= sz) {
		if (!strcmp(self + (sz - sz_needle), find))
			return 1;
	}

	return 0;
}

long cstring_find(const char self[], const char find[], size_t start_index) {
	size_t sz = strlen(self);
	if (sz > start_index) {
		char *found = strstr(self + start_index, find);
		if (found) {
			return (long) (found - self);
		}
	}

	return -1;
}

long cstring_rfind(char self[], const char find[], long rstart_index) {
	size_t sz = strlen(self);
	size_t sz_needle = strlen(find);

	if (rstart_index <= 0)
		rstart_index = (sz - 1);

	if (sz > rstart_index && sz_needle <= sz) {
		for (size_t i = rstart_index;; i--) {
			if (cstring_starts_with(self, find, i))
				return i;

			if (!i)
				break;
		}
	}

	return -1;
}

void cstring_clear(cstring *self) {
	self->length = 0;
	self->string[0] = '\0';
}

char *cstring_convert(cstring *self) {
	char *string;

	if (!self)
		return NULL;

	// Note: this could be skipped.
	cstring_compact(self);

	string = (self->string);
	self->string = NULL;

	free_cstring(self);

	return string;
}

cstring *cstring_clone(cstring *self) {
	if (self == NULL)
		return NULL;

	cstring *clone = new_cstring();
	cstring_add(clone, self->string);

	return clone;
}

void cstring_rtrim(cstring *self, char car) {
	for (size_t i = self->length - 1; i >= 0; i--) {
		if (self->string[i] != car)
			break;
		self->string[i] = '\0';
		self->length--;
	}
}

void cstring_trim(cstring *self, char car) {
	if (car == '\0')
		return;

	cstring_rtrim(self, car);

	int i = 0;
	while (self->string[i] == car)
		i++;

	if (i) {
		cstring *tmp = new_cstring();
		cstring_add(tmp, self->string + i);

		cstring_swap(self, tmp);
		free_cstring(tmp);
	}
}

size_t cstring_remove_crlf(cstring *self) {
	size_t removed;

	removed = cstring_sremove_crlf(self->string, self->length);
	self->length -= removed;

	return removed;
}

size_t cstring_sremove_crlf(char data[], size_t n) {
	size_t removed;

	removed = n;
	while (removed > 0
			&& (data[removed - 1] == '\r' || data[removed - 1] == '\n')) {
		removed--;
	}

	data[removed] = '\0';

	return removed;
}

void cstring_toupper(cstring *self) {
	cstring_change_case(self, 1);
}

void cstring_tolower(cstring *self) {
	cstring_change_case(self, 0);
}

void cstring_change_case(cstring *self, int up) {
	wchar_t *wide;
	char tmp[10];
	const char *src = self->string;
	size_t s, i;
	mbstate_t state;

	// init the state (passing NULL is not thread-safe)
	memset(&state, '\0', sizeof(mbstate_t));

	// won't contain MORE chars (but maybe less)
	wide = (wchar_t *) malloc((self->length + 1) * sizeof(wchar_t));
	s = mbsrtowcs(wide, &src, self->length, &state);
	wide[s] = (wchar_t) '\0';
	cstring_clear(self);
	for (i = 0; i <= s; i++) {
		if (up)
			wide[i] = (wchar_t) towupper((wint_t) wide[i]);
		else
			wide[i] = (wchar_t) towlower((wint_t) wide[i]);
		memset(&state, '\0', sizeof(mbstate_t));
		wcrtomb(tmp, wide[i], &state);
		cstring_add(self, tmp);
	}
	free(wide);
}

int cstring_readline(cstring *self, FILE *file) {
	char buffer[BUFFER_SIZE];
	size_t size = 0;
	int full_line;

	// sanity check:
	if (!file)
		return 0;

	buffer[BUFFER_SIZE - 1] = '\0'; // just in case

	if (!feof(file)) {
		cstring_clear(self);
		buffer[0] = '\0';

		// Note: strlen() could return 0 if the file contains \0
		// at the start of a line
		if (!fgets(buffer, (int) BUFFER_SIZE - 1, file))
			return 0;
		size = strlen(buffer);

		full_line = ((file && feof(file)) || size == 0
				|| buffer[size - 1] == '\n');
		size -= cstring_sremove_crlf(buffer, size);
		cstring_add(self, buffer);

		// No luck, we need to continue getting data
		while (!full_line) {
			if (!fgets(buffer, (int) BUFFER_SIZE - 1, file))
				break;
			size = strlen(buffer);

			full_line = ((file && feof(file)) || size == 0
					|| buffer[size - 1] == '\n');
			size -= cstring_sremove_crlf(buffer, size);
			cstring_add(self, buffer);
		}

		return 1;
	}

	return 0;
}

void cstring_add_path(cstring *self, const char subpath[]) {
	cstring_add_car(self, CSTRING_SEP);
	cstring_add(self, subpath);
}

int cstring_pop_path(cstring *self, int how_many) {
	char sep[] = { CSTRING_SEP };
	int count = 0;

	cstring_rtrim(self, CSTRING_SEP);
	for (int i = 0; i < how_many; i++) {
		size_t idx = cstring_rfind(self->string, sep, 0);
		if (!idx)
			break;

		cstring_cut_at(self, idx - 1);
		count++;
	}

	return count;
}

cstring *cstring_getdir(const char path[]) {
	cstring *result;
	size_t i;

	size_t sz = strlen(path);

	i = sz - 1;
	if (i >= 0 && path[i] == CSTRING_SEP)
		i--;
	for (; i >= 0 && path[i] != CSTRING_SEP; i--)
		;

	if (i < 0)
		return new_cstring();

	result = new_cstring();
	cstring_addn(result, path, i);

	return result;
}

cstring *cstring_getfile(cstring *path) {
	cstring *result;
	ssize_t i;

	i = (ssize_t) path->length - 1;
	if (i >= 0 && path->string[i] == CSTRING_SEP)
		i--;
	for (; i >= 0 && path->string[i] != CSTRING_SEP; i--)
		;

	if (i < 0 || (size_t) (i + 1) >= path->length)
		return new_cstring();

	result = new_cstring();
	cstring_add(result, path->string + i + 1);
	return result;
}

cstring *cstring_getfiles(const char path[]) {
	cstring *copy = new_cstring();
	cstring_add(copy, path);

	cstring *result = cstring_getfile(copy);

	free_cstring(copy);

	return result;
}

int cstring_is_whole(cstring *self) {
	return mbstowcs(NULL, self->string, 0) != (size_t) -1;
}
