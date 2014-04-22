/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    start.c
    Copyright (C) 2000  Kh. Naba Kumar Singh

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
 * Handle start of debugger. It can be done in three ways:
 *  - Load a target of the current project_root_uri
 *  - Load a executable file
 *  - Attach to an already running program
 *---------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "start.h"

#include "queue.h"

/*#define DEBUG*/
#include <libanjuta/anjuta-debug.h>
#include <libanjuta/resources.h>
#include <libanjuta/anjuta-project.h>
#include <libanjuta/interfaces/ianjuta-project-manager.h>
#include <libanjuta/interfaces/ianjuta-document-manager.h>
#include <libanjuta/interfaces/ianjuta-builder.h>
#include <libanjuta/interfaces/ianjuta-file-savable.h>
#include <libanjuta/anjuta-utils.h>

#include <gio/gio.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/* Types
 *---------------------------------------------------------------------------*/

typedef struct _AttachProcess AttachProcess;
typedef struct _AddSourceDialog AddSourceDialog;
typedef struct _LoadFileCallBack LoadFileCallBack;
	
enum
{
	CLEAR_INITIAL,
	CLEAR_UPDATE,
	CLEAR_REVIEW,
	CLEAR_FINAL
};

struct _AttachProcess
{
    GtkWidget*  dialog;
    GtkWidget*  treeview;
	gint        pid;

	gboolean    hide_paths;
	gboolean    hide_params;
	gboolean    process_tree;

	gchar*      ps_output;
	GSList*     iter_stack;
	gint        iter_stack_level;
	gint        num_spaces_to_skip;
	gint        num_spaces_per_level;
};

enum {
	PID_COLUMN,
	USER_COLUMN,
	START_COLUMN,
	COMMAND_COLUMN,
	COLUMNS_NB
};

static char *column_names[COLUMNS_NB] = {
	N_("PID"), N_("User"), N_("Time"), N_("Command")
};

enum {
	VARIABLE_COLUMN,
	VALUE_COLUMN,
	VAR_COLUMNS_NB
};

struct _AddSourceDialog
{
	GtkTreeView *tree;
	GtkFileChooser *entry;
	GtkListStore *model;
};

struct _LoadFileCallBack
{
	AnjutaPlugin *plugin;
	DmaDebuggerQueue *debugger;
};

struct _DmaStart
{
	AnjutaPlugin *plugin;

	DmaDebuggerQueue *debugger;

	gboolean stop_at_beginning;
	GList *source_dirs;

	gchar *remote_debugger;
	
	gchar *build_target;
	IAnjutaBuilderHandle build_handle;
};

/* Widgets found in glade file
 *---------------------------------------------------------------------------*/

#define ADD_SOURCE_DIALOG "source_paths_dialog"
#define SOURCE_ENTRY "src_entry"
#define SOURCE_LIST "src_clist"
#define ADD_BUTTON "source_paths_add_button"
#define REMOVE_BUTTON "remove_button"
#define UP_BUTTON "up_button"
#define DOWN_BUTTON "down_button"

#define REMOTE_DEBUG_DIALOG "remote_dialog"
#define TCPIP_ADDRESS_ENTRY "tcpip_address_entry"
#define TCPIP_PORT_ENTRY "tcpip_port_entry"
#define TCPIP_RADIO "tcpip_radio"
#define TCPIP_CONTAINER "tcpip_container"
#define SERIAL_PORT_ENTRY "serial_port_entry"
#define SERIAL_RADIO "serial_radio"
#define SERIAL_CONTAINER "serial_container"

#define CHECK_DEBUG_DIALOG "check_debug_dialog"
#define DO_NOT_SHOW_CHECK "hide_checkbox"

/* Constants
 *---------------------------------------------------------------------------*/

#define RUN_PROGRAM_URI	"run_program_uri"
#define RUN_PROGRAM_ARGS "run_program_args"
#define RUN_PROGRAM_DIR	"run_program_directory"
#define RUN_PROGRAM_ENV	"run_program_environment"
#define RUN_PROGRAM_NEED_TERM	"run_program_need_terminal"

#define RUN_PROGRAM_ACTION_GROUP "ActionGroupRun"
#define RUN_PROGRAM_PARAMETER_ACTION "ActionProgramParameters"

#define PREF_SCHEMA "org.gnome.anjuta.plugins.debug-manager"
#define PREFS_NOT_CHECK_DEBUG "silent-non-debug-config"

static void attach_process_clear (AttachProcess * ap, gint ClearRequest);

/* Helper functions
 *---------------------------------------------------------------------------*/

/* This functions get all directories of the current project containing
 * static or shared library. Perhaps a more reliable way to find these
 * directories is to really get the directories of all source files */

