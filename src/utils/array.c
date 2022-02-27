/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2022 Niki Roo
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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "array.h"

struct array_p {
	size_t elem_size;
	size_t count;
	size_t buffer;
	void *data;
};

/* make sure we have at least n elements in the buffer, grow if needed */
int array_assure(array *me, size_t nb_elem);

/* for qsort operations */
int array_qsorts_func(const void *a, const void *b);
int array_qsorti_func(const void *a, const void *b);
int array_qsortl_func(const void *a, const void *b);
int array_qsortf_func(const void *a, const void *b);
int array_qsorts_rfunc(const void *a, const void *b);
int array_qsorti_rfunc(const void *a, const void *b);
int array_qsortl_rfunc(const void *a, const void *b);
int array_qsortf_rfunc(const void *a, const void *b);

/* for reafilei, readfiles */
void array_readfile_funci(array *me, const char line[]);
void array_readfile_funcs(array *me, const char line[]);

array *new_array(size_t elem_size, size_t initial) {
	array *me = NULL;
	if (initial)
		me = malloc(sizeof(array));
	if (me) {
		me->elem_size = elem_size;
		me->count = 0;
		me->buffer = initial;
		me->data = malloc(elem_size * initial);
		if (!me->data) {
			free(me);
			me = NULL;
		}
	}
	
	return me;
}

void free_array(array *me) {
	if (!me)
		return;

	me->count = 0;
	me->buffer = 0;
	free(me->data);
	free(me);
}

void array_clear(array *me) {
	me->count = 0;
}

// convert to void * data (free the rest)
void *array_convert(array *me) {
	void *data = me->data;
	free(me);
	return data;
}

void *array_data(array *me) {
	return me->data;
}

size_t array_count(array *me) {
	return me->count;
}

void array_compact(array *me) {
	int c = me->count > 0 ? me->count : 1;
	me->data = realloc(me->data, c * me->elem_size);
	me->buffer = c;
}

void array_all(array *me, void(*func)()) {
	for (size_t i = 0 ; i < me->count ; i++) {
		func(((void **)me->data)[i]);
	}
}

void array_free_all(array *me) {
	array_all(me, free);
	me->count = 0;
}

void array_qsort(array *me, int (*compar)(const void *, const void *)) {
	qsort(me->data, me->count, me->elem_size, compar);
}

void array_qsorts(array *me, int rev) {
	array_qsort(me, rev ? array_qsorts_rfunc : array_qsorts_func);
}
int array_qsorts_func(const void *a, const void *b) {
	char *stra = ((char **)a)[0];
	char *strb = ((char **)b)[0];
	return strcmp(stra, strb);
}
int array_qsorts_rfunc(const void *a, const void *b) {
	char *stra = ((char **)a)[0];
	char *strb = ((char **)b)[0];
	return strcmp(strb, stra);
}

void array_qsorti(array *me, int rev) {
	array_qsort(me, rev ? array_qsorti_rfunc : array_qsorti_func);
}
int array_qsorti_func(const void *a, const void *b) {
	long ia, ib;
	ia = ((int *)a)[0];
	ib = ((int *)b)[0];
	if (ia < ib) return -1;
	return !(ia == ib);
}
int array_qsorti_rfunc(const void *a, const void *b) {
	long ia, ib;
	ia = ((int *)a)[0];
	ib = ((int *)b)[0];
	if (ia > ib) return -1;
	return !(ia == ib);
}

void array_qsortl(array *me, int rev) {
	array_qsort(me, rev ? array_qsortl_rfunc : array_qsortl_func);
}
int array_qsortl_func(const void *a, const void *b) {
	long la, lb;
	la = ((long *)a)[0];
	lb = ((long *)b)[0];
	if (la < lb) return -1;
	return !(la == lb);
}
int array_qsortl_rfunc(const void *a, const void *b) {
	long la, lb;
	la = ((long *)a)[0];
	lb = ((long *)b)[0];
	if (la > lb) return -1;
	return !(la == lb);
}

void array_qsortf(array *me, int rev) {
	array_qsort(me, rev ? array_qsortf_rfunc : array_qsortf_func);
}
int array_qsortf_func(const void *a, const void *b) {
	float fa, fb;
	fa = ((float *)a)[0];
	fb = ((float *)b)[0];
	// Also works:
	//memcpy(&fa, a, sizeof(float));
	//memcpy(&fb, b, sizeof(float));
	if (fa < fb) return -1;
	return !(fa == fb);
}
int array_qsortf_rfunc(const void *a, const void *b) {
	float fa, fb;
	fa = ((float *)a)[0];
	fb = ((float *)b)[0];
	// Also works:
	//memcpy(&fa, a, sizeof(float));
	//memcpy(&fb, b, sizeof(float));
	if (fa > fb) return -1;
	return !(fa == fb);
}

int array_add(array *me, void *data) {
	return array_setn(me, me->count, data, 1);
}

int array_addn(array *me, void *data, size_t n) {
	return array_setn(me, me->count, data, n);
}

