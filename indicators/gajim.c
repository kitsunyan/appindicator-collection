#include <common/common.h>
#include <common/app-indicator.h>
#include <common/icon-stub.h>

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

typedef struct {
	gchar * status;
	gchar * tooltip_text;
	gboolean highlight;
} GajimData;

static void gajim_data_free(GajimData * data) {
	g_free(data->status);
	g_free(data->tooltip_text);
	g_free(data);
}

static void update_icon_tooltip(IconStub * icon_stub) {
	GajimData * data = icon_stub_get_data(icon_stub);
	gchar * name = g_strdup_printf(ICON_PREFIX "%s%s",
		data->status, data->highlight ? "-highlight" : "");
	icon_stub_set_icon_tooltip(icon_stub, name, data->tooltip_text);
	g_free(name);
}

typedef struct {
	gchar * text;
	const gchar * label;
} TooltipForeach;

static gchar * obtain_tooltip_append(gchar * current, const gchar * account, const gchar * status) {
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

static void obtain_tooltip_foreach(GtkWidget * widget, gpointer user_data) {
	TooltipForeach * tooltip_foreach = user_data;
	if (GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), obtain_tooltip_foreach, user_data);
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

static void obtain_tooltip(IconStub * icon_stub, GtkWidget * widget) {
	gchar * new_tooltip_text = NULL;
	if (widget != NULL) {
		debug("obtain custom widget %ld", (gsize) widget);
		TooltipForeach tooltip_foreach = {NULL, };
		gtk_container_foreach(GTK_CONTAINER(widget), obtain_tooltip_foreach, &tooltip_foreach);
		new_tooltip_text = tooltip_foreach.text;
	}
	GajimData * data = icon_stub_get_data(icon_stub);
	if (g_strcmp0(data->tooltip_text, new_tooltip_text)) {
		g_free(data->tooltip_text);
		data->tooltip_text = new_tooltip_text;
		update_icon_tooltip(icon_stub);
	} else {
		g_free(new_tooltip_text);
	}
}

gpointer g_object_newv(GType object_type, guint n_parameters, GParameter * parameters) {
	if (object_type == GTK_TYPE_STATUS_ICON) {
		GajimData * data = g_new0(GajimData, 1);
		IconStub * icon_stub = icon_stub_new("gajim", "Gajim", ICON_PREFIX "offline", data);
		icon_stub_set_query_tooltip(icon_stub, TRUE);
		g_signal_connect(icon_stub, "configure-menu",
			G_CALLBACK(icon_stub_configure_menu_prepend_activate), NULL);
		g_signal_connect(icon_stub, "update-icon-tooltip",
			G_CALLBACK(update_icon_tooltip), NULL);
		g_signal_connect(icon_stub, "obtain-tooltip",
			G_CALLBACK(obtain_tooltip), NULL);
		g_object_weak_ref(G_OBJECT(icon_stub), (GWeakNotify) gajim_data_free, data);
		return icon_stub;
	} else {
		super_lookup_static(g_object_newv, gpointer, GType, guint, GParameter *);
		return g_object_newv_super(object_type, n_parameters, parameters);
	}
}

void gtk_status_icon_set_visible(GtkStatusIcon * status_icon, gboolean visible) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	icon_stub_set_visible(icon_stub, visible);
}

void gtk_status_icon_set_blinking(GtkStatusIcon * status_icon, gboolean blinking) {
	IconStub * icon_stub = ICON_STUB(status_icon);
	GajimData * data = icon_stub_get_data(icon_stub);
	if (data->highlight != blinking) {
		data->highlight = blinking;
		update_icon_tooltip(icon_stub);
	}
}

void gtk_status_icon_set_from_pixbuf(GtkStatusIcon * status_icon, GdkPixbuf * pixbuf) {
	const gchar * status = lookup_status(pixbuf);
	if (status == NULL) {
		status = "offline";
	}
	debug("set pixbuf %ld %s", (gsize) pixbuf, status);
	IconStub * icon_stub = ICON_STUB(status_icon);
	GajimData * data = icon_stub_get_data(icon_stub);
	g_free(data->status);
	data->status = g_strdup(status);
	update_icon_tooltip(icon_stub);
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

void icon_stub_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
}

void icon_stub_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	switch (prop_id) {
		case ICON_STUB_PROPERTY_HAS_TOOLTIP: {
			break;
		}
		default: {
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
}

void * dlsym_override(const char * symbol) {
	dlsym_override_library(icon_stub);
	dlsym_override_library(app_indicator);
	dlsym_compare(g_object_newv);
	dlsym_compare(gtk_status_icon_set_visible);
	dlsym_compare(gtk_status_icon_set_blinking);
	dlsym_compare(gtk_status_icon_set_from_pixbuf);
	dlsym_compare(gtk_image_set_from_file);
	return NULL;
}
