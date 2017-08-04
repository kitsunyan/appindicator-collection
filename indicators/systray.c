#include <common/common.h>
#include <common/app-indicator.h>
#include <common/icon-stub.h>

typedef struct {
	const gchar * name;
	gchar * tooltip_text;
	gboolean has_tooltip;
	gboolean visible;
	gboolean blinking;
	gchar * title;
	GtkImageType image_type;
	gchar * image_string;
	GdkPixbuf * image_pixbuf;
	GIcon * image_gicon;
	guint image_hash;
} SystrayData;

static void systray_data_free_icon(SystrayData * data) {
	data->image_type = GTK_IMAGE_EMPTY;
	g_free(data->image_string);
	data->image_string = NULL;
	if (data->image_pixbuf != NULL) {
		g_object_unref(data->image_pixbuf);
		data->image_pixbuf = NULL;
	}
	if (data->image_gicon != NULL) {
		g_object_unref(data->image_gicon);
		data->image_gicon = NULL;
	}
	data->image_hash = 0;
}

static void systray_data_free(SystrayData * data) {
	g_free(data->tooltip_text);
	g_free(data->title);
	systray_data_free_icon(data);
	g_free(data);
}

static void update_icon_tooltip(IconStub * icon_stub) {
	SystrayData * data = icon_stub_get_data(icon_stub);

	gchar * name = NULL;
	if (data->image_type == GTK_IMAGE_STOCK) {
		name = g_strdup_printf("%s-%s", data->name, data->image_string);
	} else if (data->image_string != NULL) {
		name = g_strdup(data->image_string);
	} else if (data->image_hash != 0) {
		name = g_strdup_printf("%s-%08x", data->name, data->image_hash);
	} else {
		name = g_strdup(data->name);
	}

	icon_stub_set_icon_tooltip(icon_stub, name, data->tooltip_text);
}

static void obtain_tooltip(IconStub * icon_stub, GtkWidget * widget, const gchar * tooltip_text) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_tooltip_text((gpointer) icon_stub, tooltip_text);
G_GNUC_END_IGNORE_DEPRECATIONS
}

GtkStatusIcon * gtk_status_icon_new() {
	const gchar * title = getenv("SYSTRAY_APPINDICATOR_TITLE");
	if (title == NULL || strlen(title) == 0) {
		title = "System Tray Icon";
	}

	SystrayData * data = g_new0(SystrayData, 1);
	data->name = g_get_prgname();
	data->visible = TRUE;
	data->image_type = GTK_IMAGE_EMPTY;

	IconStub * icon_stub = icon_stub_new(data->name, title, "", data);
	g_object_weak_ref(G_OBJECT(icon_stub), (GWeakNotify) systray_data_free, data);

	g_signal_connect(icon_stub, "configure-menu",
		G_CALLBACK(icon_stub_configure_menu_prepend_activate), NULL);
	g_signal_connect(icon_stub, "update-icon-tooltip",
		G_CALLBACK(update_icon_tooltip), NULL);
	g_signal_connect(icon_stub, "obtain-tooltip",
		G_CALLBACK(obtain_tooltip), NULL);

	return (gpointer) icon_stub;
}

GtkStatusIcon * gtk_status_icon_new_from_pixbuf(GdkPixbuf * pixbuf) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	GtkStatusIcon * status_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_pixbuf(status_icon, pixbuf);
	return status_icon;
G_GNUC_END_IGNORE_DEPRECATIONS
}

GtkStatusIcon * gtk_status_icon_new_from_file(const gchar * filename) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	GtkStatusIcon * status_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_file(gtk_status_icon_new(), filename);
	return status_icon;
G_GNUC_END_IGNORE_DEPRECATIONS
}

GtkStatusIcon * gtk_status_icon_new_from_stock(const gchar * stock_id) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	GtkStatusIcon * status_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_stock(gtk_status_icon_new(), stock_id);
	return status_icon;
G_GNUC_END_IGNORE_DEPRECATIONS
}

GtkStatusIcon * gtk_status_icon_new_from_icon_name(const gchar * icon_name) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	GtkStatusIcon * status_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_icon_name(gtk_status_icon_new(), icon_name);
	return status_icon;
G_GNUC_END_IGNORE_DEPRECATIONS
}

