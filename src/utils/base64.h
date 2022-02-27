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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h>
#include "cstring.h"

typedef struct {
	char *table;
} base64;

/**
 * Create a new base64 codec.
 *
 * @return a new codec
 */
base64 *base64_new();

/**
 * Free the given code.
 */
void base64_free(base64 *self);

cstring *base64_encode(base64 *self, cstring *data);

cstring *base64_decode(base64 *self, cstring *data);

cstring *base64_encodes(base64 *self, char *data);

cstring *base64_encodesi(base64 *self, char *data, size_t size);

cstring *base64_decodes(base64 *self, char *data);

#endif

#ifdef __cplusplus
}
#endif
