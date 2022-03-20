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

#include <stdlib.h>

#include "launcher.h"
#include "utils/cstring.h"

#define TEST_FILE_READLINE "utils/test_readline.txt"

cstring_t *s;

static void setup() {
	s = new_cstring();
}

static void teardown() {
	free_cstring(s);
	s = NULL;
}

static void reset() {
	teardown();
	setup();
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

START(add_all_but_p)
		char *str;

		str = "testy";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Short string", str, s->string);
		reset();

		str = "Fanfan entre dans un bar";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Medium string", str, s->string);
		reset();

		str = "Ligne 1\nLigne 2\nLigne 3, attention 4 = vide\n";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("Multi-line", str, s->string);
		reset();

		str = "Les accents en français sont bien là et se "
				"retrouvent avec une fréquence élevée";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("accents", str, s->string);
		reset();

		str = "cents: ¢, copyright: ©, arrows: →↓↑←";
		cstring_add(s, str);
		ASSERT_EQUALS_STR("UTF-8", str, s->string);
		reset();

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
		reset();

		str = "String 1, String 2";
		cstring_add(s, "String 1, ");
		cstring_add(s, "String 2");
		ASSERT_EQUALS_STR("Concat with add", str, s->string);
		reset();

		str = "0123456789";
		cstring_addf(s, str, 4);
		ASSERT_EQUALS_STR("addf error (start at 4)", "456789", s->string);
		reset();

		str = "0123456789";
		cstring_addf(s, str, 4);
		cstring_addf(s, str, 8);
		str = "45678989";
		ASSERT_EQUALS_STR("Concat with addf", str, s->string);
		reset();

		str = "0123456789";
		cstring_addn(s, str, 4);
		ASSERT_EQUALS_STR("addn error (4)", "0123", s->string);
		reset();

		str = "0123456789";
		cstring_addn(s, str, 4);
		cstring_addn(s, str, 2);
		str = "012301";
		ASSERT_EQUALS_STR("Concat with addn", str, s->string);
		reset();

		str = "0123456789";
		cstring_addfn(s, str, 4, 3);
		ASSERT_EQUALS_STR("addf error (4, 3)", "456", s->string);
		reset();

		str = "0123456789";
		cstring_addfn(s, str, 4, 3);
		cstring_addfn(s, str, 2, 2);
		str = "45623";
		ASSERT_EQUALS_STR("Concat with addfn", str, s->string);
		reset();

		END

START(addp)
		char *str;

		cstring_addp(s, "%d", 42);
		ASSERT_EQUALS_STR("Simple int", "42", s->string);
		reset();

		cstring_addp(s, "%02d", 1);
		ASSERT_EQUALS_STR("Leading zero int", "01", s->string);
		reset();

		cstring_addp(s, "%d", 352646);
		ASSERT_EQUALS_STR("Large int", "352646", s->string);
		reset();

		str = "Simple test string";
		cstring_addp(s, "%s", str);
		ASSERT_EQUALS_STR("Simple string", str, s->string);
		reset();

		str = "String 1, String 2";
		cstring_addp(s, "%s", "String 1, ");
		cstring_addp(s, "%s", "String 2");
		ASSERT_EQUALS_STR("Cumulative strings", str, s->string);
		reset();

		END

START(cut_at)
		cstring_add(s, "0123456789");
		cstring_cut_at(s, 4);
		ASSERT_EQUALS_STR("cut at failed", "0123", s->string);

		END

START(substring)
		free_cstring(s);
		s = cstring_substring("0123456789", 4, 2);
		ASSERT_EQUALS_STR("substring failed 4,2", "45", s->string);

		free_cstring(s);
		s = cstring_substring("0123456789", 0, 4);
		ASSERT_EQUALS_STR("substring failed 0,4", "0123", s->string);

		free_cstring(s);
		s = cstring_substring("0123456789", 3, 0);
		ASSERT_EQUALS_STR("substring failed 3,0", "3456789", s->string);

		free_cstring(s);
		s = cstring_substring("0123456789", 0, 0);
		ASSERT_EQUALS_STR("substring failed 0,0", "0123456789", s->string);

		END

START(reverse)
		cstring_add(s, "kayak");
		cstring_reverse(s->string);
		ASSERT_EQUALS_STR("reversing a palyndrome failed", "kayak", s->string);
		reset();

		cstring_add(s, "plantigrade");
		cstring_reverse(s->string);
		ASSERT_EQUALS_STR("reverse failed", "edargitnalp", s->string);
		reset();

		cstring_add(s, "123");
		cstring_reverse(s->string);
		ASSERT_EQUALS_STR("reverse failed", "321", s->string);
		reset();

		cstring_add(s, "1234");
		cstring_reverse(s->string);
		ASSERT_EQUALS_STR("reverse failed", "4321", s->string);
		reset();

		END

START(replace)
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(a) Bad number of replacements returned", 0,
				cstring_replace(s, "", ""));
		ASSERT_EQUALS_STR("Replacement failed", "test string AC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(b) Bad number of replacements returned", 1,
				cstring_replace(s, "AC/DC", "AC/DC"));
		ASSERT_EQUALS_STR("Replacement failed", "test string AC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(c) Bad number of replacements returned", 1,
				cstring_replace(s, "AC/DC", "woof"));
		ASSERT_EQUALS_STR("Replacement failed", "test string woof", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(d) Bad number of replacements returned", 2,
				cstring_replace(s, "C", "*"));
		ASSERT_EQUALS_STR("Replacement failed", "test string A*/D*", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(e) Bad number of replacements returned", 1,
				cstring_replace(s, "test ", ""));
		ASSERT_EQUALS_STR("Replacement failed", "string AC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(f) Bad number of replacements returned", 1,
				cstring_replace(s, "test ", "longer test "));
		ASSERT_EQUALS_STR("Replacement failed", "longer test string AC/DC",
				s->string);

		END

