/**
 * @file print.h
 * @author Samantaz Fox
 * @date 2019
 *
 * @brief print.h
 *
 * print.h
 *
 * Small utility header designed to print file name and line number
 * along with debug/info messages.
 *
 * Usage example:
 *   ERROR("program exited with code %d\n", getErrorCode() );
 *
 * Known issues:
 *   - Must take at least one argument in addtion to format string
 *
 *
 * Copyright 2019 (C) Samantaz Fox
 * https://github.com/SamantazFox/Micro-projects/raw/master/library/print.h
 *
 * This file is in the public domain.
 * Feel free to copy, modify or redistribute it!
 *
*/

#ifndef __PRINT_H__
#define __PRINT_H__


#ifdef __cplusplus
 #include <cstdio>
#else
 #include <stdio.h>
#endif


#define TRACE(_format, ...) \
	do { \
		fprintf(stdout, "[Trace] %s:%d\t" _format, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

#define DEBUG(_format, ...) \
	do { \
		fprintf(stdout, "[Debug] %s:%d\t" _format, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

#define INFO(_format, ...) \
	do { \
		fprintf(stdout, "[Info ] %s:%d\t" _format, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

#define WARN(_format, ...) \
	do { \
		fprintf(stderr, "[Warn ] %s:%d\t" _format, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

#define ERROR(_format, ...) \
	do { \
		fprintf(stderr, "[Error] %s:%d\t" _format, __FILE__, __LINE__, __VA_ARGS__); \
	} while (0)


#endif  /* !__PRINT_H__ */
