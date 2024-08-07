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

/* Declarations */

NSUB_FORMAT nsub_parse_fmt(char *type, int required);
void help(char *program);

int main(int argc, char **argv) {
	int rep = 0;

	int from = NSUB_FMT_UNKNOWN;
	int to = NSUB_FMT_UNKNOWN;
	char *in_file = NULL;
	char *out_file = NULL;
	int apply_offset = 0;
	int add_offset = 0;
	double conv = 1;

	if (argc <= 1) {
		help(argv[0]);
		return 5;
	}

	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (!strcmp("--help", arg) || !strcmp("-h", arg)) {
			help(argv[0]);
			return 0;
		} else if (!strcmp("--from", arg) || !strcmp("-f", arg)) {
			if (i + 1 >= argc) {
				fprintf(stderr,
					"The parameter --from/-f "
					"requires an argument\n"
				);
				return 5;
			}
			from = nsub_parse_fmt(argv[++i], 1);
			if (from == NSUB_FMT_ERROR) {
				fprintf(stderr, 
					"Unsupported input "
					"format: %s\n", argv[i]
				);
				return 8;
			}
		} else if (!strcmp("--to", arg) || !strcmp("-t", arg)) {
			if (i + 1 >= argc) {
				fprintf(stderr, 
					"The parameter --to/-t "
					"requires an argument\n"
				);
				return 5;
			}
			to = nsub_parse_fmt(argv[++i], 1);
			if (to == NSUB_FMT_ERROR) {
				fprintf(stderr, 
					"Unsupported output "
					"format: %s\n", argv[i]
				);
				return 9;
			}
		} else if (!strcmp("--apply-offset", arg) 
				|| !strcmp("-a", arg)) {
			apply_offset = 1;
		} else if (!strcmp("--ntsc", arg) 
				|| !strcmp("-n", arg)) {
			conv = 25.00 / 29.97;
		} else if (!strcmp("--pal", arg) 
				|| !strcmp("-p", arg)) {
			conv = 29.97 / 25.00;
		} else if (!strcmp("--offset", arg) 
				|| !strcmp("-o", arg)) {
			if (i + 1 >= argc) {
				fprintf(stderr,
					"The parameter --offset/-o requires "
					"an argument\n"
				);
				return 5;
			}

			if (sscanf(argv[++i], "%i", &add_offset) == EOF) {
				fprintf(stderr, 
					"Bad parameter to %s: %s\n",
					arg, argv[i-1]
				);
				return 5;
			}
		} else if (!strcmp("--ratio", arg) 
				|| !strcmp("-r", arg)) {
			if (i + 1 >= argc) {
				fprintf(stderr,
					"The parameter --ratio/-r requires "
					"an argument\n"
				);
				return 5;
			}

			if (sscanf(argv[++i], "%lf", &conv) == EOF) {
				fprintf(stderr, 
					"Bad parameter to %s: %s\n",
					arg, argv[i-1]
				);
				return 5;
			}
		} else if (!strcmp("--output", arg) || !strcmp("-o", arg)) {
			if (i + 1 >= argc) {
				fprintf(stderr,
					"The parameter --output/-o requires "
					"an argument\n"
				);
				return 5;
			}
			out_file = argv[++i];
			if (to == NSUB_FMT_UNKNOWN) {
				char *ext = strrchr(argv[i], '.');
				if (ext) {
					ext++;
					to = nsub_parse_fmt(ext, 0);
				}
			}
		} else if (!in_file) {
			in_file = arg;
			if (from == NSUB_FMT_UNKNOWN) {
				char *ext = strrchr(arg, '.');
				if (ext) {
					ext++;
					from = nsub_parse_fmt(ext, 0);
				}
			}
		} else if (!out_file) {
			out_file = arg;
			if (to == NSUB_FMT_UNKNOWN) {
				char *ext = strrchr(arg, '.');
				if (ext) {
					ext++;
					to = nsub_parse_fmt(ext, 0);
				}
			}
		} else {
			fprintf(stderr, "Syntax error\n");
			return 5;
		}
	}

	if (from == NSUB_FMT_UNKNOWN) {
		fprintf(stderr,
			"Cannot detect input format, "
			"please specify it with '--from'\n"
		);
		return 6;
	}

	if (to == NSUB_FMT_UNKNOWN) {
		fprintf(stderr,
			"Cannot detect output format, "
			"please specify it with '--to'\n"
		);
		return 7;
	}

	FILE *in = stdin;
	if (in_file && !(in_file[0] == '-' && !in_file[1])) {
		in = fopen(in_file, "r");
		if (!in) {
			fprintf(stderr, 
				"Cannot open input file: %s\n", in_file
			);
			rep = 2;
		}
	}

	FILE *out = stdout;
	if (!rep && out_file && !(out_file[0] == '-' && !out_file[1])) {
		out = fopen(out_file, "w");
		if (!in) {
			fprintf(stderr, 
				"Cannot create output file: %s\n", out_file
			);
			rep = 3;
		}
	}

	if (!rep) {
		song_t *song = nsub_read(in, from);
		if (!song)
			rep = 22;

		if (!rep && !nsub_write(out, song, to, apply_offset, 
				add_offset, conv))
			rep = 33;

		free_song(song);
	}

	if (in && in != stdin)
		fclose(in);

	if (out && out != stdout)
		fclose(out);

	return rep;
}