START(replace_car)
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(a) Bad number of replacements returned", 0,
				cstring_replace_car(s->string, '?', '!'));
		ASSERT_EQUALS_STR("Replacement failed", "test string AC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(b) Bad number of replacements returned", 1,
				cstring_replace_car(s->string, '/', '/'));
		ASSERT_EQUALS_STR("Replacement failed", "test string AC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(c) Bad number of replacements returned", 1,
				cstring_replace_car(s->string, 'A', 'a'));
		ASSERT_EQUALS_STR("Replacement failed", "test string aC/DC", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(d) Bad number of replacements returned", 2,
				cstring_replace_car(s->string, 'C', '*'));
		ASSERT_EQUALS_STR("Replacement failed", "test string A*/D*", s->string);

		reset();
		cstring_add(s, "test string AC/DC");
		ASSERT_EQUALS_INT("(e) Bad number of replacements returned", 1,
				cstring_replace_car(s->string, '/', '\0'));
		ASSERT_EQUALS_STR("Replacement failed", "test string AC", s->string);

		END

START(starts_with)
		char *start;

		cstring_add(s, "fichier.ext");

		start = "fichier";
		if (!cstring_starts_with(s->string, start, 0))
			FAIL("fichier.ext does not start with %s", start);

		start = "ichier";
		if (cstring_starts_with(s->string, start, 0))
			FAIL("fichier.ext starts with %s", start);

		start = "afichier";
		if (cstring_starts_with(s->string, start, 0))
			FAIL("fichier.ext starts with %s", start);

		start = "";
		if (!cstring_starts_with(s->string, start, 0))
			FAIL("fichier.ext does not start with nothing");

		start = "chier";
		if (!cstring_starts_with(s->string, start, 2))
			FAIL("fichier.ext +2 does not start with %s", start);

		END

START(ends_with)
		char *end;

		cstring_add(s, "fichier.ext");

		end = ".ext";
		if (!cstring_ends_with(s->string, end))
			FAIL("fichier.ext does not end in %s", end);

		end = ".ex";
		if (cstring_ends_with(s->string, end))
			FAIL("fichier.ext ends in %s", end);

		end = "aext";
		if (cstring_ends_with(s->string, end))
			FAIL("fichier.ext does not end in %s", end);

		end = "";
		if (!cstring_ends_with(s->string, end))
			FAIL("fichier.ext does not end with nothing");

		END