static GList*
get_source_directories (AnjutaPlugin *plugin)
{
	gchar *cwd;
	GList *node, *search_dirs = NULL;
	GList *slibs_dirs = NULL;
	GList *libs_dirs = NULL;
	GValue value = {0,};
	GFile *file;

	return NULL;
	cwd = anjuta_util_get_current_dir();
	file = g_file_new_for_path (cwd);
	search_dirs = g_list_prepend (search_dirs, g_file_get_uri (file));
	g_object_unref (file);
	g_free (cwd);

	/* Check if a project is already open */
	anjuta_shell_get_value (plugin->shell, IANJUTA_PROJECT_MANAGER_PROJECT_ROOT_URI, &value, NULL);
	
	/* Set source file search directories */
	if (g_value_get_string (&value) != NULL)
	{
		IAnjutaProjectManager *pm;
		pm = anjuta_shell_get_interface (plugin->shell,
										 IAnjutaProjectManager, NULL);
		if (pm)
		{
			slibs_dirs =
				ianjuta_project_manager_get_targets (pm,
					ANJUTA_PROJECT_TARGET | ANJUTA_PROJECT_SHAREDLIB,
				    NULL);
			libs_dirs =
				ianjuta_project_manager_get_targets (pm,
					ANJUTA_PROJECT_TARGET | ANJUTA_PROJECT_STATICLIB,
				    NULL);
		}
	}
	slibs_dirs = g_list_reverse (slibs_dirs);
	libs_dirs = g_list_reverse (libs_dirs);
	
	node = slibs_dirs;
	while (node)
	{
		gchar *path;
		GFile *parent = g_file_get_parent (node->data);
		path = g_file_get_uri (parent);
		g_object_unref (parent);
		search_dirs = g_list_prepend (search_dirs, path);
		node = g_list_next (node);
	}
	
	node = libs_dirs;
	while (node)
	{
		gchar *path;
		path = g_file_get_path (node->data);
		search_dirs = g_list_prepend (search_dirs, path);
		node = g_list_next (node);
	}
	
	g_list_foreach (slibs_dirs, (GFunc)g_free, NULL);
	g_list_free (slibs_dirs);
	g_list_foreach (libs_dirs, (GFunc)g_free, NULL);
	g_list_free (libs_dirs);
	
	return g_list_reverse (search_dirs);
}

static void
free_source_directories (GList *dirs)
{
	g_list_foreach (dirs, (GFunc)g_free, NULL);
	g_list_free (dirs);
}

/* Callback for saving session
 *---------------------------------------------------------------------------*/

static void
on_session_save (AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, DmaStart *self)
{
	if (phase != ANJUTA_SESSION_PHASE_NORMAL)
		return;

	anjuta_session_set_string_list (session, "Debugger", "Source directories", self->source_dirs);
	anjuta_session_set_int (session, "Debugger", "Stop at beginning", self->stop_at_beginning + 1);
	anjuta_session_set_string (session, "Debugger", "Remote target", self->remote_debugger);
}

static void on_session_load (AnjutaShell *shell, AnjutaSessionPhase phase, AnjutaSession *session, DmaStart *self)
{
	gint stop_at_beginning;

	if (phase != ANJUTA_SESSION_PHASE_NORMAL)
		return;

	/* Initialize source_dirs */
 	if (self->source_dirs != NULL)
	{		
 		g_list_foreach (self->source_dirs, (GFunc)g_free, NULL);
 		g_list_free (self->source_dirs);
 	}
 	self->source_dirs = anjuta_session_get_string_list (session, "Debugger", "Source directories");

	stop_at_beginning = anjuta_session_get_int (session, "Debugger", "Stop at beginning");
	if (stop_at_beginning == 0)
		self->stop_at_beginning = TRUE;	/* Default value */
	else
		self->stop_at_beginning = stop_at_beginning - 1;
	
	g_free (self->remote_debugger);
	self->remote_debugger = anjuta_session_get_string (session, "Debugger", "Remote target");
}

/* Attach to process private functions
 *---------------------------------------------------------------------------*/

static AttachProcess *
attach_process_new (void)
{
	AttachProcess *ap;
	ap = g_new0 (AttachProcess, 1);
	attach_process_clear (ap, CLEAR_INITIAL);
	return ap;
}

static void
attach_process_clear (AttachProcess * ap, gint ClearRequest)
{
	GtkTreeModel *model;

	// latest ps output
	switch (ClearRequest)
	{
	case CLEAR_UPDATE:
	case CLEAR_FINAL:
		if (ap->ps_output)
		{
			g_free (ap->ps_output);
		}
	case CLEAR_INITIAL:
		ap->ps_output = NULL;
	}

	// helper variables
	switch (ClearRequest)
	{
	case CLEAR_INITIAL:
	case CLEAR_UPDATE:
	case CLEAR_REVIEW:
		ap->pid = -1L;
		ap->iter_stack = NULL;
		ap->iter_stack_level = -1;
		ap->num_spaces_to_skip = -1;
	}

	// tree model
	switch (ClearRequest)
	{
	case CLEAR_UPDATE:
	case CLEAR_REVIEW:
	case CLEAR_FINAL:
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (ap->treeview));
		gtk_tree_store_clear (GTK_TREE_STORE (model));
	}

	// dialog widget
	if (ClearRequest == CLEAR_FINAL)
	{
		gtk_widget_destroy (ap->dialog);
		ap->dialog = NULL;
	}
}

static inline gchar *
skip_spaces (gchar *pos)
{
	while (*pos == ' ')
		pos++;
	return pos;
}

static inline gchar *
skip_token (gchar *pos)
{
	while (*pos != ' ')
		pos++;
	*pos++ = '\0';
	return pos;
}

static gchar *
skip_token_and_spaces (gchar *pos)
{
	pos = skip_token (pos);
	return skip_spaces (pos);
}

static GtkTreeIter *
iter_stack_push_new (AttachProcess *ap, GtkTreeStore *store)
{
	GtkTreeIter *new_iter, *top_iter;
	new_iter = g_new (GtkTreeIter, 1);
	top_iter = (GtkTreeIter *) (g_slist_nth_data (ap->iter_stack, 0));
	ap->iter_stack =
			g_slist_prepend (ap->iter_stack, (gpointer) (new_iter));
	gtk_tree_store_append (store, new_iter, top_iter);
	ap->iter_stack_level++;
	return new_iter;
}

static gboolean
iter_stack_pop (AttachProcess *ap)
{
	if (ap->iter_stack_level < 0)
		return FALSE;

	GtkTreeIter *iter =
			(GtkTreeIter *) (g_slist_nth_data (ap->iter_stack, 0));
	ap->iter_stack =
			g_slist_delete_link (ap->iter_stack, ap->iter_stack);
	g_free (iter);
	ap->iter_stack_level--;
	return TRUE;
}

