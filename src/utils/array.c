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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "array.h"

typedef struct {
	size_t elem_size;
	size_t buffer;
	void *data;
} priv_t;

/* make sure we have at least n elements in the buffer, grow if needed */
static int array_assure(array_t *me, size_t nb_elem);

/* for qsort operations */
static int array_qsorts_func(const void *a, const void *b);
static int array_qsorti_func(const void *a, const void *b);
static int array_qsortl_func(const void *a, const void *b);
static int array_qsortf_func(const void *a, const void *b);
static int array_qsorts_rfunc(const void *a, const void *b);
static int array_qsorti_rfunc(const void *a, const void *b);
static int array_qsortl_rfunc(const void *a, const void *b);
static int array_qsortf_rfunc(const void *a, const void *b);

/* for reafilei, readfiles */
static void array_readfile_funci(array_t *me, const char line[]);
static void array_readfile_funcs(array_t *me, const char line[]);

array_t *new_array(size_t elem_size, size_t initial) {
	array_t *me = NULL;

	if (initial)
		me = malloc(sizeof(array_t));

	if (me) {
		me->priv = malloc(sizeof(priv_t));
		if (me->priv) {
			priv_t *priv = (priv_t *) me->priv;
			strcpy(me->CNAME, "[CArray ]");
			priv->elem_size = elem_size;
			me->count = 0;
			priv->buffer = initial;
			priv->data = malloc(elem_size * initial);
			if (!priv->data) {
				free(me->priv);
				free(me);
				me = NULL;
			}
		}
	}

	return me;
}

void free_array(array_t *me) {
	if (!me)
		return;

	priv_t *priv = (priv_t *) me->priv;
	me->count = 0;
	priv->buffer = 0;
	free(priv->data);
	free(priv);
	free(me);
}

void array_clear(array_t *me) {
	me->count = 0;
}

// convert to void * data (free the rest)
void *array_convert(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;
	void *data = priv->data;
	free(priv);
	free(me);
	return data;
}

void *array_data(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;
	return priv->data;
}

size_t array_count(array_t *me) {
	return me->count;
}

void *array_new(array_t *me) {
	return array_newn(me, 1);
}

void *array_newn(array_t *me, size_t how_many) {
	if (!array_assure(me, me->count + how_many))
		return 0;

	me->count += how_many;
	return array_get(me, me->count - how_many);
}

void *array_first(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;
	return priv->data;
}

void *array_next(array_t *me, void *ptr) {
	priv_t *priv = (priv_t *) me->priv;

	// cast to (char *) because we want 'byte' arithmetic
	char *cptr = (char *) ptr;
	char *cdata = (char*) priv->data;

	if (cptr) {
		cptr += priv->elem_size;
		char *last = cdata + ((me->count - 1) * priv->elem_size);
		if (cptr <= last) {
			return cptr;
		}
	}

	return NULL;
}

void *array_get(array_t *me, size_t i) {
	priv_t *priv = (priv_t *) me->priv;

	// cast to (char *) because we want 'byte' arithmetic
	return (void *) (((char *) priv->data) + (i * priv->elem_size));
}

void *array_pop(array_t *me) {
	return array_cut_at(me, me->count - 1);
}

void *array_cut_at(array_t *me, size_t n) {
	if (n < me->count) {
		void *item = array_get(me, n);
		me->count = n;
		return item;
	}

	return NULL;
}

void array_compact(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;

	int c = me->count ? me->count : 1;
	priv->data = realloc(priv->data, c * priv->elem_size);
	priv->buffer = c;
}

void array_qsort(array_t *me, int (*compar)(const void *, const void *)) {
	priv_t *priv = (priv_t *) me->priv;
	qsort(priv->data, me->count, priv->elem_size, compar);
}

