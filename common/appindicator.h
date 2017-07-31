#ifndef __APPINDICATOR_FEATURES_H__
#define __APPINDICATOR_FEATURES_H__

#include <libappindicator/app-indicator.h>

void app_indicator_set_item_is_menu(AppIndicator * indicator, gboolean item_is_menu);
gboolean app_indicator_get_item_is_menu(AppIndicator * indicator);

#endif
