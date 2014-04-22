/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    breakpoints.c
    Copyright (C) 2007 Sébastien Granjoux

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Handle breakpoints, mainly the graphical interface as the communication
 * with the debugger is handled by the corresponding debugger plugin. The
 * grahical interface includes:
 *       - A dialog including a tree view with all breakpoints
 *       - Several associated dialog (create a new breakpoints, confirmation...)
 *       - The markers in the editor plugin
 *
 * The list of breakpoints is kept in the tree view (GtkTreeModel).
 *
 * The AnjutaDebuggerBreakpoint object is used to communicate with the debugger plugin,
 * it includes all useful information for a breakpoint. Part of this structure
 * is filled by the debugger plugin, another part by the debug manager plugin.
 * This object is normally created and destroyed by the debug manager, but
 * the debugger plugin could create one too (but never destroy it).
 *
 * The BreakpointItem object includes a AnjutaDebuggerBreakpoint and adds all useful
 * information for the graphical interface (pointer to the editor...)
 *---------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/*#define DEBUG*/

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#include <libanjuta/resources.h>
#include <libanjuta/anjuta-debug.h>
#include <libanjuta/interfaces/ianjuta-document-manager.h>
#include <libanjuta/interfaces/ianjuta-markable.h>
#include <libanjuta/interfaces/ianjuta-file.h>
#include <libanjuta/interfaces/ianjuta-debugger-breakpoint.h>

#include "breakpoints.h"
#include "utilities.h"
#include "queue.h"

/* Markers definition */
#define BREAKPOINT_ENABLED   IANJUTA_MARKABLE_BREAKPOINT_ENABLED
#define BREAKPOINT_DISABLED  IANJUTA_MARKABLE_BREAKPOINT_DISABLED
#define BREAKPOINT_NONE      -1

typedef struct _BreakpointItem BreakpointItem;
typedef struct _BreakpointPacket BreakpointPacket;

/* Breakpoint data */
struct _BreakpointItem
{
	IAnjutaDebuggerBreakpointItem bp;      /* Breakpoint data */
	guint ref;

	gint handle;              	 /* Handle to mark in editor */
	IAnjutaEditor *editor;
	GFile *file;

	gint changed;				/* Bit field tagging change in breakpoint */


	GtkTreeIter iter;

	BreakpointsDBase *bd;
};

struct _BreakpointsDBase
{
	DebugManagerPlugin *plugin;
	DmaDebuggerQueue *debugger;

	GtkListStore *model;

	GtkBuilder *bxml;
	gchar *cond_history, *loc_history;

	/* Widgets */
	GtkWidget *window;
	GtkTreeView *treeview;
	GtkWidget *add_button;
	GtkWidget *remove_button;
	GtkWidget *jumpto_button;
	GtkWidget *properties_button;
	GtkWidget *removeall_button;
	GtkWidget *enableall_button;
	GtkWidget *disableall_button;

	/* Menu action */
	GtkActionGroup *debugger_group;
	GtkActionGroup *permanent_group;
};


enum {
	ENABLED_COLUMN,
	LOCATION_COLUMN,
	ADDRESS_COLUMN,
	TYPE_COLUMN,
	CONDITION_COLUMN,
	PASS_COLUMN,
	STATE_COLUMN,
	DATA_COLUMN,
	COLUMNS_NB
};

/* Helper functions
 *---------------------------------------------------------------------------*/

/* BreakItem functions
 *---------------------------------------------------------------------------*/

static void
breakpoint_item_update_from_debugger (BreakpointItem *bi, const IAnjutaDebuggerBreakpointItem* bp)
{
	if (bp == NULL) return;

	bi->bp.id = bp->id;
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_REMOVED)
	{
		bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_REMOVED;
		return;
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE)
	{
		bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE;
		g_free (bi->bp.file);
		bi->bp.file = g_strdup (bp->file);
		bi->bp.line = bp->line;
		if ((bi->file == NULL) && (g_path_is_absolute (bp->file)))
		{
			bi->file = g_file_new_for_path (bp->file);
		}
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION)
	{
		bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION;
		g_free (bi->bp.function);
		bi->bp.function = g_strdup (bp->function);
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS)
	{
		bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS;
		bi->bp.address = bp->address;
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_TIME)
	{
		bi->bp.times = bp->times;
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_TEMPORARY)
	{
		bi->bp.temporary = bp->temporary;
	}
	if (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_PENDING)
	{
		bi->bp.pending = bp->pending;
	}
	if ((bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE) &&
		!(bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE))
	{
		bi->bp.enable = bp->enable;
	}
	if ((bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION) &&
		!(bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION))
	{
		g_free (bi->bp.condition);
		bi->bp.condition = bp->condition == NULL ? NULL : g_strdup (bp->condition);
	}
	if ((bp->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE) &&
		!(bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE))
	{
       	bi->bp.ignore = bp->ignore;
	}
}

static void
breakpoint_item_ref (BreakpointItem *bi)
{
	bi->ref++;
}

static gboolean
breakpoint_item_unref (BreakpointItem *bi)
{
	g_return_val_if_fail (bi != NULL, FALSE);

	bi->ref--;
	if (bi->ref > 0) return FALSE;

	/* Remove from list */
	gtk_list_store_remove (bi->bd->model, &bi->iter);

	if (bi->editor != NULL)
	{
		g_object_remove_weak_pointer (G_OBJECT (bi->editor), (gpointer *)(gpointer)&bi->editor);
		bi->editor = NULL;
	}

	g_clear_object(&bi->file);

	g_free ((char *)bi->bp.file);
	bi->bp.file = NULL;
	g_free ((char *)bi->bp.function);
	bi->bp.function = NULL;
	g_free ((char *)bi->bp.condition);
	bi->bp.condition = NULL;

	return TRUE;
}

/* BreakItem constructor & destructor
 *---------------------------------------------------------------------------*/

static BreakpointItem *
breakpoint_item_new (BreakpointsDBase *bd)
{
	BreakpointItem *bi;
	bi = g_new0 (BreakpointItem, 1);
	bi->ref = 1;
	bi->bd = bd;
	bi->editor = NULL;
	bi->handle = -1;

	bi->bp.type = 0;

	gtk_list_store_append (bd->model, &bi->iter);
	gtk_list_store_set (bd->model, &bi->iter, DATA_COLUMN, bi, -1);

	return bi;
}

static BreakpointItem *
breakpoint_item_new_from_uri (BreakpointsDBase *bd, const gchar* uri, guint line, gboolean enable)
{
	BreakpointItem *bi;

	bi = breakpoint_item_new (bd);

	bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE | IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;
	if (uri != NULL)
	{
		bi->file = g_file_new_for_uri (uri);
		bi->bp.file = anjuta_util_get_local_path_from_uri (uri);
		bi->bp.line = line;
	}
	bi->bp.enable = enable;

	return bi;
}

static BreakpointItem *
breakpoint_item_new_from_file (BreakpointsDBase *bd, GFile *file, guint line, gboolean enable)
{
	BreakpointItem *bi;

	bi = breakpoint_item_new (bd);

	bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE | IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;
	if (file != NULL)
	{
		bi->file = g_object_ref (file);
		bi->bp.file = g_file_get_path (file);
		bi->bp.line = line;
	}
	bi->bp.enable = enable;

	return bi;
}

