/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-editor-tip.c -- Autogenerated from libanjuta.idl
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
 * SECTION:ianjuta-editor-tip
 * @title: IAnjutaEditorTip
 * @short_description: Editor call tips assistance framework
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-editor-tip.h
 *
 */

#include "ianjuta-editor-tip.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_editor_tip_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-editor-tip-quark");
	}
	
	return quark;
}

/**
 * ianjuta_editor_tip_cancel:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Cancels the last shown tooltip
 */
void
ianjuta_editor_tip_cancel (IAnjutaEditorTip *obj, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_TIP(obj));
	IANJUTA_EDITOR_TIP_GET_IFACE (obj)->cancel (obj, err);
}

/* Default implementation */
static void
ianjuta_editor_tip_cancel_default (IAnjutaEditorTip *obj, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_editor_tip_show:
 * @obj: Self
 * @tips: (element-type utf8): list of alternative tips.
 * @position: Tip position.
 * @err: Error propagation and reporting
 *
 * Show tips showing more information on current context. No user feedback
 * is required when tips are shown. @position indicates
 * the position before which is the known context and after which are
 * the suggestions. Usually the editor would use this to
 * align the choices displayed such that the carat is just at this
 * position when the choices are displayed.
 *
 */
void
ianjuta_editor_tip_show (IAnjutaEditorTip *obj, GList* tips,   IAnjutaIterable *position, GError **err)
{
	g_return_if_fail (IANJUTA_IS_EDITOR_TIP(obj));
	g_return_if_fail ((position == NULL) ||IANJUTA_IS_ITERABLE(position));
	IANJUTA_EDITOR_TIP_GET_IFACE (obj)->show (obj, tips, position, err);
}

/* Default implementation */
static void
ianjuta_editor_tip_show_default (IAnjutaEditorTip *obj, GList* tips,   IAnjutaIterable *position, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_editor_tip_visible:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Returns: whether a tooltip is crrently shown
 */
gboolean
ianjuta_editor_tip_visible (IAnjutaEditorTip *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_EDITOR_TIP(obj), FALSE);
	return IANJUTA_EDITOR_TIP_GET_IFACE (obj)->visible (obj, err);
}

/* Default implementation */
static gboolean
ianjuta_editor_tip_visible_default (IAnjutaEditorTip *obj, GError **err)
{
	g_return_val_if_reached (FALSE);
}

static void
ianjuta_editor_tip_base_init (IAnjutaEditorTipIface* klass)
{
	static gboolean initialized = FALSE;

	klass->cancel = ianjuta_editor_tip_cancel_default;
	klass->show = ianjuta_editor_tip_show_default;
	klass->visible = ianjuta_editor_tip_visible_default;
	
	if (!initialized) {

		initialized = TRUE;
	}
}

GType
ianjuta_editor_tip_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaEditorTipIface),
			(GBaseInitFunc) ianjuta_editor_tip_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaEditorTip", &info, 0);
		g_type_interface_add_prerequisite (type, IANJUTA_TYPE_EDITOR);
	}
	return type;			
}