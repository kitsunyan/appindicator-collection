#define DEBUG_NAME "TELEGRAM"

#include <common/common.h>
#include <libappindicator/app-indicator.h>

void app_indicator_set_icon_full(AppIndicator * self, const gchar * icon_name, const gchar * icon_desc) {
	super_lookup_static(app_indicator_set_icon_full, void,
		AppIndicator *, const gchar *, const gchar *);
	const gchar * title = app_indicator_get_title(self);
	if (title == NULL || !g_strcmp0(title, "telegram")) {
		app_indicator_set_title(self, "Telegram Desktop");
	}
	gboolean highlight = strstr(icon_name, "ico_") == icon_name;
	gchar * count_str = g_strrstr(icon_name, "_");
	if (count_str != NULL) {
		count_str++;
	}
	gint count = 0;
	if (count_str > 0) {
		guint64 count_full = g_ascii_strtoull(count_str, NULL, 10);
		count = count_full > 1000 ? 1001 : (gint) count_full;
	}
	const gchar * new_icon_name = count > 0 ? highlight ? "telegram-tray-highlight"
		: "telegram-tray-new" : "telegram-tray";
	gchar * text = count > 0 ? count > 1000 ? g_strdup("Unread messages: >1000")
		: g_strdup_printf("Unread messages: %d", count) : g_strdup("No unread messages");
	app_indicator_set_icon_full_super(self, new_icon_name, text);
	g_free(text);
}

void * dlsym_override(const char * symbol) {
	dlsym_compare(app_indicator_set_icon_full);
	return NULL;
}
