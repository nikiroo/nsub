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

char *nsub_srt_time_str(int time, int show_sign);
void nsub_write_srt_lyric(FILE *out, lyric_t *lyric, int offset);

/* Public */

int nsub_write_srt(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset) {
	int offset;

	// header: none

	// metas: none

	// offset is not supported in SRT (so, always applied)
	{
		offset = song->offset;
	}

	// other metas: none

	// lyrics
	array_loop(song->lyrics, lyric, lyric_t)
	{
		nsub_write_srt_lyric(out, lyric, offset);
	}

	return 1;
}

/* Private */

void nsub_write_srt_lyric(FILE *out, lyric_t *lyric, int offset) {
	if (lyric->type == NSUB_EMPTY) {
		// not supported, ignored
		return;
	}

	if (lyric->type == NSUB_COMMENT || lyric->type == NSUB_UNKNOWN) {
		// not supported, ignored
		return;
	}

	//if (lyric->name)
	// not supported, ignored

	char *start = nsub_srt_time_str(lyric->start + offset, 0);
	char *stop = nsub_srt_time_str(lyric->stop + offset, 0);
	fprintf(out, "%d\n%s --> %s\n%s\n\n", lyric->num, start, stop, lyric->text);
	free(start);
	free(stop);
}

char *nsub_srt_time_str(int time, int show_sign) {
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

	sprintf(time_str, "%s%02d:%02d:%02d,%03d", sign, h, m, s, c);

	return time_str;
}