static void
iter_stack_clear (AttachProcess *ap)
{
	while (iter_stack_pop (ap));
}

static gchar *
calc_depth_and_get_iter (AttachProcess *ap, GtkTreeStore *store,
			GtkTreeIter **iter, gchar *pos)
{
	gchar *orig_pos;
	guint num_spaces, depth, i;

	// count spaces
	orig_pos = pos;
	while (*pos == ' ')
		pos++;
	num_spaces = pos - orig_pos;

	if (ap->process_tree)
	{
		if (ap->num_spaces_to_skip < 0)
		{
			// first process to be inserted
			ap->num_spaces_to_skip = num_spaces;
			ap->num_spaces_per_level = -1;
			*iter = iter_stack_push_new (ap, store);
		}
		else
		{
			if (ap->num_spaces_per_level < 0)
			{
				if (num_spaces == ap->num_spaces_to_skip)
				{
					// num_spaces_per_level still unknown
					iter_stack_pop (ap);
					*iter = iter_stack_push_new (ap, store);
				}
				else
				{
					// first time at level 1
					ap->num_spaces_per_level = 
							num_spaces - ap->num_spaces_to_skip;
					*iter = iter_stack_push_new (ap, store);
				}
			}
			else
			{
				depth = (num_spaces - ap->num_spaces_to_skip) /
						ap->num_spaces_per_level;
				if (depth == ap->iter_stack_level)
				{
					// level not changed
					iter_stack_pop (ap);
					*iter = iter_stack_push_new (ap, store);
				}
				else
					if (depth == ap->iter_stack_level + 1)
						*iter = iter_stack_push_new (ap, store);
					else
						if (depth < ap->iter_stack_level)
						{
							// jump some levels backward
							depth = ap->iter_stack_level - depth;
							for (i = 0; i <= depth; i++)
								iter_stack_pop (ap);
							*iter = iter_stack_push_new (ap, store);
						}
						else
						{
							// should never get here
							g_warning("Unknown error");
							iter_stack_pop (ap);
							*iter = iter_stack_push_new (ap, store);
						}
			}
		}
	}
	else
	{
		iter_stack_pop (ap);
		*iter = iter_stack_push_new (ap, store);
	}

	return pos;
}

static gchar *
skip_path (gchar *pos)
{
	/* can't use g_path_get_basename() - wouldn't work for a processes
	started with parameters containing '/' */
	gchar c, *final_pos = pos;

	if (*pos == G_DIR_SEPARATOR)
		do
		{
			c = *pos;
			if (c == G_DIR_SEPARATOR)
			{
				final_pos = ++pos;
				continue;
			}
			else
				if (c == ' ' || c == '\0')
					break;
				else
					++pos;
		}
		while (1);

	return final_pos;
}

static inline void
remove_params (gchar *pos)
{
	do
	{
		if (*(++pos) == ' ')
			*pos = '\0';
	}
	while (*pos);
}

static void
attach_process_add_line (AttachProcess *ap, GtkTreeStore *store, gchar *line)
{
	gchar *pid, *user, *start, *command, *tmp;
	// guint i, level;
	GtkTreeIter *iter;

	pid = skip_spaces (line); // skip leading spaces
	user = skip_token_and_spaces (pid); // skip PID
	start = skip_token_and_spaces (user); // skip USER
	tmp = skip_token (start); // skip START (do not skip spaces)

	command = calc_depth_and_get_iter (ap, store, &iter, tmp);

	if (ap->hide_paths)
	{
		command = skip_path (command);
	}

	if (ap->hide_params)
	{
		remove_params(command);
	}

	gtk_tree_store_set (store, iter,
						PID_COLUMN, pid,
						USER_COLUMN, user,
						START_COLUMN, start,
						COMMAND_COLUMN, command,
						-1);
}

static void
attach_process_review (AttachProcess *ap)
{
	gchar *ps_output, *begin, *end;
	guint line_num = 0;
	GtkTreeStore *store;

	g_return_if_fail (ap);
	g_return_if_fail (ap->ps_output);
	store = GTK_TREE_STORE (gtk_tree_view_get_model 
							(GTK_TREE_VIEW (ap->treeview)));
	g_return_if_fail (store);

	ps_output = g_strdup (ap->ps_output);
	end = ps_output;
	while (*end)
	{
		begin = end;
		while (*end && *end != '\n') end++;
		if (++line_num > 2) // skip description line & process 'init'
		{
			*end = '\0';
			attach_process_add_line (ap, store, begin);
		}
		end++;
	}
	g_free (ps_output);

	iter_stack_clear (ap);
	gtk_tree_view_expand_all (GTK_TREE_VIEW (ap->treeview));
}

static void
attach_process_update (AttachProcess * ap)
{
	gchar *tmp, *tmp1, *cmd;
	gchar *shell;
	gchar *argv[4];
	GError *err = NULL;
	GtkTreeStore *store;
	gboolean result;

	g_return_if_fail (ap);
	store = GTK_TREE_STORE (gtk_tree_view_get_model
							(GTK_TREE_VIEW (ap->treeview)));
	g_return_if_fail (store);
	
	if (anjuta_util_prog_is_installed ("ps", TRUE) == FALSE)
		return;

	tmp = anjuta_util_get_a_tmp_file ();
#if defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__NetBSD__)
	cmd = g_strconcat ("ps axw -o pid,user,lstart,args > ", tmp, NULL);
#else
	cmd = g_strconcat ("ps axw -H -o pid,user,start_time,args > ", tmp, NULL);
#endif
	shell = anjuta_util_user_shell ();
	argv[0] = shell;
	argv[1] = "-c";
	argv[2] = cmd;
	argv[3] = NULL;
	if (!g_spawn_sync (NULL, argv, NULL, 0, NULL, NULL, NULL, NULL, NULL, &err))
	{
		anjuta_util_dialog_error (NULL, _("Unable to execute: \"%s\". "
		                                  "The returned error was: \"%s\"."), cmd, err->message);
		g_error_free (err);
		g_free (tmp);
		g_free (cmd);
		return;
	}

	g_free (cmd);

	result = g_file_get_contents (tmp, &tmp1, NULL, NULL);
	remove (tmp);
	g_free (tmp);
	if (! result)
	{
		anjuta_util_dialog_error_system (NULL, errno,
										 _("Unable to open the file: %s\n"),
										 tmp);
		return;
	}

	attach_process_clear (ap, CLEAR_UPDATE);
	ap->ps_output = anjuta_util_convert_to_utf8 (tmp1);
	g_free (tmp1);
	if (ap->ps_output)
	{
		attach_process_review (ap);
	}
}

