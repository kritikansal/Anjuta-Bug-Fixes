/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta-shell.h
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
#ifndef _ANJUTA_SHELL_H
#define _ANJUTA_SHELL_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include <libanjuta/anjuta-shell.h>
#include <libanjuta/anjuta-status.h>
#include <libanjuta/anjuta-ui.h>
#include <libanjuta/anjuta-preferences.h>
#include <libanjuta/anjuta-plugin-manager.h>
#include <libanjuta/anjuta-profile-manager.h>
#include <libanjuta/anjuta-session.h>
#include <libanjuta/anjuta-save-prompt.h>

G_BEGIN_DECLS

#define ANJUTA_TYPE_SHELL            (anjuta_shell_get_type ())
#define ANJUTA_SHELL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ANJUTA_TYPE_SHELL, AnjutaShell))
#define ANJUTA_IS_SHELL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANJUTA_TYPE_SHELL))
#define ANJUTA_SHELL_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ANJUTA_TYPE_SHELL, AnjutaShellIface))

#define ANJUTA_SHELL_ERROR anjuta_shell_error_quark()

typedef struct _AnjutaShell      AnjutaShell;
typedef struct _AnjutaShellIface AnjutaShellIface;

/**
 * AnjutaShellError:
 * @ANJUTA_SHELL_ERROR_DOESNT_EXIST:
 */
typedef enum
{
	ANJUTA_SHELL_ERROR_DOESNT_EXIST,
} AnjutaShellError;

typedef enum
{
	ANJUTA_SHELL_PLACEMENT_NONE = 0,
	ANJUTA_SHELL_PLACEMENT_TOP,
	ANJUTA_SHELL_PLACEMENT_BOTTOM,
	ANJUTA_SHELL_PLACEMENT_RIGHT,
	ANJUTA_SHELL_PLACEMENT_LEFT,
	ANJUTA_SHELL_PLACEMENT_CENTER,
	ANJUTA_SHELL_PLACEMENT_FLOATING
} AnjutaShellPlacement;

struct _AnjutaShellIface {
	GTypeInterface g_iface;
	
	/* Signals */
	void (*value_added)  (AnjutaShell *shell, char *name, GValue *value);
	void (*value_removed) (AnjutaShell *shell, char *name);
	void (*save_session) (AnjutaShell *shell, AnjutaSessionPhase phase,
						  AnjutaSession *session);
	void (*load_session) (AnjutaShell *shell, AnjutaSessionPhase phase,
						  AnjutaSession *session);
	void (*save_prompt)  (AnjutaShell *shell, AnjutaSavePrompt *save_prompt);
	
	/* Virtual Table */
	AnjutaStatus* (*get_status) (AnjutaShell  *shell, GError **error);
	AnjutaUI* (*get_ui) (AnjutaShell  *shell, GError **error);
	AnjutaPreferences* (*get_preferences) (AnjutaShell *shell, GError **error);
	AnjutaPluginManager* (*get_plugin_manager) (AnjutaShell *shell,
												GError **error);
	AnjutaProfileManager* (*get_profile_manager) (AnjutaShell *shell,
												  GError **error);
	
	void (*add_widget_full)   (AnjutaShell  *shell,
							   GtkWidget    *widget,
							   const char   *name,
							   const char   *title,
							   const char   *stock_id,
							   AnjutaShellPlacement placement,
							   gboolean locked,
							   GError      **error);
	void (*add_widget_custom)  (AnjutaShell  *shell,
	                            GtkWidget    *widget,
	                            const char   *name,
	                            const char   *title,
	                            const char   *stock_id,
	                            GtkWidget    *label,
	                            AnjutaShellPlacement placement,
	                            GError      **error);
	
	void (*remove_widget)     (AnjutaShell  *shell,
							   GtkWidget    *widget,
							   GError      **error);
	void (*present_widget)    (AnjutaShell  *shell,
							   GtkWidget    *widget,
							   GError      **error);
	
	void (*iconify_dockable_widget) (AnjutaShell *shell, 
                               GtkWidget    *widget,
                               GError      **error);
	void (*hide_dockable_widget) (AnjutaShell *shell, 
                               GtkWidget    *widget,
                               GError      **error);
	void (*show_dockable_widget) (AnjutaShell *shell, 
                               GtkWidget    *widget,
                               GError      **error);
	void (*maximize_widget)   (AnjutaShell  *shell,
	                           const char   *widget_name,
                               GError      **error);
	void (*unmaximize)        (AnjutaShell  *shell,
                               GError      **error);
	void (*add_value)         (AnjutaShell  *shell,
							   const char   *name,
							   const GValue *value,
							   GError       **error);
	void (*get_value)         (AnjutaShell  *shell,
							   const char   *name,
							   GValue       *value,
							   GError       **error);
	void (*remove_value)      (AnjutaShell  *shell,
							   const char   *name,
							   GError       **error);
	void (*saving_push)       (AnjutaShell* shell);
	void (*saving_pop)        (AnjutaShell* shell);
	GObject* (*get_object)    (AnjutaShell  *shell,
							   const char   *iface_name,
							   GError       **error);
	AnjutaShell * (*create_window) (AnjutaShell *shell,
	                            GError      **error);
};

