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
static int is_lrc_offset(char *line);
// test if it is a timed lyric
static int is_lrc_lyric(char *line, int*end);
// test if this is a meta line
static int is_lrc_meta(char *line, int *colon, int *end);
// count the ms in the line "[(00:0)0:14.80]" or "[offset: +0:12]"
static int lrc_millisec(char *line);

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

		{
			lyric_t *lyric = array_last(song->lyrics);
			if (lyric->type == NSUB_LYRIC)
				lyric->stop = start;
			if (lyric->type == NSUB_COMMENT)
				name = lyric->text;

			// Comments starting with "[" (or "-- [") should not be names
			if (name && name[0] == '[')
				name = NULL;
		}

		int text_offset = end + 2;
		while (line[text_offset] == ' ')
			text_offset++;

		if (line[text_offset]) {
			if (name) {
				lyric_t *tmp = array_pop(song->lyrics);
				uninit_lyric(tmp);
				song->current_num--;
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
			song->lang = strdup(line + text_offset);
		} else if (!strcmp("created_by", line + 1)) {
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

static int is_lrc_offset(char *line) {
	// [offset: +0:12]

	// skip spaces
	while (*line == ' ')
		line++;

	// skip [offset
	char *offset = { "[offset" };
	while (*line && *offset && *offset == *line) {
		line++;
		offset++;
	}
	if (*offset)
		return 0;

	// skip spaces then ':'
	while (*line == ' ')
		line++;
	if (*line && *line != ':')
		return 0;
	line++;

	// allow sign
	if (*line == '-' || *line == '+')
		line++;
	while (*line == ' ')
		line++;

	// find end
	int i = 0;
	while (line[i] && line[i] != ']')
		i++;
	if (line[i] != ']')
		return 0;

	// validate timing
	cstring_t*tmp = cstring_substring(line, 0, i);
	int ok = nsub_is_timing(tmp->string, '.', 2);
	free_cstring(tmp);
	return ok;
}

static int is_lrc_lyric(char *line, int *end) {
	// "[(00:0)0:14.80] bla bla bla"

	*end = 0;

	// skip spaces
	while (*line == ' ')
		line++;

	// skip [
	if (*line != '[')
		return 0;
	line++;

	// find end
	int i = 0;
	while (line[i] && line[i] != ']')
		i++;
	if (line[i] != ']')
		return 0;
	*end = i;

	// validate timing
	cstring_t*tmp = cstring_substring(line, 0, i);
	int ok = nsub_is_timing(tmp->string, '.', 2);
	free_cstring(tmp);
	return ok;
}

static int is_lrc_meta(char *line, int *colon, int *end) {
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

static int lrc_millisec(char *line) {
	// count the ms in the line "[(00:0)0:14.80]" or "[offset: +0:12]"

	int sign = 1;
	int dummy = 0;

	// skip spaces
	while (*line == ' ')
		line++;

	if (is_lrc_lyric(line, &dummy)) {
		// skip [
		line++;
	} else if (is_lrc_offset(line)) {
		// skip [offset:
		while (*line != ':')
			line++;
		while (*line == ' ')
			line++;

		// allow sign
		if (*line == '+') {
			line++;
		} else if (*line == '-') {
			line++;
			sign = -1;
		}
		while (*line == ' ')
			line++;
	} else {
		/* should not happen! */
		fprintf(stderr,
				"Warning: called lrc_millisec with bad input [%s], ignoring...\n",
				line);
		return 0;
	}

	// find end
	int i = 0;
	while (line[i] != ']')
		i++;

	cstring_t*tmp = cstring_substring(line, 0, i);
	int ms = nsub_to_ms(tmp->string, '.');
	free_cstring(tmp);
	return sign * ms;
}