void array_qsorts(array_t *me, int rev) {
	array_qsort(me, rev ? array_qsorts_rfunc : array_qsorts_func);
}
static int array_qsorts_func(const void *a, const void *b) {
	char *stra = ((char **) a)[0];
	char *strb = ((char **) b)[0];
	return strcmp(stra, strb);
}
static int array_qsorts_rfunc(const void *a, const void *b) {
	char *stra = ((char **) a)[0];
	char *strb = ((char **) b)[0];
	return strcmp(strb, stra);
}

void array_qsorti(array_t *me, int rev) {
	array_qsort(me, rev ? array_qsorti_rfunc : array_qsorti_func);
}
static int array_qsorti_func(const void *a, const void *b) {
	long ia, ib;
	ia = ((int *) a)[0];
	ib = ((int *) b)[0];
	if (ia < ib)
		return -1;
	return !(ia == ib);
}
static int array_qsorti_rfunc(const void *a, const void *b) {
	long ia, ib;
	ia = ((int *) a)[0];
	ib = ((int *) b)[0];
	if (ia > ib)
		return -1;
	return !(ia == ib);
}

void array_qsortl(array_t *me, int rev) {
	array_qsort(me, rev ? array_qsortl_rfunc : array_qsortl_func);
}
static int array_qsortl_func(const void *a, const void *b) {
	long la, lb;
	la = ((long *) a)[0];
	lb = ((long *) b)[0];
	if (la < lb)
		return -1;
	return !(la == lb);
}
static int array_qsortl_rfunc(const void *a, const void *b) {
	long la, lb;
	la = ((long *) a)[0];
	lb = ((long *) b)[0];
	if (la > lb)
		return -1;
	return !(la == lb);
}

void array_qsortf(array_t *me, int rev) {
	array_qsort(me, rev ? array_qsortf_rfunc : array_qsortf_func);
}
static int array_qsortf_func(const void *a, const void *b) {
	float fa, fb;
	fa = ((float *) a)[0];
	fb = ((float *) b)[0];
	// Also works:
	//memcpy(&fa, a, sizeof(float));
	//memcpy(&fb, b, sizeof(float));
	if (fa < fb)
		return -1;
	return !(fa == fb);
}
static int array_qsortf_rfunc(const void *a, const void *b) {
	float fa, fb;
	fa = ((float *) a)[0];
	fb = ((float *) b)[0];
	// Also works:
	//memcpy(&fa, a, sizeof(float));
	//memcpy(&fb, b, sizeof(float));
	if (fa > fb)
		return -1;
	return !(fa == fb);
}

int array_push(array_t *me, void *data) {
	return array_setn(me, me->count, data, 1);
}

int array_pushn(array_t *me, void *data, size_t n) {
	return array_setn(me, me->count, data, n);
}

int array_copy(array_t *me, void *target, size_t i) {
	return array_copyn(me, target, i, 1);
}

int array_copyn(array_t *me, void *target, size_t i, size_t n) {
	priv_t *priv = (priv_t *) me->priv;

	if (i + n < me->count) {
		// cast to (char *) because we want 'byte' arithmetic
		memcpy(target, ((char *) (priv->data)) + (i * priv->elem_size),
				n * priv->elem_size);

		return 1;
	}

	return 0;
}

int array_set(array_t *me, size_t i, void *data) {
	return array_setn(me, i, data, 1);
}

int array_setn(array_t *me, size_t i, void *data, size_t n) {
	// allow new items BUT NOT holes in the array
	if (i > me->count)
		return 0;

	if (!array_assure(me, i + n))
		return 0;

	priv_t *priv = (priv_t *) me->priv;

	// cast to (char *) because we want 'byte' arithmetic
	memcpy(((char *) (priv->data)) + (i * priv->elem_size), data,
			n * priv->elem_size);
	if ((i + n) > me->count)
		me->count = i + n;

	return 1;
}

size_t array_readfilei(array_t *me, FILE *in) {
	return array_readfile(me, in, array_readfile_funci);
}

size_t array_readfiles(array_t *me, FILE *in) {
	return array_readfile(me, in, array_readfile_funcs);
}

