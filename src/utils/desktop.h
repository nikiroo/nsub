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
typedef struct desktop_p desktop;

/**
 * Create a new desktop object from the given <tt>.desktop</tt> file.
 *
 * @param filename the path to the actual <tt>.desktop</tt> file
 * @param best_size the default size for the icon (see icon selection in the
 * 		description of the {@see desktop} object
 *
 * @return the desktop object
 */
desktop *new_desktop(const char filename[], int best_size);

/**
 * Free the given desktop object
 */
void free_desktop(desktop *self);

/** Return the name of the desktop object (you do <b>not</b> own it). */
const char *desktop_get_name(desktop *self);
/** Return the exec command of the desktop object (you do <b>not</b> own it). */
const char *desktop_get_exec(desktop *self);
/** Return the icon name of the desktop object (you do <b>not</b> own it). */
const char *desktop_get_icon(desktop *self);
/** Return the icon file of the desktop object (you do <b>not</b> own it). */
const char *desktop_get_icon_file(desktop *self);

/**
 * Return the external ID of this desktop object
 * ({@see desktop_set_id(desktop*, int)}.
 *
 * @return the external ID
 */
int desktop_get_id(desktop *self);

/**
 * Set a custom external ID for this desktop object, for your own use.
 *
 * @param id the ID to set
 */
void desktop_set_id(desktop *self, int id);

/**
 * Return all the submenu items of this desktop objects (for a menu/submenu).
 *
 * TODO: switch to full objects
 * @return an array of pointers to desktop objects
 */
array_t *desktop_get_children(desktop *app);

/**
 * Find a submenu item by the given ID ({@see desktop_set_id(desktop *, int)}).
 *
 * TODO: use full objects instead
 * @param children the array of pointers to desktop objects to look through
 * @param menu_id the ID of the submenu we want to find
 *
 * @return the given submenu if found, or NULL
 */
desktop *desktop_find_id(array_t *children, int menu_id);

/**
 * Look for the icon file related to this basename.
 *
 * @param basename the base name of the icon we want to look for
 * @param icon_size the best_size to use for the icon (see the description of
 * 		the {@desktop} object)
 */
char *desktop_find_icon(const char basename[], int icon_size);

#endif /* DESKTOP_H */

#ifdef __cplusplus
}
#endif

