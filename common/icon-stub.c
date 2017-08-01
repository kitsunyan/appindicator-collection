#include "common.h"
#include "app-indicator.h"
#include "icon-stub.h"

typedef struct _IconStubClass {
	GObjectClass __parent__;
} IconStubClass;

typedef struct _IconStub {
	GObject __parent__;
	gpointer data;
	gboolean custom_toolip;
	GtkWidget * menu;
	GtkWidget * primary_item;
	AppIndicator * indicator;
	GtkTooltip * tooltip_stub;
	guint active_loop;
} IconStub;


enum {
	PROP_0,
	PROP_EMBEDDED,
	PROP_HAS_TOOLTIP
};

enum {
	ACTIVATE,
	POPUP_MENU,
	QUERY_TOOLTIP,
	SIZE_CHANGED,
	CONFIGURE_MENU,
	UPDATE_ICON_TOOLTIP,
	OBTAIN_TOOLTIP,
	LAST_SIGNAL
};

static guint icon_stub_signals[LAST_SIGNAL] = {0, };

static void icon_stub_init(IconStub * icon_stub);
static void icon_stub_class_init(IconStubClass * klass);
static gboolean icon_stub_set_menu_tooltip_loop(gpointer user_data);

G_DEFINE_TYPE(IconStub, icon_stub, G_TYPE_OBJECT);

static void icon_stub_init(IconStub * icon_stub) {
	icon_stub->data = NULL;
	icon_stub->custom_toolip = FALSE;
	icon_stub->menu = NULL;
	icon_stub->primary_item = NULL;
	icon_stub->indicator = NULL;
	icon_stub->tooltip_stub = g_object_new(GTK_TYPE_TOOLTIP, NULL);
	icon_stub->active_loop = 0;
}

static void icon_stub_finalize(GObject * object) {
	IconStub * icon_stub = ICON_STUB(object);
	if (icon_stub->menu != NULL) {
		g_object_unref(icon_stub->menu);
	}
	if (icon_stub->indicator != NULL) {
		g_object_unref(icon_stub->indicator);
	}
	g_object_unref(icon_stub->tooltip_stub);
	if (icon_stub->active_loop != 0) {
		g_source_remove(icon_stub->active_loop);
	}
	G_OBJECT_CLASS(icon_stub_parent_class)->finalize(object);
}

static void icon_stub_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
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

static void icon_stub_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	switch (prop_id) {
		case PROP_HAS_TOOLTIP: {
			break;
		}
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
		}
	}
}

