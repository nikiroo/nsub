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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "cstring.h"

cstring *base64_decodesi(base64 *self, char *data, size_t len);
//cstring *base64_encodesi(base64 *self, char *data, size_t len);

char *b64_encode(const char *data, const char *encoding_table,
			size_t input_length,
			size_t *output_length);

char *b64_decode(const char *data, const char *decoding_table,
			size_t input_length,
			size_t *output_length);

static char b64_encoding_table[] = {	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
					'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
					'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
					'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
					'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
					'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
					'w', 'x', 'y', 'z', '0', '1', '2', '3',
					'4', '5', '6', '7', '8', '9', '+', '/'};

static int b64_mod_table[] = {0, 2, 1};

base64 *base64_new() {
	base64 *self;
	int i;
	
	self = (base64 *)malloc(sizeof(base64));
	self->table = malloc(256 * sizeof(char));
	
	for (i = 0; i < 64; i++)
		self->table[(unsigned char)b64_encoding_table[i]] = i;
	
	return self;
}

void base64_free(base64 *self) {
	if (!self)
		return;
	
	free(self->table);
	free(self);
}

cstring *base64_encode(base64 *self, cstring *data) {
	if (!data)
		return NULL;
	
	return base64_encodesi(self, data->string, data->length);
}

cstring *base64_decode(base64 *self, cstring *data) {
	if (!data)
		return NULL;
	
	return base64_decodesi(self, data->string, data->length);
}

cstring *base64_encodes(base64 *self, char *data) {
	size_t len;
	
	if (!data)
		return NULL;
	
	len = strlen(data);
	
	return base64_encodesi(self, data, len);
}

cstring *base64_decodes(base64 *self, char *data) {
	size_t len;
	
	if (data)
		len = strlen(data);
	
	return base64_decodesi(self, data, len);
}

cstring *base64_encodesi(base64 *self, char *data, size_t len) {
	cstring *rep;
	char *encoded;
	size_t size;
	
	rep = NULL;
	size = 0;
	encoded = b64_encode(data, b64_encoding_table, len, &size);
	
	if (encoded) {
		rep = new_cstring();
		free(rep->string);
		rep->string = encoded;
		rep->length = size;
		cstring_compact(rep);
	}
	
	return rep;
}

cstring *base64_decodesi(base64 *self, char *data, size_t len) {
	cstring *rep;
	char *decoded;
	size_t size;
	
	rep = NULL;
	size = 0;
	decoded = b64_decode(data, self->table, len, &size);
	
	if (decoded) {
		rep = new_cstring();
		free(rep->string);
		rep->string = decoded;
		rep->length = size;
		cstring_compact(rep);
	}
	
	return rep;
}

char *b64_encode(const char *data, const char *encoding_table,
			size_t input_length,
			size_t *output_length) {
	unsigned int i, j;

	*output_length = 4 * ((input_length + 2) / 3) + 1;

	char *encoded_data = malloc(*output_length);
	if (encoded_data == NULL)
		return NULL;

	for (i = 0, j = 0 ; i < input_length ; ) {
		uint32_t octet_a = i < input_length ? 
			(unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ?
			(unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ?
			(unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) 
				+ (octet_b << 0x08) 
				+  octet_c;

		encoded_data[j++] = 
			(char)encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] =
			(char)encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] =
			(char)encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] =
			(char)encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (i = 0 ; (int)i < b64_mod_table[input_length % 3] ; i++)
		encoded_data[*output_length - 2 - i] = '=';
	encoded_data[*output_length - 1] = '\0';

	return encoded_data;
}


char *b64_decode(const char *data, const char *decoding_table,
			size_t input_length,
			size_t *output_length) {
	
	unsigned int i, j;

	if (input_length % 4 != 0)
		return NULL;

	*output_length = (input_length / 4 * 3) + 1;
	if (data[input_length - 2] == '=') (*output_length)--;
	if (data[input_length - 3] == '=') (*output_length)--;

	char *decoded_data = malloc(*output_length);
	if (decoded_data == NULL)
		return NULL;

	i = j = 0;
	for ( ; i < input_length ; i += 4) {
		uint32_t sextet_a = 0;
		uint32_t sextet_b = 0;
		uint32_t sextet_c = 0;
		uint32_t sextet_d = 0;
		
		if (data[i] != '=') {
			sextet_a = decoding_table[(unsigned char)data[i + 0]];
			sextet_b = decoding_table[(unsigned char)data[i + 1]];
			sextet_c = decoding_table[(unsigned char)data[i + 2]];
			sextet_d = decoding_table[(unsigned char)data[i + 3]];
		}
		
		uint32_t triple = (sextet_a << 3 * 6)
				+ (sextet_b << 2 * 6)
				+ (sextet_c << 1 * 6)
				+ (sextet_d << 0 * 6);

		if (j + 1 < *output_length)
			decoded_data[j++] = (char)((triple >> 2 * 8) & 0xFF);
		if (j + 1 < *output_length)
			decoded_data[j++] = (char)((triple >> 1 * 8) & 0xFF);
		if (j + 1  < *output_length)
			decoded_data[j++] = (char)((triple >> 0 * 8) & 0xFF);
	}

	decoded_data[*output_length - 1] = '\0';
	return decoded_data;
}

