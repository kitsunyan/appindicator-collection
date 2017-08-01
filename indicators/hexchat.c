#include <common/common.h>
#include <common/app-indicator.h>

#define ICON_NAME_NORMAL "hexchat-tray"
#define ICON_NAME_NEW "hexchat-tray-new"

typedef struct _HexchatIconClass {
	GObjectClass __parent__;
} HexchatIconClass;

typedef struct _HexchatIcon {
	GObject __parent__;
	GdkPixbuf * normal_pixbuf;
	GdkPixbuf * current_pixbuf;
	gchar * tooltip_text;
	GtkMenu * menu;
	AppIndicator * indicator;
	guint active_loop;
} HexchatIcon;

enum {
	PROP_0,
	PROP_EMBEDDED
};

enum {
	ACTIVATE,
	POPUP_MENU,
	LAST_SIGNAL
};

static guint hexchat_icon_signals[LAST_SIGNAL] = {0, };

static void hexchat_icon_init(HexchatIcon * icon);
static void hexchat_icon_class_init(HexchatIconClass * klass);
static gboolean hexchat_icon_set_menu_loop(gpointer user_data);

G_DEFINE_TYPE(HexchatIcon, hexchat_icon, G_TYPE_OBJECT);

#define HEXCHAT_TYPE_ICON (hexchat_icon_get_type())
#define HEXCHAT_ICON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), HEXCHAT_TYPE_ICON, HexchatIcon))
#define HEXCHAT_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), HEXCHAT_TYPE_ICON, HexchatIconClass))
#define HEXCHAT_IS_ICON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), HEXCHAT_TYPE_ICON))
#define HEXCHAT_IS_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), HEXCHAT_TYPE_ICON))
#define HEXCHAT_ICON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), HEXCHAT_TYPE_ICON, HexchatIconClass))

static void hexchat_icon_init(HexchatIcon * icon) {
	icon->normal_pixbuf = NULL;
	icon->current_pixbuf = NULL;
	icon->tooltip_text = NULL;
	icon->menu = NULL;
	icon->indicator = NULL;
	icon->active_loop = 0;
}

static void hexchat_icon_finalize(GObject * object) {
	HexchatIcon * icon = HEXCHAT_ICON(object);
	g_object_unref(icon->normal_pixbuf);
	g_object_unref(icon->current_pixbuf);
	g_free(icon->tooltip_text);
	if (icon->menu != NULL) {
		g_object_unref(icon->menu);
	}
	if (icon->indicator != NULL) {
		g_object_unref(icon->indicator);
	}
	if (icon->active_loop != 0) {
		g_source_remove(icon->active_loop);
	}
	G_OBJECT_CLASS(hexchat_icon_parent_class)->finalize(object);
}

