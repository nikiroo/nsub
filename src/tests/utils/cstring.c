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
#include "utils/cstring.h"

#define TEST_FILE_READLN "utils/test_readln.txt"

cstring *s;

void test_cstring_setup() {
	s = new_cstring();
}

void test_cstring_teardown() {
	free_cstring(s);
	s = NULL;
}

void cstring_reset() {
	test_cstring_teardown();
	test_cstring_setup();
}

START(init)
		if (!s)
			FAIL("new_cstring returned NULL");

		if (s->length)
			FAIL("empty cstring has a size of %zu", s->length);

		END

START(add_car)
		cstring_add_car(s, 'T');
		ASSERT_EQUALS_STR("Cannot add a single char", "T", s->string);

		cstring_add_car(s, 'e');
		cstring_add_car(s, 's');
		cstring_add_car(s, 't');
		cstring_add_car(s, 'y');
		ASSERT_EQUALS_STR("Cannot add multiple chars", "Testy", s->string);

		END

START(adds)
		char *str;

		str = "testy";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Short string", str, s->string);
		cstring_reset();

		str = "Fanfan entre dans un bar";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Medium string", str, s->string);
		cstring_reset();

		str = "Ligne 1\nLigne 2\nLigne 3, attention 4 = vide\n";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Multi-line", str, s->string);
		cstring_reset();

		str =
				"Les accents en français sont bien là et se retrouvent avec une fréquence élevée";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("accents", str, s->string);
		cstring_reset();

		str = "cents: ¢, copyright: ©, arrows: →↓↑←";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("UTF-8", str, s->string);
		cstring_reset();

		str = "Os iusti meditabitur sapientiam,\n"
				"Et lingua\n"
				"eius loquetur\n"
				"indicium.\n"
				"\n"
				"Beatus\n"
				"vir qui\n"
				"suffert tentationem, Quoniqm\n"
				"cum probates\n"
				"fuerit accipient\n"
				"coronam vitae\n"
				"\n"
				"Kyrie, fons bonitatis.\n"
				"Kyrie, ignis divine, eleison.\n"
				"\n"
				"O quam sancta, quam serena,\n"
				"Quam benigma, quam amoena esse Virgo creditur.\n"
				"O quam sancta, quam serena,\n"
				"Quam benigma, quam amoena,\n"
				"O castitatis lilium.\n"
				"\n"
				"Kyrie, fons bonitatis.\n"
				"Kyrie, ignis divine, eleison.\n"
				"\n"
				"O quam sancta, quam serena,\n"
				"Quam benigma, quam amoena,\n"
				"O castitatis lilium.\n";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Long, multi-line string", str, s->string);
		cstring_reset();

		END

START(clear)
		if (!s)
			FAIL("new_cstring returned NULL");

		if (s->length)
			FAIL("empty cstring has a size of %zu", s->length);

		END

START(addp)
		char *str;

		cstring_addp(s, "%d", 42);
		ASSERT_EQUALS_STR("Simple int", "42", s->string);
		cstring_reset();

		cstring_addp(s, "%02d", 1);
		ASSERT_EQUALS_STR("Leading zero int", "01", s->string);
		cstring_reset();

		cstring_addp(s, "%d", 352646);
		ASSERT_EQUALS_STR("Large int", "352646", s->string);
		cstring_reset();

		str = "Simple test string";
		cstring_addp(s, "%s", str);
		ASSERT_EQUALS_STR("Simple string", str, s->string);
		cstring_reset();

		cstring_addp(s, "%s", "String 1, ");
		str = "String 2";
		cstring_addp(s, "%s", "String 2");
		ASSERT_EQUALS_STR("Cumulative strings", "String 1, String 2", s->string);
		cstring_reset();

		END

START(readln)
		int read;
		FILE *testin = fopen(TEST_FILE_READLN, "r");
		if (!testin)
			FAIL("Test file not found: test_readln.txt");

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("first line should not be last");
		ASSERT_EQUALS_STR("first line incorrect", "ligne 1", s->string);
		cstring_reset();

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("second line should not be last");
		ASSERT_EQUALS_STR("second line incorrect", "", s->string);
		cstring_reset();

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("third line should not be last");
		ASSERT_EQUALS_STR("third line incorrect", "ligne 3", s->string);
		cstring_reset();

		if (cstring_readline(s, testin)) {
			FAIL("fourth line should not exist");
		}

		END

START(ends_with)
		char *end;

		cstring_add(s, "fichier.ext");

		end = ".ext";
		if (!cstring_ends_with(s, end))
			FAIL("fichier.ext does not end in %s", end);

		end = ".ex";
		if (cstring_ends_with(s, end))
			FAIL("fichier.ext ends in %s", end);

		end = "aext";
		if (cstring_ends_with(s, end))
			FAIL("fichier.ext does not end in %s", end);

		end = "";
		if (!cstring_ends_with(s, end))
			FAIL("fichier.ext does not end with nothing");

		END

START(starts_with)
		char *start;

		cstring_add(s, "fichier.ext");

		start = "fichier";
		if (!cstring_starts_with(s, start, 0))
			FAIL("fichier.ext does not start with %s", start);

		start = "ichier";
		if (cstring_starts_with(s, start, 0))
			FAIL("fichier.ext starts with %s", start);

		start = "afichier";
		if (cstring_starts_with(s, start, 0))
			FAIL("fichier.ext starts with %s", start);

		start = "";
		if (!cstring_starts_with(s, start, 0))
			FAIL("fichier.ext does not start with nothing");

		start = "chier";
		if (!cstring_starts_with(s, start, 2))
			FAIL("fichier.ext +2 does not start with %s", start);

		END

START(many_adds)
		size_t count = 50000000;
		for (size_t i = 0; i < count; i++) {
			cstring_add(s, "1234567890");
		}

		ASSERT_EQUALS_SIZE("Lot of adds", count * 10, s->length);

		END

Suite *test_cstring(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, test_cstring_setup, test_cstring_teardown);
	tcase_add_test(core, init);
	tcase_add_test(core, add_car);
	tcase_add_test(core, adds);
	tcase_add_test(core, clear);
	tcase_add_test(core, addp);
	tcase_add_test(core, readln);
	tcase_add_test(core, ends_with);
	tcase_add_test(core, starts_with);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_cstring_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, test_cstring_setup, test_cstring_teardown);
	tcase_add_test(tmore, many_adds);

	suite_add_tcase(suite, tmore);

	return suite;
}
