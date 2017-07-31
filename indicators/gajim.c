#define DEBUG_NAME "GAJIM"

#include <common/common.h>
#include <common/appindicator.h>

#define ICON_PREFIX "gajim-tray-"

static GList * status_pixbufs = NULL;

typedef struct {
	GdkPixbuf * pixbuf;
	gchar * status;
} StatusPixbuf;

static const gchar * lookup_status(GdkPixbuf * pixbuf) {
	for (GList * li = status_pixbufs; li != NULL; li = li->next) {
		StatusPixbuf * status_pixbuf = li->data;
		if (status_pixbuf->pixbuf == pixbuf) {
			return status_pixbuf->status;
		}
	}
	return NULL;
}

typedef struct _GajimIconClass {
	GObjectClass __parent__;
} GajimIconClass;

typedef struct _GajimIcon {
	GObject __parent__;
	gchar * status;
	gchar * tooltip_text;
	gboolean highlight;
	GtkMenu * menu;
	GtkWidget * primary_item;
	AppIndicator * indicator;
	GtkTooltip * tooltip_stub;
	guint active_loop;
} GajimIcon;

enum {
	PROP_0,
	PROP_HAS_TOOLTIP
};

enum {
	ACTIVATE,
	POPUP_MENU,
	QUERY_TOOLTIP,
	SIZE_CHANGED,
	LAST_SIGNAL
};

static guint gajim_icon_signals[LAST_SIGNAL] = {0, };

static void gajim_icon_init(GajimIcon * icon);
static void gajim_icon_class_init(GajimIconClass * klass);
static gboolean gajim_icon_set_menu_tooltip_loop(gpointer user_data);

G_DEFINE_TYPE(GajimIcon, gajim_icon, G_TYPE_OBJECT);

#define GAJIM_TYPE_ICON (gajim_icon_get_type())
#define GAJIM_ICON(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAJIM_TYPE_ICON, GajimIcon))
#define GAJIM_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAJIM_TYPE_ICON, GajimIconClass))
#define GAJIM_IS_ICON(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAJIM_TYPE_ICON))
#define GAJIM_IS_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAJIM_TYPE_ICON))
#define GAJIM_ICON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GAJIM_TYPE_ICON, GajimIconClass))

static void gajim_icon_init(GajimIcon * icon) {
	icon->status = NULL;
	icon->tooltip_text = NULL;
	icon->highlight = FALSE;
	icon->menu = NULL;
	icon->primary_item = NULL;
	icon->indicator = NULL;
	icon->tooltip_stub = g_object_new(GTK_TYPE_TOOLTIP, NULL);
	icon->active_loop = 0;
}

static void gajim_icon_finalize(GObject * object) {
	GajimIcon * icon = GAJIM_ICON(object);
	g_free(icon->status);
	g_free(icon->tooltip_text);
	if (icon->menu != NULL) {
		g_object_unref(icon->menu);
	}
	if (icon->indicator != NULL) {
		g_object_unref(icon->indicator);
	}
	g_object_unref(icon->tooltip_stub);
	if (icon->active_loop != 0) {
		g_source_remove(icon->active_loop);
	}
	G_OBJECT_CLASS(gajim_icon_parent_class)->finalize(object);
}

static void gajim_icon_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GajimIcon * icon = GAJIM_ICON(object);
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

