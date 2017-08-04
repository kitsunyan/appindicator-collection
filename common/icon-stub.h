#ifndef __ICON_STUB_H__
#define __ICON_STUB_H__

#include <gtk/gtk.h>

typedef struct _IconStubClass IconStubClass;
typedef struct _IconStub IconStub;

#define ICON_STUB_TYPE (icon_stub_get_type())
#define ICON_STUB(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ICON_STUB_TYPE, IconStub))
#define ICON_STUB_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ICON_STUB_TYPE, IconStubClass))
#define IS_ICON_STUB(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ICON_STUB_TYPE))
#define IS_ICON_STUB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ICON_STUB_TYPE))
#define ICON_STUB_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), ICON_STUB_TYPE, IconStubClass))

GType icon_stub_get_type();

enum {
	ICON_STUB_PROPERTY_0,
	ICON_STUB_PROPERTY_BLINKING,
	ICON_STUB_PROPERTY_EMBEDDED,
	ICON_STUB_PROPERTY_FILE,
	ICON_STUB_PROPERTY_GICON,
	ICON_STUB_PROPERTY_HAS_TOOLTIP,
	ICON_STUB_PROPERTY_ICON_NAME,
	ICON_STUB_PROPERTY_ORIENTATION,
	ICON_STUB_PROPERTY_PIXBUF,
	ICON_STUB_PROPERTY_SCREEN,
	ICON_STUB_PROPERTY_SIZE,
	ICON_STUB_PROPERTY_STOCK,
	ICON_STUB_PROPERTY_STORAGE_TYPE,
	ICON_STUB_PROPERTY_TITLE,
	ICON_STUB_PROPERTY_TOOLTIP_MARKUP,
	ICON_STUB_PROPERTY_TOOLTIP_TEXT,
	ICON_STUB_PROPERTY_VISIBLE
} IconStubProperty;

void icon_stub_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
void icon_stub_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

void icon_stub_configure_menu_prepend_activate(IconStub * icon_stub, GtkWidget * menu);
void icon_stub_configure_menu_select_activate(IconStub * icon_stub, GtkWidget * menu_item);
void icon_stub_configure_menu_select_activate_head(IconStub * icon_stub, GtkWidget * menu);

IconStub * icon_stub_new(const gchar * id, const gchar * title, const gchar * default_icon, gpointer data);

gpointer icon_stub_get_data(IconStub * icon_stub);

void icon_stub_set_query_tooltip(IconStub * icon_stub, gboolean query_tooltip);
void icon_stub_set_icon_tooltip(IconStub * icon_stub, const gchar * name, const gchar * tooltip_text);
void icon_stub_set_visible(IconStub * icon_stub, gboolean visible);

void * icon_stub_dlsym_override(const char * symbol);

#define status_icon_check_condition_void(condition, function, args, call) \
if (condition) { \
	super_lookup_static(function, void, args); \
	function##_super(call); \
	return; \
}

#define status_icon_check_condition(condition, function, result, args, call) \
if (condition) { \
	super_lookup_static(function, result, args); \
	return function##_super(call); \
}

#define status_icon_check_void(function, args, call) \
status_icon_check_condition_void(GTK_IS_STATUS_ICON(status_icon), function, \
	pass_args(args), pass_args(call))

#define status_icon_check(function, result, args, call) \
status_icon_check_condition(GTK_IS_STATUS_ICON(status_icon), function, result, \
	pass_args(args), pass_args(call))

#define status_icon_check_init_void(function, args, call) \
status_icon_check_condition_void(app_indicator_falling_back, function, \
	pass_args(args), pass_args(call))

#define status_icon_check_init(function, result, args, call) \
status_icon_check_condition(app_indicator_falling_back, function, result, \
	pass_args(args), pass_args(call))

#endif
