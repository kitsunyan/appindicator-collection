#include "common.h"

int preload_debug = 2;
int local_dlsym = 0;

void * _dl_sym(void *, const char *, void *);

void * dlsym(void * handle, const char * symbol) {
	static void * (* dlsym_super)(void *, const char *) = NULL;
	if (dlsym_super == NULL) {
		dlsym_super = _dl_sym(RTLD_NEXT, "dlsym", dlsym);
	}
	if (symbol != NULL && !strcmp(symbol, "dlsym")) {
		return (void *) dlsym;
	}
	void * result = symbol != NULL ? dlsym_override(symbol) : NULL;
	return result != NULL ? result : dlsym_super(handle, symbol);
}
