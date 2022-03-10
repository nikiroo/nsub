/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2013 Niki Roo
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
 * @file base64.h
 * @author Niki
 * @date 2013 - 2022
 *
 * @brief Base64 encode and decode
 *
 * This file only provides 2 functions, <tt>base64_encode</tt> and
 * <tt>base64_decode</tt>, which works on NUL-terminated strings and do what
 * you expect them to.
 *
 * @see base64_encode(const char data[])
 * @see base64_decode(const char data[])
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h>

/**
 * Encode the given data to Base64.
 *
 * @note can return NULL if there is not enough memory to allocated the answer
 *
 * @param data the data to encode
 *
 * @return a newly-allocated string for which you are responsible, or NULL
 */
char *base64_encode(const char data[]);

/**
 * Decode the given data to Base64.
 *
 * @note can return NULL if there is not enough memory to allocated the answer
 *
 * @param data the data to decode
 *
 * @return a newly-allocated string for which you are responsible, or NULL
 */
char *base64_decode(const char data[]);

#endif

#ifdef __cplusplus
}
#endif
