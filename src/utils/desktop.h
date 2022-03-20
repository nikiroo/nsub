/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2021 Niki Roo
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
 * @file desktop.h
 * @author Niki
 * @date 2021 - 2022
 *
 * @brief Manipulate <tt>.desktop</tt> files (as described by
 * <a href='https://freedesktop.org/'>FreeDesktop.org</a>)
 *
 * This structure helps you manipulate <tt>.desktop</tt> files (as described by
 * <a href='https://freedesktop.org/'>FreeDesktop.org</a>).
 *
 * @note the desktop object can use icons; for the selection of those, an exact
 * 		match will first be tried (same name as the <tt>desktop</tt> file, with
 * 		a <tt>.png</tt> extension), then we will look into the local
 * 		<tt>.local/share/icons</tt> and if we still haven't found an icon, into
 * 		the theme (first by looking for a <tt>best_size</tt> sized icon and if
 * 		not in <tt>scalable</tt>)
 *
 * @note we support the use of desktop objects for menu, too, and that includes
 * 		submenu items support
 */
#ifndef DESKTOP_H
#define DESKTOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "array.h"

/**
 * The structure used to represent desktop objects.
 */
struct {
	char CNAME[10];
	/** The user name of the desktop object. */
	char *name;
	/** The icon name, if any. */
	char *icon;
	/** The icon file that corresponds, if any. */
	char *icon_file;
	/** The EXEC command to start. */
	char *exec;
	/** The submenu items of this desktop object (for a menu/submenu). */
	array_t *children;
	/** A custom external ID for this desktop object, for your own use. */
	int id;
}typedef desktop_t;

/**
 * Create a new desktop object from the given <tt>.desktop</tt> file.
 *
 * @note always identical to <tt>malloc</tt> + <tt>init_desktop</tt>
 *
 * @param filename the path to the actual <tt>.desktop</tt> file
 * @param best_size the default size for the icon (see icon selection in the
 * 		description of the {@see desktop} object
 *
 * @see malloc()
 * @see init_desktop(desktop_t *self, const char filename[], int best_size)
 * @see free_desktop(desktop_t *self)
 *
 * @return the desktop object
 */
desktop_t *new_desktop(const char filename[], int best_size);

/**
 * Create a new desktop object from the given <tt>.desktop</tt> file.
 *
 * @param filename the path to the actual <tt>.desktop</tt> file
 * @param best_size the default size for the icon (see icon selection in the
 * 		description of the {@see desktop} object
 *
 * @see new_desktop(const char filename[], int best_size)
 * @see uninit_desktop(desktop_t *self)
 *
 * @return TRUE if success (could fail if the target is not a <tt>.desktop</tt>
 * 		file
 */
int init_desktop(desktop_t *self, const char filename[], int best_size);

/**
 * Free the given desktop object.
 *
 * @note always equivalent to <tt>uninit_desktop</tt> + <tt>free</tt>
 *
 * @see uninit_desktop(desktop_t *self)
 * @see free(void *data)
 */
void free_desktop(desktop_t *self);

/**
 * Free the resources used by the given desktop object -- do not use it anymore
 * after this call.
 */
void uninit_desktop(desktop_t *self);

/**
 * Find a submenu item by the given ID ({@see desktop_set_id(desktop *, int)}).
 *
 * TODO: use full objects instead
 * @param children the array of pointers to desktop objects to look through
 * @param menu_id the ID of the submenu we want to find
 *
 * @return the given submenu if found, or NULL
 */
desktop_t *desktop_find_id(array_t *children, int menu_id);

/**
 * Look for the icon file related to this basename.
 *
 * @param basename the base name of the icon we want to look for
 * @param icon_size the best_size to use for the icon (see the description of
 * 		the {@desktop} object)
 *
 * @return the path to the best related icon we found (you own it), or NULL
 */
char *desktop_find_icon(const char basename[], int icon_size);

#endif /* DESKTOP_H */

#ifdef __cplusplus
}
#endif