size_t array_readfile(array_t *me, FILE *in,
		void (*doline)(array_t *me, const char line[])) {
	array_t *mot = NULL;

	char zero = '\0';
	char buffer[4096];
	size_t i, n, start, count;
	mot = new_array(sizeof(char), 80);
	priv_t *mot_priv = (priv_t *) mot->priv;

	count = 0;
	for (n = fread(buffer, sizeof(char), sizeof(buffer), in)
	; n; n = fread(buffer, sizeof(char), sizeof(buffer), in)) {
		for (i = 0; i < n; i++) {
			for (start = i; i < n && buffer[i] != '\n'; i++)
				;
			if (i > start) {
				array_pushn(mot, buffer + start, (i - start));
			}

			if (i == start || (i < n && buffer[i] == '\n')) {
				array_push(mot, &zero);
				doline(me, mot_priv->data);
				count++;
				array_clear(mot);
			}
		}
	}

	if (mot->count) {
		array_push(mot, &zero);
		doline(me, mot_priv->data);
		count++;
	}

	free_array(mot);
	return count;
}

void array_print(array_t *me) {
	array_print_fmt(me, NULL, NULL);
}

void array_prints(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;

	array_print_fmt(me, NULL, NULL);
	for (size_t i = 0; i < me->count; i++) {
		void *d = (void *) (((void **) priv->data)[i]);
		fprintf(stdout, "> %zu: %s\n", i, (char *) d);
	}
}

void array_printi(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;

	array_print_fmt(me, NULL, NULL);
	int *ii = malloc(me->count * priv->elem_size);
	memcpy(ii, priv->data, me->count * priv->elem_size);
	for (size_t i = 0; i < me->count; i++) {
		fprintf(stdout, "> %zu: %d\n", i, ii[i]);
	}
	free(ii);
}

void array_printl(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;

	array_print_fmt(me, NULL, NULL);
	long *l = malloc(me->count * priv->elem_size);
	memcpy(l, priv->data, me->count * priv->elem_size);
	for (size_t i = 0; i < me->count; i++) {
		fprintf(stdout, "> %zu: %ld\n", i, l[i]);
	}
	free(l);
}

void array_printf(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;

	array_print_fmt(me, NULL, NULL);
	float *f = malloc(me->count * priv->elem_size);
	memcpy(f, priv->data, me->count * priv->elem_size);
	for (size_t i = 0; i < me->count; i++) {
		fprintf(stdout, "> %zu: %f\n", i, f[i]);
	}
	free(f);
}

void array_print_fmt(array_t *me, void (*display)(char *buffer, void *),
		char *buffer) {
	priv_t *priv = (priv_t *) me->priv;

	fprintf(stdout,
			"Array of %zu elements of %zu bytes (buffer of %zu elements)\n",
			me->count, priv->elem_size, priv->buffer);
	if (display) {
		for (size_t i = 0; i < me->count; i++) {
			void *d = (void *) (((void **) priv->data)[i]);
			display(d, buffer);
			fprintf(stdout, "> %zu: %s\n", i, buffer);
		}
	}
}

/* Privates functions */

static int array_assure(array_t *me, size_t nb_elem) {
	priv_t *priv = (priv_t *) me->priv;

	if (priv->buffer < nb_elem) {
		priv->buffer *= 2;
		if (priv->buffer < nb_elem) {
			priv->buffer = nb_elem;
		}

		void *tmp = realloc(priv->data, priv->elem_size * priv->buffer);
		if (!tmp)
			return 0;

		priv->data = tmp;
	}

	return 1;
}

static void array_readfile_funci(array_t *me, const char line[]) {
	int i = atoi(line);
	array_push(me, &i);
}

static void array_readfile_funcs(array_t *me, const char line[]) {
	char *myline = malloc((strlen(line) + 1) * sizeof(char));
	strcpy(myline, line);
	char **n = array_new(me);
	*n = myline;
}
