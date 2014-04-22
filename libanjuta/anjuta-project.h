/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * anjuta-project.h
 * Copyright (C) Sébastien Granjoux 2009 <seb.sfo@free.fr>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ANJUTA_PROJECT_H_
#define _ANJUTA_PROJECT_H_

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define ANJUTA_TYPE_PROJECT_PROPERTY		(anjuta_project_property_get_type ())
#define ANJUTA_IS_PROJECT_PROPERTY

typedef struct _AnjutaProjectProperty AnjutaProjectProperty;
typedef struct _AnjutaProjectPropertyInfo AnjutaProjectPropertyInfo;

typedef enum
{
	ANJUTA_PROJECT_PROPERTY_STRING = 1,
	ANJUTA_PROJECT_PROPERTY_LIST,
	ANJUTA_PROJECT_PROPERTY_BOOLEAN,
	ANJUTA_PROJECT_PROPERTY_MAP
} AnjutaProjectValueType;

typedef enum
{
	ANJUTA_PROJECT_PROPERTY_READ_ONLY = 1 << 0,
	ANJUTA_PROJECT_PROPERTY_READ_WRITE = 1 << 1,
	ANJUTA_PROJECT_PROPERTY_HIDDEN = 1 << 2,
	ANJUTA_PROJECT_PROPERTY_STATIC = 1 << 3,
} AnjutaProjectPropertyFlags;

struct _AnjutaProjectProperty
{
	gchar *name;
	gchar *value;
	AnjutaProjectPropertyInfo *info;
	gpointer user_data;
};

GType anjuta_project_property_get_type (void);
AnjutaProjectProperty *anjuta_project_property_new (const gchar *value, const gchar *name, gpointer user_data);
AnjutaProjectProperty * anjuta_project_property_copy (AnjutaProjectProperty *prop);
void anjuta_project_property_free (AnjutaProjectProperty *prop);


struct _AnjutaProjectPropertyInfo
{
    gchar *id;
	gchar *name;
    AnjutaProjectValueType type;
    AnjutaProjectPropertyFlags flags;
    gchar *description;
    AnjutaProjectProperty *default_value;
	gpointer user_data;
};

GType anjuta_project_property_info_get_type (void);
AnjutaProjectPropertyInfo *anjuta_project_property_info_new (const gchar *id, const gchar *name, AnjutaProjectValueType type, AnjutaProjectPropertyFlags flags, const gchar *description, AnjutaProjectProperty *default_value, gpointer user_data);
AnjutaProjectPropertyInfo * anjuta_project_property_info_copy (AnjutaProjectPropertyInfo *info);
void anjuta_project_property_info_free (AnjutaProjectPropertyInfo *info);


#define ANJUTA_TYPE_PROJECT_NODE 			(anjuta_project_node_get_type ())
#define ANJUTA_PROJECT_NODE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), ANJUTA_TYPE_PROJECT_NODE, AnjutaProjectNode))
#define ANJUTA_PROJECT_NODE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), ANJUTA_TYPE_PROJECT_NODE, AnjutaProjectNodeClass))
#define ANJUTA_IS_PROJECT_NODE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), ANJUTA_TYPE_PROJECT_NODE))
#define ANJUTA_IS_PROJECT_NODE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), ANJUTA_TYPE_PROJECT_NODE))
#define ANJUTA_PROJECT_NODE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), ANJUTA_TYPE_PROJECT_NODE, AnjutaProjectNodeClass))

typedef struct _AnjutaProjectNode		AnjutaProjectNode;
typedef struct _AnjutaProjectNodeClass	AnjutaProjectNodeClass;