static BreakpointItem *
breakpoint_item_new_from_string (BreakpointsDBase *bd, const gchar* string, const gchar* uri)
{
	BreakpointItem *bi;

	bi = breakpoint_item_new (bd);

	if (*string == '*')
	{
		/* break at address */
		if ((string[1] == '0') && ((string[2] == 'x') || (string[2] == 'X')))
		{
			bi->bp.address = strtoul (string + 3, NULL, 16);
		}
		else
		{
			bi->bp.address = strtoul (string + 3, NULL, 10);
		}
		bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS;
	}
	else if ((uri != NULL) && isdigit (*string))
	{
		bi->file = g_file_new_for_uri (uri);
		bi->bp.file = anjuta_util_get_local_path_from_uri (uri);
		bi->bp.line = strtoul (string, NULL, 10);
		bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE;
	}
	else
	{
		const gchar *ptr;

		ptr = strchr (string, ':');

		if (ptr == NULL)
		{
			/* break on function */
			bi->bp.function = g_strdup (string);
			bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION;
		}
		else
		{
			if (isdigit (ptr[1]))
			{
				bi->bp.line = strtoul (ptr + 1, NULL, 10);
				bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE;
			}
			else
			{
				bi->bp.function = g_strdup (ptr + 1);
				bi->bp.type = IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION;
			}
			bi->bp.file = g_strndup (string, ptr - string);
			if (g_path_is_absolute (bi->bp.file))
			{
				bi->file = g_file_new_for_path (bi->bp.file);
			}
		}
	}

	bi->bp.enable = TRUE;
	bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;

	return bi;
}

/* User interface functions
 *---------------------------------------------------------------------------*/

static void
breakpoints_dbase_set_in_editor (BreakpointsDBase *bd, BreakpointItem *bi)
{
	IAnjutaMarkable *ed = NULL;
	gint line = -1;

	g_return_if_fail (bi != NULL);

	if (bi->editor == NULL)
	{
		return;
	}

	ed = IANJUTA_MARKABLE (bi->editor);
	if (bi->handle != -1)
	{
		line = ianjuta_markable_location_from_handle (ed, bi->handle, NULL);
		if (line != 0)
		{
			/* Remove old mark */
			ianjuta_markable_unmark (ed, line, BREAKPOINT_ENABLED, NULL);
			ianjuta_markable_unmark (ed, line, BREAKPOINT_DISABLED,NULL);
		}
		else
		{
			/* Mark is already removed, perhaps the file has been reloaded */
			line = bi->bp.line;
		}
	}
	else
	{
		line = bi->bp.line;
	}

	/* Add new mark */
	bi->handle = ianjuta_markable_mark (ed, line, bi->bp.enable ? BREAKPOINT_ENABLED : BREAKPOINT_DISABLED, NULL, NULL);
}

static void
breakpoints_dbase_clear_in_editor (BreakpointsDBase *bd, BreakpointItem *bi)
{
	IAnjutaMarkable *ed;
	gint line = -1;

	g_return_if_fail (bi != NULL);

	if (bi->editor == NULL)
	{
		/* No editor, no need to remove marker */
		return;
	}
	else
	{
		ed = IANJUTA_MARKABLE (bi->editor);
		line = ianjuta_markable_location_from_handle (ed, bi->handle, NULL);

		/* Remove old mark */
		ianjuta_markable_unmark (ed, line, BREAKPOINT_ENABLED, NULL);
		ianjuta_markable_unmark (ed, line, BREAKPOINT_DISABLED,NULL);
		bi->bp.line = line;
		bi->handle = -1;
	}
}

static void
breakpoints_dbase_update_in_treeview (BreakpointsDBase *bd, BreakpointItem *bi)
{
	gchar *adr;
	gchar *location;
	gchar *pass;
	gchar *state;
	const gchar *filename;
	const gchar *format;

	adr = g_strdup_printf ("0x%lx", bi->bp.address);
	if (bi->bp.file != NULL)
	{
		filename = strrchr(bi->bp.file, G_DIR_SEPARATOR);
		filename = filename == NULL ? bi->bp.file : filename + 1; /* display name only */
		format = bi->bp.function == NULL ? "%s:%d" : "%s:%d in %s";
	}
	else
	{
		filename = bi->bp.function;
		format = filename == NULL ? "??" : "%s";
	}
	location = g_strdup_printf (format, filename, bi->bp.line, bi->bp.function);
	if (bi->bp.id == 0)
	{
		pass = g_strdup_printf ("%d", bi->bp.ignore);
	}
	else if (bi->bp.ignore)
	{
		pass = g_strdup_printf ("%d of %d", bi->bp.times, bi->bp.ignore);
	}
	else
	{
		pass = g_strdup_printf ("%d", bi->bp.times);
	}
	if (bi->bp.id == 0)
	{
		format = "pending";
	}
	else if (bi->bp.temporary)
	{
		format = "temporary (%d)";
	}
	else if (bi->bp.pending)
	{
		format = "pending (%d)";
	}
	else
	{
		format = "permanent (%d)";
	}
	state = g_strdup_printf (format, bi->bp.id);

	gtk_list_store_set (bd->model, &bi->iter,
						ENABLED_COLUMN, bi->bp.enable,
						LOCATION_COLUMN, location,
						ADDRESS_COLUMN, adr,
						TYPE_COLUMN, "breakpoint",
						CONDITION_COLUMN, bi->bp.condition,
						PASS_COLUMN, pass,
						STATE_COLUMN, state,
						-1);
	g_free (state);
	g_free (pass);
	g_free (location);
	g_free (adr);
}

static void
breakpoints_dbase_breakpoint_removed (BreakpointsDBase *bd, BreakpointItem *bi)
{
	/* Delete maker */
	breakpoints_dbase_clear_in_editor (bd, bi);

	/* Emit signal */
	bi->bp.type |= IANJUTA_DEBUGGER_BREAKPOINT_REMOVED;
	g_signal_emit_by_name (bd->plugin, "breakpoint-changed", &bi->bp);

	breakpoint_item_unref (bi);
}

static void
breakpoints_dbase_breakpoint_updated (BreakpointsDBase *bd, BreakpointItem *bi)
{
	if ((bi->bp.id == 0) && (bi->bp.temporary))
	{
		/* Temporary breakpoint are never pending */
		breakpoints_dbase_breakpoint_removed (bd, bi);
	}
	else
	{
		/* Update treeview and marker */
		breakpoints_dbase_update_in_treeview (bd, bi);

		breakpoints_dbase_set_in_editor (bd, bi);

		/* Emit signal */
		g_signal_emit_by_name (bi->bd->plugin, "breakpoint-changed", &bi->bp);
	}
}

static void
on_editor_saved (IAnjutaEditor *editor, GFile* file, BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);
	gchar* uri;

	g_return_if_fail (model != NULL);

	if (!file)
		return;
	uri = g_file_get_uri (file);

	/* Update breakpoint position */
	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (GTK_TREE_MODEL (bd->model), &iter, DATA_COLUMN, &bi, -1);

			if ((bi->editor == editor) && (bi->handle != -1))
			{
				gint line;

				line = ianjuta_markable_location_from_handle (IANJUTA_MARKABLE (editor), bi->handle, NULL);
				if (line != bi->bp.line)
				{
					bi->bp.line = line;
					breakpoints_dbase_breakpoint_updated (bd, bi);
				}
			}
		} while (gtk_tree_model_iter_next (model, &iter));
	}
	g_free (uri);
}

static void
breakpoints_dbase_connect_to_editor (BreakpointsDBase *bd, IAnjutaEditor* ed)
{
	if (!g_signal_handler_find (ed, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, bd))
	{
		/* signal is not already connected */
		g_signal_connect (ed, "saved", G_CALLBACK (on_editor_saved), bd);
	}
}

static void
breakpoints_dbase_disconnect_from_editors (BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if (bi->editor != NULL)
			{
				g_signal_handlers_disconnect_matched (bi->editor, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, bd);
			}
		} while (gtk_tree_model_iter_next (model, &iter));
	}
}

