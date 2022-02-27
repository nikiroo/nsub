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

#ifndef DESKTOP_H
#define DESKTOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "array.h"

typedef struct desktop_p desktop;

desktop *new_desktop(const char filename[], int best_size);
void free_desktop(desktop *app);

const char *desktop_get_name(desktop *app);
const char *desktop_get_exec(desktop *app);
const char *desktop_get_icon(desktop *app);
const char *desktop_get_icon_file(desktop *app);

// external id (that you may set/use yourself)
int desktop_get_id(desktop *app);
void desktop_set_id(desktop *app, int id);

array *desktop_get_children(desktop *app);

desktop *desktop_find_id(array *children, int menu_id);

// Static
char *desktop_find_icon(const char basename[], int icon_size);

#endif /* DESKTOP_H */

#ifdef __cplusplus
}
#endif

