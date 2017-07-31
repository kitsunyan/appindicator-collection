#ifndef __COMMON_H__
#define __COMMON_H__

#define _GNU_SOURCE 1

#include <ctype.h>
#include <config.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int preload_debug;
extern int local_dlsym;

#define debug(format, ...) { \
	if (preload_debug == 2) { \
		preload_debug = getenv("APPINDICATOR_DEBUG") != NULL ? 1 : 0; \
	} \
	if (preload_debug == 1) { \
		printf("APPINDICATOR: " format "\n", __VA_ARGS__); \
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

void * dlsym_override_private(const char * symbol);
void * dlsym_override(const char * symbol);

#endif
