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
 * @file array.h
 * @author Niki
 * @date 2020 - 2022
 *
 * @brief A simple auto-growing array-list
 * 
 * A simple auto-growing array-list, with a minimum buffer of 1.
 *
 * It has a link to `qsort`, too, so you don't need to retype all 
 * the parameters you already passed to the array.
 *
 * It is created with `new_array()` and must be freed with :
 * - `free_array()` for normal operations
 * - `array_convert()` if you want to free the array but keep the data
 *   (the data is now your responsibility to free or not, it was allocated
 *   with `malloc`/`realloc`)
 *
 * Example usage:
 * ```C
 *	array lines = new_array(sizeof(char *), 100);
 * 	
 *	const char *l1 = "2. À l'arrière";
 *	const char *l2 = "3. En réserve";
 *	const char *l3 = "1. Première ligne";
 *
 *  // push mode (usually used for int, char, long...)
 *	array_push(lines, &l1);
 *	array_push(lines, &l2);
 *
 *  // new mode (usually used for structures)
 *	char **tmp = array_new(lines);
 *	*tmp = l3;
 *
 *  // sort as Strings (also possible with int, long and custom functions)
 *	array_qsorts(lines, 0);
 *
 *	char **last_line = array_get(lines, array_count(lines) - 1);
 *	printf("Last line is now: %s\n", *last_line);
 *	// -> last_line is now: 3. En réserve
 *
 *	array_loop(lines, line, char) {
 *	    printf("Line: %s\n", line);
 *	}
 *	// -> Line: 1. Première ligne
 *	// -> Line: 2. En réserve
 *	// -> Line: 3. À l'arrière
 *
 *	free_array(lines);
 * ```
 */

#ifndef ARRAY_H
#define ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>

/**
 * Declare a new <tt>TYPE *</tt> pointer and loop through the array with it.
 *
 * How to use:
 * ```C
 * array_loop(me, line, char) {
 *     printf("Item: %s\n", line);
 * }
 * ```
 */
#define array_loop(me, ptr, TYPE) \
	for (TYPE *ptr = array_first(me); ptr; ptr = array_next(me, ptr))

/**
 * Similar to <tt>array_loop</tt>, but add a counter <tt>i</tt> starting at 0.
 *
 * @see array_loop
 *
 * How to use:
 * ```C
 * array_loop_i(me, line, char, i) {
 *     printf("Item n°%d: %s\n", i, line);
 * }
 * ```
 *
 * @note this macro does expand as 2 separate lines, surround with { } if needed
 */
#define array_loop_i(me, ptr, TYPE, i) \
	size_t i = 0; \
	for (TYPE *ptr = array_first(me); ptr; ptr = array_next(me, ptr), i++)

/**
 * @brief A simple auto-growing array-list
 *
 * The structure contains a private field (which you should not use) and the
 * current count of how many items were added. You should probably not modify
 * the count either (setting it higher is a bad idea and while it should be
 * possible to set it lower, you are strongly advised to use
 * <tt>array_cut_at</tt> instead).
 *
 * @see array_cut_at
 */
typedef struct {
	char CNAME[10];
	size_t count;
	void *priv;
} array_t;

/**
 * Create a new array.
 *
 * @note always identical to <tt>malloc</tt> + <tt>init_array</tt>
 *
 * @param elem_size the size of one element
 * @param initial the initial number of items the buffer can hold (<b>not</b> 0)
 *
 * @see malloc()
 * @see init_array(array_t *self)
 *
 * @return a new array (you must later call `free_array()` or `array_convert()`)
 */
array_t *new_array(size_t elem_size, size_t initial);

/**
 * Initialise a new array.
 *
 * @param elem_size the size of one element
 * @param initial the initial number of items the buffer can hold (<b>not</b> 0)
 */
int init_array(array_t *self, size_t elem_size, size_t initial);

/** 
 * Free the resources held for the given array: you must not use it any more.
 * Note that if you use pointers and not direct data, you may want to free
 * those yourself first.
 *
 * @note always equivalent to <tt>uninit_array</tt> + <tt>free</tt>
 *
 * @see uninit_array(array_t *self)
 * @see free(void *data)
 * @see array_clear
 * @see array_loop
 */
void free_array(array_t *me);

/**
 * Free the resources held for the given array: you must not use it any more.
 * Note that if you use pointers and not direct data, you may want to free
 * those yourself first.
 *
 * @see free_array(array_t *self)
 * @see array_clear
 * @see array_loop
 */