static void
breakpoints_dbase_set_all_in_editor (BreakpointsDBase* bd, IAnjutaEditor* te)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GFile* file;

	g_return_if_fail (te != NULL);
	g_return_if_fail (bd != NULL);
	g_return_if_fail (bd->treeview != NULL);

	file = ianjuta_file_get_file (IANJUTA_FILE (te), NULL);
	if (file == NULL)
		return;

	if (!IANJUTA_IS_MARKABLE (te))
	{
		/* Nothing to do, editor does not support mark */
		return;
	}

	model = gtk_tree_view_get_model (bd->treeview);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if ((bi->editor == NULL) && (bi->file != NULL) && g_file_equal (file, bi->file))
			{
				bi->editor = te;
				bi->handle = -1;
				g_object_add_weak_pointer (G_OBJECT (te), (gpointer)&bi->editor);
				breakpoints_dbase_connect_to_editor (bd, te);
			}
			if (bi->editor == te)
			{
				breakpoints_dbase_set_in_editor (bd, bi);
			}
		} while (gtk_tree_model_iter_next (model, &iter));
	}
	g_object_unref(file);
}

static void
on_document_added (IAnjutaDocumentManager* docman, IAnjutaDocument* doc,
				   gpointer user_data)
{
	BreakpointsDBase *bd = (BreakpointsDBase *)user_data;

	/* Restore breakpoints */
	if (IANJUTA_IS_EDITOR (doc))
		breakpoints_dbase_set_all_in_editor (bd, IANJUTA_EDITOR(doc));
}

/* Private functions
 *---------------------------------------------------------------------------*/

static gboolean
ianjuta_debugger_breakpoint_is_equal (const IAnjutaDebuggerBreakpointItem *bpa, const IAnjutaDebuggerBreakpointItem *bpb)
{
	g_return_val_if_fail ((bpa != NULL) && (bpb != NULL), FALSE);

   	/* Special case if both pointer are equal */
	if (bpa == bpb) return TRUE;

	/* Compare their id */
	if (bpa->id == bpb->id) return TRUE;

	/* If both breakpoint id are define (!=0) and different */
	if ((bpa->id != 0) && (bpb->id != 0)) return FALSE;

	/* Check line and file */
	if (bpa->type & bpb->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE)
	{
		if (bpa->line != bpb->line) return FALSE;
		if (strcmp(bpa->file, bpb->file) != 0) return FALSE;
	}

	/* Check function name */
	if (bpa->type & bpb->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION)
	{
		if (strcmp(bpa->function, bpb->function) != 0) return FALSE;
	}

	/* Check address */
	if (bpa->type & bpb->type & IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS)
	{
		if (bpa->address != bpb->address) return FALSE;
	}

	/* Check conditions */
	if (bpa->type & bpb->type & IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION)
	{
		if (strcmp(bpa->condition, bpb->condition) != 0) return FALSE;
	}

	return TRUE;
}

static BreakpointItem*
breakpoints_dbase_find_breakpoint (BreakpointsDBase *bd, const IAnjutaDebuggerBreakpointItem *bp)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	g_return_val_if_fail (bd->treeview != NULL, NULL);

	model = gtk_tree_view_get_model (bd->treeview);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if (ianjuta_debugger_breakpoint_is_equal (&bi->bp, bp)) return bi;
		} while (gtk_tree_model_iter_next (model, &iter));
	}

	return NULL;
}

/* Debugger functions
 *---------------------------------------------------------------------------*/

static void
on_breakpoint_callback (const gpointer data, gpointer breakpoint, GError* err);

static gboolean
breakpoints_dbase_remove_in_debugger (BreakpointsDBase *bd, BreakpointItem *bi)
{
	gboolean ok;

	/* Remove breakpoint in debugger */
	breakpoint_item_ref (bi);
	ok = dma_queue_remove_breakpoint (
				bd->debugger,
				bi->bp.id,
				on_breakpoint_callback,
				bi);
	if (!ok) breakpoint_item_unref (bi);

	return ok;
}

static gboolean
breakpoints_dbase_add_in_debugger (BreakpointsDBase *bd, BreakpointItem *bi)
{
	gboolean ok = TRUE;

	if (bi->bp.id != 0)
	{
		/* Breakpoint already exist, remove it first */
		breakpoint_item_ref (bi);
		ok = dma_queue_remove_breakpoint (
					bd->debugger,
					bi->bp.id,
					on_breakpoint_callback,
					bi);
		if (!ok) breakpoint_item_unref (bi);
		ok = TRUE;
	}

	if (bi->bp.condition != NULL)
	{
		bi->changed = IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION;
	}
	if (bi->bp.ignore != 0)
	{
		bi->changed = IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE;
	}
	if (bi->bp.enable != TRUE)
	{
		bi->changed = IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;
	}

	/* Add breakpoint in debugger */
	if (bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE)
	{
		breakpoint_item_ref (bi);
		ok = dma_queue_add_breakpoint_at_line (
					bd->debugger,
					&(bi->bp.id),
					bi->bp.file,
					bi->bp.line,
					on_breakpoint_callback,
					bi);
	}
	else if (bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION)
	{
		breakpoint_item_ref (bi);
		ok = dma_queue_add_breakpoint_at_function (
					bd->debugger,
					&(bi->bp.id),
					bi->bp.file == NULL ? "" : bi->bp.file,
					bi->bp.function,
					on_breakpoint_callback,
					bi);
	}
	else if ((bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS)
			 && dma_debugger_queue_is_supported(bd->debugger, HAS_ADDRESS_BREAKPOINT))
	{
		breakpoint_item_ref (bi);
		ok = dma_queue_add_breakpoint_at_address (
					bd->debugger,
					&(bi->bp.id),
					bi->bp.address,
					on_breakpoint_callback,
					bi);
	}
	if (!ok) breakpoint_item_unref (bi);

	return ok;
}

static void
breakpoints_dbase_update_in_debugger (BreakpointsDBase *bd, BreakpointItem *bi)
{
	gboolean ok;

	if (bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE)
	{
		bi->changed &= ~IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;
		breakpoint_item_ref (bi);
		ok = dma_queue_enable_breakpoint (
					bd->debugger,
					bi->bp.id,
					bi->bp.enable,
					on_breakpoint_callback,
					bi);
		if (!ok) breakpoint_item_unref (bi);
	}

	if ((bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION)
		&& dma_debugger_queue_is_supported(bd->debugger, HAS_CONDITION_BREAKPOINT))
	{
		bi->changed &= ~IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION;
		breakpoint_item_ref (bi);
		ok = dma_queue_condition_breakpoint (
					bd->debugger,
					bi->bp.id,
					bi->bp.condition,
					on_breakpoint_callback,
					bi);
		if (!ok) breakpoint_item_unref (bi);
	}
	if ((bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE)
		&& dma_debugger_queue_is_supported(bd->debugger, HAS_IGNORE_BREAKPOINT))
	{
		bi->changed &= ~IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE;
		breakpoint_item_ref (bi);
		ok = dma_queue_ignore_breakpoint (
					bd->debugger,
					bi->bp.id,
					bi->bp.ignore,
					on_breakpoint_callback,
					bi);
		if (!ok) breakpoint_item_unref (bi);
	}
}

static void
on_breakpoint_callback (const gpointer data, gpointer user_data, GError* err)
{
	const IAnjutaDebuggerBreakpointItem* bp = (const IAnjutaDebuggerBreakpointItem*)data;
	BreakpointItem *bi = (BreakpointItem *)user_data;
	BreakpointsDBase *bd = bi->bd;

	if (breakpoint_item_unref (bi))
	{
		/* Breakpoint item has been destroyed */
		return;
	}

	if (err == NULL)
	{
		if ((bp != NULL) && (bp->type & IANJUTA_DEBUGGER_BREAKPOINT_REMOVED))
		{
			breakpoints_dbase_breakpoint_removed (bd, bi);
		}
		else
		{
			breakpoint_item_update_from_debugger (bi, bp);
			breakpoints_dbase_update_in_debugger (bd, bi);
			breakpoints_dbase_breakpoint_updated (bd, bi);
		}
	}
}

