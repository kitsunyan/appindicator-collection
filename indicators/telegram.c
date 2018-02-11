#include <common/common.h>
#include <common/app-indicator.h>

AppIndicator * app_indicator_new(const gchar * id, const gchar * icon_name, AppIndicatorCategory category) {
	return app_indicator_new_extended(id, icon_name, category);
}

void app_indicator_set_icon_full(AppIndicator * self, const gchar * icon_name, const gchar * icon_desc) {
	super_lookup_static(app_indicator_set_icon_full, void,
		AppIndicator *, const gchar *, const gchar *);
	const gchar * title = app_indicator_get_title(self);
	if (title == NULL || !g_strcmp0(title, "telegram")) {
		app_indicator_set_title(self, "Telegram Desktop");
	}

	const gchar * icon_name_short = g_strrstr(icon_name, "/");
	if (icon_name_short != NULL) {
		icon_name_short = &icon_name_short[1];
	} else {
		icon_name_short = icon_name;
	}
	gboolean highlight = strstr(icon_name_short, "ico_") == icon_name_short;

	gint count = 0;
	gchar * count_str = g_strrstr(icon_name, "_");
	if (count_str != NULL) {
		guint64 count_full = g_ascii_strtoull(&count_str[1], NULL, 10);
		count = count_full > 1000 ? 1001 : (gint) count_full;
	}

	const gchar * new_icon_name = count > 0 ? highlight ? "telegram-tray-highlight"
		: "telegram-tray-new" : "telegram-tray";
	gchar * text = count > 0 ? count > 1000 ? g_strdup("Unread messages: >1000")
		: g_strdup_printf("Unread messages: %d", count) : g_strdup("No unread messages");
	app_indicator_set_icon_full_super(self, new_icon_name, text);
	g_free(text);
}

typedef struct {
	AppIndicator * indicator;
	GtkWidget * item_show;
	GtkWidget * item_hide;
} IndicatorStorage;

static void update_activation(IndicatorStorage * storage) {
	gboolean shown = gtk_widget_get_sensitive(storage->item_hide);
	debug("update activation %d", shown);
	app_indicator_set_secondary_activate_target(storage->indicator,
		shown ? storage->item_hide : storage->item_show);
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
		IndicatorStorage * storage = g_new0(IndicatorStorage, 1);
		storage->indicator = loop->indicator;
		storage->item_show = list->data;
		storage->item_hide = list->next->data;
		g_object_weak_ref(G_OBJECT(storage->indicator), (GWeakNotify) g_free, storage);
		g_list_free(list);

		g_object_weak_unref(G_OBJECT(loop->menu), (GWeakNotify) g_source_remove,
			GUINT_TO_POINTER(loop->handler));
		g_free(loop);

		g_signal_connect_swapped(storage->item_hide, "notify::sensitive",
			G_CALLBACK(update_activation), storage);
		update_activation(storage);

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
	dlsym_compare(app_indicator_new);
	dlsym_compare(app_indicator_set_icon_full);
	dlsym_compare(app_indicator_set_menu);
	return NULL;
}
