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

static int is_srt_id(char *line);
static int is_srt_timing(char *line);
static int get_start(char *line);
static int get_stop(char *line);

int nsub_read_webvtt(song_t *song, char *line) {
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
					"Warning: line %zu is out of order (it is numbered %i), ignoring order...\n",
					count, new_count);
		}
	} else if (is_srt_timing(line)) {
		song_add_lyric(song, 0, 0, NULL, NULL);
		lyric = array_get(song->lyrics, array_count(song->lyrics) - 1);

		lyric->start = get_start(line);
		lyric->stop = get_stop(line);
	} else {
		// a header has been found
		if (!lyric) {
			return 1;
		}

		char *text = lyric->text;
		if (text)
			text = cstring_concat(text, "\n", line, NULL);
		else
			text = strdup(line);

		free(lyric->text);
		lyric->text = text;
	}

	return 1;
}

/* Private */

static int is_srt_id(char *line) {
	for (char *ptr = (char *) line; *ptr; ptr++) {
		int digit = (*ptr >= '0' && *ptr <= '9');
		int space = (*ptr == ' ');

		if (!digit && !space)
			return 0;
	}

	return 1;
}

static int is_srt_timing(char *line) {
	// Canonical example:
	// 00:00:14.800 --> 00:00:17.400 align: center

	cstring_t *tmp;
	int ok;
	int i;

	// skip spaces
	while (*line == ' ')
		line++;

	// check part 1
	i = 0;
	while (line[i] && line[i] != ' ' && line[i] != '-')
		i++;
	if (!line[i])
		return 0;
	tmp = cstring_substring(line, 0, i);
	ok = nsub_is_timing(tmp->string, '.', 3);
	free_cstring(tmp);
	if (!ok)
		return 0;

	// skip part 1
	line += i;

	// skip spaces
	while (*line == ' ')
		line++;

	// skip -->
	char *arrow = { "-->" };
	while (*line && *arrow && *arrow == *line) {
		line++;
		arrow++;
	}
	if (*arrow)
		return 0;

	// skip spaces
	while (*line == ' ')
		line++;

	// find end
	i = 0;
	while (line[i] && line[i] != ' ')
		i++;

	// validate part 2
	tmp = cstring_substring(line, 0, i);
	ok = nsub_is_timing(tmp->string, '.', 3);
	free_cstring(tmp);
	return !!ok;
}

static int get_start(char *line) {
	// 00:00:14,800 --> 00:00:17,400 align: center

	if (!is_srt_timing(line)) {
		/* should not happen! */
		fprintf(stderr,
				"Warning: called get_start with bad input [%s], ignoring...\n",
				line);
		return 0;
	}

	// skip spaces
	while (*line == ' ')
		line++;

	// find end
	int i = 0;
	while (line[i] && line[i] != ' ' && line[i] != '-')
		i++;

	cstring_t*start = cstring_substring(line, 0, i);
	int ms = nsub_to_ms(start->string, '.');
	free_cstring(start);
	return ms;
}

static int get_stop(char *line) {
	// 00:00:14,800 --> 00:00:17,400 align: center

	if (!is_srt_timing(line)) {
		/* should not happen! */
		fprintf(stderr,
				"Warning: called get_stop with bad input [%s], ignoring...\n",
				line);
		return 0;
	}

	// skip to >
	while (*line != '>')
		line++;
	line++;

	// skip spaces
	while (*line == ' ')
		line++;

	// find end
	int i = 0;
	while (line[i] && line[i] != ' ')
		i++;

	cstring_t*end = cstring_substring(line, 0, i);
	int ms = nsub_to_ms(end->string, '.');
	free_cstring(end);
	return ms;
}
