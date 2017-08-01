#include "common.h"
#include "app-indicator.h"

#define KEY_PRIMARY_ACTIVATE "app-indicator-primary-activate"

void app_indicator_set_item_is_menu(AppIndicator * indicator, gboolean item_is_menu) {
	g_object_set_data(G_OBJECT(indicator), KEY_PRIMARY_ACTIVATE, GINT_TO_POINTER(!item_is_menu));
}

gboolean app_indicator_get_item_is_menu(AppIndicator * indicator) {
	gboolean primary_activate = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(indicator),
		KEY_PRIMARY_ACTIVATE));
	return !primary_activate;
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

void * dlsym_override_private(const char * symbol) {
	dlsym_compare(g_dbus_node_info_new_for_xml);
	dlsym_compare(g_dbus_connection_register_object);
	return NULL;
}

#else

void * dlsym_override_private(const char * symbol) {
	return NULL;
}

#endif