static
void on_update_breakpoint_from_debugger (gpointer data, gpointer user_data)
{
	IAnjutaDebuggerBreakpointItem *bp = (IAnjutaDebuggerBreakpointItem *)data;
	BreakpointsDBase *bd = (BreakpointsDBase *)user_data;
	BreakpointItem *bi;

	bi = breakpoints_dbase_find_breakpoint (bd, bp);

	if (bi == NULL)
	{
		/* Breakpoint exist in the debugger but not in the manager, create it */
		bi = breakpoint_item_new (bd);
	}
	breakpoint_item_update_from_debugger (bi, bp);
	bi->changed |= IANJUTA_DEBUGGER_BREAKPOINT_UPDATED;
}

static
gboolean on_update_breakpoint_in_ui (GtkTreeModel *model, GtkTreePath *path,
							   GtkTreeIter *iter, gpointer user_data)
{
	BreakpointItem *bi;
	BreakpointsDBase *bd;

	gtk_tree_model_get (model, iter, DATA_COLUMN, &bi, -1);
	bd = bi->bd;

	if (bi->changed & IANJUTA_DEBUGGER_BREAKPOINT_UPDATED)
	{
		/* Breakpoint is present in debugger */
		bi->changed &= ~IANJUTA_DEBUGGER_BREAKPOINT_UPDATED;
	}
	else
	{
		/* Breakpoint is not present in debugger */
		bi->bp.id = 0;
	}
	breakpoints_dbase_breakpoint_updated (bd, bi);

	return FALSE;
}

static void
on_breakpoint_list_callback (const gpointer data, gpointer user_data, GError* err)
{
	GList *list = (GList *)data;
	BreakpointsDBase *bd = (BreakpointsDBase *)user_data;

	/* Update all breakpoints */
	g_list_foreach (list, on_update_breakpoint_from_debugger, bd);

	/* Remove all not updated breakpoints */
	gtk_tree_model_foreach (GTK_TREE_MODEL (bd->model), on_update_breakpoint_in_ui, bd);
}

static void
breakpoints_dbase_list_all_in_debugger (BreakpointsDBase *bd)
{
	g_return_if_fail (bd->debugger != NULL);

	dma_queue_list_breakpoint (bd->debugger, on_breakpoint_list_callback, bd);
}

/* Private functions
 *---------------------------------------------------------------------------*/

/* Send all breakpoints in debugger */

static void
breakpoints_dbase_add_all_in_debugger (BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	g_return_if_fail (bd->treeview != NULL);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
			breakpoints_dbase_add_in_debugger (bd, bi);
		} while (gtk_tree_model_iter_next (model, &iter));
	}
}

/* Send all pending breakpoints in debugger */

static void
breakpoints_dbase_add_all_pending_in_debugger (BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if (bi->bp.id == 0)
			{
				breakpoints_dbase_add_in_debugger (bd, bi);
			}
		} while (gtk_tree_model_iter_next (model, &iter));
	}
}

/* Remove all breakpoints in debugger but do not delete them */

static void
breakpoints_dbase_remove_all_in_debugger (BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		gboolean next;

		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
			next = gtk_tree_model_iter_next (model, &iter);

			bi->bp.id = 0;
			breakpoints_dbase_breakpoint_updated (bd, bi);
			/* breakpoints_dbase_breakpoint_updated can delete pending breakpoints
			 * the iterator of the breakpoint can be invalidated */
		} while (next);
	}
}

/* Add breakpoint in tree view, in editor and in debugger */

static void
breakpoints_dbase_add_breakpoint (BreakpointsDBase *bd,  BreakpointItem *bi)
{
	IAnjutaEditor* ed;

	/* Add in current editor if possible */
	ed = dma_get_current_editor (ANJUTA_PLUGIN(bd->plugin));
	if ((ed != NULL) && IANJUTA_IS_MARKABLE (ed))
	{
		GFile* file;

		file = ianjuta_file_get_file (IANJUTA_FILE (ed), NULL);
		if ((file != NULL) && (bi->file != NULL) && g_file_equal (file, bi->file))
		{
			bi->editor = ed;
			bi->handle = -1;
			g_object_add_weak_pointer (G_OBJECT (ed), (gpointer)&bi->editor);
			breakpoints_dbase_connect_to_editor (bd, ed);
		}
		if (file)
			g_object_unref (file);
	}

	if (bd->debugger != NULL)
	{
		breakpoints_dbase_add_in_debugger (bd, bi);
	}
	else
	{
		breakpoints_dbase_breakpoint_updated (bd, bi);
	}
}

/* Remove breakpoint in tree view, in editor and in debugger */

static void
breakpoints_dbase_remove_breakpoint (BreakpointsDBase *bd, BreakpointItem *bi)
{
	if (bd->debugger != NULL)
	{
		if (breakpoints_dbase_remove_in_debugger (bd, bi)) return;
	}
	breakpoints_dbase_breakpoint_removed (bd, bi);
}

/* Enable or disable breakpoint in tree view, in editor and in debugger */

static void
breakpoints_dbase_enable_breakpoint (BreakpointsDBase *bd, BreakpointItem *bi, gboolean enable)
{
	bi->bp.enable = enable;
	bi->changed |= IANJUTA_DEBUGGER_BREAKPOINT_WITH_ENABLE;

	if (bd->debugger != NULL)
	{
		breakpoints_dbase_update_in_debugger (bd, bi);
	}
	else
	{
		breakpoints_dbase_breakpoint_updated (bd, bi);
	}
}

/* Toggle breakpoint enable in tree view, in editor and in debugger */

static void
breakpoints_dbase_toggle_enable (BreakpointsDBase *bd, GtkTreeModel *model, GtkTreeIter iter)
{
	BreakpointItem *bi;

	gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

	breakpoints_dbase_enable_breakpoint (bd, bi, bi->bp.enable ? FALSE : TRUE);
}

/* Update breakpoint in tree view, in editor and in debugger */

static void
breakpoints_dbase_update_breakpoint (BreakpointsDBase *bd,  BreakpointItem *bi)
{
	if (bd->debugger != NULL)
	{
		breakpoints_dbase_update_in_debugger (bd, bi);
	}
	else
	{
		breakpoints_dbase_breakpoint_updated (bd, bi);
	}
}

/* Remove all breakpoints in tree view, in editor and in debugger */

static void
breakpoints_dbase_remove_all (BreakpointsDBase *bd)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);


	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		gboolean next;

		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
			next = gtk_tree_model_iter_next (model, &iter);

			breakpoints_dbase_remove_breakpoint (bd, bi);
			/* Avoid an infinite loop if the breakpoint is not removed due to
			 * an error */
		} while (next);
	}
}

/* Enable or disable all breakpoints in tree view, in editor and in debugger */

static void
breakpoints_dbase_enable_all (BreakpointsDBase *bd, gboolean enable)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (GTK_TREE_MODEL (bd->model), &iter, DATA_COLUMN, &bi, -1);

			breakpoints_dbase_enable_breakpoint (bd, bi, enable);
		} while (gtk_tree_model_iter_next (model, &iter));
	}
}

static BreakpointItem*
breakpoints_dbase_find_breakpoint_from_mark (BreakpointsDBase *bd, IAnjutaEditor *ed, guint line)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if ((bi->editor == ed) && (bi->handle != -1)
				&& (ianjuta_markable_location_from_handle (IANJUTA_MARKABLE(ed), bi->handle, NULL) == line))
			{
				return bi;
			}
		} while (gtk_tree_model_iter_next (model, &iter));
	}

	return NULL;
}

static BreakpointItem*
breakpoints_dbase_find_breakpoint_from_line (BreakpointsDBase *bd, GFile *file, guint line)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

			if ((line == bi->bp.line) && (bi->file != NULL) && g_file_equal (file, bi->file)) return bi;
		} while (gtk_tree_model_iter_next (model, &iter));
	}

	return NULL;
}

static GList*
breakpoints_dbase_get_breakpoint_list (BreakpointsDBase *bd)
{
	GList* list = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (bd->model);

	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			BreakpointItem *bi;

			gtk_tree_model_get (GTK_TREE_MODEL (bd->model), &iter, DATA_COLUMN, &bi, -1);
			list = g_list_prepend (list, bi);

		} while (gtk_tree_model_iter_next (model, &iter));
	}

	list = g_list_reverse (list);

	return list;
}