static void gajim_icon_class_init(GajimIconClass * klass) {
	GObjectClass * object_class = G_OBJECT_CLASS(klass);
	object_class->set_property = gajim_icon_set_property;
	object_class->finalize = gajim_icon_finalize;

	g_object_class_install_property(object_class, PROP_HAS_TOOLTIP,
		g_param_spec_boolean("has-tooltip", NULL, NULL, TRUE, G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

	gajim_icon_signals[ACTIVATE] = g_signal_new(g_intern_static_string("activate"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 0);

	gajim_icon_signals[POPUP_MENU] = g_signal_new(g_intern_static_string("popup-menu"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);

	gajim_icon_signals[QUERY_TOOLTIP] = g_signal_new(g_intern_static_string("query-tooltip"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 4,
		G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, GTK_TYPE_TOOLTIP);

	gajim_icon_signals[SIZE_CHANGED] = g_signal_new(g_intern_static_string("size-changed"),
		G_TYPE_FROM_CLASS(object_class), G_SIGNAL_RUN_LAST, 0, NULL, NULL,
		g_cclosure_marshal_generic, G_TYPE_NONE, 1, G_TYPE_INT);
}

static void gajim_icon_update_icon_tooltip(GajimIcon * icon) {
	gchar * name = g_strdup_printf(ICON_PREFIX "%s%s",
		icon->status, icon->highlight ? "-highlight" : "");
	app_indicator_set_icon_full(icon->indicator, name, icon->tooltip_text);
	g_free(name);
}

static void gajim_icon_primary_activate(GajimIcon * icon) {
	g_signal_emit(G_OBJECT(icon), gajim_icon_signals[ACTIVATE], 0);
}

static void gajim_icon_primary_unref(gpointer user_data, GObject * object) {
	GajimIcon * icon = user_data;
	if (icon->primary_item == GTK_WIDGET(object)) {
		icon->primary_item = NULL;
	}
}

static void gajim_icon_activate(GajimIcon * icon) {
	if (icon->active_loop != 0) {
		g_source_remove(icon->active_loop);
	}
	icon->active_loop = g_idle_add(gajim_icon_set_menu_tooltip_loop, icon);
}

static gboolean obtain_mode = FALSE;
static GtkMenu * obtain_menu_result;
static GtkWidget * obtain_widget_result;

static void gajim_icon_set_menu_foreach(GtkWidget * widget, gpointer user_data) {
	if (GTK_IS_MENU_ITEM(widget)) {
		GtkMenuItem * item = GTK_MENU_ITEM(widget);
		GtkWidget * submenu = gtk_menu_item_get_submenu (item);
		if (submenu != NULL) {
			gtk_container_foreach(GTK_CONTAINER(submenu),
				gajim_icon_set_menu_foreach, user_data);
		} else {
			const gchar * key = "appindicator-activate-connected";
			gboolean connected = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item), key));
			if (!connected) {
				g_signal_connect_swapped(item, "activate",
					G_CALLBACK(gajim_icon_activate), user_data);
				g_object_set_data(G_OBJECT(item), key, GINT_TO_POINTER(TRUE));
			}
		}
	}
}

typedef struct {
	gchar * text;
	const gchar * label;
} TooltipForeach;

static gchar * gajim_icon_set_tooltip_append(gchar * current, const gchar * account, const gchar * status) {
	if (current == NULL) {
		return status != NULL && strlen(status) > 0
			? g_strdup_printf("%s: %s", account, status) : g_strdup(account);
	} else {
		gchar * result = status != NULL && strlen(status) > 0
			? g_strdup_printf("%s\n%s: %s", current, account, status) : g_strdup_printf("%s\n%s", current, account);
		g_free(current);
		return result;
	}
}

static void gajim_icon_set_tooltip_foreach(GtkWidget * widget, gpointer user_data) {
	TooltipForeach * tooltip_foreach = user_data;
	if (GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), gajim_icon_set_tooltip_foreach, user_data);
	} else if (GTK_IS_LABEL(widget)) {
		tooltip_foreach->label = gtk_label_get_text(GTK_LABEL(widget));
	} else if (GTK_IS_IMAGE(widget) && tooltip_foreach->label != NULL) {
		GdkPixbuf * pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
		if (pixbuf != NULL) {
			const gchar * status = pixbuf != NULL ? lookup_status(pixbuf) : NULL;
			if (tooltip_foreach->label != NULL && strlen(tooltip_foreach->label) > 0) {
				gchar * line = status != NULL && strlen(status) > 0
					? g_strdup_printf("%s: %s", tooltip_foreach->label, status)
					: g_strdup(tooltip_foreach->label);
				if (tooltip_foreach->text != NULL) {
					gchar * text = g_strdup_printf("%s\n%s", line, tooltip_foreach->text);
					g_free(tooltip_foreach->text);
					tooltip_foreach->text = text;
					g_free(line);
				} else {
					tooltip_foreach->text = line;
				}
			}
			tooltip_foreach->label = NULL;
		}
	}
}

