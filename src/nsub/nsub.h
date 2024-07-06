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

/**
 * @file nsub.h
 * @author Niki
 * @date 2024
 *
 * @brief Subtitle/Lyrics conversion program (webvtt/srt/lrc)
 *
 * A small program to convert between subtitles and lyrics formats.
 * It supports webvtt, srt and lrc files.
 *
 * Use <tt>nsub --help</tt> for more information.
 */

#ifndef NSUB_H
#define NSUB_H

/*
 * Description of the lyrics of a song.
 *
 * @note all timings are in milliseconds.
 */
#include "cutils/array.h"

/**
 * A subtitle or lyric format to import from/export to.
 */
typedef int NSUB_FORMAT;

/** The format is not supported. */
#define NSUB_FMT_ERROR -1
/** The format is (not yet) known. */
#define NSUB_FMT_UNKNOWN 0
/** Lyrcis format (usually for music lyrics). */
#define NSUB_FMT_LRC 1
/** The W3C standard format for video subtitles. */
#define NSUB_FMT_WEBVTT 2
/** A de-facto standard for video subtitles (from the program SubRip). */
#define NSUB_FMT_SRT 3

/**
 * A type of lyric.
 */
typedef int NSUB_TYPE;

/** Unknown type. */
#define NSUB_UNKNOWN 0
/** Empty line (more or less a comment). */
#define NSUB_EMPTY 1
/** A comment (with content). */
#define NSUB_COMMENT 2
/** A lyric. */
#define NSUB_LYRIC 3

/**
 * A lyric (the text of a line from a dialogue or part of a song).
 */
typedef struct {
	NSUB_TYPE type;
	/**
	 * The sequential number of this lyric (only valid for NSUB_LYRIC).
	 *
	 * @note the numbering start at 1
	 */
	int num;
	/**
	 * The time in milliseconds (total song/video play time) after which this
	 * lyric start.
	 */
	int start;
	/**
	 * The time in milliseconds (total song/video play time) after which this
	 * lyric stop.
	 */
	int stop;
	/**
	 * The name of this lyric (not the text content, but a name for this line).
	 *
	 * @note most format do not understand this, and make it a comment or ignore
	 * 		it altogether (example: « Chorus 1 »)
	 */
	char *name;
	/**
	 * The actual content of this lyric or comment.
	 */
	char *text;
} lyric_t;

/**
 * Some piece of meta-data.
 */
typedef struct {
	char *key;
	char *value;
} meta_t;

/**
 * A song (or video).
 *
 * The main goal of this structure is to group the lyrics.
 */
typedef struct {
	/** The actual lyrics. */
	array_t *lyrics;
	/** The meta-data if any. */
	array_t *metas;
	/** An offset to apply to every lyrics timings. */
	int offset;
	/**
	 *  The last lyric number (start at 1, 0 means no lyric).
	 *
	 *  @see lyric_t.num
	 */
	int current_num;
	/** The language of the lyrics. */
	char *lang;
} song_t;

/* Song & Lyric */

song_t *new_song();
void free_song(song_t *song);
void song_add_unknown(song_t *song, char *text);
void song_add_empty(song_t *song);
void song_add_comment(song_t *song, char *comment);
void song_add_lyric(song_t *song, int start, int stop, char *name, char *text);
void song_add_meta(song_t *song, char *key, char *value);
void uninit_lyric(lyric_t *lyric);

/* Read */

/**
 * Convert a text line into milliseconds (for instance, 00:00:17,400).
 * Text <b>must</b> be conform, though less groups or less digits per group
 * is allowed.
 *
 * @note maximum number of groups: 4,
 * 		 maximum number of digits per
 * 			group: 2 except decimal group which is allowed up to 3
 *
 * @param line the line to convert
 * @param deci_sep the decimal separator symbol (usually '.' or ',')
 * @param max_deci maximum number of digits for the decimal value (max is 3)
 *
 * @return the number of milliseconds it means
 */
int nsub_to_ms(const char line[], char deci_sym);

/**
 * Validate that the given line is a timing (for instance, 00:00:17,400).
 * Text <b>must</b> be conform, though less groups or less digits per group
 * is allowed.
 *
 * @note maximum number of groups: 4,
 * 		 maximum number of digits per
 * 			group: 2 except decimal group which is allowed up to 3
 *
 * @param line the line to check
 * @param deci_sep the decimal separator symbol (usually '.' or ',')
 * @param max_deci maximum number of digits for the decimal value
 *
 * @return TRUE if it is
 */
int nsub_is_timing(const char line[], char deci_sym, int max_deci);

/**
 * Apply a conversion ratio to the given time.
 *
 * @param time the initial time to compute from
 * @param conv the conversion ratio to apply (i.e., 1 = no conversion)
 *
 * @return the converted time
 */
int apply_conv(int time, double conv);

song_t *nsub_read(FILE *in, NSUB_FORMAT fmt);
int nsub_read_lrc(song_t *song, char *line);
int nsub_read_webvtt(song_t *song, char *line);
int nsub_read_srt(song_t *song, char *line);

/* Write */

// conv = time conversion ratio
int nsub_write(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset,
		int add_offset, double conv);
int nsub_write_lrc(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset,
		int add_offset, double conv);
int nsub_write_webvtt(FILE *out, song_t *song, NSUB_FORMAT fmt,
		int apply_offset, int add_offset, double conv);
int nsub_write_srt(FILE *out, song_t *song, NSUB_FORMAT fmt, int apply_offset,
		int add_offset, double conv);

#endif /* NSUB_H */
