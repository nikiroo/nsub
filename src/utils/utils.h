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
 * @brief Include all the other .h as well as a C99-compatible strdup/strnlen
 * 		functions.
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

/* Helps with C99 compatibility for code that is not */

#ifndef strnlen
size_t strnlen(const char *s, size_t maxlen);
#endif
#ifndef strdup
char *strdup(const char *source);
#endif

/* */

#endif // UTILS_H

#ifdef __cplusplus
}
#endif