static void gajim_icon_set_menu_tooltip(GajimIcon * icon) {
	obtain_menu_result = NULL;
	obtain_mode = TRUE;
	g_signal_emit(G_OBJECT(icon), gajim_icon_signals[POPUP_MENU], 0, 0, 0);
	obtain_mode = FALSE;
	GtkMenu * menu = obtain_menu_result;
	if (menu != NULL) {
		if (icon->menu != NULL) {
			g_object_unref(icon->menu);
		}
		icon->menu = menu;
		g_object_ref(menu);
		GList * list = gtk_container_get_children(GTK_CONTAINER(menu));
		if (list != NULL) {
			GtkWidget * first_item = list->data;
			if (first_item != icon->primary_item) {
				icon->primary_item = gtk_menu_item_new();
				gtk_menu_shell_prepend(GTK_MENU_SHELL(menu), icon->primary_item);
				gtk_widget_show(icon->primary_item);
				g_signal_connect_swapped(icon->primary_item, "activate",
					G_CALLBACK(gajim_icon_primary_activate), icon);
				g_object_weak_ref(G_OBJECT(icon->primary_item),
					gajim_icon_primary_unref, NULL);
				app_indicator_set_secondary_activate_target(icon->indicator, icon->primary_item);
			}
			g_list_free(list);
		}
		app_indicator_set_menu(icon->indicator, menu);
		gtk_container_foreach(GTK_CONTAINER(menu), gajim_icon_set_menu_foreach, icon);
	}
	obtain_widget_result = NULL;
	obtain_mode = TRUE;
	g_signal_emit(G_OBJECT(icon), gajim_icon_signals[QUERY_TOOLTIP], 0, 0, 0, FALSE, icon->tooltip_stub);
	obtain_mode = FALSE;
	GtkWidget * custom_widget = obtain_widget_result;
	gchar * new_tooltip_text = NULL;
	if (custom_widget != NULL) {
		debug("obtain custom widget %ld", (gsize) custom_widget);
		TooltipForeach tooltip_foreach = {NULL, };
		gtk_container_foreach(GTK_CONTAINER(custom_widget), gajim_icon_set_tooltip_foreach, &tooltip_foreach);
		new_tooltip_text = tooltip_foreach.text;
	}
	if (g_strcmp0(icon->tooltip_text, new_tooltip_text)) {
		g_free(icon->tooltip_text);
		icon->tooltip_text = new_tooltip_text;
		gajim_icon_update_icon_tooltip(icon);
	} else {
		g_free(new_tooltip_text);
	}
}

static gboolean gajim_icon_set_menu_tooltip_loop(gpointer user_data) {
	GajimIcon * icon = user_data;
	gajim_icon_set_menu_tooltip(icon);
	icon->active_loop = g_timeout_add(10000, gajim_icon_set_menu_tooltip_loop, user_data);
	return G_SOURCE_REMOVE;
}

