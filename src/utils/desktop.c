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

#include <stdio.h>
#include <dirent.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "utils.h"

#define EXT "desktop"

/* Private functions */
static int desktop_compare(const void *a, const void* b);
static int desktop_test_file(const char filename[]);
/* */

desktop_t *new_desktop(const char filename[], int best_size) {
	desktop_t *me = malloc(sizeof(desktop_t));
	if (!init_desktop(me, filename, best_size)) {
		free(me);
		me = NULL;
	}

	return me;
}

int init_desktop(desktop_t *me, const char filename[], int best_size) {
	strcpy(me->CNAME, "[Desktop]");

	me->name = NULL;
	me->exec = NULL;
	me->icon = NULL;
	me->icon_file = NULL;
	me->children = NULL;
	me->id = 0;

	// Copy name
	me->name = strdup(filename);

	// Get extension an remove ".desktop" from name
	char *ext = rindex(me->name, '.');
	if (ext) {
		size_t idot = (ext - me->name);
		ext++;
		if (!strcmp(ext, EXT))
			me->name[idot] = '\0';
	}
	if (ext)
		ext = strdup(ext);

	// If PNG of the same name, use as default icon
	me->icon_file = desktop_find_icon(me->name, best_size);

	// Pretify default name (remove dir part)
	char *slash = rindex(me->name, '/');
	if (slash && !slash[1]) {
		slash[0] = '\0';
		slash = rindex(me->name, '/');
	}
	if (slash) {
		char *copy = strdup(slash + 1);
		free(me->name);
		me->name = copy;
	}

	// Try with the base name, too
	if (!me->icon_file) {
		me->icon_file = desktop_find_icon(me->name, best_size);
	}

	DIR *dp = opendir(filename);
	if (dp) {
		// Try to get the default folder icon if no icon yet
		if (!me->icon_file) {
			me->icon_file = desktop_find_icon("folder", best_size);
		}

		me->children = new_array(sizeof(desktop_t*), 32);
		for (struct dirent *ep = readdir(dp); ep; ep = readdir(dp)) {
			if (!strcmp(ep->d_name, "."))
				continue;
			if (!strcmp(ep->d_name, ".."))
				continue;

			desktop_t *child = array_new(me->children);
			char *childname = cstring_concat(filename, "/", ep->d_name, NULL);
			if (!init_desktop(child, childname, best_size))
				array_pop(me->children);
			free(childname);
		}

		array_qsort(me->children, desktop_compare);

		closedir(dp);
		free(ext);
		return 1;
	}

	// Only process ".desktop" files
	if (!ext || strcmp(ext, EXT)) {
		free_desktop(me);
		free(ext);
		return 0;
	}

	FILE *file;
	cstring_t *line;
	char *startsWith;

	file = fopen(filename, "r");
	if (file) {
		line = new_cstring();
		while (cstring_readline(line, file)) {
			startsWith = "Name=";
			if (cstring_starts_with(line->string, startsWith, 0)) {
				free(me->name);
				me->name = strdup(line->string + strlen(startsWith));
			}

			startsWith = "Exec=";
			if (cstring_starts_with(line->string, startsWith, 0)) {
				free(me->exec);
				me->exec = strdup(line->string + strlen(startsWith));
				// TODO: %f %F %u %U %i %c %k: inject values instead
				char *cars = "ifFuUck";
				for (char *ptr = index(me->exec, '%'); ptr;
						ptr = index(ptr, '%')) {
					if (index(cars, ptr[1])) {
						ptr[0] = ' ';
						ptr[1] = ' ';
					}
					ptr++;
				}
			}

			startsWith = "Icon=";
			if (cstring_starts_with(line->string, startsWith, 0)) {
				free(me->icon);
				me->icon = strdup(line->string + strlen(startsWith));
			}
		}
		free_cstring(line);
		fclose(file);
	}

	// Find icon file linked to icon
	if (me->icon && !me->icon_file) {
		me->icon_file = desktop_find_icon(me->icon, best_size);
	}
	// ...or any we can find, actually
	if (!me->icon_file) {
		me->icon_file = desktop_find_icon(me->name, best_size);
	}

	free(ext);
	return 1;
}