GtkStatusIcon * gtk_status_icon_new_from_gicon(GIcon * icon) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	GtkStatusIcon * status_icon = gtk_status_icon_new();
	gtk_status_icon_set_from_gicon(gtk_status_icon_new(), icon);
	return status_icon;
G_GNUC_END_IGNORE_DEPRECATIONS
}

void gtk_status_icon_set_from_pixbuf(GtkStatusIcon * status_icon, GdkPixbuf * pixbuf) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	systray_data_free_icon(data);
	if (pixbuf != NULL) {
		data->image_pixbuf = g_object_ref(pixbuf);
		data->image_type = GTK_IMAGE_PIXBUF;
		data->image_hash = gdk_pixbuf_hash(pixbuf);
	}

	update_icon_tooltip(icon_stub);
}

void gtk_status_icon_set_from_file(GtkStatusIcon * status_icon, const gchar * filename) {
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_from_pixbuf(status_icon, pixbuf);
G_GNUC_END_IGNORE_DEPRECATIONS
	if (pixbuf != NULL) {
		g_object_unref(pixbuf);
	}
}

void gtk_status_icon_set_from_stock(GtkStatusIcon * status_icon, const gchar * stock_id) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	systray_data_free_icon(data);
	if (stock_id != NULL) {
		data->image_string = g_strdup(stock_id);
		data->image_type = GTK_IMAGE_STOCK;
	}

	update_icon_tooltip(icon_stub);
}

void gtk_status_icon_set_from_icon_name(GtkStatusIcon * status_icon, const gchar * icon_name) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	systray_data_free_icon(data);
	if (icon_name != NULL) {
		data->image_string = g_strdup(icon_name);
		data->image_type = GTK_IMAGE_ICON_NAME;
	}

	update_icon_tooltip(icon_stub);
}

void gtk_status_icon_set_from_gicon(GtkStatusIcon * status_icon, GIcon * icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	systray_data_free_icon(data);
	if (icon != NULL) {
		data->image_gicon = g_object_ref(icon);
		data->image_type = GTK_IMAGE_GICON;

		if (G_IS_THEMED_ICON(icon)) {
			GThemedIcon * themed_icon = G_THEMED_ICON(icon);
			const gchar * const * names = g_themed_icon_get_names(themed_icon);
			if (names != NULL && names[0] != NULL) {
				data->image_string = g_strdup(names[0]);
			}
		} else {
			data->image_hash = g_icon_hash(icon);
		}
	}

	update_icon_tooltip(icon_stub);
}

GtkImageType gtk_status_icon_get_storage_type(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->image_type;
}

GdkPixbuf * gtk_status_icon_get_pixbuf(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->image_type == GTK_IMAGE_PIXBUF ? data->image_pixbuf : NULL;
}

const gchar * gtk_status_icon_get_stock(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->image_type == GTK_IMAGE_STOCK ? data->image_string : NULL;
}

const gchar * gtk_status_icon_get_icon_name(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->image_type == GTK_IMAGE_ICON_NAME ? data->image_string : NULL;
}

GIcon * gtk_status_icon_get_gicon(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->image_type == GTK_IMAGE_GICON ? data->image_gicon : NULL;
}

gint gtk_status_icon_get_size(GtkStatusIcon * status_icon) {
	return 22;
}

void gtk_status_icon_set_screen(GtkStatusIcon * status_icon, GdkScreen * screen) {}

GdkScreen * gtk_status_icon_get_screen(GtkStatusIcon * status_icon) {
	return gdk_screen_get_default();
}

void gtk_status_icon_set_tooltip(GtkStatusIcon * status_icon, const gchar * tooltip_text) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_tooltip_text(status_icon, tooltip_text);
G_GNUC_END_IGNORE_DEPRECATIONS
}

void gtk_status_icon_set_tooltip_text(GtkStatusIcon * status_icon, const gchar * tooltip_text) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	g_free(data->tooltip_text);
	data->tooltip_text = tooltip_text != NULL ? g_strdup(tooltip_text) : NULL;

	update_icon_tooltip(icon_stub);
}

gchar * gtk_status_icon_get_tooltip_text(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return g_strdup(data->tooltip_text);
}

void gtk_status_icon_set_tooltip_markup(GtkStatusIcon * status_icon, const gchar * markup) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_tooltip_text(status_icon, markup);
G_GNUC_END_IGNORE_DEPRECATIONS
}

