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

/*
 Name:        cstring.c
 Copyright:   niki (cc-by-nc) 2011
 Author:      niki
 Date:        2011-06-16
 Description: cstring is a collection of helper functions to manipulate string of text
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <wctype.h>
#include <stddef.h>
#include <stdarg.h>

/* Windows (and maybe others?) doesn't know about strnlen */
#ifndef strnlen
size_t strnlen(const char *s, size_t maxlen);
size_t strnlen(const char *s, size_t maxlen) {
	size_t i;
	for (i = 0; s[i]; i++) {
		if (i >= maxlen)
			return maxlen;
	}

	return i;
}
#endif

#include "cstring.h"
#include "net.h"
#include "base64.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 81
#endif

#ifdef WIN32
#define CSTRING_SEP '\\'
#else
#define CSTRING_SEP '/'
#endif

//start of private prototypes

struct cstring_private_struct {
	size_t buffer_length;
};

void cstring_change_case(cstring *self, int up);

//end of private prototypes

cstring *new_cstring() {
	cstring *string;

	string = malloc(sizeof(cstring));
	strcpy(string->CNAME, "[CString]");
	string->priv = malloc(sizeof(struct cstring_private_struct));
	string->length = 0;
	string->priv->buffer_length = BUFFER_SIZE;
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

void cstring_grow(cstring *self, int min) {
	cstring_grow_to(self, self->length + min);
}

void cstring_grow_to(cstring *self, int buffer) {
	if (buffer > self->priv->buffer_length) {
		self->priv->buffer_length = buffer;
		self->string = (char *) realloc(self->string,
				sizeof(char) * self->priv->buffer_length);
	}
}

char *cstring_convert(cstring *self) {
	char *string;

	if (!self)
		return NULL;

	// Note: this could be skipped.
	cstring_compact(self);

	free(self->priv);
	string = (self->string);
	free(self);

	return string;
}

cstring *cstring_clone(cstring *self) {
	if (self == NULL)
		return NULL;

	return cstring_clones(self->string);
}

char *cstring_sclone(cstring *self) {
	if (self == NULL)
		return NULL;

	return cstring_convert(cstring_clone(self));
}

char *cstring_sclones(const char self[]) {
	return cstring_convert(cstring_clones(self));
}

cstring *cstring_clones(const char self[]) {
	cstring *clone;

	if (self == NULL)
		return NULL;

	clone = new_cstring();
	cstring_add(clone, self);

	return clone;
}

void cstring_compact(cstring *self) {
	if (self != NULL) {
		self->priv->buffer_length = self->length + 1;
		self->string = (char *) realloc(self->string, self->length + 1);
	}
}

void cstring_cut_at(cstring *self, size_t size) {
	if (self->length > size) {
		self->string[size] = '\0';
		self->length = size;
	}
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

cstring *cstring_substring(cstring *self, size_t start, size_t length) {
	cstring *sub;
	char *source;

	if (length == 0) {
		length = self->length - start;
	}

	sub = new_cstring();
	source = self->string;
	source = source + start;

	cstring_addn(sub, source, length);

	return sub;
}

int cstring_starts_with(cstring *self, const char find[], size_t start_index) {
	return cstring_sstarts_with(self->string, find, start_index);
}

int cstring_sstarts_with(const char string[], const char find[],
		size_t start_index) {
	size_t i;

	for (i = 0;
			string[start_index + i] == find[i]
					&& string[start_index + i] != '\0' && find[i] != '\0'; i++)
		;

	return find[i] == '\0';
}

int cstring_ends_with(cstring *self, const char find[]) {
	size_t sz_needle = strlen(find);
	if (sz_needle <= self->length) {
		if (!strcmp(self->string + (self->length - sz_needle), find))
			return 1;
	}

	return 0;
}

int cstring_sends_with(const char self[], const char find[]) {
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

long cstring_rfind(char self[], const char find[], size_t rstart_index) {
	size_t sz = strlen(self);
	size_t sz_needle = strlen(find);

	if (rstart_index <= 0)
		rstart_index += (sz - 1);

	if (sz > rstart_index && sz_needle <= sz) {
		for (size_t i = sz - sz_needle; i; i--) {
			if (cstring_sstarts_with(self, find, i))
				return i;
		}
	}

	return -1;
}

int cstring_replace_car(cstring *self, char from, char to) {
	size_t i;
	int occur = 0;

	for (i = 0; i < self->length; i++) {
		if (self->string[i] == from) {
			self->string[i] = to;
			occur++;
		}
	}

	return occur;
}

int cstring_replace(cstring *self, const char from[], const char to[]) {
	cstring *buffer;
	size_t i;
	size_t step;
	char *swap;
	int occur;

	// easy optimization:
	if (from && to && from[0] && to[0] && !from[1] && !to[1])
		return cstring_replace_car(self, from[0], to[0]);

	// optimize for same-size strings?

	step = strlen(from) - 1;
	buffer = new_cstring();
	occur = 0;
	for (i = 0; i < self->length; i++) {
		if (cstring_starts_with(self, from, i)) {
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

void cstring_clear(cstring *self) {
	self->length = 0;
	self->string[0] = '\0';
}

void cstring_reverse(cstring *self) {
	size_t i;
	size_t last;
	char tmp;

	if (self->length > 0) {
		last = self->length - 1;
		for (i = 0; i <= (last / 2); i++) {
			tmp = self->string[i];
			self->string[i] = self->string[last - i];
			self->string[last - i] = tmp;
		}
	}
}

void cstring_add_car(cstring *self, char source) {
	char source2[2];

	source2[0] = source;
	source2[1] = '\0';

	cstring_add(self, source2);
}

void cstring_addp(cstring *self, const char *fmt, ...) {
	va_list ap;
	char empty = '\0';

	va_start(ap, fmt);
	int sz = vsnprintf(&empty, 0, fmt, ap);
	va_end(ap);

	char *tmp = malloc((sz + 1) * sizeof(char));
	cstring_grow(self, sz);

	va_start(ap, fmt);
	sz = vsnprintf(tmp, sz + 1, fmt, ap);
	va_end(ap);

	cstring_add(self, tmp);

	free(tmp);
}

void cstring_add(cstring *self, const char source[]) {
	cstring_addf(self, source, 0);
}

void cstring_addf(cstring *self, const char source[], size_t indexi) {
	size_t ss, ptr;

	if (source != NULL && strlen(source) > indexi) {
		ss = strlen(source) - indexi;
		while ((self->length + ss) >= (self->priv->buffer_length)) {
			self->priv->buffer_length += BUFFER_SIZE;
		}
		self->string = (char *) realloc(self->string,
				sizeof(char) * self->priv->buffer_length);

		for (ptr = self->length; ptr <= (self->length + ss); ptr++) {
			self->string[ptr] = source[ptr - self->length + indexi];
		}
		self->length += ss;
	}
}

void cstring_addn(cstring *self, const char source[], size_t n) {
	cstring_addfn(self, source, 0, n);
}

void cstring_addfn(cstring *self, const char source[], size_t indexi, size_t n) {
	size_t i;
	char *tmp;

	for (i = indexi; i < (n + indexi) && source[i] != '\0'; i++)
		;
	if (source[i] == '\0') {
		cstring_addf(self, source, indexi);
	} else {
		tmp = (char *) malloc(sizeof(char) * (n + 1));
		strncpy(tmp, source + indexi, n);
		tmp[n] = '\0';
		cstring_add(self, tmp);
		free(tmp);
	}
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

		free(self->string);
		self->priv->buffer_length = tmp->priv->buffer_length;
		self->string = tmp->string;
		tmp->string = NULL;
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

	result = cstring_clones(path->string + i + 1);
	return result;
}

cstring *cstring_getfiles(const char path[]) {
	cstring *copy, *result;

	copy = cstring_clones(path);
	result = cstring_getfile(copy);
	free_cstring(copy);
	return result;
}

cstring *cstring_to_b64(cstring *self) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return base64_encode(cstring_b64, self);
}

char *cstring_to_sb64(cstring *self) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return cstring_convert(base64_encode(cstring_b64, self));
}

char *cstring_to_sb64s(char *self, size_t size) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return cstring_convert(base64_encodesi(cstring_b64, self, size));
}

cstring *cstring_from_b64(cstring *self) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return base64_decode(cstring_b64, self);
}

char *cstring_from_sb64(cstring *self) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return cstring_convert(base64_decode(cstring_b64, self));
}

char *cstring_from_sb64s(char *self) {
	static base64 *cstring_b64 = NULL;
	if (!cstring_b64)
		cstring_b64 = base64_new();

	return cstring_convert(base64_decodes(cstring_b64, self));
}

int cstring_is_whole(cstring *self) {
	return mbstowcs(NULL, self->string, 0) != (size_t) -1;
}
