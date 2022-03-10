/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2011 Niki Roo
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
 Copyright:   niki (gpl3 or later) 2011
 Author:      niki
 Date:        2011-06-16
 Description: cstring is a collection of helper functions to manipulate text
 */

#include "cstring.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

// For upper/lowercase
#include <locale.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 81
#endif

#ifdef WIN32
#define CSTRING_SEP '\\'
#else
#define CSTRING_SEP '/'
#endif

// Private functions

typedef struct {
	size_t buffer_length;
} priv_t;

/** Swap the data */
static void cstring_swap(cstring_t *a, cstring_t *b);
/** Change the case to upper -or- lower case (UTF8-compatible) */
static void cstring_change_case(cstring_t *self, int up);
/** For path-related functions */
static void normalize_path(cstring_t *self);

// Private variables

static char *locale = NULL;

// end of privates

cstring_t *new_cstring() {
	cstring_t *string;

	string = malloc(sizeof(cstring_t));
	strcpy(string->CNAME, "[CString]");
	string->priv = malloc(sizeof(priv_t));
	string->length = 0;
	((priv_t *) string->priv)->buffer_length = BUFFER_SIZE;
	string->string = malloc(sizeof(char) * BUFFER_SIZE);
	string->string[0] = '\0';

	return string;
}

void free_cstring(cstring_t *string) {
	if (!string)
		return;

	free(string->priv);
	free(string->string);

	free(string);
}

