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

#include "nsub.h"
#include "utils/utils.h"

/* Declarations */

char *nsub_lrc_time_str(int time, int show_sign);
void nsub_write_lrc_lyric(FILE *out, lyric_t *lyric, int offset);

/* Public */

int nsub_write_lrc(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset) {
	int offset;

	// header: none

	// metas
	array_loop(song->metas, meta, meta_t)
	{
		fprintf(out, "[%s: %s]\n", meta->key, meta->value);
	}

	// offset
	{
		char *offset_str;
		if (apply_offset) {
			offset = song->offset;
			offset_str = nsub_lrc_time_str(0, 1);
		} else {
			offset = 0;
			offset_str = nsub_lrc_time_str(song->offset, 1);
		}
		fprintf(out, "[offset: %s]\n", offset_str);
		free(offset_str);
	}

	// other metas
	{
		fprintf(out, "[created_by: nsub (https://github.com/nikiroo/nsub)]\n");
		if (song->lang)
			fprintf(out, "[language: %s]\n", song->lang);
	}

	// lyrics
	array_loop(song->lyrics, lyric, lyric_t)
	{
		nsub_write_lrc_lyric(out, lyric, offset);
	}

	return 1;
}

/* Private */

void nsub_write_lrc_lyric(FILE *out, lyric_t *lyric, int offset) {
	static int lrc_last_stop = 0;

	if (lyric->type == NSUB_EMPTY) {
		if (lrc_last_stop) {
			char *time = nsub_lrc_time_str(lrc_last_stop, 0);
			fprintf(out, "[%s]\n", time);
			free(time);
		} else {
			fprintf(out, "\n");
		}

		lrc_last_stop = 0;
		return;
	}

	if (lyric->type == NSUB_COMMENT || lyric->type == NSUB_UNKNOWN) {
		cstring_t *tmp = cstring_clone(lyric->text);
		if (tmp) {
			cstring_replace(tmp, "\n", "\\n");
		}

		fprintf(out, "-- %s\n", tmp->string);
		lrc_last_stop = 0;

		free_cstring(tmp);
		return;
	}

	if (lyric->name) {
		cstring_t *tmp = cstring_clone(lyric->name);
		if (tmp) {
			cstring_replace(tmp, "\n", "\\n");
		}

		fprintf(out, "-- %s\n", tmp->string);

		free_cstring(tmp);
	}

	char *time = nsub_lrc_time_str(lyric->start + offset, 0);
	cstring_t *tmp = cstring_clone(lyric->text);
	if (tmp) {
		cstring_replace(tmp, "\n", "\\n");
	}
	fprintf(out, "[%s] %s\n", time, tmp->string);
	free(time);
	free_cstring(tmp);

	lrc_last_stop = lyric->stop + offset;
}

char *nsub_lrc_time_str(int time, int show_sign) {
	char *sign = "";
	if (show_sign && time >= 0)
		sign = "+";
	if (time < 0)
		sign = "-";

	if (time < 0)
		time = (-time);

	int h = (time / 1000) / 3600;
	int m = ((time / 1000) / 60) % 60;
	int s = ((time / 1000)) % 60;
	int c = (time / 10) % 100;

	char *time_str = malloc(12 * sizeof(char));

	if (h) {
		sprintf(time_str, "%s%d:%02d:%02d.%02d", sign, h, m, s, c);
	} else {
		sprintf(time_str, "%s%02d:%02d.%02d", sign, m, s, c);
	}

	return time_str;
}