gchar * gtk_status_icon_get_tooltip_markup(GtkStatusIcon * status_icon) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	return gtk_status_icon_get_tooltip_text(status_icon);
G_GNUC_END_IGNORE_DEPRECATIONS
}

void gtk_status_icon_set_has_tooltip(GtkStatusIcon * status_icon, gboolean has_tooltip) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	data->has_tooltip = has_tooltip;
	icon_stub_set_query_tooltip(icon_stub, has_tooltip);
}

gboolean gtk_status_icon_get_has_tooltip(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->has_tooltip;
}

void gtk_status_icon_set_title(GtkStatusIcon * status_icon, const gchar * title) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	g_free(data->title);
	data->title = title != NULL ? g_strdup(title) : NULL;
}

const gchar * gtk_status_icon_get_title(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->title;
}

void gtk_status_icon_set_name(GtkStatusIcon * status_icon, const gchar * name) {}

void gtk_status_icon_set_visible(GtkStatusIcon * status_icon, gboolean visible) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	data->visible = visible;
	icon_stub_set_visible(icon_stub, visible);
}

gboolean gtk_status_icon_get_visible(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->visible; 
}

void gtk_status_icon_set_blinking(GtkStatusIcon * status_icon, gboolean blinking) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);

	if (data->blinking != blinking) {
		data->blinking = blinking;
		update_icon_tooltip(icon_stub);
	}
}

gboolean gtk_status_icon_get_blinking(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	SystrayData * data = icon_stub_get_data(icon_stub);
	return data->blinking;
}

gboolean gtk_status_icon_is_embedded(GtkStatusIcon * status_icon) {
	return TRUE;
}

void gtk_status_icon_position_menu(GtkMenu * menu, gint * x, gint * y,
	gboolean * push_in, gpointer user_data) {}

gboolean gtk_status_icon_get_geometry(GtkStatusIcon * status_icon, GdkScreen ** screen,
	GdkRectangle * area, GtkOrientation * orientation) {
	if (area != NULL) {
		area->x = area->y = 0;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		area->width = area->height = gtk_status_icon_get_size(status_icon);
G_GNUC_END_IGNORE_DEPRECATIONS
	}

	if (orientation != NULL) {
		*orientation = GTK_ORIENTATION_HORIZONTAL;
	}

	return TRUE;
}

guint32 gtk_status_icon_get_x11_window_id(GtkStatusIcon * status_icon) {
	return 0;
}

void icon_stub_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	gpointer icon_stub_ptr = object;
	#define case_prop_get(prop, what, getter) \
		case ICON_STUB_PROPERTY_##what: { \
			g_value_set_##prop(value, getter); \
			break; \
		}

	switch (prop_id) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		case_prop_get(boolean, BLINKING, gtk_status_icon_get_blinking(icon_stub_ptr));
		case_prop_get(boolean, EMBEDDED, gtk_status_icon_is_embedded(icon_stub_ptr));
		case_prop_get(object, GICON, gtk_status_icon_get_gicon(icon_stub_ptr));
		case_prop_get(boolean, HAS_TOOLTIP, gtk_status_icon_get_has_tooltip(icon_stub_ptr));
		case_prop_get(string, ICON_NAME, gtk_status_icon_get_icon_name(icon_stub_ptr));
		case_prop_get(enum, ORIENTATION, GTK_ORIENTATION_HORIZONTAL);
		case_prop_get(object, PIXBUF, gtk_status_icon_get_pixbuf(icon_stub_ptr));
		case_prop_get(object, SCREEN, gtk_status_icon_get_screen(icon_stub_ptr));
		case_prop_get(int, SIZE, gtk_status_icon_get_size(icon_stub_ptr));
		case_prop_get(string, STOCK, gtk_status_icon_get_stock(icon_stub_ptr));
		case_prop_get(enum, STORAGE_TYPE, gtk_status_icon_get_storage_type(icon_stub_ptr));
		case_prop_get(string, TITLE, gtk_status_icon_get_title(icon_stub_ptr));
		case_prop_get(string, TOOLTIP_MARKUP, gtk_status_icon_get_tooltip_markup(icon_stub_ptr));
		case_prop_get(string, TOOLTIP_TEXT, gtk_status_icon_get_tooltip_text(icon_stub_ptr));
		case_prop_get(boolean, VISIBLE, gtk_status_icon_get_visible(icon_stub_ptr));
