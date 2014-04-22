/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta-ui.h
 * Copyright (C) Naba Kumar  <naba@gnome.org>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef _ANJUTA_UI_H_
#define _ANJUTA_UI_H_

/* Usage Notes:
 * 1) Any object which added any action or action group is responsible
 * for removing them when done (for example, a plugin object).
 *
 * 2) Any object which merged a UI is responsible for unmerging it
 * when done with it (for example, a plugin object).
 *
 * 3) Avoid using EggMenuMerge object gotten by anjuta_ui_get_menu_merge(),
 * because AnjutaUI keeps track of all actions/action-groups added to or
 * removed from it and accordingly updates the required UI interfaces.
 * Use the EggMenuMerge object only to do things not doable by AnjutaUI.
 */
#include <gtk/gtk.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define ANJUTA_TYPE_UI        (anjuta_ui_get_type ())
#define ANJUTA_UI(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), ANJUTA_TYPE_UI, AnjutaUI))
#define ANJUTA_UI_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), ANJUTA_TYPE_UI, AnjutaUIClass))
#define ANJUTA_IS_UI(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), ANJUTA_TYPE_UI))
#define ANJUTA_IS_UI_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), ANJUTA_TYPE_UI))

typedef struct _AnjutaUI        AnjutaUI;
typedef struct _AnjutaUIClass   AnjutaUIClass;
typedef struct _AnjutaUIPrivate AnjutaUIPrivate;

struct _AnjutaUI {
	GtkUIManager parent;
	
	AnjutaUIPrivate *priv;
};

struct _AnjutaUIClass {
	GtkUIManagerClass parent;
};

GType anjuta_ui_get_type (void);

/* Creates a new AnjutaUI object */
AnjutaUI* anjuta_ui_new (void);

/* Adds a group of Action entries with the give group name.
 * Caller does not get a reference to the returned ActionGroup. Use it
 * as reference ID to remove the action-group later (after which the object
 * will no longer be valid).
 */
GtkActionGroup* anjuta_ui_add_action_group_entries (AnjutaUI *ui,
											const gchar *action_group_name,
											const gchar *action_group_label,
											GtkActionEntry *entries,
											gint num_entries,
											const gchar *translation_domain,
											gboolean can_customize,
											gpointer user_data);

GtkActionGroup* anjuta_ui_add_toggle_action_group_entries (AnjutaUI *ui,
											const gchar *action_group_name,
											const gchar *action_group_label,
											GtkToggleActionEntry *entries,
											gint num_entries,
											const gchar *translation_domain,
											gboolean can_customize,
											gpointer user_data);

void anjuta_ui_add_action_group (AnjutaUI *ui,
								 const gchar *action_group_name,
								 const gchar *action_group_label,
								 GtkActionGroup *action_group,
								 gboolean can_customize);

/* Removes the group of Actions */
void anjuta_ui_remove_action_group (AnjutaUI *ui, GtkActionGroup *action_group);

/* Get the action object from the given group with the given name */
GtkAction * anjuta_ui_get_action (AnjutaUI *ui,
								  const gchar *action_group_name,
								  const gchar *action_name);

/* Activates (calls the action callback) the action given by the action
 * path. Path is given by "ActionGroupName/ActionName".
 */
void anjuta_ui_activate_action_by_path (AnjutaUI *ui,
										const gchar *action_path);

/* Activates the action in the given Action group object with the given
 * action name.
 */
void anjuta_ui_activate_action_by_group (AnjutaUI *ui,
										 GtkActionGroup *action_group,
										 const gchar *action_name);

/* Merges the given UI description file (written in xml)
   Returns an id representing it */
gint anjuta_ui_merge (AnjutaUI *ui, const gchar *ui_filename);

/* Unmerges the give merge id */
void anjuta_ui_unmerge (AnjutaUI *ui, gint id);

/* Gets the icon factory */
GtkIconFactory* anjuta_ui_get_icon_factory (AnjutaUI* ui);

/* Get accel group associated with UI */
GtkAccelGroup* anjuta_ui_get_accel_group (AnjutaUI *ui);

GtkWidget* anjuta_ui_get_accel_editor (AnjutaUI *ui);

/* Dump the whole tree in STDOUT. Useful for debugging */
void anjuta_ui_dump_tree (AnjutaUI *ui);

/* Loading and saving accels */
void anjuta_ui_load_accels (const gchar *filename);

void anjuta_ui_save_accels (const gchar *filename);

void anjuta_ui_unload_accels (AnjutaUI *ui);

/**
 * BEGIN_REGISTER_ICON:
 * @plugin: The anjuta plugin to register the icons for
 *
 * Convenience macros to register stock icons 
 */
#define BEGIN_REGISTER_ICON(plugin) \
{ \
	AnjutaUI *ui = anjuta_shell_get_ui ((plugin)->shell, NULL); \
	GtkIconFactory *icon_factory = anjuta_ui_get_icon_factory (ui); \
	GtkIconSet *icon_set; \
	GtkIconSource *	icon_source = gtk_icon_source_new ();

/**
 * REGISTER_ICON_FULL:
 * @icon: name of the icon (without size and extension)
 * @stock_id: stock-id that should be assigned to the icon
 *
 * Register icon with size 16 and size 24, icon should be the 
 * filename (without path) striped of the size (16,24) and the *.png
 * (e.g anjuta-icon-24.png => anjuta-icon)
 */

#define REGISTER_ICON_FULL(icon, stock_id) \
	icon_set = gtk_icon_set_new(); \
	gtk_icon_source_set_filename (icon_source, PACKAGE_PIXMAPS_DIR"/"icon"-16.png"); \
	gtk_icon_source_set_size (icon_source, 16); \
	gtk_icon_set_add_source (icon_set, icon_source); \
	gtk_icon_source_set_filename (icon_source, PACKAGE_PIXMAPS_DIR"/"icon"-24.png"); \
	gtk_icon_source_set_size (icon_source, 24); \
	gtk_icon_set_add_source (icon_set, icon_source); \
	icon_set = gtk_icon_set_new(); \
	gtk_icon_set_add_source (icon_set, icon_source); \
	gtk_icon_factory_add (icon_factory, stock_id, icon_set);

/**
 * REGISTER_ICON:
 * @icon: name of the icon (without size and extension)
 * @stock_id: stock-id that should be assigned to the icon
 *
 * Register icon for all sizes (will be scaled) 
 * icon should be the full filename without path (e.g anjuta-icon.png)
 */

#define REGISTER_ICON(icon, stock_id) \
	{ \
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file (PACKAGE_PIXMAPS_DIR"/"icon, NULL); \
	if (pixbuf) \
	{ \
		icon_set = gtk_icon_set_new_from_pixbuf (pixbuf); \
		gtk_icon_factory_add (icon_factory, stock_id, icon_set); \
		g_object_unref (pixbuf); \
	} \
	}


/** 
 * END_REGISTER_ICON:
 *
 * Ends a BEGIN_REGISTER_ICON sequence
 */
#define END_REGISTER_ICON \
	gtk_icon_source_free (icon_source); \
}

G_END_DECLS

#endif
