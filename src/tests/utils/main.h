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

#ifndef SRC_TESTS_UTILS_MAIN_H_
#define SRC_TESTS_UTILS_MAIN_H_

#include <check.h>

Suite *test_cstring(const char title[]);
Suite *test_cstring_more(const char title[]);
Suite *test_array(const char title[]);
Suite *test_array_more(const char title[]);
Suite *test_base64(const char title[]);
Suite *test_base64_more(const char title[]);
Suite *test_desktop(const char title[]);
Suite *test_desktop_more(const char title[]);

#endif /* SRC_TESTS_UTILS_MAIN_H_ */
