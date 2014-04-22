/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Anjuta
 * Copyright (C) 2000 Dave Camp, Naba Kumar  <naba@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 */

#ifndef ANJUTA_PLUGIN_H
#define ANJUTA_PLUGIN_H

#include <glib.h>
#include <glib-object.h>

#include <string.h>
#include <libanjuta/anjuta-shell.h>
#include <libanjuta/anjuta-ui.h>
#include <libanjuta/anjuta-preferences.h>
#include <libanjuta/anjuta-utils.h>

G_BEGIN_DECLS

/* Add this alias in case some plugin outside Anjuta tree still uses it */
typedef GTypeModule AnjutaGluePlugin;

typedef struct _AnjutaPlugin        AnjutaPlugin;
typedef struct _AnjutaPluginClass   AnjutaPluginClass;
typedef struct _AnjutaPluginPrivate AnjutaPluginPrivate;

/**
 * AnjutaPluginValueAdded:
 * @plugin: The #AnjutaPlugin based plugin
 * @name: name of value being added.
 * @value: value of value being added.
 * @user_data: User data set during anjuta_plugin_add_watch()
 *
 * The callback to pass to anjuta_plugin_add_watch(). When a @name value
 * is added to shell by another plugin, this callback will be called.
 */
typedef void (*AnjutaPluginValueAdded) (AnjutaPlugin *plugin, 
										const char *name,
										const GValue *value,
										gpointer user_data);

/**
 * AnjutaPluginValueRemoved:
 * @plugin: The #AnjutaPlugin based plugin
 * @name: name of value being added.
 * @user_data: User data set during anjuta_plugin_add_watch()
 *
 * The callback to pass to anjuta_plugin_add_watch(). When the @name value
 * is removed from the shell (by the plugin exporting this value), this
 * callback will be called.
 */
typedef void (*AnjutaPluginValueRemoved) (AnjutaPlugin *plugin, 
										  const char *name,
										  gpointer user_data);


#define ANJUTA_TYPE_PLUGIN         (anjuta_plugin_get_type ())
#define ANJUTA_PLUGIN(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), ANJUTA_TYPE_PLUGIN, AnjutaPlugin))
#define ANJUTA_PLUGIN_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), ANJUTA_TYPE_PLUGIN, AnjutaPluginClass))
#define ANJUTA_IS_PLUGIN(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), ANJUTA_TYPE_PLUGIN))
#define ANJUTA_IS_PLUGIN_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), ANJUTA_TYPE_PLUGIN))
#define ANJUTA_PLUGIN_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), ANJUTA_TYPE_PLUGIN, AnjutaPluginClass))

struct _AnjutaPlugin {
	GObject parent;	

	/* The shell in which the plugin has been added */
	AnjutaShell *shell;
	
	/*< private >*/
	AnjutaPluginPrivate *priv;
};

struct _AnjutaPluginClass {
	GObjectClass parent_class;

	/* Signals  */
	void (*activated) (AnjutaPlugin *plugin);
	void (*deactivated) (AnjutaPlugin *plugin);

	/* Virtual functions */
	gboolean (*activate) (AnjutaPlugin *plugin);
	gboolean (*deactivate) (AnjutaPlugin *plugin);
};

GType anjuta_plugin_get_type   (void);

gboolean anjuta_plugin_activate (AnjutaPlugin *plugin);

gboolean anjuta_plugin_deactivate (AnjutaPlugin *plugin);

gboolean anjuta_plugin_is_active (AnjutaPlugin *plugin);

guint anjuta_plugin_add_watch (AnjutaPlugin *plugin, 
							   const gchar *name,
							   AnjutaPluginValueAdded added,
							   AnjutaPluginValueRemoved removed,
							   gpointer user_data);

void anjuta_plugin_remove_watch (AnjutaPlugin *plugin, guint id,
								 gboolean send_remove);

AnjutaShell *anjuta_plugin_get_shell (AnjutaPlugin* plugin);