gpointer g_object_newv(GType object_type, guint n_parameters, GParameter * parameters) {
	if (object_type == GTK_TYPE_STATUS_ICON) {
		GajimIcon * icon = g_object_new(GAJIM_TYPE_ICON, NULL);
		icon->indicator = app_indicator_new("gajim", ICON_PREFIX "offline",
			APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
		app_indicator_set_title(icon->indicator, "Gajim");
		app_indicator_set_status(icon->indicator, APP_INDICATOR_STATUS_ACTIVE);
		app_indicator_set_item_is_menu(icon->indicator, FALSE);
		icon->active_loop = g_idle_add(gajim_icon_set_menu_tooltip_loop, icon);
		return (gpointer) icon;
	} else {
		super_lookup_static(g_object_newv, gpointer, GType, guint, GParameter *);
		return g_object_newv_super(object_type, n_parameters, parameters);
	}
}

void gtk_status_icon_set_visible(GtkStatusIcon * status_icon, gboolean visible) {
	GajimIcon * icon = GAJIM_ICON(status_icon);
	app_indicator_set_status(icon->indicator,
		visible ? APP_INDICATOR_STATUS_ACTIVE : APP_INDICATOR_STATUS_PASSIVE);
}

void gtk_status_icon_set_blinking(GtkStatusIcon * status_icon, gboolean blinking) {
	GajimIcon * icon = GAJIM_ICON(status_icon);
	if (icon->highlight != blinking) {
		icon->highlight = blinking;
		gajim_icon_update_icon_tooltip(icon);
	}
}

void gtk_status_icon_set_from_pixbuf(GtkStatusIcon * status_icon, GdkPixbuf * pixbuf) {
	const gchar * status = lookup_status(pixbuf);
	if (status == NULL) {
		status = "offline";
	}
	debug("set pixbuf %ld %s", (gsize) pixbuf, status);
	GajimIcon * icon = GAJIM_ICON(status_icon);
	g_free(icon->status);
	icon->status = g_strdup(status);
	gajim_icon_update_icon_tooltip(icon);
}

void gtk_menu_popup(GtkMenu * menu, GtkWidget * parent_menu_shell, GtkWidget * parent_menu_item,
	GtkMenuPositionFunc func, gpointer data, guint button, guint32 activate_time) {
	super_lookup_static(gtk_menu_popup, void,
		GtkMenu *, GtkWidget *, GtkWidget *, GtkMenuPositionFunc, gpointer, guint, guint32);
	if (obtain_mode) {
		obtain_menu_result = menu;
	} else {
		gtk_menu_popup_super(menu, parent_menu_shell, parent_menu_item,
			func, data, button, activate_time);
	}
}

void gtk_tooltip_set_custom(GtkTooltip * tooltip, GtkWidget * custom_widget) {
	super_lookup_static(gtk_tooltip_set_custom, void, GtkTooltip *, GtkWidget *);
	if (obtain_mode) {
		obtain_widget_result = custom_widget;
	} else {
		gtk_tooltip_set_custom_super(tooltip, custom_widget);
	}
}

static void pixbuf_unref(gpointer user_data, GObject * object) {
	GdkPixbuf * pixbuf = GDK_PIXBUF(object);
	for (GList * li = status_pixbufs; li != NULL; li = li->next) {
		StatusPixbuf * status_pixbuf = li->data;
		if (status_pixbuf->pixbuf == pixbuf) {
			debug("remove status %s", status_pixbuf->status);
			g_free(status_pixbuf->status);
			g_free(status_pixbuf);
			status_pixbufs = g_list_delete_link(status_pixbufs, li);
			break;
		}
	}
}

void gtk_image_set_from_file(GtkImage * image, const gchar * filename) {
	super_lookup_static(gtk_image_set_from_file, void, GtkImage *, const gchar *);
	gtk_image_set_from_file_super(image, filename);
	if (filename != NULL && strstr(filename, "/16x16/") != NULL) {
		GdkPixbuf * pixbuf = gtk_image_get_pixbuf(image);
		if (pixbuf != NULL) {
			const gchar * end = &g_strrstr(filename, "/")[1];
			const gchar * dot = g_strrstr(end, ".");
			gchar * name = dot != NULL ? g_strndup(end, (gsize) (dot - end)) : g_strdup(end);
			for (gchar * c = name; *c != '\0'; c++) {
				if (*c == '_') {
					*c = '-';
				}
			}
			StatusPixbuf * status_pixbuf = g_new0(StatusPixbuf, 1);
			status_pixbuf->pixbuf = pixbuf;
			status_pixbuf->status = name;
			status_pixbufs = g_list_prepend(status_pixbufs, status_pixbuf);
			debug("add status %ld %s", (gsize) pixbuf, status_pixbuf->status);
			g_object_weak_ref(G_OBJECT(pixbuf), pixbuf_unref, NULL);
		}
	}
}

void * dlsym_override(const char * symbol) {
	dlsym_compare(g_object_newv);
	dlsym_compare(gtk_status_icon_set_visible);
	dlsym_compare(gtk_status_icon_set_blinking);
	dlsym_compare(gtk_status_icon_set_from_pixbuf);
	dlsym_compare(gtk_menu_popup);
	dlsym_compare(gtk_tooltip_set_custom);
	dlsym_compare(gtk_image_set_from_file);
	return NULL;
}
