/*
 * CUtils: some small C utilities
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

#include "launcher.h"

#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

// Shared globals
int launcher_color = -1;

// Private functions
static int has_colour();
static char *waiting_color();
static char *passed_color();
static char *failed_color();
static char *stop_color();

// Private vars
// static char BLEU[] = { (char) 27, '[', '3', '4', 'm', '\0' };
// static char TEAL[] = { (char) 27, '[', '3', '6', 'm', '\0' };
// static char GRIS[] = { (char) 27, '[', '3', '7', 'm', '\0' };
static char VERT[] = { (char) 27, '[', '3', '2', 'm', '\0' };
static char ROUGE[] = { (char) 27, '[', '3', '1', 'm', '\0' };
static char ORANGE[] = { (char) 27, '[', '3', '3', 'm', '\0' };
static char STOP[] = { (char) 27, '[', '0', 'm', '\0' };
static char bs9[] = { 8, 8, 8, 8, 8, 8, 8, 8, 8, '\0' };

// CK_ENV : Gets the print mode from the environment variable CK_VERBOSITY,
// which can have the values "silent", "minimal", "normal", "verbose". If the
// variable is not found or the value is not recognized, the print mode is set
// to CK_NORMAL.

// How to start the program:
// ========================
//
// $0
//	test the code (normal tests only)
// $0 --more
// 	run extra tests (if available; usually, they are specified as extra if they
//  take long to process)
//
//
// $0 --name NAME
//	do not test, just format the name as if it was tested
// $0 --passed
//	format the line as if it was passes
// $0 --failed 
//	format the line as if it was failed

char *tests_name = NULL;

void test_init(const char name[]) {
	int i;
	int cols;
	struct winsize ws;

	ioctl(1, TIOCGWINSZ, &ws);
	cols = ws.ws_col;

	if (!tests_name) {
		tests_name = malloc(sizeof(char) * (cols + 1));
	}

	for (i = 0; i < (cols + 1); i++) {
		if (i < 4)
			tests_name[i] = ' ';
		else
			tests_name[i] = '.';
	}

	strcpy(tests_name + 4, name);
	tests_name[strlen(name) + 4] = ' ';
	tests_name[cols - 6 - 1 - 4] = '\0';

	fprintf(stderr, "%s", tests_name);
	fprintf(stderr, "%s[%s%s%s]    ", " ", waiting_color(), " ?? ",
			stop_color());
}

void test_success() {
	fprintf(stderr, "%s[%s%s%s]    ", bs9, passed_color(), " OK ",
			stop_color());
}

void test_failure() {
	fprintf(stderr, "%s[%s%s%s]    ", bs9, failed_color(), "FAIL",
			stop_color());
}

int test_start(int more) {
	int failed;
	SRunner *runner;

	runner = get_tests(more);

	failed = 0;
	if (runner) {
		srunner_run_all(runner, CK_ENV);

		failed = srunner_ntests_failed(runner);
		srunner_free(runner);
	} else {
		printf(">>> No tests have been found <<<\n");
	}

	return failed;
}

int main(int argc, char **argv) {
	int more = 0;
	int cont = 1;

	for (int i = 1; i < argc; i++) {
		if (!strcmp("--name", argv[i])) {
			if ((i + 1) >= argc)
				return 1;
			test_init(argv[++i]);
			cont = 0;
		} else if (!strcmp("--passed", argv[i])) {
			test_success();
			cont = 0;
		} else if (!strcmp("--failed", argv[i])) {
			if ((i + 1) >= argc)
				return 1;
			test_failure();
			cont = 0;
		} else if (!strcmp("--more", argv[i])) {
			if ((i + 1) >= argc)
				return 1;
			more = 1;
		} else if (!strcmp("--color", argv[i])) {
			launcher_color = 1;
		} else if (!strcmp("--no-color", argv[i])) {
			launcher_color = 0;
		}
	}

	if (!cont)
		return 0;

	return test_start(more);
}

static int has_colour() {
	if (launcher_color == -1) {
		// TODO: detect if terminal supports colour mode
		// for instance, check if $COLORTERM is not empty?
		launcher_color = 1;
	}

	return launcher_color;
}

static char *waiting_color() {
	if (has_colour())
		return ORANGE;
	return "";
}

static char *passed_color() {
	if (has_colour())
		return VERT;
	return "";
}

static char *failed_color() {
	if (has_colour())
		return ROUGE;
	return "";
}

static char *stop_color() {
	if (has_colour())
		return STOP;
	return "";
}
