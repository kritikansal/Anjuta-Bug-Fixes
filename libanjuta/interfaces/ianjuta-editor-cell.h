/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-editor-cell.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_EDITOR_CELL_H_
#define _IANJUTA_EDITOR_CELL_H_

#include <glib-object.h>
#include <libanjuta/interfaces/ianjuta-editor.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_EDITOR_CELL (ianjuta_editor_cell_get_type ())
#define IANJUTA_EDITOR_CELL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_EDITOR_CELL, IAnjutaEditorCell))
#define IANJUTA_IS_EDITOR_CELL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_EDITOR_CELL))
#define IANJUTA_EDITOR_CELL_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_EDITOR_CELL, IAnjutaEditorCellIface))

#define IANJUTA_EDITOR_CELL_ERROR ianjuta_editor_cell_error_quark()

typedef struct _IAnjutaEditorCell IAnjutaEditorCell;
typedef struct _IAnjutaEditorCellIface IAnjutaEditorCellIface;


struct _IAnjutaEditorCellIface {
	GTypeInterface g_iface;
	

	IAnjutaEditorAttribute (*get_attribute) (IAnjutaEditorCell *obj, GError **err);
	gchar (*get_char) (IAnjutaEditorCell *obj, gint char_index, GError **err);
	gchar * (*get_character) (IAnjutaEditorCell *obj, GError **err);
	gint (*get_length) (IAnjutaEditorCell *obj, GError **err);

};


GQuark ianjuta_editor_cell_error_quark     (void);
GType  ianjuta_editor_cell_get_type        (void);

IAnjutaEditorAttribute ianjuta_editor_cell_get_attribute (IAnjutaEditorCell *obj, GError **err);

gchar ianjuta_editor_cell_get_char (IAnjutaEditorCell *obj, gint char_index, GError **err);

gchar * ianjuta_editor_cell_get_character (IAnjutaEditorCell *obj, GError **err);

gint ianjuta_editor_cell_get_length (IAnjutaEditorCell *obj, GError **err);


G_END_DECLS

#endif
