#include "common.h"
#include "app-indicator.h"

#include <libappindicator/app-indicator-enum-types.h>

gboolean app_indicator_falling_back = FALSE;

typedef struct _ExtendedClass {
	AppIndicatorClass __parent__;
} ExtendedClass;

typedef struct _Extended {
	AppIndicator __parent__;
	gboolean item_is_menu;
} Extended;

static void extended_init(Extended * extended);
static void extended_class_init(ExtendedClass * klass);

G_DEFINE_TYPE(Extended, extended, APP_INDICATOR_TYPE);

#define EXTENDED_TYPE (extended_get_type())
#define EXTENDED(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EXTENDED_TYPE, Extended))
#define IS_EXTENDED(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EXTENDED_TYPE))

static GtkStatusIcon * extended_fallback(AppIndicator * indicator) {
	app_indicator_falling_back = TRUE;
	APP_INDICATOR_CLASS(extended_parent_class)->fallback(indicator);
	app_indicator_falling_back = FALSE;
}

static void extended_init(Extended * extended) {
	extended->item_is_menu = TRUE;
}

static void extended_class_init(ExtendedClass * klass) {
	AppIndicatorClass * app_indicator_class = APP_INDICATOR_CLASS(klass);
	app_indicator_class->fallback = extended_fallback;
}

AppIndicator * app_indicator_new_extended(const gchar * id, const gchar * icon_name,
	AppIndicatorCategory category) {
	GEnumValue * value = g_enum_get_value((GEnumClass *) g_type_class_ref
		(APP_INDICATOR_TYPE_INDICATOR_CATEGORY), category);

	return g_object_new(EXTENDED_TYPE, "id", id, "category", value->value_nick,
		"icon-name", icon_name, NULL);
}

void app_indicator_set_item_is_menu(AppIndicator * indicator, gboolean item_is_menu) {
	if (IS_EXTENDED(indicator)) {
		Extended * extended = EXTENDED(indicator);
		extended->item_is_menu = item_is_menu;
	}
}

gboolean app_indicator_get_item_is_menu(AppIndicator * indicator) {
	if (IS_EXTENDED(indicator)) {
		Extended * extended = EXTENDED(indicator);
		return extended->item_is_menu;
	}
	return TRUE;
}

guint gdk_pixbuf_hash(GdkPixbuf * pixbuf) {
	if (pixbuf == NULL) {
		return 0;
	}

	guint length = 0;
	guchar * pixels = gdk_pixbuf_get_pixels_with_length(pixbuf, &length);

	guint prime = 31;
	guint hash_code = 0;
	for (guint i = 0; i < length; i++) {
		hash_code = prime * hash_code + pixels[i];
	}

	return hash_code;
}

#ifdef WITH_ACTIVATE

static const gchar * xml_declaration = "<interface name=\"org.kde.StatusNotifierItem\">\n";

static const gchar * xml_property_item_is_menu =
"		<property name=\"ItemIsMenu\" type=\"b\" access=\"read\" />\n";

static const gchar * xml_method_activate =
"		<method name=\"Activate\">\n"
"			<arg type=\"i\" name=\"x\" direction=\"in\" />\n"
"			<arg type=\"i\" name=\"y\" direction=\"in\" />\n"
"		</method>\n";

static gchar * insert_xml(gchar * target, const gchar * original, const gchar * what) {
	const gchar * work = target != NULL ? target : original;
	const gchar * after = strstr(work, xml_declaration);
	if (after == NULL) {
		return target;
	}
	after = &after[strlen(xml_declaration)];
	gint index = (gint) (after - work);
	gchar * temp = g_strndup(work, index);
	gchar * result = g_strdup_printf("%s%s%s", temp, what, after);
	g_free(temp);
	g_free(target);
	return result;
}

GDBusNodeInfo * g_dbus_node_info_new_for_xml(const gchar * xml_data, GError ** error) {
	super_lookup_static(g_dbus_node_info_new_for_xml, GDBusNodeInfo *, const gchar *, GError **);
	gchar * target_xml_data = NULL;

	if (xml_data != NULL && strstr(xml_data, "\n<node name=\"/StatusNotifierItem\">\n") != NULL) {
		if (strstr(xml_data, "name=\"ItemIsMenu\"") == NULL) {
			target_xml_data = insert_xml(target_xml_data, xml_data, xml_property_item_is_menu);
		}

		if (strstr(xml_data, "name=\"Activate\"") == NULL) {
			target_xml_data = insert_xml(target_xml_data, xml_data, xml_method_activate);
		}
	}

	if (target_xml_data != NULL) {
		GDBusNodeInfo * result = g_dbus_node_info_new_for_xml_super(target_xml_data, error);
		g_free(target_xml_data);
		return result;
	} else {
		return g_dbus_node_info_new_for_xml_super(xml_data, error);
	}
}

static GDBusInterfaceMethodCallFunc app_indicator_method_call_super = NULL;
static GDBusInterfaceGetPropertyFunc app_indicator_get_property_super = NULL;

static void app_indicator_method_call(GDBusConnection * connection, const gchar * sender,
	const gchar * object_path, const gchar * interface_name, const gchar * method_name,
	GVariant * parameters, GDBusMethodInvocation * invocation, gpointer user_data) {
	if (!g_strcmp0(method_name, "Activate")) {
		method_name = "SecondaryActivate";
	}

	app_indicator_method_call_super(connection, sender, object_path, interface_name, method_name,
		parameters, invocation, user_data);
}

static GVariant * app_indicator_get_property(GDBusConnection * connection, const gchar * sender,
	const gchar * object_path, const gchar * interface_name, const gchar * property_name,
	GError ** error, gpointer user_data) {
	if (!g_strcmp0(property_name, "ItemIsMenu")) {
		AppIndicator * indicator = APP_INDICATOR(user_data);
		return g_variant_new_boolean(app_indicator_get_item_is_menu(indicator));
	}

	app_indicator_get_property_super(connection, sender, object_path, interface_name, property_name,
		error, user_data);
}

static GDBusInterfaceVTable vtable_override = {NULL, };

guint g_dbus_connection_register_object(GDBusConnection * connection, const gchar * object_path,
	GDBusInterfaceInfo * interface_info, const GDBusInterfaceVTable * vtable, gpointer user_data,
	GDestroyNotify user_data_free_func, GError ** error) {
	super_lookup_static(g_dbus_connection_register_object, guint,
		GDBusConnection *, const gchar *, GDBusInterfaceInfo *, const GDBusInterfaceVTable *,
		gpointer, GDestroyNotify, GError **);

	const gchar * prefix = "/org/ayatana/NotificationItem/";
	if (strstr(object_path, prefix) == object_path) {
		const gchar * name = &object_path[strlen(prefix)];
		if (strstr(name, "/") == NULL) {
			if (app_indicator_method_call_super == NULL) {
				app_indicator_method_call_super = vtable->method_call;
				app_indicator_get_property_super = vtable->get_property;
				vtable_override.method_call = app_indicator_method_call;
				vtable_override.get_property = app_indicator_get_property;
				vtable_override.set_property = vtable->set_property;
			}
			vtable = &vtable_override;
		}
	}

	g_dbus_connection_register_object_super(connection, object_path, interface_info, vtable,
		user_data, user_data_free_func, error);
}

void * app_indicator_dlsym_override(const char * symbol) {
	dlsym_compare(g_dbus_node_info_new_for_xml);
	dlsym_compare(g_dbus_connection_register_object);
	return NULL;
}

#else

void * app_indicator_dlsym_override(const char * symbol) {
	return NULL;
}

#endif