/* Debugger Callbacks
 *---------------------------------------------------------------------------*/

static void
on_breakpoint_sharedlib_event (BreakpointsDBase *bd)
{
	breakpoints_dbase_add_all_pending_in_debugger (bd);
}

static void
on_program_running (BreakpointsDBase *bd)
{
	/* Deactivate breakpoint functions */
	gtk_action_group_set_sensitive (bd->debugger_group, FALSE);
}

static void
on_program_stopped (BreakpointsDBase *bd)
{
	g_return_if_fail (bd->debugger != NULL);

	gtk_action_group_set_sensitive (bd->debugger_group, TRUE);

	/* Refresh breakpoint
	 * Hit count could have changed by example */
	breakpoints_dbase_list_all_in_debugger (bd);
}

static void
on_program_exited (BreakpointsDBase *bd)
{
	g_return_if_fail (bd->debugger != NULL);

	gtk_action_group_set_sensitive (bd->debugger_group, TRUE);
}

static void
on_program_unloaded (BreakpointsDBase *bd)
{
	g_return_if_fail (bd->debugger != NULL);

	breakpoints_dbase_remove_all_in_debugger (bd);
	bd->debugger = NULL;
	gtk_action_group_set_sensitive (bd->debugger_group, TRUE);

	/* Disconnect from other debugger signal */
	g_signal_handlers_disconnect_by_func (bd->plugin, G_CALLBACK (on_breakpoint_sharedlib_event), bd);
	g_signal_handlers_disconnect_by_func (bd->plugin, G_CALLBACK (on_program_stopped), bd);
	g_signal_handlers_disconnect_by_func (bd->plugin, G_CALLBACK (on_program_running), bd);
	g_signal_handlers_disconnect_by_func (bd->plugin, G_CALLBACK (on_program_exited), bd);
	g_signal_handlers_disconnect_by_func (bd->plugin, G_CALLBACK (on_program_unloaded), bd);
}

static void
on_program_loaded (BreakpointsDBase *bd)
{
	DmaDebuggerQueue* debugger;

	DEBUG_PRINT("on program loaded in breakpoints %p bd %p", bd->debugger, bd);
	/* Debugger shouldn't be connected */
	g_return_if_fail (bd->debugger == NULL);

	debugger = dma_debug_manager_get_queue (ANJUTA_PLUGIN_DEBUG_MANAGER (bd->plugin));
	if (!dma_debugger_queue_is_supported (debugger, HAS_BREAKPOINT)) return;

	bd->debugger = debugger;
	breakpoints_dbase_add_all_in_debugger (bd);

	/* Connect to other debugger signal */
	g_signal_connect_swapped (bd->plugin, "sharedlib-event", G_CALLBACK (on_breakpoint_sharedlib_event), bd);
	g_signal_connect_swapped (bd->plugin, "program-unloaded", G_CALLBACK (on_program_unloaded), bd);
	g_signal_connect_swapped (bd->plugin, "program-stopped", G_CALLBACK (on_program_stopped), bd);
	g_signal_connect_swapped (bd->plugin, "program-exited", G_CALLBACK (on_program_exited), bd);
	g_signal_connect_swapped (bd->plugin, "program-running", G_CALLBACK (on_program_running), bd);
}

static void
on_debugger_started (BreakpointsDBase *bd)
{
	GtkTreeViewColumn *column;
	DmaDebuggerQueue* debugger;

	debugger = dma_debug_manager_get_queue (ANJUTA_PLUGIN_DEBUG_MANAGER (bd->plugin));

	/* Remove breakpoint attributes not supported by current debugger */
	if (!dma_debugger_queue_is_supported(debugger, HAS_ADDRESS_BREAKPOINT))
	{
		column = gtk_tree_view_get_column (bd->treeview, ADDRESS_COLUMN);
		gtk_tree_view_column_set_visible (column, FALSE);
	}
	if (!dma_debugger_queue_is_supported(debugger, HAS_IGNORE_BREAKPOINT))
	{
		column = gtk_tree_view_get_column (bd->treeview, PASS_COLUMN);
		gtk_tree_view_column_set_visible (column, FALSE);
	}
	if (!dma_debugger_queue_is_supported(debugger, HAS_CONDITION_BREAKPOINT))
	{
		column = gtk_tree_view_get_column (bd->treeview, CONDITION_COLUMN);
		gtk_tree_view_column_set_visible (column, FALSE);
	}
}

static void
on_debugger_stopped (BreakpointsDBase *bd)
{
	/* Restore breakpoint attributes not supported by current debugger */
	GtkTreeViewColumn *column;

	column = gtk_tree_view_get_column (bd->treeview, ADDRESS_COLUMN);
	gtk_tree_view_column_set_visible (column, TRUE);
	column = gtk_tree_view_get_column (bd->treeview, PASS_COLUMN);
	gtk_tree_view_column_set_visible (column, TRUE);
	column = gtk_tree_view_get_column (bd->treeview, CONDITION_COLUMN);
	gtk_tree_view_column_set_visible (column, TRUE);
}


/* Saving preferences callbacks
 *---------------------------------------------------------------------------*/

static void
on_session_save (AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, BreakpointsDBase *bd)
{
	GList *bi_list;
	GList *list = NULL;

	if (phase != ANJUTA_SESSION_PHASE_NORMAL)
		return;

	bi_list = breakpoints_dbase_get_breakpoint_list (bd);
	for (; bi_list != NULL; bi_list = g_list_delete_link (bi_list, bi_list))
	{
		BreakpointItem *bi = (BreakpointItem *)bi_list->data;

		if ((bi->bp.type & (IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE | IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION))
				&& (bi->bp.temporary == FALSE))
		{
			/* Only permanent breakpoint are saved */
			gchar *relative_uri;

			relative_uri = anjuta_session_get_relative_uri_from_file (session, bi->file, NULL);
			list = g_list_prepend (list, g_strdup_printf("%d:%s:%u:%u:%s", bi->bp.enable ? 1 : 0, relative_uri, bi->bp.line, bi->bp.ignore, bi->bp.condition == NULL ? "" : bi->bp.condition));
			g_free (relative_uri);
		}
	}
	list = g_list_reverse (list);
	if (list != NULL)
		anjuta_session_set_string_list (session, "Debugger", "Breakpoint", list);
	g_list_foreach (list, (GFunc)g_free, NULL);
	g_list_free (list);
}

static void
on_session_load (AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, BreakpointsDBase *bd)
{
	GList *list;

	if (phase != ANJUTA_SESSION_PHASE_NORMAL)
		return;

	breakpoints_dbase_remove_all (bd);
	list = anjuta_session_get_string_list (session, "Debugger", "Breakpoint");
	for (; list != NULL; list = g_list_delete_link (list, list))
	{
		BreakpointItem *bi;
		gchar *uri = (gchar *)list->data;
		gchar *pos;
		guint line;
		gchar *cond;
		guint ignore;
		gboolean enable;
		GFile *file;

		pos = strrchr (uri, ':');
		*pos = '\0';
		cond = pos + 1;
		pos = strrchr (uri, ':');
		*pos = '\0';
		ignore = strtoul (pos + 1, NULL, 10);
		pos = strrchr (uri, ':');
		*pos = '\0';
		line = strtoul (pos + 1, NULL, 10);
		enable = uri[0] == '0' ? FALSE : TRUE;
		file = anjuta_session_get_file_from_relative_uri (session, uri + 2, NULL);
		bi = breakpoint_item_new_from_file (bd, file, line, enable);
		g_object_unref (file);

		if (*cond != '\0')
		{
			bi->bp.condition = g_strdup (cond);
		}
		bi->bp.ignore = ignore;

		breakpoints_dbase_add_breakpoint (bd, bi);
		g_free (uri);
	}
}

