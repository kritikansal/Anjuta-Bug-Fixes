/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-file-loader.c -- Autogenerated from libanjuta.idl
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

/**
 * SECTION:ianjuta-file-loader
 * @title: IAnjutaFileLoader
 * @short_description: Loader to load files
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-file-loader.h
 *
 * Loaders can deterime correct plugin to open a file.
 */

#include "ianjuta-file-loader.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_file_loader_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-file-loader-quark");
	}
	
	return quark;
}

/**
 * ianjuta_file_loader_load:
 * @obj: Self
 * @file: File to load
 * @readonly: Open in readonly mode.
 * @err: Error propagation and reporting
 *
 * Determines a plugin which can open the given file, activates it
 * opening the file and returns the interface of the plugin activated.
 *
 * Return value: Plugin interface used to load the file.
 */
GObject*
ianjuta_file_loader_load (IAnjutaFileLoader *obj, GFile* file,   gboolean readonly, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_FILE_LOADER(obj), NULL);
	return IANJUTA_FILE_LOADER_GET_IFACE (obj)->load (obj, file, readonly, err);
}

/* Default implementation */
static GObject*
ianjuta_file_loader_load_default (IAnjutaFileLoader *obj, GFile* file,   gboolean readonly, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_loader_peek_interface:
 * @obj: Self
 * @file: Meta file to peek
 * @err: Error propagation and reporting
 *
 * Peeks the file and determines the interface which can load
 * this file.
 *
 * Return value: Plugin interface name that can load the file.
 */
gchar*
ianjuta_file_loader_peek_interface (IAnjutaFileLoader *obj, GFile* file, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_FILE_LOADER(obj), NULL);
	return IANJUTA_FILE_LOADER_GET_IFACE (obj)->peek_interface (obj, file, err);
}

/* Default implementation */
static gchar*
ianjuta_file_loader_peek_interface_default (IAnjutaFileLoader *obj, GFile* file, GError **err)
{
	g_return_val_if_reached (NULL);
}

static void
ianjuta_file_loader_base_init (IAnjutaFileLoaderIface* klass)
{
	static gboolean initialized = FALSE;

	klass->load = ianjuta_file_loader_load_default;
	klass->peek_interface = ianjuta_file_loader_peek_interface_default;
	
	if (!initialized) {

		initialized = TRUE;
	}
}

GType
ianjuta_file_loader_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaFileLoaderIface),
			(GBaseInitFunc) ianjuta_file_loader_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaFileLoader", &info, 0);
		g_type_interface_add_prerequisite (type, IANJUTA_TYPE_LOADER);
	}
	return type;			
}
