/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-message-manager.c -- Autogenerated from libanjuta.idl
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
 * SECTION:ianjuta-message-manager
 * @title: IAnjutaMessageManager
 * @short_description: The plugin that managers all message views
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-message-manager.h
 *
 */

#include "ianjuta-message-manager.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_message_manager_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-message-manager-quark");
	}
	
	return quark;
}

/**
 * ianjuta_message_manager_add_view:
 * @obj: Self
 * @name: Name/Title of the new view
 * @icon: Path to an icon or ""
 * @err: Error propagation and reporting
 *
 * Adds a new view to the message-manager
 *
 * Return value: The new message-view
 */
IAnjutaMessageView*
ianjuta_message_manager_add_view (IAnjutaMessageManager *obj, const gchar *name,   const gchar *icon, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj), NULL);
	return IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->add_view (obj, name, icon, err);
}

/* Default implementation */
static IAnjutaMessageView*
ianjuta_message_manager_add_view_default (IAnjutaMessageManager *obj, const gchar *name,   const gchar *icon, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_message_manager_get_all_views:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Get all message-views
 *
 * Return value: (element-type IAnjutaMessageView): A GList* of all views. You must not
 * manipulate the list.
 */
GList*
ianjuta_message_manager_get_all_views (IAnjutaMessageManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj), 0);
	return IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->get_all_views (obj, err);
}

/* Default implementation */
static GList*
ianjuta_message_manager_get_all_views_default (IAnjutaMessageManager *obj, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_message_manager_get_current_view:
 * @obj: Self
 * @err: Error propagation and reporting
 *
 * Get the view with is currently on top of
 * the notebook or NULL if the message-manager is empty.
 *
 * Return value: Current view; #IAnjutaMessageView object.
 * NULL, if there is no views.
 */
IAnjutaMessageView*
ianjuta_message_manager_get_current_view (IAnjutaMessageManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj), NULL);
	return IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->get_current_view (obj, err);
}

/* Default implementation */
static IAnjutaMessageView*
ianjuta_message_manager_get_current_view_default (IAnjutaMessageManager *obj, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_message_manager_get_view_by_name:
 * @obj: Self
 * @name: Name/Title of the view
 * @err: Error propagation and reporting
 *
 * Get the view with the given name or NULL if
 * it does not exist.
 *
 * Return value: The message-view or NULL
 */
IAnjutaMessageView*
ianjuta_message_manager_get_view_by_name (IAnjutaMessageManager *obj, const gchar *name, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj), NULL);
	return IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->get_view_by_name (obj, name, err);
}

/* Default implementation */
static IAnjutaMessageView*
ianjuta_message_manager_get_view_by_name_default (IAnjutaMessageManager *obj, const gchar *name, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_message_manager_remove_view:
 * @obj: Self
 * @view: The view to remove
 * @err: Error propagation and reporting
 *
 * Remove view from the message-manager. The view
 * will become invalid.
 */
void
ianjuta_message_manager_remove_view (IAnjutaMessageManager *obj, IAnjutaMessageView *view, GError **err)
{
	g_return_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj));
	g_return_if_fail ((view == NULL) ||IANJUTA_IS_MESSAGE_VIEW(view));
	IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->remove_view (obj, view, err);
}

/* Default implementation */
static void
ianjuta_message_manager_remove_view_default (IAnjutaMessageManager *obj, IAnjutaMessageView *view, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_message_manager_set_current_view:
 * @obj: Self
 * @view: A message view
 * @err: Error propagation and reporting
 *
 * Set view to be on top of the notebook.
 *
 */
void
ianjuta_message_manager_set_current_view (IAnjutaMessageManager *obj, IAnjutaMessageView *view, GError **err)
{
	g_return_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj));
	g_return_if_fail ((view == NULL) ||IANJUTA_IS_MESSAGE_VIEW(view));
	IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->set_current_view (obj, view, err);
}

