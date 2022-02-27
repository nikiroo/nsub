#include <check.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "launcher.h"

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

void test_init(const char * const name) {
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

	strcpy(tests_name + cols - 6 - 1 - 4, " [ ?? ]    ");

	fprintf(stderr, "%s", tests_name);
}

void test_success() {
	fprintf(stderr, "%s", " OK ]    ");
}

void test_failure() {
	fprintf(stderr, "%s", "FAIL]    ");
}

int main(int argc, char **argv) {
	int failed;
	SRunner *runner;
	int more;

	if (argc > 1 && !strcmp("--name", argv[1])) {
		test_init(argv[2]);
		return 0;
	}
	if (argc > 1 && !strcmp("--passed", argv[1])) {
		test_success();
		return 0;
	}
	if (argc > 1 && !strcmp("--failed", argv[1])) {
		test_failure();
		return 0;
	}

	more = 0;
	if (argc > 1 && !strcmp("--more", argv[1]))
		more = 1;
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

