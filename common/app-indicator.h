#ifndef __APP_INDICATOR_FEATURES_H__
#define __APP_INDICATOR_FEATURES_H__

#include <libappindicator/app-indicator.h>

extern gboolean app_indicator_falling_back;

AppIndicator * app_indicator_new_extended(const gchar * id, const gchar * icon_name,
	AppIndicatorCategory category);

void app_indicator_set_item_is_menu(AppIndicator * indicator, gboolean item_is_menu);
gboolean app_indicator_get_item_is_menu(AppIndicator * indicator);

guint gdk_pixbuf_hash(GdkPixbuf * pixbuf);

void * app_indicator_dlsym_override(const char * symbol);

#endif
