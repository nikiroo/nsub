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

/**
 * @file utils.h
 * @author Niki
 * @date 2020
 *
 * @brief Include all the other .H as well as a C99-compatible utils_strdup function.
 */
#ifndef UTILS_H
#define UTILS_H

#include "array.h"
#include "desktop.h"
#include "print.h"
#include "timing.h"

/**
 * A C99-compatible strdup function.
 *
 * @param source the source string to copy
 *
 * @return a new string (malloc'ed) which your are now respionsible of
 */
char *utils_strdup(const char *source);

#endif // UTILS_H