/* Breakpoint edit dialog
 *---------------------------------------------------------------------------*/

static void
breakpoints_dbase_edit_breakpoint (BreakpointsDBase *bd, BreakpointItem *bi)
{
	GtkBuilder *bxml;
	GtkWidget *dialog;
	GtkWidget *location_label, *location_entry;
	GtkWidget *condition_entry, *condition_label;
	GtkWidget *pass_entry, *pass_label;
	gchar *buff;
	gchar *location = NULL;
	gchar *uri = NULL;
	gboolean new_break = FALSE;

	bxml = anjuta_util_builder_new (GLADE_FILE, NULL);
	if (!bxml) return;
	anjuta_util_builder_get_objects (bxml,
	    "breakpoint_properties_dialog", &dialog,
	    "breakpoint_location_label", &location_label,
	    "breakpoint_location_entry", &location_entry,
	    "breakpoint_condition_entry", &condition_entry,
		"breakpoint_condition_label", &condition_label,
		"breakpoint_pass_entry", &pass_entry,
		"breakpoint_pass_label", &pass_label,
	    NULL);
	g_object_unref (bxml);

	gtk_window_set_transient_for (GTK_WINDOW (dialog),
								  GTK_WINDOW (ANJUTA_PLUGIN (bd->plugin)->shell));

	if (bd->debugger != NULL)
	{
		if (!dma_debugger_queue_is_supported(bd->debugger, HAS_IGNORE_BREAKPOINT))
		{
			gtk_widget_hide (pass_entry);
			gtk_widget_hide (pass_label);
		}
		if (!dma_debugger_queue_is_supported(bd->debugger, HAS_CONDITION_BREAKPOINT))
		{
			gtk_widget_hide (condition_entry);
			gtk_widget_hide (condition_label);
		}
	}

	if (bi == NULL)
	{
		IAnjutaEditor *te;
		guint line = 0;

		/* New breakpoint */
		gtk_widget_show (location_entry);
		gtk_widget_hide (location_label);

		/* Get current editor and line */
		te = dma_get_current_editor (ANJUTA_PLUGIN(bd->plugin));
		if (te != NULL)
		{
			GFile* file = ianjuta_file_get_file (IANJUTA_FILE(te), NULL);
			if (file != NULL)
			{
				uri = g_file_get_uri (file);
				line = ianjuta_editor_get_lineno (te, NULL);
				g_object_unref (file);
			}
		}
		//NULL uri is ok here
		bi = breakpoint_item_new_from_uri (bd, uri, line, TRUE);
		new_break = TRUE;
	}
	else
	{
		/* Update breakpoint */
		gtk_widget_hide (location_entry);
		gtk_widget_show (location_label);
	}

	if (bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_LINE)
	{
		/* file and line */
		location = g_strdup_printf ("%s:%d", bi->bp.file, bi->bp.line);
	}
	else if (bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_FUNCTION)
	{
		/* file and function */
		location = g_strdup_printf ("%s:%s", bi->bp.file, bi->bp.function);
	}
	else if (bi->bp.type & IANJUTA_DEBUGGER_BREAKPOINT_ON_ADDRESS)
	{
		/* address */
		location = g_strdup_printf ("*%lx", bi->bp.address);
	}

	if (gtk_widget_get_visible (location_entry))
	{
		gtk_entry_set_text (GTK_ENTRY (location_entry), location == NULL ? "" : location);
	}
	else
	{
		gtk_label_set_text (GTK_LABEL (location_label), location == NULL ? "" : location);
	}

	if (bi->bp.condition && strlen (bi->bp.condition) > 0)
		gtk_entry_set_text (GTK_ENTRY (condition_entry), bi->bp.condition);

	buff = g_strdup_printf ("%d", bi->bp.ignore);
	gtk_entry_set_text (GTK_ENTRY (pass_entry), buff);
	g_free (buff);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
	{
		const gchar *condition;
		guint ignore;
		const gchar *new_location;

		ignore = atoi (gtk_entry_get_text (GTK_ENTRY (pass_entry)));
		condition = gtk_entry_get_text (GTK_ENTRY (condition_entry));
		while (isspace(*condition)) condition++;
		if (*condition == '\0') condition = NULL;

		if (gtk_widget_get_visible (location_entry))
		{
			new_location = gtk_entry_get_text (GTK_ENTRY (location_entry));
			while (isspace(*new_location)) new_location++;

			if ((location == NULL) || (strcmp (new_location, location) != 0))
			{
				/* location has been changed, create a new breakpoint */
				breakpoint_item_unref (bi);
				bi = NULL;

				if (*new_location != '\0')
				{
					bi = breakpoint_item_new_from_string (bd, new_location, NULL);
				}
			}
		}
		else
		{
			new_location = NULL;
		}

		if (bi != NULL)
		{
			if (bi->bp.ignore != ignore)
			{
				bi->bp.ignore = ignore;
				bi->changed |= IANJUTA_DEBUGGER_BREAKPOINT_WITH_IGNORE;
			}
			if ((condition != bi->bp.condition) && ((condition == NULL) || (bi->bp.condition == NULL) || (strcmp (bi->bp.condition, condition) != 0)))
			{
				if (bi->bp.condition) g_free ((char *)bi->bp.condition);
				bi->bp.condition = condition != NULL ? g_strdup (condition) : NULL;
				bi->changed |= IANJUTA_DEBUGGER_BREAKPOINT_WITH_CONDITION;
			}
			if (new_location != NULL)
			{
				breakpoints_dbase_add_breakpoint (bd, bi);
			}
			else
			{
				breakpoints_dbase_update_breakpoint (bd, bi);
			}
		}
	}
	else if (new_break)
	{
		/* Remove breakpoint if a new one has been created */
		breakpoint_item_unref (bi);
	}
	g_free (uri);
	g_free (location);
	gtk_widget_destroy (dialog);
}

/* Breakpoint actions list
 *---------------------------------------------------------------------------*/

static void
on_jump_to_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	gboolean valid;

	selection =	gtk_tree_view_get_selection (bd->treeview);
	valid = gtk_tree_selection_get_selected (selection, &model, &iter);
	if (valid)
	{
		BreakpointItem *bi;
		gchar *uri;
		gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);

		uri = g_file_get_uri (bi->file);
		g_signal_emit_by_name (bd->plugin, "location-changed", bi->bp.address, uri, bi->bp.line);
		g_free (uri);
	}
}


static void
update_breakpoint(BreakpointsDBase *bd, IAnjutaEditor *te, GFile *file, guint line_number)
{
	/* Find corresponding breakpoint
	 * Try to find right mark (it could have moved) first */
	BreakpointItem *bi;

	bi = breakpoints_dbase_find_breakpoint_from_mark (bd, te, line_number);
	DEBUG_PRINT("breakpoints db %p, editor %p, uri %s, line_number %d, BreakpointItem %p", bd, te, g_file_get_path (file), line_number, bi);
	if (bi == NULL)
	{
		bi = breakpoints_dbase_find_breakpoint_from_line (bd, file, line_number);
	}

	if (bi == NULL)
	{
		bi = breakpoint_item_new_from_file (bd, file, line_number, TRUE);

		breakpoints_dbase_add_breakpoint (bd, bi);
	}
	else
	{
		breakpoints_dbase_remove_breakpoint (bd, bi);
	}
}

/* update the breakpoint with current active line in the editor */
static void
on_toggle_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	IAnjutaEditor *te;
	GFile* file;
	guint line;

	/* Get current editor and line */
	te = dma_get_current_editor (ANJUTA_PLUGIN (bd->plugin));
	if (te == NULL) return;       /* Missing editor */
	file = ianjuta_file_get_file (IANJUTA_FILE (te), NULL);
	if (file == NULL) return;     /* File not saved yet, it's not possible to put a breakpoint in it */
	line = ianjuta_editor_get_lineno (te, NULL);

	update_breakpoint(bd, te, file, line);
}

