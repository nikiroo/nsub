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

#include "utils/array.h"

#include <check.h>
#include <stddef.h>
#include <string.h>

#include "launcher.h"

static array_t *a;

static void setup() {
	a = new_array(sizeof(char), 80);
}

static void teardown() {
	free_array(a);
	a = NULL;
}

static void reset() {
	teardown();
	setup();
}

START(init)
		if (!a)
			FAIL("new_array returned NULL");

		if (array_count(a))
			FAIL("empty array has a size of %zu", array_count(a));

		END

START(clear)
		char car = 'T';

		array_push(a, &car);
		array_push(a, &car);
		array_clear(a);
		if (array_count(a))
			FAIL("empty array has a size of %zu", array_count(a));

		END

START(convert)
		char car = 'T';
		char eos = '\0';
		char *str;

		array_push(a, &car);
		array_push(a, &car);
		array_push(a, &eos);

		str = (char*) array_convert(a);
		ASSERT_EQUALS_STR("array of chars to string conversion", "TT", str);

		free(str);
		a = NULL;

		END

START(data)
		char car = 'T';
		char eos = '\0';
		char *str;

		array_push(a, &car);
		array_push(a, &car);
		array_push(a, &eos);

		str = (char*) array_data(a);
		ASSERT_EQUALS_STR("array of chars to string conversion", "TT", str);

		END

START(count)
		char car = 'T';
		char eos = '\0';

		ASSERT_EQUALS_INT("empty array", 0, array_count(a));
		reset();

		array_push(a, &car);
		ASSERT_EQUALS_INT("1-char array", 1, array_count(a));
		reset();

		array_push(a, &car);
		array_push(a, &car);
		array_push(a, &eos);
		ASSERT_EQUALS_INT("array with a NUL at the end", 3, array_count(a));
		reset();

		END

START(pointers)
		char *ptr;
		char car;

		ptr = array_new(a);
		*ptr = 'T';
		ASSERT_EQUALS_INT("add a new element via ptr", 1, array_count(a));

		ptr = array_first(a);
		car = *ptr;
		if (car != 'T')
			FAIL("first_ptr: 'T' was expected, but we got: '%c'", car);

		reset();

		char *testy = "Testy";
		ptr = array_newn(a, strlen(testy) + 1);
		for (size_t i = 0; i <= strlen(testy); i++) {
			*ptr = testy[i];
			ptr++;
		}

		ptr = array_first(a);
		for (size_t i = 0; i < strlen(testy); i++) {
			ASSERT_EQUALS_STR("next_ptr", testy + i, (char* )ptr);
			ptr = array_next(a, ptr);
		}

		array_loop_i(a, ptr2, char, i) {
			if (i < strlen(testy))
				ASSERT_EQUALS_STR("array_loop", testy + i, ptr2);
			else if (i == strlen(testy))
				ASSERT_EQUALS_STR("array_loop (last)", "", ptr2);
			else
				FAIL("array_loop went out of bounds");
		}

		END

START(get)
		char *str = "Testy";
		size_t sz = strlen(str);

		array_pushn(a, str, sz + 1);

		for (size_t i = 0; i < sz + 1; i++) {
			char expected = str[i];
			char *ret = array_get(a, i);
			ASSERT_EQUALS_INT("get each element", expected, *ret);
		}

		END

START(pop)
		char *rep;

		if (array_pop(a))
			FAIL("popping an item from an empty array");

		rep = array_new(a);
		*rep = 'T';

		rep = array_pop(a);
		if (!rep)
			FAIL("cannot pop item from 1-sized array");

		ASSERT_EQUALS_INT("bad item popped", (int )'T', (int )*rep);

		if (a->count)
			FAIL("popped 1-sized array still has %d items", a->count);

		rep = array_new(a);
		*rep = 'T';
		rep = array_new(a);
		*rep = 'T';

		rep = array_pop(a);
		ASSERT_EQUALS_INT("bad item 1 popped", (int )'T', (int )*rep);
		ASSERT_EQUALS_INT("popping should remove 1 from count", 1, a->count);

		rep = array_pop(a);
		ASSERT_EQUALS_INT("bad item 2 popped", (int )'T', (int )*rep);
		ASSERT_EQUALS_INT("popping should remove 1 from count", 0, a->count);

		END

START(cut_at)
		char *rep;

		if (array_cut_at(a, 4))
			FAIL("cutting an empty array returned something");

		rep = array_new(a);
		*rep = 'T';
		rep = array_new(a);
		*rep = 'T';

		if (array_cut_at(a, 4))
			FAIL("cutting an array above its count returned something");

		if (!array_cut_at(a, 1))
			FAIL("failed to cut an array");
		ASSERT_EQUALS_INT("cutting at 1 should get a 1-sized array", 1,
				a->count);

		if (!array_cut_at(a, 0))
			FAIL("failed to cut an array");
		ASSERT_EQUALS_INT("cutting at 0 should get a 1-sized array", 0,
				a->count);

		END

START(compact)
		char car = 'T';
		char eos = '\0';

		array_compact(a);

		array_push(a, &car);
		array_push(a, &car);
		array_push(a, &eos);

		ASSERT_EQUALS_STR("compacted array has wrong data", "TT",
				(char* )array_data(a));

		END

		// TODO
START(NO_TEST_YET_qsort)
		END
START(NO_TEST_YET_push)
		END
START(NO_TEST_YET_set)
		END
START(NO_TEST_YET_copy)
		END
START(NO_TEST_YET_readfile)
		END
START(NO_TEST_YET_print)
		END

Suite *test_array(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, setup, teardown);
	tcase_add_test(core, init);
	tcase_add_test(core, clear);
	tcase_add_test(core, convert);
	tcase_add_test(core, data);
	tcase_add_test(core, count);
	tcase_add_test(core, pointers); // new, newn, first, next
	tcase_add_test(core, get);
	tcase_add_test(core, pop);
	tcase_add_test(core, cut_at);
	tcase_add_test(core, compact);

	tcase_add_test(core, NO_TEST_YET_qsort);
	tcase_add_test(core, NO_TEST_YET_push);
	tcase_add_test(core, NO_TEST_YET_set);
	tcase_add_test(core, NO_TEST_YET_copy);
	tcase_add_test(core, NO_TEST_YET_readfile);
	tcase_add_test(core, NO_TEST_YET_print);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_array_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, setup, teardown);
	// TODO

	suite_add_tcase(suite, tmore);

	return suite;
}
