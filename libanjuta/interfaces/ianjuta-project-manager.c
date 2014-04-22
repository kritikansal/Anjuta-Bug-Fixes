/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-project-manager.c -- Autogenerated from libanjuta.idl
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
 * SECTION:ianjuta-project-manager
 * @title: IAnjutaProjectManager
 * @short_description: Interface for project managers
 * @see_also:
 * @stability: Unstable
 * @include: libanjuta/interfaces/ianjuta-project-manager.h
 *
 */

#include "ianjuta-project-manager.h"
#include "libanjuta-iface-marshallers.h"

GQuark 
ianjuta_project_manager_error_quark (void)
{
	static GQuark quark = 0;
	
	if (quark == 0) {
		quark = g_quark_from_static_string ("ianjuta-project-manager-quark");
	}
	
	return quark;
}

/**
 * ianjuta_project_manager_add_group:
 * @obj: Self.
 * @name: Group name or URI.
 * @default_group: (allow-none): A #GFile corresponding to the default parent group or
 *					%NULL if don't care.
 * @err: Error propagation and reporting.
 *
 * Prompts the user to add a new group to the project. The user can select
 * a parent group different from the one set as default.
 *
 * Returns: (transfer full): A #GFile corresponding to the new group added in the project.
 * You own the returned file; use g_object_unref() to release it.
 */
