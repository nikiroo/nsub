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

#include "main.h"
#include "launcher.h"

SRunner *runner = NULL;
void add_test(Suite *test);

SRunner *get_tests(int more) {
	add_test(test_cstring("cstring"));
	if (more)
		add_test(test_cstring_more("cstring -- more (longer)"));

	add_test(test_array("array"));
	if (more)
		add_test(test_array_more("array -- more (longer)"));

	add_test(test_base64("base64"));
	if (more)
		add_test(test_base64_more("base64 -- more (longer)"));

	return runner;
}

void add_test(Suite *test) {
	if (!runner) {
		runner = srunner_create(test);
	} else {
		srunner_add_suite(runner, test);
	}
}
