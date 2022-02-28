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
 * @file utils.h
 * @author Niki
 * @date 2020
 *
 * @brief Include all the other .H as well as a C99-compatible utils_strdup function.
 */
#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cstring.h"
#include "array.h"
#include "desktop.h"
#include "print.h"
#include "timing.h"

/**
 * A C99-compatible <tt>strdup</tt> function (i.e., a <tt>strdup</tt> for
 * CentOS 6).
 *
 * @param source the source string to copy
 *
 * @return a new string (malloc'ed) which your are now responsible of
 */
char *utils_strdup(const char *source);

/**
 * A C99-compatible <tt>strdup</tt> function (i.e., a <tt>strdup</tt> for
 * CentOS 6).
 *
 * @param source the source string to copy
 *
 * @return a new string (malloc'ed) which your are now responsible of
 */
cstring *utils_cstrdup(const cstring *source);

#endif // UTILS_H

#ifdef __cplusplus
}
#endif