static void
on_selection_changed (GtkTreeSelection *selection, AttachProcess *ap)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	g_return_if_fail (ap);
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gchar* text;
		gtk_tree_model_get (model, &iter, PID_COLUMN, &text, -1);
		ap->pid = atoi (text);
		gtk_dialog_set_response_sensitive (GTK_DIALOG (ap->dialog), 
										   GTK_RESPONSE_OK, TRUE);
	}
	else
	{
		gtk_dialog_set_response_sensitive (GTK_DIALOG (ap->dialog), 
										   GTK_RESPONSE_OK, FALSE);
		ap->pid = -1L;
	}
}

static gboolean
on_delete_event (GtkWidget *dialog, GdkEvent *event, AttachProcess *ap)
{
	g_return_val_if_fail (ap, FALSE);
	attach_process_clear (ap, CLEAR_FINAL);
	return FALSE;
}

static gint
sort_pid (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b,
		gpointer user_data)
{
	gchar *nptr;
	gint pid1, pid2;

	gtk_tree_model_get (model, a, PID_COLUMN, &nptr, -1);
	pid1 = atoi (nptr);

	gtk_tree_model_get (model, b, PID_COLUMN, &nptr, -1);
	pid2 = atoi (nptr);

	return pid1 - pid2;
}

static void
on_toggle_hide_paths (GtkToggleButton *togglebutton, AttachProcess * ap)
{
	ap->hide_paths = gtk_toggle_button_get_active (togglebutton);
	attach_process_clear (ap, CLEAR_REVIEW);
	attach_process_review (ap);
}

static void
on_toggle_hide_params (GtkToggleButton *togglebutton, AttachProcess * ap)
{
	ap->hide_params = gtk_toggle_button_get_active (togglebutton);
	attach_process_clear (ap, CLEAR_REVIEW);
	attach_process_review (ap);
}

static void
on_toggle_process_tree (GtkToggleButton *togglebutton, AttachProcess * ap)
{
	ap->process_tree = gtk_toggle_button_get_active (togglebutton);
	attach_process_clear (ap, CLEAR_REVIEW);
	attach_process_review (ap);
}

static pid_t
attach_process_show (AttachProcess * ap, GtkWindow *parent)
{
	GtkTreeView *view;
	GtkTreeStore *store;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtkCheckButton *checkb_hide_paths;
	GtkCheckButton *checkb_hide_params;
	GtkCheckButton *checkb_process_tree;
	gint i, res;
	pid_t selected_pid = -1;
	
	g_return_val_if_fail (ap != NULL, -1);

	if (!ap->dialog)
	{
		GtkBuilder *bxml;

		bxml = anjuta_util_builder_new (GLADE_FILE, NULL);
		if (!bxml) return 0;
		anjuta_util_builder_get_objects (bxml,
		    "attach_process_dialog", &ap->dialog,
		    "attach_process_tv", &ap->treeview,
		    "checkb_hide_paths", &checkb_hide_paths,
		    "checkb_hide_params", &checkb_hide_params,
 			"checkb_process_tree", &checkb_process_tree,
		    NULL);
		g_object_unref (bxml);
	
		view = GTK_TREE_VIEW (ap->treeview);
		store = gtk_tree_store_new (COLUMNS_NB,
									G_TYPE_STRING,
									G_TYPE_STRING,
									G_TYPE_STRING,
									G_TYPE_STRING);
		gtk_tree_view_set_model (view, GTK_TREE_MODEL (store));
		gtk_tree_selection_set_mode (gtk_tree_view_get_selection (view),
						 GTK_SELECTION_SINGLE);
		g_object_unref (G_OBJECT (store));
	
		renderer = gtk_cell_renderer_text_new ();
	
		for (i = PID_COLUMN; i < COLUMNS_NB; i++) {
			GtkTreeViewColumn *column;
	
			column = gtk_tree_view_column_new_with_attributes (column_names[i],
														renderer, "text", i, NULL);
			gtk_tree_view_column_set_sort_column_id(column, i);
			gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
			gtk_tree_view_append_column (view, column);
			if (i == COMMAND_COLUMN)
				gtk_tree_view_set_expander_column(view, column);
		}
		gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (store), PID_COLUMN,
						sort_pid, NULL, NULL);
		gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (store),
						START_COLUMN, GTK_SORT_DESCENDING);
	
		ap->hide_paths = gtk_toggle_button_get_active (
							GTK_TOGGLE_BUTTON (checkb_hide_paths));
		ap->hide_params = gtk_toggle_button_get_active (
							GTK_TOGGLE_BUTTON (checkb_hide_params));
		ap->process_tree = gtk_toggle_button_get_active (
							GTK_TOGGLE_BUTTON (checkb_process_tree));
	
		attach_process_update (ap);
	
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ap->treeview));
		g_signal_connect (G_OBJECT (selection), "changed",
						  G_CALLBACK (on_selection_changed), ap);
		g_signal_connect (G_OBJECT (ap->dialog), "delete_event",
						  G_CALLBACK (on_delete_event), ap);
		g_signal_connect (checkb_hide_paths, "toggled",
							G_CALLBACK (on_toggle_hide_paths), ap);
		g_signal_connect (checkb_hide_params, "toggled",
							G_CALLBACK (on_toggle_hide_params), ap);
		g_signal_connect (checkb_process_tree, "toggled",
							G_CALLBACK (on_toggle_process_tree), ap);
	}
	
	gtk_window_set_transient_for (GTK_WINDOW (ap->dialog),
								  GTK_WINDOW (parent));
	/* gtk_widget_show (ap->dialog); */
	res = gtk_dialog_run (GTK_DIALOG (ap->dialog));
	while (res == GTK_RESPONSE_APPLY)
	{
		attach_process_update (ap);
		res = gtk_dialog_run (GTK_DIALOG (ap->dialog));
	}
	if (res == GTK_RESPONSE_OK)
	{
		selected_pid = ap->pid;
	}
	attach_process_clear (ap, CLEAR_FINAL);
	return selected_pid;
}

