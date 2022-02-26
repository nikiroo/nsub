/*
 * NSub: Subtitle/Lyrics conversion program (webvtt/srt/lrc)
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

#include <string.h>

#include "nsub.h"
#include "utils/utils.h"

/* Declarations */

// test if this is an offset line
int is_lrc_offset(char *line);
// test if it is a timed lyric
int is_lrc_lyric(char *line, int*end);
// test if this is a meta line
int is_lrc_meta(char *line, int *colon, int *end);
// count the ms in the line "[(00:0)0:14.80]" or "[offset: +0:12]"
int lrc_millisec(char *line);

/* Public */

int nsub_read_lrc(song_t *song, char *line) {
	int colon;
	int end;

	int empty = 1;
	for (int i = 0; empty && line[i]; i++) {
		if (line[i] != ' ')
			empty = 0;
	}

	if (empty) {
		song_add_empty(song);
	} else if (is_lrc_offset(line)) {
		song->offset = lrc_millisec(line);
	} else if (is_lrc_lyric(line, &end)) {
		int start = lrc_millisec(line);
		char *name = NULL;

		size_t count = array_count(song->lyrics);
		if (count) {
			lyric_t *lyric = (lyric_t*) array_get_ptr(song->lyrics, count - 1);
			if (lyric->type == NSUB_LYRIC)
				lyric->stop = start;
			if (lyric->type == NSUB_COMMENT)
				name = lyric->text;

			// Comments starting with "[" (or "-- [") should not be names
			if (name && name[0] == '[')
				name = NULL;
		}

		int text_offset = end + 1;
		while (line[text_offset] == ' ')
			text_offset++;

		if (line[text_offset]) {
			if (name) {
				song->current_num--;
				array_set_size(song->lyrics, count - 1);
			}
			song_add_lyric(song, start, start + 5000, name, line + text_offset);
		} else {
			song_add_empty(song);
		}
	} else if (is_lrc_meta(line, &colon, &end)) {
		int text_offset = colon + 1;
		while (line[text_offset] == ' ')
			text_offset++;

		line[colon] = '\0';
		line[end] = '\0';
		if (!strcmp("language", line + 1)) {
			song->lang = utils_strdup(line + text_offset);
		}else if (!strcmp("created_by", line + 1)) {
			// skip (we KNOW what program we are)
		} else {
			song_add_meta(song, line + 1, line + text_offset);
		}
		line[colon] = ':';
		line[end] = ']';
	} else {
		if (line[0] == '-' && line[1] == '-' && line[2] == ' ')
			line += 3;
		song_add_comment(song, line);
	}

	return 1;
}

/* Private */

int is_lrc_offset(char *line) {
	const char offset[] = { "[offset" };
	int i = 0;
	while (line[i] && offset[i] && line[i] == offset[i])
		i++;

	if (!offset[i]) {
		while (line[i] == ' ')
			i++;

		if (line[i] == ':')
			return 1;
	}

	return 0;
}

int is_lrc_lyric(char *line, int *end) {
	if (line[0] != '[')
		return 0;

	*end = 0;

	for (int i = 1; line[i]; i++) {
		char car = line[i];

		if (car == ']') {
			*end = i;
			return i >= 2;
		}

		int digit = (car >= '0' && car <= '9');
		int sep = (car == ':' || car == '.' || car == ' ');

		if (!digit && !sep)
			break;
	}

	return 0;
}

int is_lrc_meta(char *line, int *colon, int *end) {
	if (line[0] != '[')
		return 0;

	*colon = 0;
	*end = 0;

	for (int i = 1; line[i]; i++) {
		char car = line[i];

		if (car == ']')
			*end = i;
		else if (car != ' ')
			*end = 0;

		if (!(*colon) && car == ':')
			*colon = i;
	}

	return (*colon) && (*end);
}

int lrc_millisec(char *line) {
	int mults[] = { 1000, 10000, 60000, 600000, 3600000, 36000000 };

	size_t end = 0;
	for (size_t i = 0; line[i]; i++) {
		if (line[i] == ']') {
			end = i - 1;
			break;
		}
	}

	int total = 0;
	for (size_t i = end; i >= 0; i--) {
		char car = line[i];

		int digit = (car >= '0' && car <= '9');
		int sign = (car == '-' || car == '+');
		int dot = car == '.';
		int col = (car == ':');

		if (!digit && !sign && !dot && !col) {
			break;
		}

		if (dot) {
			int mult = 1;

			int j = i + 3;
			while (j > end) {
				mult *= 10;
				j--;
			}

			for (; j > i; j--) {
				total += mult * ((int) line[j] - (int) '0');
				mult *= 10;
			}

			end = i - 1;
			break;
		}
	}

	int imult = 0;
	for (size_t i = end; 1; i--) {
		char car = line[i];
		int digit = (car >= '0' && car <= '9');

		if (digit)
			total += (car - '0') * mults[imult++];

		if (car == '-')
			total = -total;

		if (i == 0)
			break;
	}

	return total;
}