typedef enum
{
	ANJUTA_PROJECT_UNKNOWN = 0,
	ANJUTA_PROJECT_SHAREDLIB,
	ANJUTA_PROJECT_STATICLIB,
	ANJUTA_PROJECT_PROGRAM,
	ANJUTA_PROJECT_PYTHON,
	ANJUTA_PROJECT_JAVA,
	ANJUTA_PROJECT_LISP,
	ANJUTA_PROJECT_HEADER,
	ANJUTA_PROJECT_MAN,
	ANJUTA_PROJECT_INFO,
	ANJUTA_PROJECT_GENERIC,
	ANJUTA_PROJECT_DATA,
	ANJUTA_PROJECT_EXTRA,
	ANJUTA_PROJECT_INTLTOOL,
	ANJUTA_PROJECT_CONFIGURE,
	ANJUTA_PROJECT_IDL,
	ANJUTA_PROJECT_MKENUMS,
	ANJUTA_PROJECT_GENMARSHAL,
	ANJUTA_PROJECT_SCRIPT,
	ANJUTA_PROJECT_ROOT_GROUP,
	ANJUTA_PROJECT_LT_MODULE,
	ANJUTA_PROJECT_PROXY = 1 << 16,
	ANJUTA_PROJECT_PROJECT = 1 << 17,
	ANJUTA_PROJECT_PRIMARY = 1 << 18,
	ANJUTA_PROJECT_EXECUTABLE = 1 << 19,
	ANJUTA_PROJECT_READ_ONLY = 1 << 20,	 /* Node cannot be created by the backend */
	ANJUTA_PROJECT_FRAME= 1 << 21,		/* Project configuration file */
	ANJUTA_PROJECT_ID_MASK = 0xFFFF << 0,
	ANJUTA_PROJECT_FLAG_MASK = 0xFF << 16,
	ANJUTA_PROJECT_TYPE_MASK = 0xFF << 24,
	ANJUTA_PROJECT_ROOT = 1 << 24,
	ANJUTA_PROJECT_GROUP = 2 << 24,
	ANJUTA_PROJECT_TARGET = 3 << 24,
	ANJUTA_PROJECT_SOURCE = 4 << 24,
	ANJUTA_PROJECT_MODULE = 5 << 24,
	ANJUTA_PROJECT_PACKAGE = 6 << 24,
	ANJUTA_PROJECT_VARIABLE = 7 << 24,
	ANJUTA_PROJECT_OBJECT = 8 << 24,
} AnjutaProjectNodeType;

typedef enum
{
	ANJUTA_PROJECT_OK = 0,
	ANJUTA_PROJECT_MODIFIED = 1 << 0,		/* Node has been modified */
	ANJUTA_PROJECT_INCOMPLETE = 1 << 1,	/* Node is not fully loaded */
	ANJUTA_PROJECT_LOADING = 1 << 2,	/* Node is send to the worker thread */
	ANJUTA_PROJECT_REMOVED = 1 << 3,		/* Node has been removed */
	ANJUTA_PROJECT_CAN_ADD_GROUP = 1 << 8,
	ANJUTA_PROJECT_CAN_ADD_TARGET = 1 << 9,
	ANJUTA_PROJECT_CAN_ADD_SOURCE = 1 << 10,
	ANJUTA_PROJECT_CAN_ADD_MODULE = 1 << 11,
	ANJUTA_PROJECT_CAN_ADD_PACKAGE = 1 << 12,
	ANJUTA_PROJECT_CAN_REMOVE = 1 << 16,
	ANJUTA_PROJECT_CAN_SAVE = 1 << 17,
	ANJUTA_PROJECT_REMOVE_FILE = 1 << 18
} AnjutaProjectNodeState;

/**
 * AnjutaProjectNode:
 *
 * The #AnjutaProjectNode struct contains private data only, and should
 * accessed using the functions below.
 */
struct _AnjutaProjectNode
{
	GInitiallyUnowned parent_instance;

	/*< private >*/
	AnjutaProjectNode	*next;
	AnjutaProjectNode *prev;
	AnjutaProjectNode	*parent;
	AnjutaProjectNode *children;

	AnjutaProjectNodeType type;
	AnjutaProjectNodeState state;

	GList *properties_info;
	GList *properties;
	GFile *file;
	gchar *name;
};

struct _AnjutaProjectNodeClass
{
	GInitiallyUnownedClass parent_class;

	void				(*updated)			(GError *error);
	void				(*loaded)			(GError *error);
};



typedef gboolean (*AnjutaProjectNodeTraverseFunc) (AnjutaProjectNode *node, gpointer data);
/**
 * AnjutaProjectNodeForeachFunc:
 * @data: (closure):
 */
typedef void (*AnjutaProjectNodeForeachFunc) (AnjutaProjectNode *node, gpointer data);


GType      anjuta_project_node_get_type (void) G_GNUC_CONST;