START(find)
		char *str = "Une petite string pour y chercher des choses";

		ASSERT_EQUALS_INT("(a) find error", 0, cstring_find(str, "Une", 0));
		ASSERT_EQUALS_INT("(b) find error", -1, cstring_find(str, "Une", 1));
		ASSERT_EQUALS_INT("(c) find error", 4, cstring_find(str, "petite", 0));
		ASSERT_EQUALS_INT("(d) find error", 4, cstring_find(str, "petite", 1));
		ASSERT_EQUALS_INT("(e) find error", 4, cstring_find(str, "petite", 2));
		ASSERT_EQUALS_INT("(f) find error", 4, cstring_find(str, "petite", 3));
		ASSERT_EQUALS_INT("(g) find error", 4, cstring_find(str, "petite", 4));
		ASSERT_EQUALS_INT("(h) find error", -1, cstring_find(str, "petite", 5));
		ASSERT_EQUALS_INT("(i) find error", 38, cstring_find(str, "choses", 0));
		ASSERT_EQUALS_INT("(j) find error", -1, cstring_find(str, "Oops", 0));
		ASSERT_EQUALS_INT("(k) find error", 2, cstring_find(str, "e", 0));
		ASSERT_EQUALS_INT("(l) find error", 2, cstring_find(str, "e", 1));
		ASSERT_EQUALS_INT("(m) find error", 2, cstring_find(str, "e", 2));
		ASSERT_EQUALS_INT("(n) find error", 5, cstring_find(str, "e", 3));
		ASSERT_EQUALS_INT("(o) find error", 9, cstring_find(str, "e", 6));

		END

START(rfind)
		char *str = "Une petite string pour y chercher des choses";

		ASSERT_EQUALS_INT("(a) find error", 0, cstring_rfind(str, "Une", 0));
		ASSERT_EQUALS_INT("(b) find error", 0, cstring_rfind(str, "Une", 1));
		ASSERT_EQUALS_INT("(c) find error", 4, cstring_rfind(str, "petite", 0));
		ASSERT_EQUALS_INT("(d) find error", 4,
				cstring_rfind(str, "petite", 11));
		ASSERT_EQUALS_INT("(e) find error", -1,
				cstring_rfind(str, "petite", 2));
		ASSERT_EQUALS_INT("(f) find error", 38,
				cstring_rfind(str, "choses", 0));
		ASSERT_EQUALS_INT("(g) find error", -1, cstring_rfind(str, "Oops", 0));
		ASSERT_EQUALS_INT("(h) find error", 42, cstring_rfind(str, "e", 0));
		ASSERT_EQUALS_INT("(i) find error", 42, cstring_rfind(str, "e", -1));
		ASSERT_EQUALS_INT("(j) find error", 35, cstring_rfind(str, "e", -2));
		ASSERT_EQUALS_INT("(k) find error", 35, cstring_rfind(str, "e", -3));
		ASSERT_EQUALS_INT("(l) find error", 35, cstring_rfind(str, "e", 38));

		END

START(clear)
		if (!s)
			FAIL("new_cstring returned NULL");

		if (s->length)
			FAIL("empty cstring has a size of %zu", s->length);

		cstring_add(s, "testy");
		cstring_clear(s);
		if (s->length)
			FAIL("cleared cstring has a size of %zu", s->length);

		END

START(convert)
		char *str;

		str = cstring_convert(s);
		s = NULL;

		ASSERT_EQUALS_STR("Convert failed with an empty string", "", str);

		reset();

		cstring_add(s, "testy");
		str = cstring_convert(s);
		s = NULL;
		ASSERT_EQUALS_STR("Convert did not return the same string", "testy",
				str);

		END

