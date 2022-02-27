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

#include "launcher.h"
#include "utils/array.h"

array *a;

void test_array_setup() {
	a = new_array(sizeof(char), 80);
}

void test_array_teardown() {
	free_array(a);
	a = NULL;
}

void array_reset() {
	test_array_teardown();
	test_array_setup();
}

START(init)
		if (!a)
			FAIL("new_array returned NULL");

		if (array_count(a))
			FAIL("empty array has a size of %zu", array_count(a));
		END

Suite *test_array(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, test_array_setup, test_array_teardown);
	tcase_add_test(core, init);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_array_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, test_array_setup, test_array_teardown);
	// TODO

	suite_add_tcase(suite, tmore);

	return suite;
}
