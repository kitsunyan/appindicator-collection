#include <common/common.h>
#include <common/app-indicator.h>
#include <common/icon-stub.h>

AppIndicator * app_indicator_new(const gchar * id, const gchar * icon_name, AppIndicatorCategory category) {
	return app_indicator_new_extended(id, icon_name, category);
}

AppIndicator * app_indicator_new_with_path(const gchar * id, const gchar * icon_name, AppIndicatorCategory category,
	const gchar * icon_theme_path) {
	AppIndicator * indicator = app_indicator_new_extended(id, icon_name, category);
	app_indicator_set_icon_theme_path(indicator, icon_theme_path);
	return indicator;
}

typedef struct {
	AppIndicator * indicator;
	GtkMenu * menu;
	guint handler;
} MenuLoop;

static gboolean setup_activation(gpointer user_data) {
	MenuLoop * loop = user_data;
	GList * list = gtk_container_get_children(GTK_CONTAINER(loop->menu));

	if (list != NULL) {
		app_indicator_set_secondary_activate_target(loop->indicator, list->data);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
}

void app_indicator_set_menu(AppIndicator * self, GtkMenu * menu) {
	super_lookup_static(app_indicator_set_menu, void, AppIndicator *, GtkMenu *);
	app_indicator_set_menu_super(self, menu);

	MenuLoop * loop = g_new0(MenuLoop, 1);
	loop->indicator = self;
	loop->menu = menu;
	loop->handler = g_idle_add(setup_activation, loop);
	g_object_weak_ref(G_OBJECT(menu), (GWeakNotify) g_source_remove,
		GUINT_TO_POINTER(loop->handler));

	app_indicator_set_item_is_menu(self, FALSE);
}

void * dlsym_override(const char * symbol) {
	dlsym_override_library(app_indicator);
	dlsym_compare(app_indicator_set_menu);
	return NULL;
}
