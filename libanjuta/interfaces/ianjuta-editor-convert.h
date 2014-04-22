/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-editor-convert.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_EDITOR_CONVERT_H_
#define _IANJUTA_EDITOR_CONVERT_H_

#include <glib-object.h>
#include <libanjuta/interfaces/ianjuta-editor.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_EDITOR_CONVERT (ianjuta_editor_convert_get_type ())
#define IANJUTA_EDITOR_CONVERT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_EDITOR_CONVERT, IAnjutaEditorConvert))
#define IANJUTA_IS_EDITOR_CONVERT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_EDITOR_CONVERT))
#define IANJUTA_EDITOR_CONVERT_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_EDITOR_CONVERT, IAnjutaEditorConvertIface))

#define IANJUTA_EDITOR_CONVERT_ERROR ianjuta_editor_convert_error_quark()

typedef struct _IAnjutaEditorConvert IAnjutaEditorConvert;
typedef struct _IAnjutaEditorConvertIface IAnjutaEditorConvertIface;


struct _IAnjutaEditorConvertIface {
	IAnjutaEditorIface g_iface;
	

	void (*to_lower) (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,  IAnjutaIterable *end_position, GError **err);
	void (*to_upper) (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,  IAnjutaIterable *end_position, GError **err);

};


GQuark ianjuta_editor_convert_error_quark     (void);
GType  ianjuta_editor_convert_get_type        (void);

void ianjuta_editor_convert_to_lower (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,  IAnjutaIterable *end_position, GError **err);

void ianjuta_editor_convert_to_upper (IAnjutaEditorConvert *obj, IAnjutaIterable *start_position,  IAnjutaIterable *end_position, GError **err);


G_END_DECLS

#endif