static void hexchat_icon_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	HexchatIcon * icon = HEXCHAT_ICON(object);
	switch (prop_id) {
		case PROP_EMBEDDED: {
			g_value_set_boolean(value, TRUE);
			break;
		}
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

static void hexchat_icon_class_init(HexchatIconClass * klass) {
	GObjectClass * object_class = G_OBJECT_CLASS(klass);
	object_class->get_property = hexchat_icon_get_property;
	object_class->finalize = hexchat_icon_finalize;

	g_object_class_install_property(object_class, PROP_EMBEDDED,
		g_param_spec_boolean("embedded", NULL, NULL, TRUE, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	hexchat_icon_signals[ACTIVATE] = g_signal_new(g_intern_static_string("activate"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 0);

	hexchat_icon_signals[POPUP_MENU] = g_signal_new(g_intern_static_string("popup-menu"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
}

static void hexchat_icon_activate(HexchatIcon * icon) {
	if (icon->active_loop != 0) {
		g_source_remove(icon->active_loop);
	}
	icon->active_loop = g_idle_add(hexchat_icon_set_menu_loop, icon);
}

static gboolean obtain_menu_mode = FALSE;
static GtkMenu * obtain_menu_result;

static void hexchat_icon_set_menu_foreach(GtkWidget * widget, gpointer user_data) {
	if (GTK_IS_MENU_ITEM(widget)) {
		GtkMenuItem * item = GTK_MENU_ITEM(widget);
		GtkWidget * submenu = gtk_menu_item_get_submenu (item);
		if (submenu != NULL) {
			gtk_container_foreach(GTK_CONTAINER(submenu),
				hexchat_icon_set_menu_foreach, user_data);
		} else {
			g_signal_connect_swapped(item, "activate",
				G_CALLBACK(hexchat_icon_activate), user_data);
		}
	}
}

static void hexchat_icon_set_menu(HexchatIcon * icon) {
	obtain_menu_result = NULL;
	obtain_menu_mode = TRUE;
	g_signal_emit(G_OBJECT(icon), hexchat_icon_signals[POPUP_MENU], 0, 0, 0);
	obtain_menu_mode = FALSE;
	GtkMenu * menu = obtain_menu_result;
	if (menu != NULL) {
		if (icon->menu != NULL) {
			g_object_unref(icon->menu);
		}
		icon->menu = menu;
		g_object_ref(menu);
		app_indicator_set_menu(icon->indicator, menu);
		GList * list = gtk_container_get_children(GTK_CONTAINER(menu));
		app_indicator_set_secondary_activate_target(icon->indicator, list != NULL ? list->data : NULL);
		g_list_free(list);
		gtk_container_foreach(GTK_CONTAINER(menu), hexchat_icon_set_menu_foreach, icon);
	}
}

static void hexchat_icon_update_icon_tooltip(HexchatIcon * icon) {
	const gchar * icon_name = icon->normal_pixbuf ==
		icon->current_pixbuf ? ICON_NAME_NORMAL : ICON_NAME_NEW;
	const gchar * text = icon->tooltip_text != NULL ? strstr(icon->tooltip_text, "HexChat: ") : NULL;
	text = text != NULL ? &text[9] : icon->tooltip_text;
	app_indicator_set_icon_full(icon->indicator, icon_name, text);
}

static gboolean hexchat_icon_set_menu_loop(gpointer user_data) {
	HexchatIcon * icon = user_data;
	hexchat_icon_set_menu(icon);
	icon->active_loop = g_timeout_add(10000, hexchat_icon_set_menu_loop, user_data);
	return G_SOURCE_REMOVE;
}

GtkStatusIcon * gtk_status_icon_new_from_pixbuf(GdkPixbuf * pixbuf) {
	HexchatIcon * icon = g_object_new(HEXCHAT_TYPE_ICON, NULL);
	icon->normal_pixbuf = pixbuf;
	icon->current_pixbuf = pixbuf;
	g_object_ref(pixbuf);
	g_object_ref(pixbuf);
	icon->indicator = app_indicator_new("hexchat", ICON_NAME_NORMAL,
		APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_title(icon->indicator, "HexChat");
	app_indicator_set_status(icon->indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_item_is_menu(icon->indicator, FALSE);
	icon->active_loop = g_idle_add(hexchat_icon_set_menu_loop, icon);
	return (gpointer) icon;
}

void gtk_status_icon_set_tooltip_text(GtkStatusIcon * status_icon, const gchar * text) {
	HexchatIcon * icon = HEXCHAT_ICON(status_icon);
	g_free(icon->tooltip_text);
	icon->tooltip_text = text != NULL ? g_strdup(text) : NULL;
	hexchat_icon_update_icon_tooltip(icon);
}

void gtk_status_icon_set_from_pixbuf(GtkStatusIcon * status_icon, GdkPixbuf * pixbuf) {
	HexchatIcon * icon = HEXCHAT_ICON(status_icon);
	g_object_unref(icon->current_pixbuf);
	icon->current_pixbuf = pixbuf;
	g_object_ref(pixbuf);
	hexchat_icon_update_icon_tooltip(icon);
}

GdkPixbuf * gtk_status_icon_get_pixbuf(GtkStatusIcon * status_icon) {
	HexchatIcon * icon = HEXCHAT_ICON(status_icon);
	return icon->current_pixbuf;
}

void gtk_menu_popup(GtkMenu * menu, GtkWidget * parent_menu_shell, GtkWidget * parent_menu_item,
	GtkMenuPositionFunc func, gpointer data, guint button, guint32 activate_time) {
	super_lookup_static(gtk_menu_popup, void,
		GtkMenu *, GtkWidget *, GtkWidget *, GtkMenuPositionFunc, gpointer, guint, guint32);
	if (obtain_menu_mode) {
		obtain_menu_result = menu;
	} else {
		gtk_menu_popup_super(menu, parent_menu_shell, parent_menu_item,
			func, data, button, activate_time);
	}
}

void * dlsym_override(const char * symbol) {
	dlsym_compare(gtk_status_icon_new_from_pixbuf);
	dlsym_compare(gtk_status_icon_set_tooltip_text);
	dlsym_compare(gtk_status_icon_set_from_pixbuf);
	dlsym_compare(gtk_status_icon_get_pixbuf);
	dlsym_compare(gtk_menu_popup);
	return NULL;
}