static void
attach_process_destroy (AttachProcess * ap)
{
	g_return_if_fail (ap);
	g_free (ap);
}

/* Check debug configuration dialog
 *---------------------------------------------------------------------------*/

static gboolean
show_check_debug_dialog (DmaStart *this)
{
	GSettings* settings = g_settings_new (PREF_SCHEMA);
	gboolean no_check_debug;
	gint res = GTK_RESPONSE_OK;

	no_check_debug = g_settings_get_boolean (settings, 
	                                         PREFS_NOT_CHECK_DEBUG);
	
	if (!no_check_debug)
	{
		GtkBuilder *bxml;
		GtkWindow *parent;
		GtkWidget *dialog;
		GtkToggleButton *do_not_show;
		
		parent = GTK_WINDOW (this->plugin->shell);

		/* Fetch out the widget we care about for now */
		bxml = anjuta_util_builder_new (GLADE_FILE, NULL);
		if (!bxml) return FALSE;
			anjuta_util_builder_get_objects (bxml,
	    	CHECK_DEBUG_DIALOG, &dialog,
	    	DO_NOT_SHOW_CHECK, &do_not_show,
		    NULL);
		g_object_unref (bxml);

		gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
		res = gtk_dialog_run (GTK_DIALOG (dialog));
	
		if (gtk_toggle_button_get_active	(do_not_show))
		{
			g_settings_set_boolean (settings, PREFS_NOT_CHECK_DEBUG, TRUE);
		}
	
		gtk_widget_destroy (dialog);
	}

	g_object_unref (settings);
	
	return res == GTK_RESPONSE_OK;
}


/* Load file private functions
 *---------------------------------------------------------------------------*/

static void
show_parameters_dialog (DmaStart *this)
{
	AnjutaUI *ui;
	GtkAction *action;
	
	ui = anjuta_shell_get_ui (ANJUTA_PLUGIN (this->plugin)->shell, NULL);
	action = anjuta_ui_get_action (ui, RUN_PROGRAM_ACTION_GROUP, RUN_PROGRAM_PARAMETER_ACTION);
	if (action != NULL)
	{
		gtk_action_activate (action);
	}
}

static gboolean
start_remote_target (DmaStart *this, const gchar *target)
{
	gchar *dir_uri;
	gchar *dir;
	gchar **env;
	gchar *args;
	gboolean run_in_terminal;

	anjuta_shell_get (ANJUTA_PLUGIN (this->plugin)->shell,
	 				  RUN_PROGRAM_DIR, G_TYPE_STRING, &dir_uri,
					  RUN_PROGRAM_ARGS, G_TYPE_STRING, &args,
					  RUN_PROGRAM_ENV, G_TYPE_STRV, &env,
					  RUN_PROGRAM_NEED_TERM, G_TYPE_BOOLEAN, &run_in_terminal,
					  NULL);	
	
	if (dir_uri != NULL)
	{
		dir = anjuta_util_get_local_path_from_uri (dir_uri);
		g_free (dir_uri);
	}
	else
	{
		dir = NULL;
	}
	
	dma_queue_set_working_directory (this->debugger, dir);
	g_free (dir);
	
	dma_queue_set_environment (this->debugger, env);
	g_strfreev (env);
	
	if (target == NULL)
	{
		dma_queue_start (this->debugger, args, run_in_terminal, FALSE);
	}
	else
	{
		dma_queue_connect (this->debugger, target, args, run_in_terminal, FALSE);
	}
	g_free (args);
	
	return TRUE;

}

static gboolean
start_target (DmaStart *this)
{
	return start_remote_target (this, NULL);
}

static gboolean
load_target (DmaStart *this, const gchar *target)
{
	gchar *mime_type;
	gchar *filename;
	GFile *file;
	GFileInfo *file_info;
	GError *error = NULL;
	
	DEBUG_PRINT ("Using target %s", target);
	file = g_file_new_for_uri (target);
	file_info = g_file_query_info (file, 
			G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
			G_FILE_QUERY_INFO_NONE,
			NULL, &error);
	if (file_info == NULL)
	{
		DEBUG_PRINT ("Error getting file info: %s", error->message);
		g_error_free (error);
		anjuta_util_dialog_error(GTK_WINDOW (this->plugin->shell),
				_("Unable to open %s. Debugger cannot start."), target);
		g_object_unref (file);
		return FALSE;
	}

	mime_type = g_file_info_get_attribute_as_string (file_info, 
			G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE);
	if (mime_type == NULL)
	{
		anjuta_util_dialog_error(GTK_WINDOW (this->plugin->shell),
				_("Unable to detect MIME type of %s. Debugger cannot start."), target);
		g_object_unref (file_info);
		g_object_unref (file);
		return FALSE;
	}

	filename = g_file_get_path (file);
	
	dma_queue_load (this->debugger, filename, mime_type, this->source_dirs);

	g_free (filename);
	g_free (mime_type);
	g_object_unref (file_info);
	g_object_unref (file);

	return TRUE;
}