/* update the breakpoint when the line number is known */
static void
on_line_marks_gutter_clicked(GtkAction * action, gint line_number, BreakpointsDBase *bd)
{
	IAnjutaEditor *te;
	GFile* file;

	/* Get current editor and line */
	te = dma_get_current_editor (ANJUTA_PLUGIN (bd->plugin));
	if (te == NULL) return;       /* Missing editor */
	file = ianjuta_file_get_file (IANJUTA_FILE (te), NULL);
	if (file == NULL) return;     /* File not saved yet, it's not possible to put a breakpoint in it */

	update_breakpoint(bd, te, file, line_number);
}

static void
on_disable_all_breakpoints_activate (GtkAction * action, BreakpointsDBase *bd)
{
	breakpoints_dbase_enable_all (bd, FALSE);
}

static void
on_clear_all_breakpoints_activate (GtkAction * action, BreakpointsDBase *bd)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (GTK_WINDOW (ANJUTA_PLUGIN (bd->plugin)->shell),
									 GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_QUESTION,
									 GTK_BUTTONS_NONE,
					_("Are you sure you want to delete all the breakpoints?"));
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
							GTK_STOCK_CANCEL, GTK_RESPONSE_NO,
							GTK_STOCK_DELETE, GTK_RESPONSE_YES,
							NULL);

	gtk_window_set_transient_for (GTK_WINDOW (dialog),
			GTK_WINDOW (ANJUTA_PLUGIN (bd->plugin)->shell) );

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
	{
		breakpoints_dbase_remove_all (bd);
	}
	gtk_widget_destroy (dialog);
}

static void
on_add_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	breakpoints_dbase_edit_breakpoint (bd, NULL);
}

static void
on_remove_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	gboolean valid;

	selection =	gtk_tree_view_get_selection (bd->treeview);
	valid = gtk_tree_selection_get_selected (selection, &model, &iter);
	if (valid)
	{
		BreakpointItem *bi;

		gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
		breakpoints_dbase_remove_breakpoint (bd, bi);
	}
}

static void
on_edit_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	gboolean valid;

	selection =	gtk_tree_view_get_selection (bd->treeview);
	valid = gtk_tree_selection_get_selected (selection, &model, &iter);
	if (valid)
	{
		BreakpointItem *bi;

		gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
		breakpoints_dbase_edit_breakpoint (bd, bi);
	}
}

static void
on_treeview_enabled_toggled (GtkCellRendererToggle *cell,
							 gchar			       *path_str,
							 BreakpointsDBase      *bd)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;

	path = gtk_tree_path_new_from_string (path_str);

	model = gtk_tree_view_get_model (bd->treeview);
	gtk_tree_model_get_iter (model, &iter, path);

	breakpoints_dbase_toggle_enable (bd, model, iter);
}

static void
on_enable_breakpoint_activate (GtkAction * action, BreakpointsDBase *bd)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	gboolean valid;

	selection =	gtk_tree_view_get_selection (bd->treeview);
	valid = gtk_tree_selection_get_selected (selection, &model, &iter);
	if (valid)
	{
		breakpoints_dbase_toggle_enable (bd, model, iter);
	}
}

static GtkActionEntry actions_debugger_breakpoints[] = {
	{
		"ActionMenuDmaBreakpoints",               /* Action name */
		NULL,                                     /* Stock icon, if any */
		N_("_Breakpoints"),                       /* Display label */
		NULL,                                     /* short-cut */
		NULL,                                     /* Tooltip */
		NULL                                      /* action callback */
	},
	{
		"ActionDmaToggleBreakpoint",              /* Action name */
		ANJUTA_STOCK_BREAKPOINT_TOGGLE,           /* Stock icon, if any */
		N_("Toggle Breakpoint"),                  /* Display label */
		"<control>b",                             /* short-cut */
		N_("Toggle breakpoint at the current location"), /* Tooltip */
		G_CALLBACK (on_toggle_breakpoint_activate) /* action callback */
	},
	{
		"ActionDmaSetBreakpoint",                 /* Action name */
		ANJUTA_STOCK_BREAKPOINT_ENABLED,          /* Stock icon, if any */
		N_("Add Breakpoint…"),                  /* Display label */
		NULL,                                     /* short-cut */
		N_("Add a breakpoint"),                   /* Tooltip */
		G_CALLBACK (on_add_breakpoint_activate)   /* action callback */
	},
	{
		"ActionDmaClearBreakpoint",               /* Action name */
		ANJUTA_STOCK_BREAKPOINT_CLEAR,            /* Stock icon, if any */
		N_("Remove Breakpoint"),                  /* Display label */
		NULL,                                     /* short-cut */
		N_("Remove a breakpoint"),                /* Tooltip */
		G_CALLBACK (on_remove_breakpoint_activate)   /* action callback */
	},
	{
		"ActionDmaEditBreakpoint",                /* Action name */
		NULL,                                     /* Stock icon, if any */
		N_("Edit Breakpoint"),                    /* Display label */
		NULL,                                     /* short-cut */
		N_("Edit breakpoint properties"),         /* Tooltip */
		G_CALLBACK (on_edit_breakpoint_activate)   /* action callback */
	},
	{
		"ActionDmaEnableDisableBreakpoint",       /* Action name */
		NULL,                					  /* Stock icon, if any */
		N_("Enable Breakpoint"),                  /* Display label */
		NULL,                                     /* short-cut */
		N_("Enable a breakpoint"),                /* Tooltip */
		G_CALLBACK (on_enable_breakpoint_activate)   /* action callback */
	},
	{
		"ActionDmaDisableAllBreakpoints",         /* Action name */
		ANJUTA_STOCK_BREAKPOINT_DISABLED,         /* Stock icon, if any */
		N_("Disable All Breakpoints"),            /* Display label */
		NULL,                                     /* short-cut */
		N_("Deactivate all breakpoints"),         /* Tooltip */
		G_CALLBACK (on_disable_all_breakpoints_activate)/* action callback */
	},
	{
		"ActionDmaClearAllBreakpoints",           /* Action name */
		ANJUTA_STOCK_BREAKPOINT_CLEAR,            /* Stock icon, if any */
		N_("R_emove All Breakpoints"),             /* Display label */
		NULL,                                     /* short-cut */
		N_("Remove all breakpoints"),             /* Tooltip */
		G_CALLBACK (on_clear_all_breakpoints_activate)/* action callback */
	},
};

static GtkActionEntry actions_permanent_breakpoints[] = {
	{
		"ActionDmaJumpToBreakpoint",              /* Action name */
		NULL,                                     /* Stock icon, if any */
		N_("Jump to Breakpoint"),                 /* Display label */
		NULL,                                     /* short-cut */
		N_("Jump to breakpoint location"),        /* Tooltip */
		G_CALLBACK (on_jump_to_breakpoint_activate)   /* action callback */
	},
};

/* Breakpoint list window
 *---------------------------------------------------------------------------*/

static gboolean
on_breakpoints_button_press (GtkWidget * widget, GdkEventButton * bevent, BreakpointsDBase *bd)
{
	if (bevent->button == 3)
	{
		AnjutaUI *ui;
		GtkMenu *popup;
		GtkTreeModel *model;
		GtkTreeSelection *selection;
		GtkTreeIter iter;
		gboolean valid;

		ui = anjuta_shell_get_ui (ANJUTA_PLUGIN(bd->plugin)->shell, NULL);
		popup =  GTK_MENU (gtk_ui_manager_get_widget (GTK_UI_MANAGER (ui), "/PopupBreakpoint"));

		selection = gtk_tree_view_get_selection (bd->treeview);
		valid = gtk_tree_selection_get_selected (selection, &model, &iter);
		if (valid)
		{
			BreakpointItem *bi;
			GtkAction *action;
			const gchar* label;
			const gchar* tooltip;

			gtk_tree_model_get (model, &iter, DATA_COLUMN, &bi, -1);
			action = gtk_action_group_get_action (bd->debugger_group, "ActionDmaEnableDisableBreakpoint");
			g_return_val_if_fail (action != NULL, FALSE);
			if (bi->bp.enable)
			{
				label =	N_("Disable Breakpoint");
				tooltip = N_("Disable a breakpoint");
			}
			else
			{
				label =	N_("Enable Breakpoint");
				tooltip = N_("Enable a breakpoint");
			}

			g_object_set (G_OBJECT (action),
					"label", label,
              				"tooltip", tooltip,
		              		NULL);
		}

		gtk_menu_popup (popup, NULL, NULL, NULL, NULL,
						bevent->button, bevent->time);
	}
	else if ((bevent->type == GDK_2BUTTON_PRESS) && (bevent->button == 1))
	{
		/* Double left mouse click */
		on_jump_to_breakpoint_activate (NULL, bd);
	}

	return FALSE;
}

