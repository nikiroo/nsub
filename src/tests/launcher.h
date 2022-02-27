#ifndef _LAUNCHER_H
#define _LAUNCHER_H

#include <check.h>

#define START(name) \
START_TEST(name) {\
	test_init(#name);\

#define END \
	test_success();\
}\
END_TEST\

#define FAIL(...) \
ck_abort_msg(__VA_ARGS__)\

#define ASSERT_EQUALS_STR(title, un, deux) \
	if (strcmp(un, deux)) { \
ck_abort_msg("%s\n\tExpected: <%s>\n\tReceived: <%s>", title, un, deux); \
}

#define ASSERT_EQUALS_INT(title, un, deux) \
	if (un != deux) { \
ck_abort_msg("%s\n\tExpected: %d\n\tReceived: %d", title, un, deux); \
}

#define ASSERT_EQUALS_SIZE(title, un, deux) \
	if (un != deux) { \
ck_abort_msg("%s\n\tExpected: %zu\n\tReceived: %zu", title, un, deux); \
}

SRunner *get_tests(int more);

void test_init(const char * const name);

void test_success();

void test_failure();

#endif