static void icon_stub_class_init(IconStubClass * klass) {
	GObjectClass * object_class = G_OBJECT_CLASS(klass);
	object_class->get_property = icon_stub_get_property;
	object_class->set_property = icon_stub_set_property;
	object_class->finalize = icon_stub_finalize;

	g_object_class_install_property(object_class, PROP_EMBEDDED,
		g_param_spec_boolean("embedded", NULL, NULL, TRUE, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(object_class, PROP_HAS_TOOLTIP,
		g_param_spec_boolean("has-tooltip", NULL, NULL, TRUE, G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

	// Stub signals

	icon_stub_signals[ACTIVATE] = g_signal_new(g_intern_static_string("activate"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 0);

	icon_stub_signals[POPUP_MENU] = g_signal_new(g_intern_static_string("popup-menu"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);

	icon_stub_signals[QUERY_TOOLTIP] = g_signal_new(g_intern_static_string("query-tooltip"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_BOOLEAN, 4,
		G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, GTK_TYPE_TOOLTIP);

	icon_stub_signals[SIZE_CHANGED] = g_signal_new(g_intern_static_string("size-changed"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_BOOLEAN, 1, G_TYPE_INT);

	// Actual signals

	icon_stub_signals[CONFIGURE_MENU] = g_signal_new(g_intern_static_string("configure-menu"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 1, GTK_TYPE_WIDGET);

	icon_stub_signals[UPDATE_ICON_TOOLTIP] = g_signal_new(g_intern_static_string("update-icon-tooltip"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 0);

	icon_stub_signals[OBTAIN_TOOLTIP] = g_signal_new(g_intern_static_string("obtain-tooltip"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 1, GTK_TYPE_WIDGET);
}

static void stub_icon_update_icon_tooltip(IconStub * icon_stub) {
	g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[UPDATE_ICON_TOOLTIP], 0);
}

static void icon_stub_menu_activate(IconStub * icon_stub) {
	if (icon_stub->active_loop != 0) {
		g_source_remove(icon_stub->active_loop);
	}
	icon_stub->active_loop = g_idle_add(icon_stub_set_menu_tooltip_loop, icon_stub);
}

static gboolean obtain_mode = FALSE;
static GtkWidget * obtain_result;

static void icon_stub_set_menu_foreach(GtkWidget * widget, gpointer user_data) {
	if (GTK_IS_MENU_ITEM(widget)) {
		GtkWidget * submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(widget));
		if (submenu != NULL) {
			gtk_container_foreach(GTK_CONTAINER(submenu),
				icon_stub_set_menu_foreach, user_data);
		} else {
			const gchar * key = "app-indicator-activate-connected";
			gboolean connected = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), key));
			if (!connected) {
				g_signal_connect_swapped(widget, "activate",
					G_CALLBACK(icon_stub_menu_activate), user_data);
				g_object_set_data(G_OBJECT(widget), key, GINT_TO_POINTER(TRUE));
			}
		}
	}
}

static void icon_stub_set_menu_tooltip(IconStub * icon_stub) {
	obtain_result = NULL;
	obtain_mode = TRUE;
	g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[POPUP_MENU], 0, 0, 0);
	obtain_mode = FALSE;
	GtkWidget * widget = obtain_result;
	if (widget != NULL) {
		if (icon_stub->menu != NULL) {
			g_object_unref(icon_stub->menu);
		}
		icon_stub->menu = widget;
		g_object_ref(widget);
		g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[CONFIGURE_MENU], 0, widget);
		app_indicator_set_menu(icon_stub->indicator, GTK_MENU(widget));
		gtk_container_foreach(GTK_CONTAINER(widget), icon_stub_set_menu_foreach, icon_stub);
	}
	if (icon_stub->custom_toolip) {
		gboolean has_tooltip = FALSE;
		obtain_result = NULL;
		obtain_mode = TRUE;
		g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[QUERY_TOOLTIP], 0,
			0, 0, FALSE, icon_stub->tooltip_stub, &has_tooltip);
		obtain_mode = FALSE;
		g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[OBTAIN_TOOLTIP], 0, obtain_result);
	}
}

static void icon_stub_primary_item_unref(gpointer user_data, GObject * object) {
	IconStub * icon_stub = user_data;
	if (icon_stub->primary_item == GTK_WIDGET(object)) {
		icon_stub->primary_item = NULL;
	}
}

static void icon_stub_primary_activate(IconStub * icon_stub) {
	g_signal_emit(G_OBJECT(icon_stub), icon_stub_signals[ACTIVATE], 0);
}

void icon_stub_configure_menu_prepend_activate(IconStub * icon_stub, GtkWidget * menu) {
	GList * list = gtk_container_get_children(GTK_CONTAINER(menu));
	if (list != NULL) {
		GtkWidget * first_item = list->data;
		if (first_item != icon_stub->primary_item) {
			icon_stub->primary_item = gtk_menu_item_new();
			gtk_menu_shell_prepend(GTK_MENU_SHELL(menu), icon_stub->primary_item);
			gtk_widget_show(icon_stub->primary_item);
			g_signal_connect_swapped(icon_stub->primary_item, "activate",
				G_CALLBACK(icon_stub_primary_activate), icon_stub);
			g_object_weak_ref(G_OBJECT(icon_stub->primary_item),
				icon_stub_primary_item_unref, NULL);
			app_indicator_set_secondary_activate_target(icon_stub->indicator,
				icon_stub->primary_item);
		}
		g_list_free(list);
	}
}

void icon_stub_configure_menu_select_activate(IconStub * icon_stub, GtkWidget * menu_item) {
	app_indicator_set_secondary_activate_target(icon_stub->indicator, menu_item);
}

void icon_stub_configure_menu_select_activate_head(IconStub * icon_stub, GtkWidget * menu) {
	GList * list = gtk_container_get_children(GTK_CONTAINER(menu));
	icon_stub_configure_menu_select_activate(icon_stub, list != NULL ? list->data : NULL);
	g_list_free(list);
}

static gboolean icon_stub_set_menu_tooltip_loop(gpointer user_data) {
	IconStub * icon_stub = user_data;
	icon_stub_set_menu_tooltip(icon_stub);
	icon_stub->active_loop = g_timeout_add(10000, icon_stub_set_menu_tooltip_loop, user_data);
	return G_SOURCE_REMOVE;
}

IconStub * icon_stub_new(const gchar * id, const gchar * title, const gchar * default_icon,
	gboolean custom_toolip, gpointer data) {
	IconStub * icon_stub = g_object_new(ICON_STUB_TYPE, NULL);
	icon_stub->data = data;
	icon_stub->indicator = app_indicator_new(id, default_icon,
		APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_title(icon_stub->indicator, title);
	app_indicator_set_status(icon_stub->indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_item_is_menu(icon_stub->indicator, FALSE);
	icon_stub->custom_toolip = custom_toolip;
	icon_stub->active_loop = g_idle_add(icon_stub_set_menu_tooltip_loop, icon_stub);
	return icon_stub;
}

gpointer icon_stub_get_data(IconStub * icon_stub) {
	return icon_stub->data;
}

void icon_stub_set_icon_tooltip(IconStub * icon_stub, const gchar * name, const gchar * tooltip_text) {
	app_indicator_set_icon_full(icon_stub->indicator, name, tooltip_text);
}

void icon_stub_set_visible(IconStub * icon_stub, gboolean visible) {
	app_indicator_set_status(icon_stub->indicator,
		visible ? APP_INDICATOR_STATUS_ACTIVE : APP_INDICATOR_STATUS_PASSIVE);
}

void gtk_menu_popup(GtkMenu * menu, GtkWidget * parent_menu_shell, GtkWidget * parent_menu_item,
	GtkMenuPositionFunc func, gpointer data, guint button, guint32 activate_time) {
	super_lookup_static(gtk_menu_popup, void,
		GtkMenu *, GtkWidget *, GtkWidget *, GtkMenuPositionFunc, gpointer, guint, guint32);
	if (obtain_mode) {
		obtain_result = GTK_WIDGET(menu);
	} else {
		gtk_menu_popup_super(menu, parent_menu_shell, parent_menu_item,
			func, data, button, activate_time);
	}
}

void gtk_tooltip_set_custom(GtkTooltip * tooltip, GtkWidget * custom_widget) {
	super_lookup_static(gtk_tooltip_set_custom, void, GtkTooltip *, GtkWidget *);
	if (obtain_mode) {
		obtain_result = custom_widget;
	} else {
		gtk_tooltip_set_custom_super(tooltip, custom_widget);
	}
}

void * icon_stub_dlsym_override(const char * symbol) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	dlsym_compare(gtk_menu_popup);
G_GNUC_END_IGNORE_DEPRECATIONS
	dlsym_compare(gtk_tooltip_set_custom);
	return NULL;
}