START(clone)
		cstring_t *clone;

		clone = cstring_clone(NULL);
		if (clone)
			FAIL("Cloning NULL must return NULL");

		clone = cstring_clone("");
		ASSERT_EQUALS_STR("Cannot clone the empty string", "", clone->string);
		free_cstring(clone);

		cstring_add(s, "Testy viva la vida");
		clone = cstring_clone(s->string);
		ASSERT_EQUALS_STR("Failed to clone the string", s->string,
				clone->string);
		free_cstring(clone);

		END

START(rtrim)
		char *expected;

		expected = "testy";
		cstring_add(s, expected);
		cstring_add(s, "  ");

		cstring_rtrim(s, ' ');
		ASSERT_EQUALS_STR("(a) Failed to rtrim", expected, s->string);

		reset();

		expected = "   testy";
		cstring_add(s, expected);
		cstring_add(s, "  ");

		cstring_rtrim(s, ' ');
		ASSERT_EQUALS_STR("(b) Failed to rtrim", expected, s->string);

		reset();

		expected = "   testy   ";
		cstring_add(s, expected);
		cstring_add(s, "...");

		cstring_rtrim(s, '.');
		ASSERT_EQUALS_STR("(c) Failed to rtrim", expected, s->string);

		reset();

		expected = "...testy   ";
		cstring_add(s, expected);
		cstring_add(s, "...");

		cstring_rtrim(s, '.');
		ASSERT_EQUALS_STR("(d) Failed to rtrim", expected, s->string);

		END

START(trim)
		char *expected;

		expected = "testy";
		cstring_add(s, expected);
		cstring_add(s, "  ");

		cstring_trim(s, ' ');
		ASSERT_EQUALS_STR("(a) Failed to trim", expected, s->string);

		reset();

		expected = "testy";
		cstring_add(s, "  ");
		cstring_add(s, expected);
		cstring_add(s, "  ");

		cstring_trim(s, ' ');
		ASSERT_EQUALS_STR("(b) Failed to trim", expected, s->string);

		reset();

		expected = "   testy   ";
		cstring_add(s, expected);
		cstring_add(s, "...");

		cstring_trim(s, '.');
		ASSERT_EQUALS_STR("(c) Failed to trim", expected, s->string);

		reset();

		expected = "   testy   ";
		cstring_add(s, "...");
		cstring_add(s, expected);
		cstring_add(s, "...");

		cstring_trim(s, '.');
		ASSERT_EQUALS_STR("(d) Failed to trim", expected, s->string);

		END

START(remove_crlf)
		char *str = "testy";

		cstring_add(s, str);
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("no-op failed", str, s->string);

		reset();
		cstring_add(s, str);
		cstring_add(s, "\n");
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("\\n failed", str, s->string);

		reset();
		cstring_add(s, str);
		cstring_add(s, "\r\n");
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("\\r\\n failed", str, s->string);

		reset();
		cstring_add(s, str);
		cstring_add(s, "\n\n");
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("\\n\\n failed", "testy\n", s->string);

		reset();
		cstring_add(s, str);
		cstring_add(s, "\r\n\r\n");
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("\\r\\n\\r\\n failed", "testy\r\n", s->string);

		reset();
		cstring_add(s, "\n");
		s->length = cstring_remove_crlf(s->string);
		ASSERT_EQUALS_STR("\\n uniq failed", "", s->string);

		END

START(toupper)
		cstring_add(s, "");
		cstring_toupper(s);
		ASSERT_EQUALS_STR("Failed to uppercase empty", "", s->string);

		reset();

		cstring_add(s, "Simple Testy");
		cstring_toupper(s);
		ASSERT_EQUALS_STR("Failed to uppercase", "SIMPLE TESTY", s->string);

		reset();

		cstring_add(s, "C'est l'été");
		cstring_toupper(s);
		ASSERT_EQUALS_STR("Failed to uppercase", "C'EST L'ÉTÉ", s->string);

		reset();

		cstring_add(s, "Test en français");
		cstring_toupper(s);
		ASSERT_EQUALS_STR("Failed to uppercase", "TEST EN FRANÇAIS", s->string);

		END