G_GNUC_END_IGNORE_DEPRECATIONS
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

void icon_stub_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
gpointer icon_stub_ptr = object;
	#define case_prop_set(prop, what, setter) \
		case ICON_STUB_PROPERTY_##what: { \
			setter(icon_stub_ptr, g_value_get_##prop(value)); \
			break; \
		}

	switch (prop_id) {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		case_prop_set(boolean, BLINKING, gtk_status_icon_set_blinking);
		case_prop_set(object, FILE, gtk_status_icon_set_from_file);
		case_prop_set(object, GICON, gtk_status_icon_set_from_gicon);
		case_prop_set(boolean, HAS_TOOLTIP, gtk_status_icon_set_has_tooltip);
		case_prop_set(string, ICON_NAME, gtk_status_icon_set_from_icon_name);
		case_prop_set(object, PIXBUF, gtk_status_icon_set_from_pixbuf);
		case_prop_set(object, SCREEN, gtk_status_icon_set_screen);
		case_prop_set(string, STOCK, gtk_status_icon_set_from_stock);
		case_prop_set(string, TITLE, gtk_status_icon_set_title);
		case_prop_set(string, TOOLTIP_MARKUP, gtk_status_icon_set_tooltip_markup);
		case_prop_set(string, TOOLTIP_TEXT, gtk_status_icon_set_tooltip_text);
		case_prop_set(boolean, VISIBLE, icon_stub_set_visible);
G_GNUC_END_IGNORE_DEPRECATIONS
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

void * dlsym_override(const char * symbol) {
	dlsym_override_library(icon_stub);
	dlsym_override_library(app_indicator);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	dlsym_compare(gtk_status_icon_new);
	dlsym_compare(gtk_status_icon_new_from_pixbuf);
	dlsym_compare(gtk_status_icon_new_from_file);
	dlsym_compare(gtk_status_icon_new_from_stock);
	dlsym_compare(gtk_status_icon_new_from_icon_name);
	dlsym_compare(gtk_status_icon_new_from_gicon);
	dlsym_compare(gtk_status_icon_set_from_pixbuf);
	dlsym_compare(gtk_status_icon_set_from_file);
	dlsym_compare(gtk_status_icon_set_from_stock);
	dlsym_compare(gtk_status_icon_set_from_icon_name);
	dlsym_compare(gtk_status_icon_set_from_gicon);
	dlsym_compare(gtk_status_icon_get_storage_type);
	dlsym_compare(gtk_status_icon_get_pixbuf);
	dlsym_compare(gtk_status_icon_get_stock);
	dlsym_compare(gtk_status_icon_get_icon_name);
	dlsym_compare(gtk_status_icon_get_gicon);
	dlsym_compare(gtk_status_icon_get_size);
	dlsym_compare(gtk_status_icon_set_screen);
	dlsym_compare(gtk_status_icon_get_screen);
#if !GTK_CHECK_VERSION(3, 0, 0)
	dlsym_compare(gtk_status_icon_set_tooltip);
#endif
	dlsym_compare(gtk_status_icon_set_tooltip_text);
	dlsym_compare(gtk_status_icon_get_tooltip_text);
	dlsym_compare(gtk_status_icon_set_tooltip_markup);
	dlsym_compare(gtk_status_icon_get_tooltip_markup);
	dlsym_compare(gtk_status_icon_set_has_tooltip);
	dlsym_compare(gtk_status_icon_get_has_tooltip);
	dlsym_compare(gtk_status_icon_set_title);
	dlsym_compare(gtk_status_icon_get_title);
	dlsym_compare(gtk_status_icon_set_name);
	dlsym_compare(gtk_status_icon_set_visible);
	dlsym_compare(gtk_status_icon_get_visible);
#if !GTK_CHECK_VERSION(3, 0, 0)
	dlsym_compare(gtk_status_icon_set_blinking);
	dlsym_compare(gtk_status_icon_get_blinking);
#endif
	dlsym_compare(gtk_status_icon_is_embedded);
	dlsym_compare(gtk_status_icon_get_x11_window_id);
G_GNUC_END_IGNORE_DEPRECATIONS
	return NULL;
}