void cstring_swap(cstring_t *a, cstring_t *b) {
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

int cstring_grow(cstring_t *self, int min_extra) {
	priv_t *priv = ((priv_t *) self->priv);

	size_t sz = priv->buffer_length;
	size_t req = self->length + min_extra;

	if (req > sz) {
		if (sz > BUFFER_SIZE)
			sz *= 2;
		else
			sz += BUFFER_SIZE;

		if (req > sz)
			sz = req;

		return cstring_grow_to(self, sz);
	}

	return 1;
}

int cstring_grow_to(cstring_t *self, int min_buffer) {
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

void cstring_compact(cstring_t *self) {
	if (self != NULL) {
		priv_t *priv = ((priv_t *) self->priv);

		priv->buffer_length = self->length + 1;
		self->string = (char *) realloc(self->string, self->length + 1);
	}
}

int cstring_add_car(cstring_t *self, char source) {
	if (!cstring_grow(self, 1))
		return 0;

	self->string[self->length] = source;
	self->length++;
	self->string[self->length] = '\0';

	return 1;
}

int cstring_add(cstring_t *self, const char source[]) {
	return cstring_addf(self, source, 0);
}

int cstring_addf(cstring_t *self, const char source[], size_t idx) {
	return cstring_addfn(self, source, idx, 0);
}

int cstring_addn(cstring_t *self, const char source[], size_t n) {
	return cstring_addfn(self, source, 0, n);
}

int cstring_addfn(cstring_t *self, const char source[], size_t idx, size_t n) {
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

int cstring_addp(cstring_t *self, const char *fmt, ...) {
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

void cstring_cut_at(cstring_t *self, size_t size) {
	if (self->length > size) {
		self->string[size] = '\0';
		self->length = size;
	}
}

cstring_t *cstring_substring(const char self[], size_t start, size_t length) {
	size_t sz = strlen(self);
	cstring_t * sub = new_cstring();

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

int cstring_replace(cstring_t *self, const char from[], const char to[]) {
	cstring_t *buffer;
	size_t i;
	size_t step;
	int occur;

	// easy optimisation:
	if (!from || !from[0])
		return 0;
	if (from && to && from[0] && to[0] && !from[1] && !to[1])
		return cstring_replace_car(self->string, from[0], to[0]);

	// optimise for same-size strings?

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

	cstring_swap(self, buffer);
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
		rstart_index += (sz - 1);

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

void cstring_clear(cstring_t *self) {
	self->length = 0;
	self->string[0] = '\0';
}

char *cstring_convert(cstring_t *self) {
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

cstring_t *cstring_clone(const char self[]) {
	if (self == NULL)
		return NULL;

	cstring_t *clone = new_cstring();
	cstring_add(clone, self);

	return clone;
}

void cstring_rtrim(cstring_t *self, char car) {
	for (size_t i = self->length - 1; i >= 0; i--) {
		if (self->string[i] != car)
			break;
		self->string[i] = '\0';
		self->length--;
	}
}

void cstring_trim(cstring_t *self, char car) {
	if (car == '\0')
		return;

	cstring_rtrim(self, car);

	int i = 0;
	while (self->string[i] == car)
		i++;

	if (i) {
		cstring_t *tmp = new_cstring();
		cstring_add(tmp, self->string + i);

		cstring_swap(self, tmp);
		free_cstring(tmp);
	}
}

size_t cstring_remove_crlf(char *self) {
	size_t sz = strlen(self);
	if (sz && self[sz - 1] == '\n')
		sz--;
	if (sz && self[sz - 1] == '\r')
		sz--;

	self[sz] = '\0';

	return sz;
}

void cstring_toupper(cstring_t *self) {
	cstring_change_case(self, 1);
}

void cstring_tolower(cstring_t *self) {
	cstring_change_case(self, 0);
}

void cstring_change_case(cstring_t *self, int up) {
	// Change LC_ALL to LANG if not found
	// TODO: only take part we need (also, this is still bad practise)
	if (!locale) {
		locale = setlocale(LC_ALL, NULL);
		if (!locale || !locale[0] || !strcmp("C", locale)) {
			char *lang = getenv("LANG");
			if (lang && lang[0]) {
				locale = setlocale(LC_ALL, lang);
				if (!locale)
					locale = "";
			}
		}
	}

	cstring_t *rep;
	mbstate_t state_from, state_to;
	wchar_t wide;
	char tmp[10];
	size_t count;

	// init the state (NULL = internal hidden state, not thread-safe)
	memset(&state_from, '\0', sizeof(mbstate_t));
	memset(&state_to, '\0', sizeof(mbstate_t));

	rep = new_cstring();

	size_t i = 0;
	while (i < self->length) {
		count = mbrtowc(&wide, self->string + i, self->length - i, &state_from);

		//incomplete (should not happen)
		if (count == (size_t) -2) {
			// return;
			cstring_add_car(rep, '_');
			i++;
			continue;
		}
		// invalid multibyte sequence
		if (count == (size_t) -1) {
			// return;
			cstring_add_car(rep, '_');
			i++;
			continue;
		}

		// End of String (should not happen, see WHILE condition)
		if (!count)
			break;

		// char is ok
		i += count;

		if (up)
			wide = (wchar_t) towupper((wint_t) wide);
		else
			wide = (wchar_t) towlower((wint_t) wide);

		count = wcrtomb(tmp, wide, &state_to);
		if (count == (size_t) -1) {
			// failed to convert :(
			cstring_add_car(rep, '_');
		} else {
			tmp[count] = '\0';
			cstring_add(rep, tmp);
		}
	}

	cstring_swap(self, rep);
	free_cstring(rep);
}

int cstring_readline(cstring_t *self, FILE *file) {
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

		// Note: fgets() could return NULL if EOF is reached
		if (!fgets(buffer, (int) BUFFER_SIZE - 1, file))
			return 0;

		size = strlen(buffer);
		full_line = ((file && feof(file)) || size == 0
				|| buffer[size - 1] == '\n');
		size = cstring_remove_crlf(buffer);
		cstring_add(self, buffer);

		// No luck, we need to continue getting data
		while (!full_line) {
			if (!fgets(buffer, (int) BUFFER_SIZE - 1, file))
				break;

			size = strlen(buffer);
			full_line = ((file && feof(file)) || size == 0
					|| buffer[size - 1] == '\n');
			size = cstring_remove_crlf(buffer);
			cstring_add(self, buffer);
		}

		return 1;
	}

	return 0;
}

static void normalize_path(cstring_t *self) {
	while (self->length && self->string[self->length - 1] == CSTRING_SEP)
		self->length--;
	self->string[self->length] = '\0';
}

void cstring_add_path(cstring_t *self, const char subpath[]) {
	while (self->length && self->string[self->length - 1] == CSTRING_SEP)
		self->length--;
	cstring_add_car(self, CSTRING_SEP);
	if (subpath && subpath[0]) {
		cstring_add(self, subpath);
	}

	normalize_path(self);
}

int cstring_pop_path(cstring_t *self, int how_many) {
	int count = 0;
	size_t tmp;
	char first = '\0';

	if (self->length)
		first = self->string[0];

	normalize_path(self);
	for (int i = 0; i < how_many; i++) {
		tmp = self->length;
		while (self->length && self->string[self->length - 1] != CSTRING_SEP)
			self->length--;
		while (self->length && self->string[self->length - 1] == CSTRING_SEP)
			self->length--;
		if (self->length != tmp)
			count++;
	}
	normalize_path(self);

	// Root is root of root
	if (first == CSTRING_SEP && !self->length)
		cstring_add_car(self, CSTRING_SEP);

	return count;
}

char *cstring_basename(const char path[], const char ext[]) {
	size_t i;
	size_t sz = strlen(path);

	i = sz;
	while (i && path[i] != CSTRING_SEP)
		i--;

	cstring_t *rep;
	if (path[i] != CSTRING_SEP) {
		rep = cstring_clone(path);
	} else {
		rep = new_cstring();
		cstring_addf(rep, path, i + 1);
	}

	if (ext && ext[0] && cstring_ends_with(rep->string, ext)) {
		cstring_cut_at(rep, rep->length - strlen(ext));
	}

	return cstring_convert(rep);
}

char *cstring_dirname(const char path[]) {
	cstring_t *rep = cstring_clone(path);
	cstring_pop_path(rep, 1);
	return cstring_convert(rep);
}

int cstring_is_utf8(cstring_t *self) {
	size_t rep = mbstowcs(NULL, self->string, 0);
	// -2 = invalid, -1 = not whole
	return (rep != (size_t) -2) && (rep != (size_t) -1);
}
