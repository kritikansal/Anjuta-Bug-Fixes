/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-editor-view.c -- Autogenerated from libanjuta.idl
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
 * SECTION:ianjuta-editor-view
 * @title: IAnjutaEditorView
 * @short_description: Text editor view interface
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-editor-view.h
 *
 * An editor view is a visual representation of the editor. An editor
 * can have multiple views. All views of an editor show the same editor
 * content (buffer). Consequently, any change done in one view is
 * updated in all other views.
 */

#include "ianjuta-editor-view.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_editor_view_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-editor-view-quark");
	}
	
	return quark;
}

/**
 * ianjuta_editor_view_create:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Creates a new view for the editor. The newly created view gets
 * the user focus and scrolls to the same location as last view.
 */
void
ianjuta_editor_view_create (IAnjutaEditorView *obj, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_VIEW(obj));
	IANJUTA_EDITOR_VIEW_GET_IFACE (obj)->create (obj, err);
}

/* Default implementation */
static void
ianjuta_editor_view_create_default (IAnjutaEditorView *obj, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_editor_view_get_count:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Total number of views currently present. It will never be less
 * than 1. Invalid return values are considered error condition.
 */
gint
ianjuta_editor_view_get_count (IAnjutaEditorView *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_EDITOR_VIEW(obj), -1);
	return IANJUTA_EDITOR_VIEW_GET_IFACE (obj)->get_count (obj, err);
}

/* Default implementation */
static gint
ianjuta_editor_view_get_count_default (IAnjutaEditorView *obj, GError **err)
{
	g_return_val_if_reached (-1);
}

/**
 * ianjuta_editor_view_remove_current:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Removes currently focused editor view. It does not remove the
 * last view of the editor. That is, if currently there is only
 * one view of the editor, this function does nothing.
 */
void
ianjuta_editor_view_remove_current (IAnjutaEditorView *obj, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_VIEW(obj));
	IANJUTA_EDITOR_VIEW_GET_IFACE (obj)->remove_current (obj, err);
}

/* Default implementation */
static void
ianjuta_editor_view_remove_current_default (IAnjutaEditorView *obj, GError **err)
{
	g_return_if_reached ();
}

static void
ianjuta_editor_view_base_init (IAnjutaEditorViewIface* klass)
{
	static gboolean initialized = FALSE;

	klass->create = ianjuta_editor_view_create_default;
	klass->get_count = ianjuta_editor_view_get_count_default;
	klass->remove_current = ianjuta_editor_view_remove_current_default;
	
	if (!initialized) {

		initialized = TRUE;
	}
}

GType
ianjuta_editor_view_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaEditorViewIface),
			(GBaseInitFunc) ianjuta_editor_view_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaEditorView", &info, 0);
		g_type_interface_add_prerequisite (type, IANJUTA_TYPE_EDITOR);
	}
	return type;			
}