START(tolower)
		cstring_add(s, "");
		cstring_tolower(s);
		ASSERT_EQUALS_STR("Failed to lowercase empty", "", s->string);

		reset();

		cstring_add(s, "Simple Testy");
		cstring_tolower(s);
		ASSERT_EQUALS_STR("Failed to lowercase", "simple testy", s->string);

		reset();

		cstring_add(s, "Été ! C'est l'été !");
		cstring_tolower(s);
		ASSERT_EQUALS_STR("Failed to lowercase", "été ! c'est l'été !",
				s->string);

		reset();

		cstring_add(s, "Test en français");
		cstring_tolower(s);
		ASSERT_EQUALS_STR("Failed to lowercase", "test en français", s->string);

		reset();

		cstring_add(s, "À la claire fontaine");
		cstring_tolower(s);
		ASSERT_EQUALS_STR("Failed to lowercase", "à la claire fontaine",
				s->string);

		END

START(readline)
		int read;
		FILE *testin = fopen(TEST_FILE_READLINE, "r");
		if (!testin)
			FAIL("Test file not found: test_readln.txt");

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("first line should not be last");
		ASSERT_EQUALS_STR("first line incorrect", "ligne 1", s->string);
		reset();

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("second line should not be last");
		ASSERT_EQUALS_STR("second line incorrect", "", s->string);
		reset();

		read = cstring_readline(s, testin);
		if (!read)
			FAIL("third line should not be last");
		ASSERT_EQUALS_STR("third line incorrect", "ligne 3", s->string);
		reset();

		if (cstring_readline(s, testin)) {
			FAIL("fourth line should not exist");
		}

		END

START(add_path)
		cstring_add_path(s, "root");
		ASSERT_EQUALS_STR("failed to create root path", "/root", s->string);

		cstring_add_path(s, "dir");
		ASSERT_EQUALS_STR("failed to add a dir", "/root/dir", s->string);

		cstring_add_path(s, "sub/");
		ASSERT_EQUALS_STR("extra / failed", "/root/dir/sub", s->string);

		END

START(pop_path)
		cstring_add(s, "");
		ASSERT_EQUALS_INT("empty test failed", 0, cstring_pop_path(s, 1));

		reset();
		cstring_add(s, "root");
		ASSERT_EQUALS_INT("0 nbr test failed", 0, cstring_pop_path(s, 0));
		ASSERT_EQUALS_STR("0 test failed", "root", s->string);

		reset();
		cstring_add(s, "root/");
		ASSERT_EQUALS_INT("0² nbr test failed", 0, cstring_pop_path(s, 0));
		ASSERT_EQUALS_STR("0² test failed", "root", s->string);

		reset();
		cstring_add(s, "/");
		ASSERT_EQUALS_INT("root test nbr failed", 0, cstring_pop_path(s, 1));
		ASSERT_EQUALS_STR("root test failed", "/", s->string);

		reset();
		cstring_add(s, "/");
		ASSERT_EQUALS_INT("root² test nbr failed", 0, cstring_pop_path(s, 2));
		ASSERT_EQUALS_STR("root² test failed", "/", s->string);

		reset();
		cstring_add(s, "/root");
		ASSERT_EQUALS_INT("/root test nbr failed", 1, cstring_pop_path(s, 1));
		ASSERT_EQUALS_STR("/root test failed", "/", s->string);

		reset();
		cstring_add(s, "/root");
		ASSERT_EQUALS_INT("/root³ test nbr failed", 1, cstring_pop_path(s, 2));
		ASSERT_EQUALS_STR("/root³ test failed", "/", s->string);

		reset();
		cstring_add(s, "/root/dir/file");
		ASSERT_EQUALS_INT("2 test nbr failed", 2, cstring_pop_path(s, 2));
		ASSERT_EQUALS_STR("2 test failed", "/root", s->string);

		reset();
		cstring_add(s, "/root/dir/file/");
		ASSERT_EQUALS_INT("trailing / test nbr failed", 1,
				cstring_pop_path(s, 1));
		ASSERT_EQUALS_STR("trailing / test failed", "/root/dir", s->string);

		END

