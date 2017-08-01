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

void icon_stub_configure_menu_prepend_activate(IconStub * icon_stub, GtkWidget * menu);
void icon_stub_configure_menu_select_activate(IconStub * icon_stub, GtkWidget * menu_item);
void icon_stub_configure_menu_select_activate_head(IconStub * icon_stub, GtkWidget * menu);

IconStub * icon_stub_new(const gchar * id, const gchar * title, const gchar * default_icon,
	gboolean custom_toolip, gpointer data);

gpointer icon_stub_get_data(IconStub * icon_stub);

void icon_stub_set_icon_tooltip(IconStub * icon_stub, const gchar * name, const gchar * tooltip_text);
void icon_stub_set_visible(IconStub * icon_stub, gboolean visible);

void * icon_stub_dlsym_override(const char * symbol);

#endif