GQuark anjuta_shell_error_quark     (void);
GType  anjuta_shell_get_type        (void);

AnjutaStatus* anjuta_shell_get_status (AnjutaShell *shell, GError **error);

AnjutaUI* anjuta_shell_get_ui (AnjutaShell *shell, GError **error);

AnjutaPreferences* anjuta_shell_get_preferences (AnjutaShell *shell,
												 GError **error);

AnjutaPluginManager* anjuta_shell_get_plugin_manager (AnjutaShell *shell,
													  GError **error);

AnjutaProfileManager* anjuta_shell_get_profile_manager (AnjutaShell *shell,
														GError **error);

void anjuta_shell_freeze (AnjutaShell *shell, GError **error);
void anjuta_shell_thaw (AnjutaShell *shell, GError **error);

void   anjuta_shell_add_widget      (AnjutaShell     *shell,
									 GtkWidget       *widget,
									 const char      *name,
									 const char      *title,
									 const char      *stock_id,
									 AnjutaShellPlacement placement,
									 GError         **error);

void   anjuta_shell_add_widget_custom (AnjutaShell   *shell,
                                       GtkWidget     *widget,
                                       const char    *name,
                                       const char    *title,
                                       const char    *stock_id,
                                       GtkWidget     *label,
                                       AnjutaShellPlacement placement,
                                       GError        **error);

void   anjuta_shell_add_widget_full      (AnjutaShell     *shell,
									 GtkWidget       *widget,
									 const char      *name,
									 const char      *title,
									 const char      *stock_id,
									 AnjutaShellPlacement placement,
									 gboolean locked,
									 GError         **error);

void   anjuta_shell_remove_widget   (AnjutaShell     *shell,
									 GtkWidget       *widget,
									 GError         **error);
void   anjuta_shell_present_widget  (AnjutaShell     *shell,
									 GtkWidget       *widget,
									 GError         **error);
void anjuta_shell_iconify_dockable_widget (AnjutaShell *shell, 
                                     GtkWidget   *widget,
                                     GError      **error);
void anjuta_shell_hide_dockable_widget (AnjutaShell *shell, 
                                     GtkWidget   *widget,
                                     GError      **error);
void anjuta_shell_show_dockable_widget (AnjutaShell *shell, 
                                     GtkWidget   *widget,
                                     GError      **error);
void anjuta_shell_maximize_widget   (AnjutaShell *shell,
                                     const char  *widget_name,
                                     GError      **error);
void anjuta_shell_unmaximize        (AnjutaShell *shell,
                                     GError      **error);
AnjutaShell* anjuta_shell_create_window (AnjutaShell* shell,
                                      GError **error);
void   anjuta_shell_add_value       (AnjutaShell     *shell,
									 const char      *name,
									 const GValue    *value,
									 GError         **error);
void   anjuta_shell_add_valist      (AnjutaShell     *shell,
									 const char      *first_name,
									 GType            first_type,
									 va_list          var_args);
void   anjuta_shell_add             (AnjutaShell     *shell,
									 const char      *first_name,
									 GType            first_type,
									 ...);
void   anjuta_shell_get_value       (AnjutaShell     *shell,
									 const char      *name,
									 GValue          *value,
									 GError         **error);
void   anjuta_shell_get_valist      (AnjutaShell     *shell,
									 const char      *first_name,
									 GType            first_type,
									 va_list          var_args);
void   anjuta_shell_get             (AnjutaShell     *shell,
									 const char      *first_name,
									 GType            first_type,
									 ...);
void   anjuta_shell_remove_value    (AnjutaShell     *shell,
									 const char      *name,
									 GError         **error);

GObject *anjuta_shell_get_object    (AnjutaShell *shell,
									 const gchar *iface_name,
									 GError **error);
void anjuta_shell_session_save      (AnjutaShell *shell,
									 const gchar *session_directory,
									 GError **error);
void anjuta_shell_session_load      (AnjutaShell *shell,
									 const gchar *session_directory,
									 GError **error);
void anjuta_shell_save_prompt       (AnjutaShell *shell,
									 AnjutaSavePrompt *prompt,
									 GError **error);
void anjuta_shell_saving_push	    (AnjutaShell* shell);
void anjuta_shell_saving_pop	    (AnjutaShell* shell);


/**
 * anjuta_shell_get_interface:
 * @shell: A #AnjutaShell object
 * @iface_type: The interface type implemented by the object to be found
 * @error: Error propagation object.
 *
 * Equivalent to anjuta_shell_get_object(), but additionally typecasts returned
 * object to the interface type. It also takes interface type directly. A
 * usage of this function is:
 * <programlisting>
 * IAnjutaDocumentManager *docman =
 *     anjuta_shell_get_interface (shell, IAnjutaDocumentManager, error);
 * </programlisting>
 */
#define anjuta_shell_get_interface(shell, iface_type, error) \
	((iface_type*) anjuta_shell_get_object((shell), #iface_type, (error)))

G_END_DECLS

#endif
