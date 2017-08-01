#include <common/common.h>
#include <common/app-indicator.h>

#define pass_args(...) __VA_ARGS__

#define def_override_void(function, args, call, source, extra) \
void function(args) { \
	super_lookup_static(function, void, args); \
	gchar * new_icon_name = create_icon_name(source, icon_name); \
	function##_super(call); \
	g_free(new_icon_name); \
	extra; \
}

#define def_override(function, result, args, call, source, extra) \
result function(args) { \
	super_lookup_static(function, result, args); \
	gchar * new_icon_name = create_icon_name(source, icon_name); \
	result res = function##_super(call); \
	g_free(new_icon_name); \
	extra; \
	return res; \
}

static gboolean menu_head_is_activate() {
	const gchar * menu_head_activate = getenv("ELECTRON_MENU_HEAD_ACTIVATE");
	return menu_head_activate != NULL && strlen(menu_head_activate) > 0;
}

static void apply_head_is_activate(AppIndicator * self) {
	if (menu_head_is_activate()) {
		app_indicator_set_item_is_menu(self, FALSE);
	}
}

static gchar * create_icon_name(AppIndicator * self,
	const gchar * id, const gchar * theme_path, const gchar * icon_name) {
	if (self != NULL) {
		const gchar * override_title = getenv("ELECTRON_APPINDICATOR_TITLE");
		debug("override title %s", override_title ? override_title : "NULL");
		if (override_title != NULL && strlen(override_title) > 0) {
			app_indicator_set_title(self, override_title);
		}
	}
	if (icon_name == NULL) {
		return NULL;
	}
	debug("create icon %s", icon_name);
	if (theme_path == NULL || theme_path[strlen(theme_path)]) {
		return g_strdup(icon_name);
	}
	debug("theme path %s", theme_path);
	gchar * icon_path = g_strdup_printf("%s/%s.png", theme_path, icon_name);
	debug("icon path %s", icon_name);
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(icon_path, NULL);
	debug("pixbuf %ld", (gsize) pixbuf);
	g_free(icon_path);
	if (pixbuf == NULL) {
		return g_strdup(icon_name);
	}
	guint length = 0;
	guchar * pixels = gdk_pixbuf_get_pixels_with_length(pixbuf, &length);
	guint prime = 31;
	guint hash_code = 0;
	for (guint i = 0; i < length; i++) {
		hash_code = prime * hash_code + pixels[i];
	}
	debug("hash code %08x", hash_code);
	g_object_unref(pixbuf);
	gchar * new_id = g_strdup(id);
	for (gchar * c = new_id; *c != '\0'; c++) {
		*c = tolower(*c);
		if (*c == ' ' || *c == '_') {
			*c = '-';
		}
		if (*c >= '0' && *c <= '9') {
			for (gchar *r = c; *r != '\0'; r++) {
				*r = *(r + 1);
			}
		}
	}
	debug("new id %s", new_id);
	gchar * result = g_strdup_printf("%s-%08x", new_id, hash_code);
	debug("result %s", result);
	g_free(new_id);
	return result;
}

def_override(app_indicator_new, AppIndicator *,
	pass_args(const gchar *id, const gchar *icon_name, AppIndicatorCategory category),
	pass_args(id, new_icon_name, category),
	pass_args(NULL, id, NULL), apply_head_is_activate(res));

def_override(app_indicator_new_with_path, AppIndicator *,
	pass_args(const gchar *id, const gchar *icon_name, AppIndicatorCategory category, const gchar *icon_theme_path),
	pass_args(id, new_icon_name, category, icon_theme_path),
	pass_args(NULL, id, icon_theme_path), apply_head_is_activate(res));

def_override_void(app_indicator_set_icon,
	pass_args(AppIndicator * self, const gchar * icon_name),
	pass_args(self, new_icon_name),
	pass_args(self, app_indicator_get_id (self), app_indicator_get_icon_theme_path(self)), {});

def_override_void(app_indicator_set_icon_full,
	pass_args(AppIndicator * self, const gchar * icon_name, const gchar * icon_desc),
	pass_args(self, new_icon_name, NULL),
	pass_args(self, app_indicator_get_id (self), app_indicator_get_icon_theme_path(self)), {});

def_override_void(app_indicator_set_attention_icon,
	pass_args(AppIndicator * self, const gchar * icon_name),
	pass_args(self, new_icon_name),
	pass_args(self, app_indicator_get_id (self), app_indicator_get_icon_theme_path(self)), {});

def_override_void(app_indicator_set_attention_icon_full,
	pass_args(AppIndicator * self, const gchar * icon_name, const gchar * icon_desc),
	pass_args(self, new_icon_name, NULL),
	pass_args(self, app_indicator_get_id (self), app_indicator_get_icon_theme_path(self)), {});

void app_indicator_set_menu(AppIndicator * self, GtkMenu * menu) {
	super_lookup_static(app_indicator_set_menu, void, AppIndicator *, GtkMenu *);
	app_indicator_set_menu_super(self, menu);
	if (menu != NULL && menu_head_is_activate()) {
		GList * list = gtk_container_get_children(GTK_CONTAINER(menu));
		if (list != NULL) {
			app_indicator_set_secondary_activate_target(self, list->data);
			g_list_free(list);
		}
	}
}

void * dlsym_override(const char * symbol) {
	dlsym_override_library(app_indicator);
	dlsym_compare(app_indicator_new);
	dlsym_compare(app_indicator_new_with_path);
	dlsym_compare(app_indicator_set_icon);
	dlsym_compare(app_indicator_set_icon_full);
	dlsym_compare(app_indicator_set_attention_icon);
	dlsym_compare(app_indicator_set_attention_icon);
	dlsym_compare(app_indicator_set_menu);
	return NULL;
}