START(basename)
		char *str;

		cstring_add(s, "");
		str = cstring_basename(s->string, NULL);
		ASSERT_EQUALS_STR("empty test", "", str);
		free(str);

		reset();
		cstring_add(s, "/root/path/dir/file");
		str = cstring_basename(s->string, NULL);
		ASSERT_EQUALS_STR("simple test", "file", str);
		free(str);

		reset();
		cstring_add(s, "/root/path/dir/file");
		str = cstring_basename(s->string, ".ext");
		ASSERT_EQUALS_STR("no ext test", "file", str);
		free(str);

		reset();
		cstring_add(s, "/root/path/dir/file.test");
		str = cstring_basename(s->string, ".ext");
		ASSERT_EQUALS_STR("wrong ext test", "file.test", str);
		free(str);

		reset();
		cstring_add(s, "/root/path/dir/file.ext");
		str = cstring_basename(s->string, ".ext");
		ASSERT_EQUALS_STR("good ext test", "file", str);
		free(str);

		END

START(dirname)
		char *str;

		cstring_add(s, "/root/path");
		str = cstring_dirname(s->string);
		ASSERT_EQUALS_STR("simple test", "/root", str);
		free(str);

		reset();
		cstring_add(s, "/root/path/");
		str = cstring_dirname(s->string);
		ASSERT_EQUALS_STR("trailing / test", "/root", str);
		free(str);

		reset();
		cstring_add(s, "/");
		str = cstring_dirname(s->string);
		ASSERT_EQUALS_STR("root is root of root test", "/", str);
		free(str);

		END

START(concat)
		char *cc;

		cc = cstring_concat(NULL);
		if (cc)
			FAIL("concat of NULL should return NULL, not: <%s>", cc);

		cc = cstring_concat("only", NULL);
		ASSERT_EQUALS_STR("Single parameter", "only", cc);
		free(cc);

		cc = cstring_concat("Only", "Fans", NULL);
		ASSERT_EQUALS_STR("Test 2 params", "OnlyFans", cc);
		free(cc);

		cc = cstring_concat("Fanfan", " ", "et", " Tulipe",
				" entrent dans un bar", NULL);
		ASSERT_EQUALS_STR("Test multiple params",
				"Fanfan et Tulipe entrent dans un bar", cc);
		free(cc);

		END

START(many_adds)
		size_t count = 10 * 1000 * 1000;
		for (size_t i = 0; i < count; i++) {
			cstring_add(s, "1234567890");
		}

		ASSERT_EQUALS_SIZE("Lot of adds", count * 10, s->length);

		END

Suite *test_cstring(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, setup, teardown);
	tcase_add_test(core, init);
	tcase_add_test(core, add_car);
	tcase_add_test(core, add_all_but_p);
	tcase_add_test(core, addp);
	tcase_add_test(core, cut_at);
	tcase_add_test(core, substring);
	tcase_add_test(core, reverse);
	tcase_add_test(core, replace);
	tcase_add_test(core, replace_car);
	tcase_add_test(core, starts_with);
	tcase_add_test(core, ends_with);
	tcase_add_test(core, find);
	tcase_add_test(core, rfind);
	tcase_add_test(core, clear);
	tcase_add_test(core, convert);
	tcase_add_test(core, clone);
	tcase_add_test(core, rtrim);
	tcase_add_test(core, trim);
	tcase_add_test(core, remove_crlf);
	tcase_add_test(core, toupper);
	tcase_add_test(core, tolower);
	tcase_add_test(core, readline);
	tcase_add_test(core, add_path);
	tcase_add_test(core, pop_path);
	tcase_add_test(core, basename);
	tcase_add_test(core, dirname);
	tcase_add_test(core, concat);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_cstring_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, setup, teardown);
	tcase_add_test(tmore, many_adds);

	suite_add_tcase(suite, tmore);

	return suite;
}
