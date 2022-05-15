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

#include <stdlib.h>
#include <string.h>

#include "nsub.h"
#include "utils/utils.h"

/* Declarations */

static int is_srt_id(const char line[]);
static int is_srt_timing(const char line[]);
static int get_start(const char line[]);
static int get_stop(const char line[]);
static int to_ms(const char line[]);

int nsub_read_srt(song_t *song, char *line) {
	int empty = 1;
	for (int i = 0; empty && line[i]; i++) {
		if (line[i] != ' ')
			empty = 0;
	}

	if (empty)
		return 1;

	size_t count = array_count(song->lyrics);
	lyric_t *lyric = NULL;
	if (count)
		lyric = array_get(song->lyrics, array_count(song->lyrics) - 1);

	if (is_srt_id(line)) {
		int new_count = atoi(line);

		if (new_count != count + 1) {
			fprintf(stderr,
					"Warning: line %zu is out of order (it is numbered %i), ignoring order...",
					count, new_count);
		}

		song_add_lyric(song, 0, 0, NULL, NULL);
	} else if (is_srt_timing(line)) {
		if (!lyric) {
			return 0;
		}

		lyric->start = get_start(line);
		lyric->stop = get_stop(line);
	} else {
		if (!lyric) {
			return 0;
		}

		char *text = lyric->text;
		if (text)
			text = cstring_concat(text, "\n", line);
		else
			text = strdup(line);

		free(lyric->text);
		lyric->text = text;
	}

	return 1;
}

/* Private */

static int is_srt_id(const char line[]) {
	for (char *ptr = (char *) line; *ptr; ptr++) {
		switch (*ptr) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case ' ':
			break;
		default:
			return 0;
		}
	}

	return 1;
}

static int is_srt_timing(const char line[]) {
	// Canonical example:
	// 00:00:14,800 --> 00:00:17,400

	int vals = 0;
	int vals_groups = 0;
	int sep = 0;
	int deci = 0;

	for (char *ptr = (char *) line; *ptr; ptr++) {
		switch (*ptr) {
		case ' ': // ignore space if not in sep
			if (sep && sep < 2)
				return 0;
			break;

		case '0': // count a new numeric
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			vals++;
			break;

		case ',': // we just did a group
		case ':':
			if (*ptr == ',')
				deci = 1;

			if (vals < 1 || vals > 2)
				return 0;

			vals = 0;
			vals_groups++;
			break;

		case '-': // Separator (-->)
			if (!sep) {
				vals_groups++;

				if (vals < 1 || (vals > 2 && !deci) || vals > 3)
					return 0;

				if (vals_groups < 1 || (vals_groups > 3 && !deci)
						|| vals_groups > 4)
					return 0;

				vals = 0;
				vals_groups = 0;
				deci = 0;
			}

			if (sep > 2)
				return 0;

			sep++;
			break;

		case '>': // Separator (-->)
			if (sep != 2)
				return 0;

			sep++;
			break;
		}
	}

	if (vals < 1 || (vals > 2 && !deci) || vals > 3)
		return 0;

	if (vals_groups < 1 || (vals_groups > 3 && !deci) || vals_groups > 4)
		return 0;

	if (sep != 3)
		return 0;

	return 1;
}

static int get_start(const char line[]) {
	char *ptr = (char *) line;
	while (*ptr == ' ')
		ptr++;

	size_t i;
	for (i = 0; ptr[i] != ' ' && ptr[i] != '-'; i++)
		;

	cstring_t*start = cstring_substring(ptr, 0, i);
	int ms = to_ms(start->string);
	free_cstring(start);
	return ms;
}

static int get_stop(const char line[]) {
	char *ptr = (char *) line;
	while (*ptr != '>')
		ptr++;
	ptr++;
	while (*ptr == ' ')
		ptr++;

	return to_ms(ptr);
}

static int to_ms(const char line[]) {
	// 00:00:17,400

	int mults[] = { 1, 1000, 60000, 3600000 };

	int group[4] = { 0, 0, 0, 0 };
	int igroup = -1;

	char mtmp[3] = { 1, 10, 100 };
	int itmp = 0;

	int has_milli = 0;

	size_t end = strlen(line) - 1;

	for (size_t i = end; i >= 0; i--) {
		char car = line[i];

		int digit = (car >= '0' && car <= '9');
		int dot = car == ',';
		int col = (car == ':');

		if (!digit && !dot && !col) {
			break;
		}

		if (digit) {
			if (itmp == 0)
				igroup++;

			group[igroup] += mtmp[itmp] * (car - (int) '0');
			itmp++;
		} else {
			if (dot)
				has_milli = 1;

			itmp = 0;
		}
	}

	int total = 0;
	int multOffset = (has_milli ? 0 : 1);
	for (int i = 0; i <= igroup; i++) {
		total += mults[i + multOffset] * group[i];
	}

	return total;
}