AnjutaProjectNode *anjuta_project_node_root (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_parent (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_first_child (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_last_child (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_next_sibling (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_prev_sibling (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_nth_child (AnjutaProjectNode *node, guint n);
AnjutaProjectNode *anjuta_project_node_parent_type (AnjutaProjectNode *node, AnjutaProjectNodeType type);

void anjuta_project_node_foreach (AnjutaProjectNode *node, GTraverseType order, AnjutaProjectNodeForeachFunc func, gpointer data);
void anjuta_project_node_children_foreach (AnjutaProjectNode *node, AnjutaProjectNodeForeachFunc func, gpointer data);
AnjutaProjectNode *anjuta_project_node_traverse (AnjutaProjectNode *node, GTraverseType order, AnjutaProjectNodeTraverseFunc func, gpointer data);
AnjutaProjectNode *anjuta_project_node_children_traverse (AnjutaProjectNode *node, AnjutaProjectNodeTraverseFunc func, gpointer data);

void anjuta_project_node_check (AnjutaProjectNode *parent);
void anjuta_project_node_dump (AnjutaProjectNode *parent);

AnjutaProjectNode *anjuta_project_node_insert_before (AnjutaProjectNode *parent, AnjutaProjectNode *sibling, AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_insert_after (AnjutaProjectNode *parent, AnjutaProjectNode *sibling, AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_remove (AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_append (AnjutaProjectNode *parent, AnjutaProjectNode *node);
AnjutaProjectNode *anjuta_project_node_prepend (AnjutaProjectNode *parent, AnjutaProjectNode *node);

AnjutaProjectNodeType anjuta_project_node_get_node_type (const AnjutaProjectNode *node);
AnjutaProjectNodeType anjuta_project_node_get_full_type (const AnjutaProjectNode *node);
AnjutaProjectNodeState anjuta_project_node_get_state (const AnjutaProjectNode *node);
const gchar *anjuta_project_node_get_name (const AnjutaProjectNode *node);
GFile *anjuta_project_node_get_file (const AnjutaProjectNode *node);

GList *anjuta_project_node_get_properties_info (AnjutaProjectNode *node);
GList *anjuta_project_node_get_properties (AnjutaProjectNode *node);
AnjutaProjectPropertyInfo *anjuta_project_node_get_property_info (AnjutaProjectNode *node, const gchar *id);
AnjutaProjectProperty *anjuta_project_node_get_property (AnjutaProjectNode *node, const gchar *id);
AnjutaProjectProperty *anjuta_project_node_get_map_property (AnjutaProjectNode *node, const gchar *id, const gchar *name);


AnjutaProjectPropertyInfo *anjuta_project_node_insert_property_info (AnjutaProjectNode *node, AnjutaProjectPropertyInfo *info);
AnjutaProjectProperty *anjuta_project_node_insert_property (AnjutaProjectNode *node, AnjutaProjectPropertyInfo *info, AnjutaProjectProperty *property);
AnjutaProjectProperty *anjuta_project_node_remove_property (AnjutaProjectNode *node, AnjutaProjectProperty *property);

gboolean anjuta_project_node_set_state (AnjutaProjectNode *node, AnjutaProjectNodeState state);
gboolean anjuta_project_node_clear_state (AnjutaProjectNode *node, AnjutaProjectNodeState state);

AnjutaProjectNode *anjuta_project_node_get_group_from_file (const AnjutaProjectNode *root, GFile *directory);
AnjutaProjectNode *anjuta_project_target_get_node_from_name (const AnjutaProjectNode *parent, const gchar *name);
AnjutaProjectNode *anjuta_project_node_get_source_from_file (const AnjutaProjectNode *parent, GFile *file);



#define ANJUTA_TYPE_PROJECT_NODE_INFO (anjuta_project_node_info_get_type ())

typedef struct _AnjutaProjectNodeInfo  AnjutaProjectNodeInfo;

struct _AnjutaProjectNodeInfo
{
	AnjutaProjectNodeType type;
	gchar *name;
	gchar *mime_type;
	gchar *property_help_id;
};

GType anjuta_project_node_info_get_type (void);

AnjutaProjectNodeInfo *anjuta_project_node_info_new (AnjutaProjectNodeType type, const gchar *name, const gchar *mime_type);
AnjutaProjectNodeInfo *anjuta_project_node_info_copy (AnjutaProjectNodeInfo *info);
void anjuta_project_node_info_free (AnjutaProjectNodeInfo *info);

const gchar *anjuta_project_node_info_name (const AnjutaProjectNodeInfo *info);
const gchar *anjuta_project_node_info_mime (const AnjutaProjectNodeInfo *info);
AnjutaProjectNodeType anjuta_project_node_info_type (const AnjutaProjectNodeInfo *info);
const gchar *anjuta_project_node_info_property_help_id (const AnjutaProjectNodeInfo *info);

G_END_DECLS

#endif
