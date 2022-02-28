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
#include "utils/array.h"
#include "utils/utils.h"

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
	size_t count;

	if (!song)
		return;

	count = array_count(song->metas);
	meta_t *meta;
	for (size_t i = 0; i < count; i++) {
		meta = array_get_ptr(song->metas, i);
		free(meta->key);
		free(meta->value);
	}
	free_array(song->metas);

	count = array_count(song->lyrics);
	lyric_t *lyric;
	for (size_t i = 0; i < count; i++) {
		lyric = array_get_ptr(song->lyrics, i);
		free(lyric->text);
	}
	free_array(song->lyrics);

	free(song->lang);
	free(song);
}

void song_add_unknown(song_t *song, char *text) {
	lyric_t lyric;
	lyric.type = NSUB_UNKNOWN;
	lyric.num = 0;
	lyric.start = 0;
	lyric.stop = 0;
	lyric.name = NULL;
	lyric.text = text ? strdup(text) : NULL;
	array_add(song->lyrics, &lyric);
}

void song_add_empty(song_t *song) {
	lyric_t lyric;
	lyric.type = NSUB_EMPTY;
	lyric.num = 0;
	lyric.start = 0;
	lyric.stop = 0;
	lyric.name = NULL;
	lyric.text = NULL;
	array_add(song->lyrics, &lyric);
}

void song_add_comment(song_t *song, char *comment) {
	lyric_t lyric;
	lyric.type = NSUB_COMMENT;
	lyric.num = 0;
	lyric.start = 0;
	lyric.stop = 0;
	lyric.name = NULL;
	lyric.text = comment ? strdup(comment) : NULL;
	array_add(song->lyrics, &lyric);
}

void song_add_lyric(song_t *song, int start, int stop, char *name, char *text) {
	song->current_num = song->current_num + 1;

	lyric_t lyric;
	lyric.type = NSUB_LYRIC;
	lyric.num = song->current_num;
	lyric.start = start;
	lyric.stop = stop;
	lyric.name = name ? strdup(name) : NULL;
	lyric.text = text ? strdup(text) : NULL;
	array_add(song->lyrics, &lyric);
}

void song_add_meta(song_t *song, char *key, char *value) {
	meta_t meta;
	meta.key = key ? strdup(key) : NULL;
	meta.value = value ? strdup(value) : NULL;
	array_add(song->metas, &meta);
}

song_t *nsub_read(FILE *in, NSUB_FORMAT fmt) {
	array *lines = new_array(sizeof(char *), 80);
	size_t count = array_readfiles(lines, in);

	song_t *song = new_song();
	char *line;
	for (size_t i = 0; i < count; i++) {
		array_get(lines, &line, i);
		switch (fmt) {
		case NSUB_FMT_LRC:
			if (!nsub_read_lrc(song, line)) {
				fprintf(stderr, "Read error on line %zu: <%s>\n", i, line);
				song = NULL;
				goto fail;
			}
			break;
		default:
			fprintf(stderr, "Unsupported read format %d\n", fmt);
			free_song(song);
			song = NULL;
			goto fail;
		}
	}

	fail:

	array_free_all(lines);
	return song;
}

int nsub_write(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset) {
	switch (fmt) {
	case NSUB_FMT_LRC:
		return nsub_write_lrc(out, song, fmt, apply_offset);
	case NSUB_FMT_WEBVTT:
		return nsub_write_webvtt(out, song, fmt, apply_offset);
	case NSUB_FMT_SRT:
		return nsub_write_srt(out, song, fmt, apply_offset);
	default:
		fprintf(stderr, "Unsupported write format %d\n", fmt);
		return 0;
	}
}