void *array_get_ptr(array *me, size_t i) {
	// cast to (char *) because we want 'byte' arithmetic
	return (void *)(((char *)me->data) + (i * me->elem_size));
}

void array_get(array *me, void *target, size_t i) {
	array_getn(me, target, i, 1);
}

void array_getn(array *me, void *target, size_t i, size_t n) {
	// cast to (char *) because we want 'byte' arithmetic
	memcpy(target, ((char *)(me->data)) + (i * me->elem_size), n * me->elem_size);
}

int array_set(array *me, size_t i, void *data) {
	return array_setn(me, i, data, 1);
}

int array_setn(array *me, size_t i, void *data, size_t n) {
	// allow new items BUT NOT holes in the array
	if (i > me->count)
		return 0;
	
	if (!array_assure(me, i + n))
		return 0;
	
	// cast to (char *) because we want 'byte' arithmetic
	memcpy(((char *)(me->data)) + (i * me->elem_size), data, n * me->elem_size);
	if ((i + n) > me->count)
		me->count = i + n;
	return 1;
}

int array_set_size(array *me, size_t n) {
	if (n < me->count) {
		me->count = n;
		return 1;
	}

	return 0;
}

size_t array_readfilei(array *me, FILE *in) {
	return array_readfile(me, in, array_readfile_funci);
}

size_t array_readfiles(array *me, FILE *in) {
	return array_readfile(me, in, array_readfile_funcs);
}

size_t array_readfile(array *me, FILE *in, void(*doline)(array *me, const char line[])) {
	array *mot = NULL;

	char zero = '\0';
	char buffer[4096];
	size_t i, n, start, count;
	mot = new_array(sizeof(char), 80);

	count = 0;
	for (n = fread(buffer, sizeof(char), sizeof(buffer), in)
	; n ; n = fread(buffer, sizeof(char), sizeof(buffer), in)) {
		for (i = 0 ; i < n ; i++) {
			for (start = i ; i < n && buffer[i] != '\n' ; i++);
			if (i > start) {
				array_addn(mot, buffer + start, (i - start));
			}

			if (i == start || (i < n && buffer[i] == '\n')) {
				array_add(mot, &zero);
				doline(me, mot->data);
				count++;
				array_clear(mot);
			}
		}
	}

	if (mot->count) {
		array_add(mot, &zero);
		doline(me, mot->data);
		count++;
	}

	free_array(mot);
	return count;
}

void array_print(array *me) {
	array_print_fmt(me, NULL, NULL);
}

void array_prints(array *me) {
	array_print_fmt(me, NULL, NULL);
	for (size_t i = 0 ; i < me->count ; i++) {
		void *d = (void *)(((void **)me->data)[i]);
		fprintf(stdout, "> %zu: %s\n", i, (char *)d);
	}
}

void array_printi(array *me) {
	array_print_fmt(me, NULL, NULL);
	int *ii = malloc(me->count * me->elem_size);
	memcpy(ii, me->data, me->count * me->elem_size);
	for (size_t i = 0 ; i < me->count ; i++) {
		fprintf(stdout, "> %zu: %d\n", i, ii[i]);
	}
	free(ii);
}

void array_printl(array *me) {
	array_print_fmt(me, NULL, NULL);
	long *l = malloc(me->count * me->elem_size);
	memcpy(l, me->data, me->count * me->elem_size);
	for (size_t i = 0 ; i < me->count ; i++) {
		fprintf(stdout, "> %zu: %ld\n", i, l[i]);
	}
	free(l);
}

void array_printf(array *me) {
	array_print_fmt(me, NULL, NULL);
	float *f = malloc(me->count * me->elem_size);
	memcpy(f, me->data, me->count * me->elem_size);
	for (size_t i = 0 ; i < me->count ; i++) {
		fprintf(stdout, "> %zu: %f\n", i, f[i]);
	}
	free(f);
}

void array_print_fmt(array *me, 
	void(*display)(char *buffer, void *), char *buffer) {

	fprintf(stdout,
		"Array of %zu elements of %zu bytes (buffer of %zu elements)\n",
		me->count, me->elem_size, me->buffer);
	if (display) {
		for (size_t i = 0 ; i < me->count ; i++) {
			void *d = (void *)(((void **)me->data)[i]);
			display(d, buffer);
			fprintf(stdout, "> %zu: %s\n", i, buffer);
		}
	}
}

/* Privates functions */

int array_assure(array *me, size_t nb_elem) {
	if (me->buffer < nb_elem) {
		me->buffer *= 2;
		if (me->buffer < nb_elem) {
			me->buffer = nb_elem;
		}
		
		me->data = realloc(me->data, me->elem_size * me->buffer);
		if (!me->data) {
			return 0;
		}
	}
	
	return 1;
}

void array_readfile_funci(array *me, const char line[]) {
	int i = atoi(line);
	array_add(me, &i);
}

void array_readfile_funcs(array *me, const char line[]) {
	char *myline = malloc((strlen(line) + 1) * sizeof(char));
	strcpy(myline, line);
	array_add(me, &myline);
}