void uninit_array(array_t *me);

/**
 * Clear the array, that is, resets its current size to 0 (buffer unchanged).
 *
 * @note if you hold custom structures with owned resources in the array, you
 * should deallocate them properly before
 */
void array_clear(array_t *me);

/** 
 * Convert the array to a block of memory where all values are adjacent.
 *
 * @note an extra NULL value is assured to be present as last element
 *
 * @return the data (you must later call `free()` on it)
 */
void *array_convert(array_t *me);

/**
 * Return a pointer to the internal storage used by this array.
 * This is the same value as would return `array_convert()`,
 * but the array is still valid.
 *
 * Be careful if you change the content (you should not).
 *
 * @note an extra NULL value is assured to be present as last element
 *
 * @return the internal storage area
 */
void *array_data(array_t *me);

/** 
 * Return the current number of elements in the array.
 *
 * @return the number of elements in the array
 */
size_t array_count(array_t *me);

/**
 * Create a new element in the array and return a pointer to it.
 *
 * @return a pointer to the (newly allocated) last element of the array
 */
void *array_new(array_t *me);

/**
 * Create <tt>n</tt> elements in the array and return a pointer to the
 * first one ({see array_next(void *)} to get the next ones).
 *
 * @param n how many elements to add
 *
 * @return a pointer to the (newly allocated) first new element of the array
 */
void *array_newn(array_t *me, size_t n);

/**
 * Return a pointer to the first element of the array (for instance, if you
 * store integers, it will be <tt>(int *)</tt>; if you store strings, it will
 * be <tt>char **</tt>).
 *
 * @return a <i>pointer</i> to the first element, or NULL if no elements are
 * 		present
 */
void *array_first(array_t *me);

/**
 * Return a pointer to the last element of the array (for instance, if you
 * store integers, it will be <tt>(int *)</tt>; if you store strings, it will
 * be <tt>char **</tt>).
 *
 * @return a <i>pointer</i> to the last element, or NULL if no elements are
 * 		present
 */
void *array_last(array_t *me);

/**
 * Return the pointer to the previous element, or NULL if it was the first.
 *
 * @param ptr a pointer from an array (the array must be valid)
 *
 * @return the previous element, or NULL
 */
void *array_prev(array_t *me, void *ptr);

/**
 * Return the pointer to the next element, or NULL if it was the last.
 *
 * @param ptr a pointer from an array (the array must be valid)
 *
 * @return the next element, or NULL
 */
void *array_next(array_t *me, void *ptr);

/**
 * Retrieve the the pointer of an item.
 * The address of the item will be returned.
 *
 * @param i the index of the element to retrieve
 *
 * @note if the index is out of bounds, you will get invalid data
 *
 * @return the pointer to the i'th element
 */
void *array_get(array_t *me, size_t i);

/**
 * Return a pointer to the last element of this array and remove it from the
 * array, if the array is not empty.
 *
 * @note be careful, the memory pointed to by the element will be reused the
 * 		next time we add an element -- you should not use it after this; in
 * 		short, the return value is mainly so you can call <tt>free</tt> on
 * 		value pointed to by this pointer (<b>not</b> the pointer itself) if it
 * 		is a pointer to memory you own, or use it locally before continuing to
 * 		use the array
 * @note in case this was not clear, do <b>not</b> call <tt>free</tt> on the
 * 		returned value
 *
 * @return a pointer to the last (now removed) item, or NULL if no element
 */
void *array_pop(array_t *me);

/**
 * Cut the array at the given size and return a pointer to the first element
 * that was removed if any.
 *
 * @note be careful, the memory pointed to by the element(s) will be reused the
 * 		next time we add an element -- you should not use it after this; in
 * 		short, the return value is mainly so you can call <tt>free</tt> on
 * 		value(s) pointed to by this pointer (<b>not</b> the pointer itself) if
 * 		it is a pointer to memory you own, or use it locally before continuing
 * 		to use the array
 * @note in case this was not clear, do <b>not</b> call <tt>free</tt> on the
 * 		returned value(s)
 *
 * @return a pointer to the first removed element, or NULL
 */
void *array_cut_at(array_t *me, size_t n);

/** 
 * Compact the array (resize the buffer so it is equals to the current number 
 * of items in the array or size 1 if there are no items in the array).
 */
void array_compact(array_t *me);

