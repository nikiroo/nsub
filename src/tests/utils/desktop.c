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

#include "../../utils/desktop.h"

#include <check.h>

#include "../launcher.h"

desktop_t *d;

static void setup() {
	d = new_desktop("utils/test.desktop", 24);
}

static void teardown() {
	free_desktop(d);
}

static void reset() {
	teardown();
	setup();
}

START(init)
		if (!d)
			FAIL("new_desktop returned NULL");

		ASSERT_EQUALS_STR("Name", "IRC", d->name);
		ASSERT_EQUALS_STR("Icon", "irssi", d->icon);
		ASSERT_EQUALS_STR("Exec", "irssi", d->icon);

		END

		// TODO
START(NO_TEST_YET_submenu)
		END
START(NO_TEST_YET_icons)
		END
START(NO_TEST_YET_find_icon)
		END
START(NO_TEST_YET_find_id)
		END

Suite *test_desktop(const char title[]) {

	d = new_desktop("test.desktop", 24);
	free_desktop(d);

	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, setup, teardown);
	tcase_add_test(core, init);
	tcase_add_test(core, NO_TEST_YET_submenu);
	tcase_add_test(core, NO_TEST_YET_icons);
	tcase_add_test(core, NO_TEST_YET_find_icon);
	tcase_add_test(core, NO_TEST_YET_find_id);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_desktop_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, setup, teardown);
	// TODO

	suite_add_tcase(suite, tmore);

	return suite;
}