static void
on_build_finished (GObject *builder, IAnjutaBuilderHandle handle, GError *err, gpointer user_data)
{
	DmaStart *this = (DmaStart *)user_data;
	
	if (err == NULL)
	{
		/* Up to date, start debugger */
		load_target (this, this->build_target);
		start_target (this);
	}
	
	g_free (this->build_target);
	this->build_target = NULL;
}

static void
on_is_built_finished (GObject *builder, IAnjutaBuilderHandle handle, GError *err, gpointer user_data)
{
	DmaStart *this = (DmaStart *)user_data;
	
	if (err == NULL)
	{
		/* Up to date, start debugger */
		load_target (this, this->build_target);
		start_target (this);
	}
	else
	{
		/* Target is not up to date or an error happens with make -q */
		this->build_handle = ianjuta_builder_build (IANJUTA_BUILDER (builder), this->build_target, on_build_finished, this, NULL);
		return;
	}
	
	g_free (this->build_target);
	this->build_target = NULL;
}

static gboolean
check_target (DmaStart *this, const gchar *target)
{
	IAnjutaBuilder *builder;

	builder = anjuta_shell_get_interface (this->plugin->shell, IAnjutaBuilder, NULL);
	if (builder != NULL)
	{
		GList *cfg_list;
		GList *found;
		
		if (this->build_target)
		{
			/* a build operation is currently running */
			if (strcmp (this->build_target, target) == 0)
			{
				/* It is the same one, just ignore */
				return TRUE;
			}
			else
			{
				/* Cancel old operation */
				ianjuta_builder_cancel (builder, this->build_handle, NULL);
			}
		}

		/* Check if debug configuration is used */
		cfg_list = ianjuta_builder_list_configuration (builder, NULL);
		found = g_list_find_custom(cfg_list, IANJUTA_BUILDER_CONFIGURATION_DEBUG, (GCompareFunc)strcmp);
		if (found != NULL)
		{
			if (ianjuta_builder_get_uri_configuration (builder, target, NULL) != found->data)
			{
				if (!show_check_debug_dialog (this)) return FALSE;
			}
		}
		
		this->build_target = g_strdup (target);

		/* Check if target is up to date */
		this->build_handle = ianjuta_builder_is_built (builder, target, on_is_built_finished, this, NULL);
		return this->build_handle != 0;
	}
	else
	{
		/* Unable to build target, just launch debugger */
		return load_target (this, target) && start_target (this);
	}	
}

static gboolean
dma_start_load_and_start_uri (DmaStart *this, const gchar *target)
{
	gchar *local_path = NULL;

	if (!dma_quit_debugger (this)) return FALSE;
	
	local_path = anjuta_util_get_local_path_from_uri (target);
	if (!local_path)
	{
		return FALSE;
	}
	g_free (local_path);
	
	return check_target (this, target);
}

/* Remote target dialog
 *---------------------------------------------------------------------------*/

static void on_radio_toggled(GtkWidget* toggle_button, GtkWidget* container)
{
	gtk_widget_set_sensitive(container, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button)));
}

static gboolean
show_remote_dialog (DmaStart *this)
{
	GtkBuilder *bxml;
	GtkWindow *parent;
	GtkWidget *dialog;
	GtkEntry *tcpip_address_entry;
	GtkEntry *tcpip_port_entry;
	GtkEntry *serial_port_entry;
	GtkToggleButton *serial_radio;
	GtkToggleButton *tcpip_radio;
	GtkWidget *tcpip_container;
	GtkWidget *serial_container;
	gint res;

	/* Fetch out the widget we care about for now */
	bxml = anjuta_util_builder_new (GLADE_FILE, NULL);
	if (!bxml) return FALSE;
	anjuta_util_builder_get_objects (bxml,
	    REMOTE_DEBUG_DIALOG, &dialog,
	    TCPIP_ADDRESS_ENTRY, &tcpip_address_entry,
	    TCPIP_PORT_ENTRY, &tcpip_port_entry,
	    SERIAL_PORT_ENTRY, &serial_port_entry,
	    TCPIP_RADIO, &tcpip_radio,
	    SERIAL_RADIO, &serial_radio,
	    TCPIP_CONTAINER, &tcpip_container,
	    SERIAL_CONTAINER, &serial_container,
	    NULL);
	g_object_unref (bxml);

	parent = GTK_WINDOW (this->plugin->shell);
	gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

	/* Connect signals */	
	g_signal_connect (G_OBJECT (tcpip_radio), "toggled", G_CALLBACK (on_radio_toggled), tcpip_container);
	g_signal_connect (G_OBJECT (serial_radio), "toggled", G_CALLBACK (on_radio_toggled), serial_container);	

	/* Populate the remote target */
	if (this->remote_debugger)
	{
		if (strncmp(this->remote_debugger, "tcp:", 4) == 0)
		{
			gchar *port = strrchr (this->remote_debugger, ':');
			
			if (port != NULL)
			{
				gtk_entry_set_text (GTK_ENTRY (tcpip_port_entry), port + 1);
				*port = '\0';
			}
			gtk_entry_set_text (GTK_ENTRY (tcpip_address_entry), this->remote_debugger + 4);
			if (port != NULL) *port = ':';
			gtk_toggle_button_set_active (tcpip_radio, TRUE);
			gtk_toggle_button_set_active (serial_radio, FALSE);
		}
		else if (strncmp(this->remote_debugger,"serial:", 7) == 0)
		{
			gtk_entry_set_text (GTK_ENTRY (serial_port_entry), this->remote_debugger + 7);
			gtk_toggle_button_set_active (serial_radio, TRUE);
			gtk_toggle_button_set_active (tcpip_radio, FALSE);
		}
	}

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	
	if (res == GTK_RESPONSE_APPLY || res == GTK_RESPONSE_ACCEPT)
	{
		/* Save these settings */
		g_free (this->remote_debugger);
		if (gtk_toggle_button_get_active (serial_radio))
		{
			this->remote_debugger = g_strconcat ("serial:",
				gtk_entry_get_text (serial_port_entry),
				NULL);
		}
		else
		{
			this->remote_debugger = g_strconcat ("tcp:",
				gtk_entry_get_text (tcpip_address_entry),
				":",
				gtk_entry_get_text (tcpip_port_entry),
				NULL);
		}
	}
	gtk_widget_destroy (dialog);

	if (res == GTK_RESPONSE_ACCEPT)
	{
		return TRUE;
	}

	return FALSE;
}


