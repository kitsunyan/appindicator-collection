#include <common/common.h>
#include <common/app-indicator.h>
#include <common/icon-stub.h>

#define ICON_NAME_NORMAL "hexchat-tray"
#define ICON_NAME_NEW "hexchat-tray-new"

typedef struct {
	GdkPixbuf * normal_pixbuf;
	GdkPixbuf * current_pixbuf;
	gchar * tooltip_text;
} HexchatData;

static void hexchat_data_free(HexchatData * data) {
	g_object_unref(data->normal_pixbuf);
	g_object_unref(data->current_pixbuf);
	g_free(data->tooltip_text);
	g_free(data);
}

static void update_icon_tooltip(IconStub * icon_stub) {
	HexchatData * data = icon_stub_get_data(icon_stub);
	const gchar * icon_name = data->normal_pixbuf ==
		data->current_pixbuf ? ICON_NAME_NORMAL : ICON_NAME_NEW;
	const gchar * text = data->tooltip_text != NULL ? strstr(data->tooltip_text, "HexChat: ") : NULL;
	text = text != NULL ? &text[9] : data->tooltip_text;
	icon_stub_set_icon_tooltip(icon_stub, icon_name, text);
}

GtkStatusIcon * gtk_status_icon_new_from_pixbuf(GdkPixbuf * pixbuf) {
	HexchatData * data = g_new0(HexchatData, 1);
	data->normal_pixbuf = g_object_ref(pixbuf);
	data->current_pixbuf = g_object_ref(pixbuf);

	IconStub * icon_stub = icon_stub_new("hexchat", "HexChat", ICON_NAME_NORMAL, data);
	g_object_weak_ref(G_OBJECT(icon_stub), (GWeakNotify) hexchat_data_free, data);

	g_signal_connect(icon_stub, "configure-menu",
		G_CALLBACK(icon_stub_configure_menu_select_activate_head), NULL);
	g_signal_connect(icon_stub, "update-icon-tooltip",
		G_CALLBACK(update_icon_tooltip), NULL);

	return (gpointer) icon_stub;
}

void gtk_status_icon_set_tooltip_text(GtkStatusIcon * status_icon, const gchar * text) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	HexchatData * data = icon_stub_get_data(icon_stub);

	g_free(data->tooltip_text);
	data->tooltip_text = text != NULL ? g_strdup(text) : NULL;

	update_icon_tooltip(icon_stub);
}

void gtk_status_icon_set_from_pixbuf(GtkStatusIcon * status_icon, GdkPixbuf * pixbuf) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	HexchatData * data = icon_stub_get_data(icon_stub);

	g_object_unref(data->current_pixbuf);
	data->current_pixbuf = pixbuf;
	g_object_ref(pixbuf);

	update_icon_tooltip(icon_stub);
}

GdkPixbuf * gtk_status_icon_get_pixbuf(GtkStatusIcon * status_icon) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	HexchatData * data = icon_stub_get_data(icon_stub);
	return data->current_pixbuf;
}

void icon_stub_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	switch (prop_id) {
		case ICON_STUB_PROPERTY_EMBEDDED: {
			g_value_set_boolean(value, TRUE);
			break;
		}
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

void icon_stub_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
}

void * dlsym_override(const char * symbol) {
	dlsym_override_library(icon_stub);
	dlsym_override_library(app_indicator);
	dlsym_compare(gtk_status_icon_new_from_pixbuf);
	dlsym_compare(gtk_status_icon_set_tooltip_text);
	dlsym_compare(gtk_status_icon_set_from_pixbuf);
	dlsym_compare(gtk_status_icon_get_pixbuf);
	return NULL;
}
