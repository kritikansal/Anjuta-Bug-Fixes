/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-symbol-manager.h -- Autogenerated from libanjuta.idl
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _IANJUTA_SYMBOL_MANAGER_H_
#define _IANJUTA_SYMBOL_MANAGER_H_

#include <glib-object.h>
#include <libanjuta/anjuta-async-notify.h>
#include <libanjuta/interfaces/ianjuta-iterable.h>
#include <libanjuta/interfaces/ianjuta-symbol.h>
#include <libanjuta/interfaces/ianjuta-symbol-query.h>
#include <libanjuta/interfaces/ianjuta-editor.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_SYMBOL_MANAGER (ianjuta_symbol_manager_get_type ())
#define IANJUTA_SYMBOL_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_SYMBOL_MANAGER, IAnjutaSymbolManager))
#define IANJUTA_IS_SYMBOL_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_SYMBOL_MANAGER))
#define IANJUTA_SYMBOL_MANAGER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_SYMBOL_MANAGER, IAnjutaSymbolManagerIface))

#define IANJUTA_SYMBOL_MANAGER_ERROR ianjuta_symbol_manager_error_quark()

typedef struct _IAnjutaSymbolManager IAnjutaSymbolManager;
typedef struct _IAnjutaSymbolManagerIface IAnjutaSymbolManagerIface;


struct _IAnjutaSymbolManagerIface {
	GTypeInterface g_iface;
	
	/* Signal */
	void (*prj_scan_end) (IAnjutaSymbolManager *obj, gint process_id);
	/* Signal */
	void (*sys_scan_end) (IAnjutaSymbolManager *obj, gint process_id);

	gboolean (*activate_package) (IAnjutaSymbolManager *obj, const gchar *pkg_name,  const gchar* pkg_version, GError **err);
	gboolean (*add_package) (IAnjutaSymbolManager *obj, const gchar* pkg_name,  const gchar* pkg_version,  GList* files, GError **err);
	IAnjutaSymbolQuery* (*create_query) (IAnjutaSymbolManager *obj, IAnjutaSymbolQueryName name,  IAnjutaSymbolQueryDb db, GError **err);
	void (*deactivate_all) (IAnjutaSymbolManager *obj, GError **err);
	void (*deactivate_package) (IAnjutaSymbolManager *obj, const gchar* pkg_name,  const gchar* pkg_version, GError **err);

};


GQuark ianjuta_symbol_manager_error_quark     (void);
GType  ianjuta_symbol_manager_get_type        (void);

gboolean ianjuta_symbol_manager_activate_package (IAnjutaSymbolManager *obj, const gchar *pkg_name,  const gchar* pkg_version, GError **err);

gboolean ianjuta_symbol_manager_add_package (IAnjutaSymbolManager *obj, const gchar* pkg_name,  const gchar* pkg_version,  GList* files, GError **err);

IAnjutaSymbolQuery* ianjuta_symbol_manager_create_query (IAnjutaSymbolManager *obj, IAnjutaSymbolQueryName name,  IAnjutaSymbolQueryDb db, GError **err);

void ianjuta_symbol_manager_deactivate_all (IAnjutaSymbolManager *obj, GError **err);

void ianjuta_symbol_manager_deactivate_package (IAnjutaSymbolManager *obj, const gchar* pkg_name,  const gchar* pkg_version, GError **err);


G_END_DECLS

#endif