/**
 * Sort the array with a call to `qsort()`.
 * All the appropriate parameters are passed, except the sorting function.
 *
 * @param compar a custom comparison function
 * @param (parameters)
 * 	* `itm1`: a **pointer** to one of your data element (for instance, 
 * 		if you store floats, it will be a `(float *)`, 
 * 		so you would need to cast it via `((float *)itm1)[0]`
 * 	* `itm2`: another **pointer** to compare with the first one
 * @param (returns)
 * 	* `-1`: if element A is less than element B
 * 	*  `0`: if both elements are equals
 * 	*  `1`: if element A is more than element B
 */
void array_qsort(array_t *me, int (*compar)(const void *itm1, const void *itm2));

/**
 * Sort the array with `qsort()`, data is `char *`.
 * 
 * @param rev FALSE for normal order, TRUE for reverse order
 *
 * @see array_qsort
 */
void array_qsorts(array_t *me, int rev);

/**
 * Sort the array with `qsort()`, data is `int`.
 * 
 * @param rev FALSE for normal order, TRUE for reverse order
 *
 * @see array_qsort
 */
void array_qsorti(array_t *me, int rev);

/** 
 * Sort the array with `qsort()`, data is `long`.
 * 
 * @param rev FALSE for normal order, TRUE for reverse order
 *
 * @see array_qsort
 */
void array_qsortl(array_t *me, int rev);

/**
 * Sort the array with `qsort()`, data is `float`.
 * 
 * @param rev FALSE for normal order, TRUE for reverse order
 *
 * @see array_qsort
 */
void array_qsortf(array_t *me, int rev);

/**
 * Add an element to the array (will create a new item in the array and copy the
 * data from the given element in it).
 *
 * @param data the memory position of the element to add
 *
 * @return FALSE if the array is too short and we cannot allocate enough 
 * 	contiguous memory for its needs
 */
int array_push(array_t *me, void *data);

/**
 * Add multiple elements to the array (will create new items in the array and
 * copy the data from the given elements in them).
 *
 * @param data the memory position of the elements to add, adjacent to each
 *  other
 * @param n the number of elements to copy from `data`
 *
 * @return FALSE if the array is too short and we cannot allocate enough 
 * 	contiguous memory for its needs
 */
int array_pushn(array_t *me, void *data, size_t n);

/**
 * Retrieve the content of an item.
 * The item will be copied to the given address location if it exists.
 *
 * @param target an address where to write a copy of the item
 * @param i the index of the element to retrieve
 *
 * @return TRUE if the item exists (if <tt>i</tt> is an element of the array)
 */
int array_copy(array_t *me, void *target, size_t i);

/**
 * Retrieve the content of multiple items if they exist.
 * The items will be copied in a sequence to the given address location.
 *
 * @param target an address where to write a copy of the items
 * @param i the index of the first element to retrieve
 * @param n the number of elements to retrieve
 *
 * @return TRUE if the item exists (if <tt>i</tt> to <tt>n</tt> are elements
 * 		of the array)
 */
int array_copyn(array_t *me, void *target, size_t i, size_t n);

/**
 * Set an element of the array to the given value.
 * Can also append an element at the end of the array (i.e., <tt>i</tt> can be
 * the size of the array and this will result in an array with one more
 * element).
 * Memory will be copied from the given data to the array.
 *
 * @param i the element index
 * @param data the data that will replace the current value, or new data
 * 	to append after the current elements (you can add items just at
 * 	the end of the array (index = count), but it is not allowed to
 * 	set items after that index, so not to create holes)
 *
 * @return FALSE if the array is too short and we cannot allocate enough 
 * 	contiguous memory for its needs, or if the index is out of bounds
 */
int array_set(array_t *me, size_t i, void *data);

/**
 * Set elements of the array to the given value.
 * Can also append elements at the end of the array (i.e., <tt>i</tt> can be
 * the size of the array and this will result in an array with <tt>n</tt> more
 * elements).
 * Memory will be copied from the given data to the array.
 *
 * @param i the element index to start the insertion at
 * @param data the data that will replace the current values, or new data
 * 	to append after the current elements (you can add items just at
 * 	the end of the array (index = count), but it is not allowed to
 * 	set items with `i` after that index, so not to create holes)
 * @param n the number of elements to copy from `data` and to insert at `i`
 *
 * @return FALSE if the array is too short and we cannot allocate enough 
 * 	contiguous memory for its needs, or if the index is out of bounds
 */
int array_setn(array_t *me, size_t i, void *data, size_t n);

#endif /* ARRAY_H */

#ifdef __cplusplus
}
#endif