/* Private */

NSUB_FORMAT nsub_parse_fmt(char *type, int required) {
	if (!strcmp("lrc", type)) {
		return NSUB_FMT_LRC;
	} else if (!strcmp("srt", type)) {
		return NSUB_FMT_SRT;
	} else if (!strcmp("webvtt", type)) {
		return NSUB_FMT_WEBVTT;
	} else if (!strcmp("vtt", type)) {
		return NSUB_FMT_WEBVTT;
	}

	if (required)
		return NSUB_FMT_ERROR;

	return NSUB_FMT_UNKNOWN;
}

void help(char *program) {
	printf("NSub subtitles conversion program\n");
	printf("Syntax:\n");
	printf("\t%s (--from FMT) (--to FMT) (--apply-offset) (--offset MSEC)\n"
			"\t\t (--ntsc) (--pal) (--ratio RATIO)\n"
			"\t\t (--output OUT_FILE) (IN_FILE)\n", 
		program
	);
	
	printf("\nOptions:\n");
	printf("\t-h/--help         : this help message\n");
	printf("\t-f/--from FMT     : select the input  format FMT\n");
	printf("\t-t/--to   FMT     : select the output format FMT\n");
	printf("\t-a/--apply-offset : apply the offset tag "
		"value to the lyrics\n"
	);
	printf("\t-o/--offset MSEC  : add a manual offset to all timings\n");
	printf("\t-n/--ntsc         : Convert timings from NTSC to PAL\n");
	printf("\t-p/--pal          : Convert timings from PAL to NTSC\n");
	printf("\t-r/--ratio RATIO  : Convert timings with a "
		"custom ratio\n");
	
	printf("\nArguments:\n");
	printf(
		"\tIN_FILE  : the input file or '-' for stdin "
		"(which is the default)\n"
	);
	printf(
		"\tOUT_FILE : the output file or '-' for stdout "
		"(which is the default)\n"
	);
	printf("\tRATIO    : the ratio to apply to timings (1 = no change)\n");
	printf(
		"\tMSEC     : the offset to add to all timings in "
		"milliseconds\n"
	);
	printf("\n");
	printf(
		"Note: the in/out formats will be guessed from the "
		"extension if needed/possible\n"
	);
	printf(
		"Note: to specify a file named dash (-), prefix it with a path"
		" (e.g., './-')\n"
	);
	printf("\n");
	printf("Supported formats:\n");
	printf("\tlrc: lyrics files\n");
	printf("\tsrt: SubRip subtitles files\n");
	printf("\tvtt/webvtt: Web Video Text Tracks\n");
}