GFile*
ianjuta_project_manager_add_group (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_group, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->add_group (obj, name, default_group, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_add_group_default (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_group, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_add_source:
 * @obj: Self.
 * @name: Source name or URI.
 * @default_target: (allow-none): A #GFile corresponding to the default target or group or
 *					%NULL if you don't care.
 * @err: Error propagation and reporting.
 *
 * Prompts the user to add a file to the project. If the user selects
 * multiple files only the first source file is returned.
 *
 * You can add non existing file. In this case the element_added
 * signal will be emitted with a non existing file. So it is
 * up to the caller to reemit this signal later when the file
 * is created.
 *
 * Returns: (transfer full): A #GFile corresponding to the new source file in the
 * project view. You own the returned file; use g_object_unref() to release it.
 */
GFile*
ianjuta_project_manager_add_source (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_target, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->add_source (obj, name, default_target, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_add_source_default (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_target, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_add_source_quiet:
 * @obj: Self.
 * @name: Source name or URI.
 * @target: A #GFile corresponding to the parent target or group.
 * @err: Error propagation and reporting.
 *
 * Adds a file to the project without prompting the user.
 *
 * You can add non existing file. In this case the element_added
 * signal will be emitted with a non existing file. So it is
 * up to the caller to reemit this signal later when the file
 * is created.
 *
 * Returns: (transfer full): A #GFile corresponding to the new source file in the project
 * view. You own the returned file; use g_object_unref() to release it.
 */
GFile*
ianjuta_project_manager_add_source_quiet (IAnjutaProjectManager *obj, const gchar *name,   GFile *target, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->add_source_quiet (obj, name, target, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_add_source_quiet_default (IAnjutaProjectManager *obj, const gchar *name,   GFile *target, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_add_sources:
 * @obj: Self.
 * @names: (element-type utf8): Sources name or URI to add.
 * @default_target: (allow-none): A #GFile corresponding to the default target or group or
 *					%NULL if don't care.
 * @err: Error propagation and reporting.
 *
 * Prompts the user to add several files to the project. Depending on the
 * project backend, it can be possible that the source files must
 * be located in a particular directory.
 *
 * You can add non existing file. In this case the element_added
 * signal will be emitted with a non existing file. So it is
 * up to the caller to reemit this signal later when the file
 * is created.
 *
 * Returns: (element-type GFile) (transfer full): A list of #GFile corresponding to all
 * new source files added in the project. You own the list with the the returned files;
 * use g_list_free() and g_object_unref() on each file to release them.
 */
GList*
ianjuta_project_manager_add_sources (IAnjutaProjectManager *obj, GList* names,   GFile *default_target, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->add_sources (obj, names, default_target, err);
}

/* Default implementation */
static GList*
ianjuta_project_manager_add_sources_default (IAnjutaProjectManager *obj, GList* names,   GFile *default_target, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_add_target:
 * @obj: Self
 * @name: Target name or URI.
 * @default_group: (allow-none): A #GFile corresponding to the default parent group or
 *					%NULL if don't care.
 * @err: Error propagation and reporting.
 *
 * Prompts the user to add a new target to the project. The user can select
 * a parent group different from the one set as default.
 *
 * Returns: (transfer full): A #GFile corresponding to the new target added in the project.
 * You own the returned file; use g_object_unref() to release it.
 */
GFile*
ianjuta_project_manager_add_target (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_group, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->add_target (obj, name, default_group, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_add_target_default (IAnjutaProjectManager *obj, const gchar *name,   GFile *default_group, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
* ianjuta_project_manager_get_capabilities:
* @obj: Self
* @err: Error propagation and reporting.
*
* Gets the capabilites of project whether it can add group, target
* sources etc.
*
* Returns: Supported capabilites.
*/
guint
ianjuta_project_manager_get_capabilities (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_capabilities (obj, err);
}

/* Default implementation */
static guint
ianjuta_project_manager_get_capabilities_default (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_reached (0);
}

/**
 * ianjuta_project_manager_get_children:
 * @obj: Self
 * @parent: A #GFile corresponding to the parent.
 * @children_type: Select one element type: source, group or target
 * @err: Error propagation and reporting.
 *
 * Recursively gets the list of all children below the corresponding
 * parent having the specify type.
 *
 * Returns: (element-type GFile) (transfer full): The list of #GFile
 * corresponding to all children or %NULL if the element has no
 * children with the corresponding type. Free the returned * list
 * with g_list_free() and the files with g_object_unref().
 */
GList*
ianjuta_project_manager_get_children (IAnjutaProjectManager *obj, GFile *parent,   gint children_type, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_children (obj, parent, children_type, err);
}

/* Default implementation */
static GList*
ianjuta_project_manager_get_children_default (IAnjutaProjectManager *obj, GFile *parent,   gint children_type, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_get_current_project:
 * @obj: Self
 * @err: Error propagation and reporting.
 *
 * Gets the current project.
 *
 * Return value: (transfer none): the currently active project. NULL if none is there.
 */
IAnjutaProject*
ianjuta_project_manager_get_current_project (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_current_project (obj, err);
}

/* Default implementation */
static IAnjutaProject*
ianjuta_project_manager_get_current_project_default (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_get_elements:
 * @obj: Self
 * @element_type: Select one element type: source, group or target
 * @err: Error propagation and reporting.
 *
 * Get a list of all elements of this type in the project.
 *
 * Returns: (element-type GFile) (transfer full): Get list of #GFile corresponding to
 * all valid elements or %NULL if there are no elements of this type. Free the returned
 * list with g_list_free() and the files with g_object_unref().
 */
GList*
ianjuta_project_manager_get_elements (IAnjutaProjectManager *obj, AnjutaProjectNodeType element_type, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_elements (obj, element_type, err);
}

/* Default implementation */
static GList*
ianjuta_project_manager_get_elements_default (IAnjutaProjectManager *obj, AnjutaProjectNodeType element_type, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_get_packages:
 * @obj: Self
 * @err: Error propagation and reporting.
 *
 * Returns: (element-type utf8) (transfer container): the list of pkg-config packages that the current project
 * requires in it's configure.ac. Can be NULL if there is no project
 * opened currently or no package is required.
 */
GList*
ianjuta_project_manager_get_packages (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_packages (obj, err);
}

/* Default implementation */
static GList*
ianjuta_project_manager_get_packages_default (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
* ianjuta_project_manager_get_parent:
* @obj: Self
* @element: A #GFile corresponding to one child.
* @err: Error propagation and reporting.
*
* Gets the parent of the corresponding child.
*
* Returns: The parent of the child, or %NULL if the element is the root.
*/
GFile*
ianjuta_project_manager_get_parent (IAnjutaProjectManager *obj, GFile *element, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_parent (obj, element, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_get_parent_default (IAnjutaProjectManager *obj, GFile *element, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
 * ianjuta_project_manager_get_selected:
 * @obj: Self
 * @err: Error propagation and reporting.
 *
 * Gets the currently selected element in the project manager view.
 *
 * Returns: (transfer full): A #GFile corresponding to the selected element in the project
 * view. You own the returned file; use g_object_unref() to release it.
 */
GFile*
ianjuta_project_manager_get_selected (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), NULL);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_selected (obj, err);
}

/* Default implementation */
static GFile*
ianjuta_project_manager_get_selected_default (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
* ianjuta_project_manager_get_target_type:
* @obj: Self
* @target: A #GFile corresponding to a target
* @err: Error propagation and reporting.
*
* Get the type of the corresponding target: program, library...
*
* Returns: Return the type of the target.
*/
AnjutaProjectNodeType
ianjuta_project_manager_get_target_type (IAnjutaProjectManager *obj, GFile *target, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_target_type (obj, target, err);
}

/* Default implementation */
static AnjutaProjectNodeType
ianjuta_project_manager_get_target_type_default (IAnjutaProjectManager *obj, GFile *target, GError **err)
{
	g_return_val_if_reached (0);
}

/**
 * ianjuta_project_manager_get_targets:
 * @obj: Self
 * @target_type: type of the target
 * @err: Error propagation and reporting.
 *
 * Get a list of targets in the project with the corresponding type.
 *
 * Returns: (element-type GFile) (transfer full): A list of #GFile corresponding to
 * each target of the requested type or %NULL if none exists. Free the returned list
 * with g_list_free() and the files with g_object_unref().
 */
GList*
ianjuta_project_manager_get_targets (IAnjutaProjectManager *obj, AnjutaProjectNodeType target_type, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), 0);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->get_targets (obj, target_type, err);
}

/* Default implementation */
static GList*
ianjuta_project_manager_get_targets_default (IAnjutaProjectManager *obj, AnjutaProjectNodeType target_type, GError **err)
{
	g_return_val_if_reached (NULL);
}

/**
* ianjuta_project_manager_is_open:
* @obj: Self
* @err: Error propagation and reporting.
*
* Gets whether a project is currently opened.
*
* Returns: %TRUE if a project is opened.
*/
gboolean
ianjuta_project_manager_is_open (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), FALSE);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->is_open (obj, err);
}

/* Default implementation */
static gboolean
ianjuta_project_manager_is_open_default (IAnjutaProjectManager *obj, GError **err)
{
	g_return_val_if_reached (FALSE);
}

/**
 * ianjuta_project_manager_remove_file:
 * @obj: Self.
 * @file: A #GFile that will be removed from the project
 * @err: Error propagation and reporting.
 *
 * Remove a source file from the project. If the file is used in several
 * targets, it is removed from all targets. The file could be removed from
 * the disk.
 *
 * Returns: %TRUE if the file has been removed from the project else %FALSE
 */
gboolean
ianjuta_project_manager_remove_file (IAnjutaProjectManager *obj, GFile *file, GError **err)
{
	g_return_val_if_fail (IANJUTA_IS_PROJECT_MANAGER(obj), FALSE);
	return IANJUTA_PROJECT_MANAGER_GET_IFACE (obj)->remove_file (obj, file, err);
}

/* Default implementation */
static gboolean
ianjuta_project_manager_remove_file_default (IAnjutaProjectManager *obj, GFile *file, GError **err)
{
	g_return_val_if_reached (FALSE);
}

static void
ianjuta_project_manager_base_init (IAnjutaProjectManagerIface* klass)
{
	static gboolean initialized = FALSE;

	klass->add_group = ianjuta_project_manager_add_group_default;
	klass->add_source = ianjuta_project_manager_add_source_default;
	klass->add_source_quiet = ianjuta_project_manager_add_source_quiet_default;
	klass->add_sources = ianjuta_project_manager_add_sources_default;
	klass->add_target = ianjuta_project_manager_add_target_default;
	klass->get_capabilities = ianjuta_project_manager_get_capabilities_default;
	klass->get_children = ianjuta_project_manager_get_children_default;
	klass->get_current_project = ianjuta_project_manager_get_current_project_default;
	klass->get_elements = ianjuta_project_manager_get_elements_default;
	klass->get_packages = ianjuta_project_manager_get_packages_default;
	klass->get_parent = ianjuta_project_manager_get_parent_default;
	klass->get_selected = ianjuta_project_manager_get_selected_default;
	klass->get_target_type = ianjuta_project_manager_get_target_type_default;
	klass->get_targets = ianjuta_project_manager_get_targets_default;
	klass->is_open = ianjuta_project_manager_is_open_default;
	klass->remove_file = ianjuta_project_manager_remove_file_default;
	
	if (!initialized) {

		/**
		* IAnjutaProjectManager::element_added:
		* @obj: Self
		* @element: A #GFile corrresponding to added element
		* @err: Error propagation and reporting.
		*
		* Emitted when a new element is added to the project. It can be
		* a source, a target or a group. It does not always correspond
		* to an existing file. This signal can be emitted several time for
		* the same element.
		*/
		g_signal_new ("element-added",
			IANJUTA_TYPE_PROJECT_MANAGER,
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (IAnjutaProjectManagerIface, element_added),
			NULL, NULL,
			libanjuta_iface_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE,
			1,
			G_TYPE_FILE);


		/**
		* IAnjutaProjectManager::element_removed:
		* @obj: Self
		* @element: A #GFile corresponding to removed element
		* @err: Error propagation and reporting.
		*
		* Emitted when an element is removed from a project. It can be
		* a source, a target or a group.
		*/
		g_signal_new ("element-removed",
			IANJUTA_TYPE_PROJECT_MANAGER,
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (IAnjutaProjectManagerIface, element_removed),
			NULL, NULL,
			libanjuta_iface_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE,
			1,
			G_TYPE_FILE);


		/**
		* IAnjutaProjectManager::element_selected:
		* @obj: Self
		* @element_uri: A #GFile corresponding to selected element
		* @err: Error propagation and reporting.
		*
		* Emitted when an element is selected in the project view. It
		* can be a source, a target or a group.
		*/
		g_signal_new ("element-selected",
			IANJUTA_TYPE_PROJECT_MANAGER,
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (IAnjutaProjectManagerIface, element_selected),
			NULL, NULL,
			libanjuta_iface_cclosure_marshal_VOID__OBJECT,
			G_TYPE_NONE,
			1,
			G_TYPE_FILE);


		/**
		* IAnjutaProjectManager::project_loaded:
		* @obj: Self
		* @err: Error propagation and reporting.
		*
		* Emitted when the project is fully loaded. It can takes a quite long
		* time if the project is big. The project is loaded in several parts
		* in a thread. All functions are available before having the project
		* fully loaded.
		*/
		g_signal_new ("project-loaded",
			IANJUTA_TYPE_PROJECT_MANAGER,
			G_SIGNAL_RUN_LAST,
			G_STRUCT_OFFSET (IAnjutaProjectManagerIface, project_loaded),
			NULL, NULL,
			libanjuta_iface_cclosure_marshal_VOID__BOXED,
			G_TYPE_NONE,
			1,
			G_TYPE_ERROR);


		initialized = TRUE;
	}
}

GType
ianjuta_project_manager_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (IAnjutaProjectManagerIface),
			(GBaseInitFunc) ianjuta_project_manager_base_init,
			NULL, 
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "IAnjutaProjectManager", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;			
}