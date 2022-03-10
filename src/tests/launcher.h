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

#define ASSERT_EQUALS_STR(title, expected, received) \
	do { if (strcmp(expected, received)) { \
ck_abort_msg("%s\n\tExpected: <%s>\n\tReceived: <%s>", title, expected, received); \
}} while(0)

#define ASSERT_EQUALS_INT(title, expected, received) \
	do { if (expected != received) { \
ck_abort_msg("%s\n\tExpected: %lld\n\tReceived: %lld", title, (long long)expected, (long long)received); \
}} while(0)

#define ASSERT_EQUALS_SIZE(title, expected, received) \
	do { if (expected != received) { \
ck_abort_msg("%s\n\tExpected: %zu\n\tReceived: %zu", title, expected, received); \
}} while(0)

extern int launcher_color;

SRunner *get_tests(int more);

void test_init(const char name[]);

int test_start(int more);

void test_success();

void test_failure();

#endif