/* Add source dialog
 *---------------------------------------------------------------------------*/

static void
on_add_uri_in_model (gpointer data, gpointer user_data)
{
	GtkListStore* model = (GtkListStore *)user_data;
	GtkTreeIter iter;
	gchar *local;
	
	local = anjuta_util_get_local_path_from_uri ((const char *)data);
	gtk_list_store_append (model, &iter);
	gtk_list_store_set (model, &iter, 0, local, -1);
	g_free (local);
}

static gboolean
on_add_source_in_list (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data)
{
	GList** list = (GList **)user_data;
	gchar* dir;
	gchar* uri;
	GFile *file;

	gtk_tree_model_get (model, iter, 0, &dir, -1);
	file = g_file_new_for_path (dir);
	uri =  g_file_get_uri (file);
	*list = g_list_prepend (*list, uri);
	g_object_unref (file);
	g_free (dir);
	
	return FALSE;
}

static void
on_source_add_button (GtkButton *button, AddSourceDialog *dlg)
{
	GtkTreeIter iter;
	const gchar *path;
	
	path = gtk_file_chooser_get_filename (dlg->entry);
	if ((path != NULL) && (*path != '\0'))
	{
		gtk_list_store_append (dlg->model, &iter);
		gtk_list_store_set (dlg->model, &iter, 0, path, -1);
	}
}

static void
on_source_remove_button (GtkButton *button, AddSourceDialog *dlg)
{
	GtkTreeIter iter;
	GtkTreeSelection* sel;

	sel = gtk_tree_view_get_selection (dlg->tree);
	if (gtk_tree_selection_get_selected (sel, NULL, &iter))
	{
		gtk_list_store_remove (dlg->model, &iter);
	}
}

static void
on_source_up_button (GtkButton *button, AddSourceDialog *dlg)
{
	GtkTreeIter iter;
	GtkTreeSelection* sel;

	sel = gtk_tree_view_get_selection (dlg->tree);
	if (gtk_tree_selection_get_selected (sel, NULL, &iter))
	{
		GtkTreePath *path;
		
		path = gtk_tree_model_get_path(GTK_TREE_MODEL (dlg->model), &iter);
		if (gtk_tree_path_prev(path))
		{
			GtkTreeIter pos;
			
        	gtk_tree_model_get_iter(GTK_TREE_MODEL (dlg->model), &pos, path);
			gtk_list_store_move_before (dlg->model, &iter, &pos);
		}
	}
}

static void
on_source_down_button (GtkButton *button, AddSourceDialog *dlg)
{
	GtkTreeIter iter;
	GtkTreeSelection* sel;
	
	sel = gtk_tree_view_get_selection (dlg->tree);
	if (gtk_tree_selection_get_selected (sel, NULL, &iter))
	{
		GtkTreeIter pos = iter;
				
		if (gtk_tree_model_iter_next (GTK_TREE_MODEL (dlg->model), &pos))
		{
			gtk_list_store_move_after (dlg->model, &iter, &pos);
		}
	}
}

static void
add_source_show (DmaStart *this)
{
	AddSourceDialog dlg;
	GtkBuilder *bxml;
	GtkWidget *widget;
	GtkWindow *parent;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *add_button;
	GObject *remove_button;
	GObject *up_button;
	GObject *down_button;

	parent = GTK_WINDOW (this->plugin->shell);

	bxml = anjuta_util_builder_new (GLADE_FILE, NULL);
	if (!bxml) return;
	anjuta_util_builder_get_objects (bxml,
	    ADD_SOURCE_DIALOG, &widget,
	    SOURCE_LIST, &dlg.tree,
	    SOURCE_ENTRY, &dlg.entry,
	    ADD_BUTTON, &add_button,
	    REMOVE_BUTTON, &remove_button,
	    UP_BUTTON, &up_button,
	    DOWN_BUTTON, &down_button,
	    NULL),
	g_object_unref (bxml);
	
	/* Connect signals */	
	g_signal_connect (add_button, "clicked", G_CALLBACK (on_source_add_button), &dlg);
	g_signal_connect (remove_button, "clicked", G_CALLBACK (on_source_remove_button), &dlg);
	g_signal_connect (up_button, "clicked", G_CALLBACK (on_source_up_button), &dlg);
	g_signal_connect (down_button, "clicked", G_CALLBACK (on_source_down_button), &dlg);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Path"), renderer, "text", 0, NULL);
	gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_append_column (dlg.tree, column);
	gtk_tree_view_set_expander_column(dlg.tree, column);
	
	dlg.model = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (dlg.tree, GTK_TREE_MODEL (dlg.model));
	
	gtk_window_set_transient_for (GTK_WINDOW (widget), parent);

	/* Initialize source path list */
	g_list_foreach (this->source_dirs, on_add_uri_in_model, dlg.model);
	
	/* Run dialog */
	for (;;)
	{
		gint response = gtk_dialog_run (GTK_DIALOG (widget));
		
		switch (response)
		{
		case GTK_RESPONSE_DELETE_EVENT:
		case GTK_RESPONSE_CLOSE:
			g_list_foreach (this->source_dirs, (GFunc)g_free, NULL);
			g_list_free (this->source_dirs);
			this->source_dirs = NULL;
			gtk_tree_model_foreach (GTK_TREE_MODEL (dlg.model), on_add_source_in_list, &this->source_dirs);
			this->source_dirs = g_list_reverse (this->source_dirs);
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_list_store_clear (dlg.model);
			g_list_foreach (this->source_dirs, on_add_uri_in_model, dlg.model);
			continue;
		default:
			break;
		}
		break;
	}
	gtk_widget_destroy (widget);
}