void free_desktop(desktop_t *me) {
	if (me)
		uninit_desktop(me);

	free(me);
}

void uninit_desktop(desktop_t *me) {
	free(me->name);
	free(me->exec);
	free(me->icon);
	free(me->icon_file);
	me->name = NULL;
	me->exec = NULL;
	me->icon = NULL;
	me->icon_file = NULL;
	free_array(me->children);
	me->children = NULL;
	me->CNAME[0] = '!';
}

desktop_t *desktop_find_id(array_t *children, int id) {
	desktop_t *found = NULL;

	array_loop(children, child, desktop_t)
	{
		if (child->id == id) {
			found = child;
			break;
		}

		found = desktop_find_id(child->children, id);
		if (found)
			break;
	}

	return found;
}

/* Private functions */

static int desktop_compare(const void *a, const void* b) {
	desktop_t *me1 = ((desktop_t**) a)[0];
	desktop_t *me2 = ((desktop_t**) b)[0];

	if (me1->children && !(me2->children))
		return -1;
	if (!(me1->children) && me2->children)
		return 1;

	return strcmp(me1->name, me2->name);
}

static int desktop_test_file(const char filename[]) {
	FILE *test;
	DIR *test_dir;

	test = fopen(filename, "r");
	if (test) {
		fclose(test);
		test_dir = opendir(filename);
		if (test_dir) {
			closedir(test_dir);
		} else {
			return 1;
		}
	}

	return 0;
}

#define TRY_DIR(a,b,c) \
	do { \
		tmp = cstring_concat(a, b, c, basename, ".png", NULL); \
		if(desktop_test_file(tmp)) \
			return tmp; \
		free(tmp); \
	} while(0)

static char *theme = NULL;
static char *ltheme = NULL;
char *desktop_find_icon(const char basename[], int icon_size) {
	char *tmp = NULL;
	char *home = getenv("HOME");
	char icon_size_str[100];
	sprintf(icon_size_str, "%dx%d", icon_size, icon_size);

	if (!theme) {
		tmp = cstring_concat(home, "/.gtkrc-2.0", NULL);
		FILE *file = fopen(tmp, "r");
		free(tmp);
		if (file) {
			const char *startsWith = "gtk-icon-theme-name=";
			size_t n = strlen(startsWith);

			cstring_t *line = new_cstring();
			while (cstring_readline(line, file)) {
				if (cstring_starts_with(line->string, startsWith, 0)) {
					free(theme);
					if (line->string[n] == '"') {
						theme = strdup(line->string + n + 1);
						theme[strlen(theme) - 1] = '\0';
					} else {
						theme = strdup(line->string + n);
					}
				}
			}
			free_cstring(line);
			fclose(file);

			if (!theme || !theme[0]) {
				theme = strdup("");
				ltheme = strdup("");
			} else {
				tmp = theme;
				theme = cstring_concat("/usr/share/icons/", tmp, "/", NULL);
				ltheme = cstring_concat(home, "/", ".icons/", tmp, "/", NULL);
				free(tmp);
			}

		}
	}

	// Allow NULL
	if (!basename || !basename[0])
		return NULL;

	// exact match
	tmp = strdup(basename);
	if (desktop_test_file(tmp))
		return tmp;
	free(tmp);

	// same name, with ".png"
	TRY_DIR("", "", "");

	// local icons
	TRY_DIR(home, "/.local/share/icons/", "");

	// theme icons
	if (theme && theme[0]) {
		// exact size (apps, places)
		TRY_DIR(theme, icon_size_str, "/apps/");
		TRY_DIR(theme, icon_size_str, "/places/");

		// scalable (apps, places)
		TRY_DIR(ltheme, "scalable", "/apps/");
		TRY_DIR(ltheme, "scalable", "/places/");
	}

	// shared icons, exact size (apps, places)
	TRY_DIR("/usr/share/icons/hicolors/", icon_size_str, "/apps/");
	TRY_DIR("/usr/share/icons/hicolors/", icon_size_str, "/places/");

	// shared icons, scalable (apps, places)
	TRY_DIR("/usr/share/icons/hicolors/scalable/apps/", "", "");
	TRY_DIR("/usr/share/icons/hicolors/scalable/places/", "", "");

	return NULL;
}
