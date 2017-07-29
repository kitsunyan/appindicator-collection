#ifndef __COMMON_H__
#define __COMMON_H__

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dlfcn.h>

extern int preload_debug;
extern int local_dlsym;

#define debug(format, ...) { \
	if (preload_debug == 2) { \
		preload_debug = getenv(DEBUG_NAME "_APPINDICATOR_DEBUG") != NULL ? 1 : 0; \
	} \
	if (preload_debug == 1) { \
		printf(DEBUG_NAME " APPINDICATOR: " format "\n", __VA_ARGS__); \
	} \
}

#define super_lookup_static(name, result, ...) \
static result (* name##_super)(__VA_ARGS__) = NULL; \
if (name##_super == NULL) { \
	local_dlsym = 1; \
	name##_super = dlsym(RTLD_NEXT, #name); \
	local_dlsym = 0; \
}

#define dlsym_compare(function) { \
	if (!local_dlsym && !strcmp(symbol, #function)) { \
		debug("override %s", symbol); \
		return (void *) function; \
	} \
}

void * dlsym_override(const char * symbol);

#endif
