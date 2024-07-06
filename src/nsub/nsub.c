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

#include <stdio.h>
#include <string.h>

#include "nsub.h"
#include "cutils/cutils.h"

/* Public */

song_t *new_song() {
	song_t *song = malloc(sizeof(song_t));
	song->lyrics = new_array(sizeof(lyric_t), 64);
	song->metas = new_array(sizeof(meta_t), 10);
	song->offset = 0;
	song->current_num = 0;
	song->lang = NULL;
	return song;
}

void free_song(song_t *song) {
	if (!song)
		return;

	array_loop(song->metas, meta, meta_t)
	{
		free(meta->key);
		free(meta->value);
	}
	free_array(song->metas);

	array_loop(song->lyrics, lyric, lyric_t)
		free(lyric->text);
	free_array(song->lyrics);

	free(song->lang);
	free(song);
}

void song_add_unknown(song_t *song, char *text) {
	lyric_t *lyric = array_new(song->lyrics);
	lyric->type = NSUB_UNKNOWN;
	lyric->num = 0;
	lyric->start = 0;
	lyric->stop = 0;
	lyric->name = NULL;
	lyric->text = text ? strdup(text) : NULL;
}

void song_add_empty(song_t *song) {
	lyric_t *lyric = array_new(song->lyrics);
	lyric->type = NSUB_EMPTY;
	lyric->num = 0;
	lyric->start = 0;
	lyric->stop = 0;
	lyric->name = NULL;
	lyric->text = NULL;
}

void song_add_comment(song_t *song, char *comment) {
	lyric_t *lyric = array_new(song->lyrics);
	lyric->type = NSUB_COMMENT;
	lyric->num = 0;
	lyric->start = 0;
	lyric->stop = 0;
	lyric->name = NULL;
	lyric->text = comment ? strdup(comment) : NULL;
}

void song_add_lyric(song_t *song, int start, int stop, char *name, char *text) {
	song->current_num = song->current_num + 1;

	lyric_t *lyric = array_new(song->lyrics);
	lyric->type = NSUB_LYRIC;
	lyric->num = song->current_num;
	lyric->start = start;
	lyric->stop = stop;
	lyric->name = name ? strdup(name) : NULL;
	lyric->text = text ? strdup(text) : NULL;
}

void song_add_meta(song_t *song, char *key, char *value) {
	meta_t *meta = array_new(song->metas);
	meta->key = key ? strdup(key) : NULL;
	meta->value = value ? strdup(value) : NULL;
}

void uninit_lyric(lyric_t *lyric) {
	if (!lyric)
		return;

	free(lyric->name);
	free(lyric->text);
}

song_t *nsub_read(FILE *in, NSUB_FORMAT fmt) {
	song_t *song = NULL;
	cstring_t *line = NULL;

	/* Which reader? */
	int (*read_a_line)(song_t *, char *) = NULL;
	switch (fmt) {
	case NSUB_FMT_LRC:
		read_a_line = nsub_read_lrc;
		break;
	case NSUB_FMT_SRT:
		read_a_line = nsub_read_srt;
		break;
	case NSUB_FMT_WEBVTT:
		read_a_line = nsub_read_webvtt;
		break;
	default:
		fprintf(stderr, "Unsupported read format %d\n", fmt);
		goto fail;
	}

	/* Read it */
	song = new_song();
	line = new_cstring();
	size_t i = 0;
	while (cstring_readline(line, in)) {
		// UTF-8 BOM detection if any
		if (!i && cstring_starts_with(line->string, "\xEF\xBB\xBF", 0)) {
			cstring_t *tmp = cstring_substring(line->string, 3, 0);
			free_cstring(line);
			line = tmp;
		}

		i++;

		if (!read_a_line(song, line->string)) {
			fprintf(stderr, "Read error on line %zu: <%s>\n", i, line->string);
			song = NULL;
			goto fail;
		}
	}

	fail:

	free_cstring(line);
	return song;
}

int nsub_write(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset,
		int add_offset, double conv) {
	switch (fmt) {
	case NSUB_FMT_LRC:
		return nsub_write_lrc(out, song, fmt, apply_offset, 
			add_offset, conv);
	case NSUB_FMT_WEBVTT:
		return nsub_write_webvtt(out, song, fmt, apply_offset, 
			add_offset, conv);
	case NSUB_FMT_SRT:
		return nsub_write_srt(out, song, fmt, apply_offset, 
			add_offset, conv);
	default:
		fprintf(stderr, "Unsupported write format %d\n", fmt);
		return 0;
	}
}

int nsub_to_ms(const char line[], char deci_sym) {
	// 00:00:17,400

	/* should not happen! */
	/* note: also, we assume max 3 decimal digits */
	if (!nsub_is_timing(line, deci_sym, 3)) {
		fprintf(stderr,
				"Warning: called nsub_to_ms with bad input [%s], ignoring...\n",
				line);
		return 0;
	}

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
		int dot = car == deci_sym;
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

int nsub_is_timing(const char line[], char deci_sym, int max_deci) {
	// 00:00:14,800

	int digits = 0;
	int groups = 0;
	int sep = 0;

	int max_digits = 2;
	int max_groups = 3;

	for (char *ptr = (char *) line; *ptr; ptr++) {
		int digit = (*ptr >= '0' && *ptr <= '9');
		int col = (*ptr == ':');
		int dot = *ptr == deci_sym;

		if (digit) {
			digits++;
		} else if (col) {
			digits = 0;
			groups++;
		} else if (dot) {
			digits = 0;
			max_digits = max_deci;
			sep++;
		} else {
			return 0;
		}

		if (digits > max_digits)
			return 0;
		if (groups > max_groups)
			return 0;
		if (sep > 1)
			return 0;
	}

	return 1;
}

int apply_conv(int time, double conv) {
	// Just so we don't require -lm...
	double tmp = time * conv;
	double diff = tmp - (int)tmp;
	
	if (diff >= 0.5)
		return (int)tmp + 1;
	return (int)tmp;
}

