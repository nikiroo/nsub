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

/* Declarations */

char *nsub_webvtt_time_str(int time, int show_sign);
void nsub_write_webvtt_lyric(FILE *out, lyric_t *lyric, int offset);

/* Public */

int nsub_write_webvtt(FILE *out, song_t *song, NSUB_FORMAT fmt,
		int apply_offset) {
	int offset;

	// header
	{
		fprintf(out, "WEBVTT\nKind: captions\n");
		if (song->lang)
			fprintf(out, "Language: %s\n", song->lang);
		fprintf(out, "\n");
	}

	// metas
	{
		size_t count = array_count(song->metas);
		meta_t *meta;
		for (size_t i = 0; i < count; i++) {
			meta = (meta_t *) array_get_ptr(song->metas, i);
			fprintf(out, "NOTE META %s: %s\n\n", meta->key, meta->value);
		}
	}

	// offset is not supported in WebVTT (so, always applied)
	{
		offset = song->offset;
	}

	// other metas
	{
		fprintf(out,
				"NOTE META created by: nsub (https://github.com/nikiroo/nsub)]\n");
	}

	// lyrics
	{
		size_t count = array_count(song->lyrics);
		lyric_t *lyric;
		for (size_t i = 0; i < count; i++) {
			lyric = (lyric_t*) array_get_ptr(song->lyrics, i);
			nsub_write_webvtt_lyric(out, lyric, offset);
		}
	}

	return 1;
}

/* Private */

void nsub_write_webvtt_lyric(FILE *out, lyric_t *lyric, int offset) {
	if (lyric->type == NSUB_EMPTY) {
		fprintf(out, "\n\n");
		return;
	}

	if (lyric->type == NSUB_COMMENT || lyric->type == NSUB_UNKNOWN) {
		fprintf(out, "NOTE %s\n\n", lyric->text);
		return;
	}

	if (lyric->name)
		fprintf(out, "%s\n", lyric->name);

	char *start = nsub_webvtt_time_str(lyric->start + offset, 0);
	char *stop = nsub_webvtt_time_str(lyric->stop + offset, 0);
	fprintf(out, "%s --> %s\n%s\n\n", start, stop, lyric->text);
	free(start);
	free(stop);
}

char *nsub_webvtt_time_str(int time, int show_sign) {
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
	int c = (time) % 1000;

	char *time_str = malloc(12 * sizeof(char));

	if (h) {
		sprintf(time_str, "%s%d:%02d:%02d.%03d", sign, h, m, s, c);
	} else {
		sprintf(time_str, "%s%02d:%02d.%03d", sign, m, s, c);
	}

	return time_str;
}