/* Default implementation */
static void
ianjuta_message_manager_set_current_view_default (IAnjutaMessageManager *obj, IAnjutaMessageView *view, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_message_manager_set_view_icon:
 * @obj: Self
 * @view: A message view
 * @icon: Sets the icon of view.
 * @err: Error propagation and reporting
 *
 * Sets the icon of view.
 *
 */
void
ianjuta_message_manager_set_view_icon (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   GdkPixbufAnimation *icon, GError **err)
{
	g_return_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj));
	g_return_if_fail ((view == NULL) ||IANJUTA_IS_MESSAGE_VIEW(view));
	g_return_if_fail ((icon == NULL) ||GDK_IS_PIXBUF_ANIMATION(icon));
	IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->set_view_icon (obj, view, icon, err);
}

/* Default implementation */
static void
ianjuta_message_manager_set_view_icon_default (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   GdkPixbufAnimation *icon, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_message_manager_set_view_icon_from_stock:
 * @obj: Self
 * @view: A message view
 * @icon: Sets the icon of view.
 * @err: Error propagation and reporting
 *
 * Sets the icon of view.
 *
 */
void
ianjuta_message_manager_set_view_icon_from_stock (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   const gchar *icon, GError **err)
{
	g_return_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj));
	g_return_if_fail ((view == NULL) ||IANJUTA_IS_MESSAGE_VIEW(view));
	IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->set_view_icon_from_stock (obj, view, icon, err);
}

/* Default implementation */
static void
ianjuta_message_manager_set_view_icon_from_stock_default (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   const gchar *icon, GError **err)
{
	g_return_if_reached ();
}

/**
 * ianjuta_message_manager_set_view_title:
 * @obj: Self
 * @view: A message view
 * @title: Sets the title of view.
 * @err: Error propagation and reporting
 *
 * Sets the title of view.
 *
 */
void
ianjuta_message_manager_set_view_title (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   const gchar *title, GError **err)
{
	g_return_if_fail (IANJUTA_IS_MESSAGE_MANAGER(obj));
	g_return_if_fail ((view == NULL) ||IANJUTA_IS_MESSAGE_VIEW(view));
	IANJUTA_MESSAGE_MANAGER_GET_IFACE (obj)->set_view_title (obj, view, title, err);
}

/* Default implementation */
static void
ianjuta_message_manager_set_view_title_default (IAnjutaMessageManager *obj, IAnjutaMessageView *view,   const gchar *title, GError **err)
{
	g_return_if_reached ();
}

static void
ianjuta_message_manager_base_init (IAnjutaMessageManagerIface* klass)
{
	static gboolean initialized = FALSE;

	klass->add_view = ianjuta_message_manager_add_view_default;
	klass->get_all_views = ianjuta_message_manager_get_all_views_default;
	klass->get_current_view = ianjuta_message_manager_get_current_view_default;
	klass->get_view_by_name = ianjuta_message_manager_get_view_by_name_default;
	klass->remove_view = ianjuta_message_manager_remove_view_default;
	klass->set_current_view = ianjuta_message_manager_set_current_view_default;
	klass->set_view_icon = ianjuta_message_manager_set_view_icon_default;
	klass->set_view_icon_from_stock = ianjuta_message_manager_set_view_icon_from_stock_default;
	klass->set_view_title = ianjuta_message_manager_set_view_title_default;
	
	if (!initialized) {

		initialized = TRUE;
	}
}

GType
ianjuta_message_manager_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaMessageManagerIface),
			(GBaseInitFunc) ianjuta_message_manager_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaMessageManager", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;			
}

GType
ianjuta_message_manager_error_get_type (void)
{
	static const GEnumValue values[] =
	{
		{ IANJUTA_MESSAGE_MANAGER_DOESNT_EXIST, "IANJUTA_MESSAGE_MANAGER_DOESNT_EXIST", "doesnt-exist" }, 
		{ 0, NULL, NULL }
	};

	static GType type = 0;

	if (! type)
	{
		type = g_enum_register_static ("IAnjutaMessageManagerError", values);
	}

	return type;
}