/* Public functions
 *---------------------------------------------------------------------------*/

gboolean
dma_quit_debugger (DmaStart* this)
{
	if (dma_debugger_queue_get_state (this->debugger) > IANJUTA_DEBUGGER_PROGRAM_LOADED)
	{
        gchar *msg = _("The program is already running.\n"
                      	"Do you still want to stop the debugger?");
		
		if (!anjuta_util_dialog_boolean_question (GTK_WINDOW (this->plugin->shell), TRUE, msg)) return FALSE;
	}
	
	dma_queue_interrupt (this->debugger);
	dma_queue_quit (this->debugger);

	return TRUE;
}

void
dma_add_source_path (DmaStart *self)
{
	add_source_show (self);	
}

void
dma_attach_to_process (DmaStart* this)
{
	pid_t selected_pid;
	GtkWindow *parent;
	AttachProcess *attach_process = NULL;
	
	if (!dma_quit_debugger (this)) return;
	
	parent = GTK_WINDOW (ANJUTA_PLUGIN (this->plugin)->shell);
	attach_process = attach_process_new();
	
	selected_pid = attach_process_show (attach_process, parent);
	if (selected_pid > 0)
	{
		long lpid = selected_pid;
		GList *search_dirs;
		
		search_dirs = get_source_directories (this->plugin);
		
		dma_queue_attach (this->debugger, lpid, this->source_dirs);
		free_source_directories (search_dirs);
	}
	attach_process_destroy(attach_process);
}

gboolean
dma_run_target (DmaStart *this, const gchar *target)
{
	gchar *target_uri = NULL;

	if (target == NULL)
	{
		anjuta_shell_get (ANJUTA_PLUGIN (this->plugin)->shell,
	 				  RUN_PROGRAM_URI, G_TYPE_STRING, &target_uri, NULL);	
		target = target_uri;
	}
	
	if (target == NULL)
	{
		/* Launch parameter dialog to get a target name */
		show_parameters_dialog (this);
		anjuta_shell_get (ANJUTA_PLUGIN (this->plugin)->shell,
	 				  RUN_PROGRAM_URI, G_TYPE_STRING, &target_uri, NULL);
		/* No target set by user */
		if (target_uri == NULL) return FALSE;
		target = target_uri;
	}
	
	if (!dma_start_load_and_start_uri (this, target)) return FALSE;
	g_free (target_uri);
	
	return TRUE;
}

gboolean
dma_run_remote_target (DmaStart *this, const gchar *remote, const gchar *target)
{
	gchar *target_uri;

	if (target == NULL)
	{
		anjuta_shell_get (ANJUTA_PLUGIN (this->plugin)->shell,
		 				  RUN_PROGRAM_URI, G_TYPE_STRING, &target_uri, NULL);	
		target = target_uri;
	}
	
	if (target == NULL)
	{
		/* Launch parameter dialog to get a target name */
		show_parameters_dialog (this);
		anjuta_shell_get (ANJUTA_PLUGIN (this->plugin)->shell,
	 				  RUN_PROGRAM_URI, G_TYPE_STRING, &target_uri, NULL);
		/* No target set by user */
		if (target_uri == NULL) return FALSE;
		target = target_uri;
	}
	
	if (remote == NULL)
	{
		if (!show_remote_dialog (this)) return FALSE;
		remote = this->remote_debugger;		
		if (remote == NULL) return FALSE;
	}

	if (!load_target (this, target)) return FALSE;
	g_free (target_uri);
	
	return start_remote_target (this, remote);
}

/* Constructor & Destructor
 *---------------------------------------------------------------------------*/

DmaStart *
dma_start_new (DebugManagerPlugin *plugin)
{
	DmaStart *self;
	
	self = g_new0 (DmaStart, 1);

	self->plugin = ANJUTA_PLUGIN (plugin);
	self->debugger = dma_debug_manager_get_queue (plugin);
	self->source_dirs = NULL;
	self->build_target = NULL;
	
	g_signal_connect (self->plugin->shell, "save-session",
					  G_CALLBACK (on_session_save), self);
    g_signal_connect (self->plugin->shell, "load-session",
					  G_CALLBACK (on_session_load), self);
	
	return self;
}

void
dma_start_free (DmaStart *this)
{
	g_signal_handlers_disconnect_by_func (this->plugin->shell, G_CALLBACK (on_session_save), this);
    g_signal_handlers_disconnect_by_func (this->plugin->shell, G_CALLBACK (on_session_load), this);
	g_list_foreach (this->source_dirs, (GFunc)g_free, NULL);
	g_list_free (this->source_dirs);
	g_free (this);
}

