#include "hash-mapping.h"

#include <stdlib.h>
#include <string.h>

static GHashTable * hash_mapping = NULL;

static const gchar * hash_mapping_obtain(const gchar * hash_code_string) {
	if (hash_mapping == NULL) {
		hash_mapping = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
		const gchar * string = getenv("APPINDICATOR_HASH_MAPPING");

		if (string != NULL) {
			int start = 0;
			gchar * key = NULL;

			for (int i = 0, length = strlen(string); i < length; i++) {
				if (string[i] == '=' && key == NULL) {
					key = g_strndup(&string[start], i - start);
					start = i + 1;
				} else if (string[i] == '/' && key != NULL) {
					gchar * value = g_strndup(&string[start], i - start);
					g_hash_table_replace(hash_mapping, key, value);
					key = NULL;
					start = i + 1;
				}
			}

			if (key != NULL) {
				gchar * value = g_strdup(&string[start]);
				g_hash_table_replace(hash_mapping, key, value);
			}
		}
	}

	const gchar * value = g_hash_table_lookup(hash_mapping, hash_code_string);
	if (value == NULL) {
		value = g_hash_table_lookup(hash_mapping, "*");
	}
	return value;
}

gchar * hash_mapping_apply(const gchar * name, guint hash_code) {
	gchar * hash_code_string = g_strdup_printf("%08x", hash_code);
	const gchar * suffix = hash_mapping_obtain(hash_code_string);

	gchar * result = NULL;
	if (suffix == NULL) {
		result = g_strdup_printf("%s-%s", name, hash_code_string);
	} else if (strlen(suffix) == 0) {
		result = g_strdup(name);
	} else {
		result = g_strdup_printf("%s-%s", name, suffix);
	}

	g_free(hash_code_string);
	return result;
}
