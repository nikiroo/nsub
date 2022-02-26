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

#ifndef NSUB_H
#define NSUB_H

/*
 * Description of the lyrics of a song.
 * Note: all timings are in milliseconds.
 */

#include "utils/array.h"

typedef int NSUB_FORMAT;

#define NSUB_FMT_ERROR -1
#define NSUB_FMT_UNKNOWN 0
#define NSUB_FMT_LRC 1
#define NSUB_FMT_WEBVTT 2
#define NSUB_FMT_SRT 3

typedef int NSUB_TYPE;

#define NSUB_UNKNOWN 0
#define NSUB_EMPTY 1
#define NSUB_COMMENT 2
#define NSUB_LYRIC 3

typedef struct {
	NSUB_TYPE type;
	int num;
	int start;
	int stop;
	char *name;
	char *text;
} lyric_t;

typedef struct {
	char *key;
	char *value;
} meta_t;

typedef struct {
	array *lyrics;
	array *metas;
	int offset;
	int current_num;
	char *lang;
} song_t;

/* Song */

song_t *new_song();
void free_song(song_t *song);
void song_add_unknown(song_t *song, char *text);
void song_add_empty(song_t *song);
void song_add_comment(song_t *song, char *comment);
void song_add_lyric(song_t *song, int start, int stop, char *name, char *text);
void song_add_meta(song_t *song, char *key, char *value);

/* Read */

song_t *nsub_read(FILE *in, NSUB_FORMAT fmt);
int nsub_read_lrc(song_t *song, char *line);
int nsub_read_webvtt(song_t *song, char *line);
int nsub_read_srt(song_t *song, char *line);

/* Write */

int nsub_write(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset);
int nsub_write_lrc(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset);
int nsub_write_webvtt(FILE *out, song_t *song, NSUB_FORMAT fmt,
		int apply_offset);
int nsub_write_srt(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset);

#endif /* NSUB_H */
