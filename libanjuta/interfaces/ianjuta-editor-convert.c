/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-editor-convert.c -- Autogenerated from libanjuta.idl
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
 * SECTION:ianjuta-editor-convert
 * @title: IAnjutaEditorConvert
 * @short_description: Text editor convert interface
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-editor-convert.h
 *
 */

#include "ianjuta-editor-convert.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_editor_convert_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-editor-convert-quark");
	}
	
	return quark;
}

/**
 * ianjuta_editor_convert_to_lower:
 * @obj: Self
 * @start_position: Start position.
 * @end_position: End position.
 * @err: Error propagation and reporting
 *
 * change characters from start position to end position to lowercase
 *
 */
void
ianjuta_editor_convert_to_lower (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,   IAnjutaIterable *end_position, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_CONVERT(obj));
	g_return_if_fail ((start_position == NULL) ||IANJUTA_IS_ITERABLE(start_position));
	g_return_if_fail ((end_position == NULL) ||IANJUTA_IS_ITERABLE(end_position));
	IANJUTA_EDITOR_CONVERT_GET_IFACE (obj)->to_lower (obj, start_position, end_position, err);
}

/* Default implementation */
static void
ianjuta_editor_convert_to_lower_default (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,   IAnjutaIterable *end_position, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_editor_convert_to_upper:
 * @obj: Self
 * @start_position: Start position.
 * @end_position: End position.
 * @err: Error propagation and reporting
 *
 * change characters from start position to end position to uppercase.
 *
 */
void
ianjuta_editor_convert_to_upper (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,   IAnjutaIterable *end_position, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_CONVERT(obj));
	g_return_if_fail ((start_position == NULL) ||IANJUTA_IS_ITERABLE(start_position));
	g_return_if_fail ((end_position == NULL) ||IANJUTA_IS_ITERABLE(end_position));
	IANJUTA_EDITOR_CONVERT_GET_IFACE (obj)->to_upper (obj, start_position, end_position, err);
}

/* Default implementation */
static void
ianjuta_editor_convert_to_upper_default (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,   IAnjutaIterable *end_position, GError **err)
{
	g_return_if_reached ();
}

static void
ianjuta_editor_convert_base_init (IAnjutaEditorConvertIface* klass)
{
	static gboolean initialized = FALSE;

	klass->to_lower = ianjuta_editor_convert_to_lower_default;
	klass->to_upper = ianjuta_editor_convert_to_upper_default;
	
	if (!initialized) {

		initialized = TRUE;
	}
}

GType
ianjuta_editor_convert_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaEditorConvertIface),
			(GBaseInitFunc) ianjuta_editor_convert_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaEditorConvert", &info, 0);
		g_type_interface_add_prerequisite (type, IANJUTA_TYPE_EDITOR);
	}
	return type;			
}
