/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-iterable-tree.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_ITERABLE_TREE_H_
#define _IANJUTA_ITERABLE_TREE_H_

#include <glib-object.h>
#include <libanjuta/interfaces/ianjuta-iterable.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_ITERABLE_TREE (ianjuta_iterable_tree_get_type ())
#define IANJUTA_ITERABLE_TREE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_ITERABLE_TREE, IAnjutaIterableTree))
#define IANJUTA_IS_ITERABLE_TREE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_ITERABLE_TREE))
#define IANJUTA_ITERABLE_TREE_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_ITERABLE_TREE, IAnjutaIterableTreeIface))

#define IANJUTA_ITERABLE_TREE_ERROR ianjuta_iterable_tree_error_quark()

typedef struct _IAnjutaIterableTree IAnjutaIterableTree;
typedef struct _IAnjutaIterableTreeIface IAnjutaIterableTreeIface;


struct _IAnjutaIterableTreeIface {
	IAnjutaIterableIface g_iface;
	

	gboolean (*children) (IAnjutaIterableTree *obj, GError **err);
	void (*foreach_post) (IAnjutaIterableTree *obj, GFunc callback,  gpointer user_data, GError **err);
	void (*foreach_pre) (IAnjutaIterableTree *obj, GFunc callback,  gpointer user_data, GError **err);
	gboolean (*has_children) (IAnjutaIterableTree *obj, GError **err);
	gboolean (*parent) (IAnjutaIterableTree *obj, GError **err);

};


GQuark ianjuta_iterable_tree_error_quark     (void);
GType  ianjuta_iterable_tree_get_type        (void);

gboolean ianjuta_iterable_tree_children (IAnjutaIterableTree *obj, GError **err);

void ianjuta_iterable_tree_foreach_post (IAnjutaIterableTree *obj, GFunc callback,  gpointer user_data, GError **err);

void ianjuta_iterable_tree_foreach_pre (IAnjutaIterableTree *obj, GFunc callback,  gpointer user_data, GError **err);

gboolean ianjuta_iterable_tree_has_children (IAnjutaIterableTree *obj, GError **err);

gboolean ianjuta_iterable_tree_parent (IAnjutaIterableTree *obj, GError **err);


G_END_DECLS

#endif