/**
 * ANJUTA_PLUGIN_BEGIN:
 * @class_name: Name of the class. e.g. EditorPlugin
 * @prefix: prefix of member function names. e.g. editor_plugin
 * 
 * This is a convienient macro defined to make it easy to write plugin
 * classes . This macro begins the class type definition. member function
 * @prefix _class_init and @prefix _instance_init should be statically defined
 * before using this macro.
 *
 * The class type definition is finished with ANJUTA_PLUGIN_END() macro. In
 * between which any number of interface definitions could be added with
 * ANJUTA_PLUGIN_ADD_INTERFACE() macro.
 */
#define ANJUTA_PLUGIN_BEGIN(class_name, prefix)                          \
extern GType                                                             \
prefix##_get_type (GTypeModule *module)                                  \
{                                                                        \
	static GType type = 0;                                               \
	if (G_UNLIKELY (!type)) {                                            \
		static const GTypeInfo type_info = {                             \
			sizeof (class_name##Class),                                  \
			NULL,                                                        \
			NULL,                                                        \
			(GClassInitFunc)prefix##_class_init,                         \
			NULL,                                                        \
			NULL,                                                        \
			sizeof (class_name),                                         \
			0,                                                           \
			(GInstanceInitFunc)prefix##_instance_init                    \
		};                                                               \
		g_return_val_if_fail (module != NULL, 0);                        \
		type = g_type_module_register_type (module,                                      \
						    ANJUTA_TYPE_PLUGIN,                          \
						    #class_name,                                 \
						    &type_info, 0);
/**
 * ANJUTA_PLUGIN_END:
 * 
 * Ends the plugin class type definition started with ANJUTA_PLUGIN_BEGIN()
 */
#define ANJUTA_PLUGIN_END                                               \
	}                                                                   \
	return type;                                                        \
}

/**
 * ANJUTA_PLUGIN_ADD_INTERFACE:
 * @interface_type: Interface type. e.g. IANJUTA_TYPE_EDITOR
 * @prefix: prefix of member function names.
 * 
 * This is a convienient macro defined to make it easy to add interfaces
 * to a plugin type. @prefix _iface_init should be statically defined
 * before using this macro. This macro should be called between
 * ANJUTA_PLUGIN_BEGIN() and ANJUTA_PLUGIN_END() macros.
 */
#define ANJUTA_PLUGIN_ADD_INTERFACE(prefix,interface_type)             \
    {                                                                  \
        GInterfaceInfo iface_info = {                                  \
            (GInterfaceInitFunc)prefix##_iface_init,                   \
            NULL,                                                      \
            NULL                                                       \
        };                                                             \
        g_type_module_add_interface (module,                           \
                                     type, interface_type,             \
                             	     &iface_info);                     \
    }

/**
 * ANJUTA_PLUGIN_BOILERPLATE:
 * @class_name: Name of the class. e.g EditorPlugin
 * @prefix: prefix of member function names. e.g. editor_plugin
 * 
 * This macro is similar to using ANJUTA_PLUGIN_BEGIN() and then immediately
 * using ANJUTA_PLUGIN_END(). It is basically a plugin type definition macro
 * that does not have any interface implementation.
 */
#define ANJUTA_PLUGIN_BOILERPLATE(class_name, prefix)                   \
ANJUTA_PLUGIN_BEGIN(class_name, prefix);                                \
ANJUTA_PLUGIN_END

/**
 * ANJUTA_SIMPLE_PLUGIN:
 * @class_name: Name of the class. e.g. EditorPlugin
 * @prefix: prefix of member function names. e.g. editor_plugin
 * 
 * Sets up necessary codes for the plugin factory to know the class type of
 * of the plugin. This macro is generally used at the end of plugin class
 * and member functions definitions. 
 */
#define ANJUTA_SIMPLE_PLUGIN(class_name, prefix)                      \
G_MODULE_EXPORT void anjuta_glue_register_components (GTypeModule *module);                   \
G_MODULE_EXPORT void                                                  \
anjuta_glue_register_components (GTypeModule *module)                 \
{                                                                     \
	prefix##_get_type (module);                                   \
}                                                                     \

G_END_DECLS

#endif
