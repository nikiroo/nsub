/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2020 Niki Roo
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
 * @file timing.h
 * @author Niki
 * @date 2020 - 2022
 *
 * @brief Timing macros START and STOP
 *
 * 2 macro are provided to print the elapsed time between the 2 to stdout.
 */

#ifndef TIMING_H
#define TIMING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>

/**
 * Start the timer.
 */
#define START struct timeval TIMING_start, TIMING_stop; \
	/* 1 usec = 0.000001 s */ \
	char cusec[7]; \
	gettimeofday(&TIMING_start, NULL);

/**
 * Stop the timer and print the elapsed time to stdout.
 */
#define STOP gettimeofday(&TIMING_stop, NULL); \
	TIMING_stop.tv_sec  = TIMING_stop.tv_sec  - TIMING_start.tv_sec; \
	TIMING_stop.tv_usec = TIMING_stop.tv_usec - TIMING_start.tv_usec; \
	sprintf(cusec, "%0.6d", TIMING_stop.tv_usec); \
	printf("TIME: %d.%s sec\n", TIMING_stop.tv_sec, cusec); \
	gettimeofday(&TIMING_start, NULL);

#endif // TIMING_H

#ifdef __cplusplus
}
#endif

