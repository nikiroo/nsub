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

struct {
	char *name;
	char *icon;
	char *icon_file;
	char *exec;
	array *children;
	int id;
}typedef desktop_p;

/* PRIVATE FUNCTIONS */
int desktop_compare(const void *a, const void* b);
char *desktop_concat(const char str1[], ...);
int desktop_test_file(const char filename[]);
/* */

desktop *new_desktop(const char filename[], int best_size) {
	desktop_p *me = malloc(sizeof(desktop_p));
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

		me->children = new_array(sizeof(desktop_p*), 32);
		for (struct dirent *ep = readdir(dp); ep; ep = readdir(dp)) {
			if (!strcmp(ep->d_name, "."))
				continue;
			if (!strcmp(ep->d_name, ".."))
				continue;
			char *childname = desktop_concat(filename, "/", ep->d_name, NULL);
			desktop_p *child = (desktop_p*) new_desktop(childname, best_size);
			free(childname);
			if (child) {
				array_add(me->children, &child);
			}
		}

		array_qsort(me->children, desktop_compare);

		closedir(dp);
		free(ext);
		return (desktop*) me;
	}

	// Only process ".desktop" files
	if (!ext || strcmp(ext, EXT)) {
		free_desktop((desktop *) me);
		free(ext);
		return NULL;
	}

	FILE *file;
	array *tab;
	size_t n_lines;

	tab = new_array(sizeof(char *), 32);

	file = fopen(filename, "r");
	n_lines = array_readfiles(tab, file);
	fclose(file);

	char *line;
	char *startsWith;
	size_t n;
	for (size_t i = 0; i < n_lines; i++) {
		array_get(tab, &line, i);

		startsWith = "Name=";
		n = strlen(startsWith);
		if (!strncmp(line, startsWith, n)) {
			free(me->name);
			me->name = strdup(line + n);
		}

		startsWith = "Exec=";
		n = strlen(startsWith);
		if (!strncmp(line, startsWith, n)) {
			free(me->exec);
			me->exec = strdup(line + n);
			// TODO: %f %F %u %U %i %c %k: inject values instead
			char *cars = "ifFuUck";
			for (char *ptr = index(me->exec, '%'); ptr; ptr = index(ptr, '%')) {
				if (index(cars, ptr[1])) {
					ptr[0] = ' ';
					ptr[1] = ' ';
				}
				ptr++;
			}
		}

		startsWith = "Icon=";
		n = strlen(startsWith);
		if (!strncmp(line, startsWith, n)) {
			free(me->icon);
			me->icon = strdup(line + n);
		}
	}

	array_free_all(tab);

	// Find icon file linked to icon
	if (me->icon && !me->icon_file) {
		me->icon_file = desktop_find_icon(me->icon, best_size);
	}
	// ...or any we can find, actually
	if (!me->icon_file) {
		me->icon_file = desktop_find_icon(me->name, best_size);
	}

	free(ext);
	return (desktop *) me;
}

void free_desktop(desktop *app) {
	desktop_p *me = (desktop_p*) app;

	if (!me)
		return;

	free(me->name);
	free(me->exec);
	free(me->icon);
	free(me->icon_file);
	free_array(me->children);

	free(me);
}

const char *desktop_get_name(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->name;
}

const char *desktop_get_exec(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->exec;
}

const char *desktop_get_icon(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->icon;
}

const char *desktop_get_icon_file(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->icon_file;
}

int desktop_get_id(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->id;
}

void desktop_set_id(desktop *app, int id) {
	desktop_p *me = (desktop_p*) app;
	me->id = id;
}

array *desktop_get_children(desktop *app) {
	desktop_p *me = (desktop_p*) app;
	return me->children;
}

desktop *desktop_find_id(array *children, int id) {
	desktop *found = NULL;
	size_t n = 0;

	if (children)
		n = array_count(children);

	desktop_p *child;
	for (size_t i = 0; !found && i < n; i++) {
		array_get(children, &child, i);
		if (child->id == id) {
			found = (desktop*) child;
			break;
		}

		found = desktop_find_id(child->children, id);
	}

	return found;
}

/* PRIVATE FUNCTIONS */

char *desktop_concat(const char str1[], ...) {
	va_list args;
	size_t total;
	size_t prec;
	char *arg;
	char *ptr;
	char *rep;

	total = strlen(str1);
	va_start(args, str1);
	while ((arg = va_arg(args, char *))) {
		total += strlen(arg);
	}
	va_end(args);

	rep = malloc(total * sizeof(char) + 1);
	ptr = rep;
	ptr = strcpy(ptr, str1);
	prec = strlen(str1);

	va_start(args, str1);
	while ((arg = va_arg(args, char *))) {
		ptr = strcpy(ptr + prec, arg);
		prec = strlen(arg);
	}
	va_end(args);

	return rep;
}

int desktop_compare(const void *a, const void* b) {
	desktop_p *me1 = ((desktop_p**) a)[0];
	desktop_p *me2 = ((desktop_p**) b)[0];

	if (me1->children && !(me2->children))
		return -1;
	if (!(me1->children) && me2->children)
		return 1;

	return strcmp(me1->name, me2->name);
}

int desktop_test_file(const char filename[]) {
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
		tmp = desktop_concat(a, b, c, basename, ".png", NULL); \
		if(desktop_test_file(tmp)) \
			return tmp; \
		free(tmp); \
	} while(0)

char *theme = NULL;
char *ltheme = NULL;
char *desktop_find_icon(const char basename[], int icon_size) {
	char *tmp = NULL;
	char *home = getenv("HOME");
	char icon_size_str[100];
	sprintf(icon_size_str, "%dx%d", icon_size, icon_size);

	if (!theme) {
		array *tab = new_array(sizeof(char *), 32);
		size_t n_lines = 0;

		tmp = desktop_concat(home, "/.gtkrc-2.0", NULL);
		FILE *file = fopen(tmp, "r");
		free(tmp);
		if (file) {
			n_lines = array_readfiles(tab, file);
			fclose(file);
		}

		char *line;
		const char *startsWith = "gtk-icon-theme-name=";
		size_t n = strlen(startsWith);

		for (size_t i = 0; i < n_lines; i++) {
			array_get(tab, &line, i);
			if (!strncmp(line, startsWith, n)) {
				free(theme);
				if (line[n] == '"') {
					theme = strdup(line + n + 1);
					theme[strlen(theme) - 1] = '\0';
				} else {
					theme = strdup(line + n);
				}
			}
		}

		if (!theme || !theme[0]) {
			theme = strdup("");
			ltheme = strdup("");
		} else {
			tmp = theme;
			theme = desktop_concat("/usr/share/icons/", tmp, "/", NULL);
			ltheme = desktop_concat(home, "/", ".icons/", tmp, "/", NULL);
			free(tmp);
		}

		array_free_all(tab);
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