static void
create_breakpoint_gui(BreakpointsDBase *bd)
{
	/* Translators: Enabled refers to a breakpoint. It is a column name, like
	 * other words. */
	static const gchar *column_names[COLUMNS_NB] =	{
		N_("Enabled"), N_("Location"), N_("Address"), N_("Type"),
		N_("Condition"), N_("Pass count"), N_("State")};
	static GType column_type[COLUMNS_NB] = {
		G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER};
	AnjutaUI *ui;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model;
	int i;

	g_return_if_fail (bd->treeview == NULL);
	g_return_if_fail (bd->window == NULL);
	g_return_if_fail (bd->debugger_group == NULL);
	g_return_if_fail (bd->permanent_group == NULL);

	/* breakpoints window */
	bd->model = gtk_list_store_newv (COLUMNS_NB, column_type);
	model = GTK_TREE_MODEL (bd->model);
	bd->treeview = GTK_TREE_VIEW (gtk_tree_view_new_with_model (model));
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (bd->treeview),
					     GTK_SELECTION_SINGLE);
	g_object_unref (G_OBJECT (model));

	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_(column_names[0]),
													   renderer,
							   						   "active",
													   ENABLED_COLUMN,
													   NULL);
	gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column (bd->treeview, column);
	g_signal_connect (renderer, "toggled",
					  G_CALLBACK (on_treeview_enabled_toggled), bd);

	renderer = gtk_cell_renderer_text_new ();
	for (i = ENABLED_COLUMN + 1; i < (COLUMNS_NB - 1); i++)
	{
		column =
			gtk_tree_view_column_new_with_attributes (_(column_names[i]),
												renderer, "text", i, NULL);
		gtk_tree_view_column_set_sizing (column,
										 GTK_TREE_VIEW_COLUMN_AUTOSIZE);
		gtk_tree_view_append_column (bd->treeview, column);
	}

	/* Register menu actions */
	ui = anjuta_shell_get_ui (ANJUTA_PLUGIN(bd->plugin)->shell, NULL);
	bd->debugger_group =
	anjuta_ui_add_action_group_entries (ui, "ActionGroupBreakpoint",
										_("Breakpoint operations"),
										actions_debugger_breakpoints,
										G_N_ELEMENTS (actions_debugger_breakpoints),
										GETTEXT_PACKAGE, TRUE, bd);
	bd->permanent_group =
	anjuta_ui_add_action_group_entries (ui, "ActionGroupBreakpointPermanent",
										_("Breakpoint operations"),
										actions_permanent_breakpoints,
										G_N_ELEMENTS (actions_permanent_breakpoints),
										GETTEXT_PACKAGE, TRUE, bd);

	/* Add breakpoint window */
	bd->window = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (bd->window);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (bd->window),
									GTK_POLICY_AUTOMATIC,
									GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (bd->window),
									 GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (bd->window), GTK_WIDGET (bd->treeview));
	gtk_widget_show_all (bd->window);
	anjuta_shell_add_widget (ANJUTA_PLUGIN(bd->plugin)->shell,
							 bd->window,
							 "AnjutaDebuggerBreakpoints", _("Breakpoints"),
							 ANJUTA_STOCK_BREAKPOINT_ENABLED, ANJUTA_SHELL_PLACEMENT_NONE,
							 NULL);

	/* Add popup menu */
	g_signal_connect (bd->treeview, "button-press-event", G_CALLBACK (on_breakpoints_button_press), bd);
}

static void
destroy_breakpoint_gui (BreakpointsDBase *bd)
{
	AnjutaUI *ui;

	/* Remove menu actions */
	ui = anjuta_shell_get_ui (ANJUTA_PLUGIN (bd->plugin)->shell, NULL);
	if (bd->debugger_group)
	{
		anjuta_ui_remove_action_group (ui, bd->debugger_group);
		bd->debugger_group = NULL;
	}
	if (bd->permanent_group)
	{
		anjuta_ui_remove_action_group (ui, bd->permanent_group);
		bd->permanent_group = NULL;
	}

	/* Destroy breakpoint window */
	if (bd->window != NULL)
	{
		gtk_widget_destroy (bd->window);
		bd->window = NULL;
	}
}

/* Constructor & Destructor
 *---------------------------------------------------------------------------*/

BreakpointsDBase *
breakpoints_dbase_new (DebugManagerPlugin *plugin)
{
	BreakpointsDBase *bd;
	IAnjutaDocumentManager* docman;

	bd = g_new0 (BreakpointsDBase, 1);

	bd->plugin = plugin;

	/* Create graphical user inteface */
	create_breakpoint_gui (bd);

	/* Connect to Load and Save event */
	g_signal_connect (ANJUTA_PLUGIN(bd->plugin)->shell, "save-session",
				  G_CALLBACK (on_session_save), bd);
	g_signal_connect (ANJUTA_PLUGIN(bd->plugin)->shell, "load-session",
				  G_CALLBACK (on_session_load), bd);

	/* Connect on load program */
	g_signal_connect_swapped (bd->plugin, "program-loaded", G_CALLBACK (on_program_loaded), bd);
	g_signal_connect_swapped (bd->plugin, "debugger-started", G_CALLBACK (on_debugger_started), bd);
	g_signal_connect_swapped (bd->plugin, "debugger-stopped", G_CALLBACK (on_debugger_stopped), bd);

	/* Connect to IAnjutaDocumentManager::document-added */
	docman = anjuta_shell_get_interface(ANJUTA_PLUGIN(plugin)->shell,
										IAnjutaDocumentManager, NULL);
	g_return_val_if_fail(docman, NULL);
	g_signal_connect(docman, "document-added", G_CALLBACK(on_document_added), bd);

	return bd;
}

void
breakpoints_dbase_destroy (BreakpointsDBase * bd)
{
	IAnjutaDocumentManager* docman;

	g_return_if_fail (bd != NULL);

	/* Disconnect all signal */
	g_signal_handlers_disconnect_matched (ANJUTA_PLUGIN(bd->plugin)->shell, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, bd);
	g_signal_handlers_disconnect_matched (bd->plugin, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, bd);
	breakpoints_dbase_disconnect_from_editors (bd);

	/* Disconnect IAnjutaDocumentManager::document-added */
	docman = anjuta_shell_get_interface(ANJUTA_PLUGIN(bd->plugin)->shell,
										IAnjutaDocumentManager, NULL);
	if (docman)
		g_signal_handlers_disconnect_by_func(docman, on_document_added, bd); 

	
	/* This is necessary to clear the editor of breakpoint markers */
	breakpoints_dbase_remove_all (bd);

	/* Destroy graphical user interface */
	destroy_breakpoint_gui (bd);

	g_free (bd->cond_history);
	g_free (bd->loc_history);

	g_free (bd);
}

/* Handler for breakpoint toggle on double clicking line marks gutter */
void breakpoint_toggle_handler(GtkAction * action, gint line_number, BreakpointsDBase *bd) {
	/* Simply delegate to an internal API for now */
	on_line_marks_gutter_clicked (action, line_number, bd);
}
