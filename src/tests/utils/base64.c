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

#include "utils/base64.h"

#include <check.h>

#include "launcher.h"

static void setup() {
}

static void teardown() {
}

static char decoded[] = "This is Le Test, we will UTF-8 the String, too!";
static char encoded[] =
		"VGhpcyBpcyBMZSBUZXN0LCB3ZSB3aWxsIFVURi04IHRoZSBTdHJpbmcsIHRvbyE=";

START(decode)
		ASSERT_EQUALS_STR("decoding", decoded, base64_decode(encoded));
		END

START(encode)
		ASSERT_EQUALS_STR("encoding", encoded, base64_encode(decoded));
		END

START(both_ways)
		char *enc = base64_encode(decoded);
		char *dec = base64_decode(enc);
		ASSERT_EQUALS_STR("both ways", decoded, dec);
		END

START(big)
		size_t sz = 10 * 1024 * 1024;
		char *dec = malloc((sz + 1) * sizeof(char));
		for (int i = 0; i < sz; i++) {
			dec[i] = '0' + (i % 10);
		}
		dec[sz] = '\0';

		char *enc = base64_encode(dec);
		char *dec2 = base64_decode(enc);

		ASSERT_EQUALS_STR("long encode/decode cycle", dec, dec2);
		free(dec2);
		free(enc);
		free(dec);

		END

START(lots)
		size_t count = 1 * 1000 * 1000;
		for (size_t i = 0; i < count; i++) {
			char *enc = base64_encode(decoded);
			char *dec = base64_decode(enc);

			if (strcmp(decoded, dec)) {
				FAIL("Failed short encode/decode cycle at index %zu", i);
			}

			free(enc);
			free(dec);
		}

		END

Suite *test_base64(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *core = tcase_create("core");
	tcase_add_checked_fixture(core, setup, teardown);
	tcase_add_test(core, decode);
	tcase_add_test(core, encode);

	suite_add_tcase(suite, core);

	return suite;
}

Suite *test_base64_more(const char title[]) {
	Suite *suite = suite_create(title);

	TCase *tmore = tcase_create("more");
	tcase_add_checked_fixture(tmore, setup, teardown);
	tcase_add_test(tmore, both_ways);
	tcase_add_test(tmore, big);
	tcase_add_test(tmore, lots);

	suite_add_tcase(suite, tmore);

	return suite;
}
