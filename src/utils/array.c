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

array_t *new_array(size_t elem_size, size_t initial) {
	if (!initial)
		return NULL;

	array_t *me = malloc(sizeof(array_t));
	if (!init_array(me, elem_size, initial)) {
		free(me);
		me = NULL;
	}

	return me;
}

int init_array(array_t *me, size_t elem_size, size_t initial) {
	if (!initial)
		return 0;

	strcpy(me->CNAME, "[CArray ]");

	me->priv = malloc(sizeof(priv_t));
	if (!me->priv)
		return 0;

	priv_t *priv = (priv_t *) me->priv;
	strcpy(me->CNAME, "[CArray ]");
	priv->elem_size = elem_size;
	me->count = 0;
	priv->buffer = initial;
	priv->data = malloc(elem_size * initial);
	if (!priv->data) {
		free(me->priv);
		return 0;
	}

	return 1;
}

void free_array(array_t *me) {
	if (me)
		uninit_array(me);

	free(me);
}

void uninit_array(array_t *me) {
	priv_t *priv = (priv_t *) me->priv;
	me->count = 0;
	priv->buffer = 0;
	free(priv->data);
	priv->data = NULL;
	free(priv);
	me->priv = NULL;
	me->CNAME[0] = '!';
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
