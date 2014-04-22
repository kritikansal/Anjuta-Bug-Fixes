/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    utilities.c
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "utilities.h"

#include <libanjuta/interfaces/ianjuta-project-manager.h>
#include <libanjuta/interfaces/ianjuta-document-manager.h>

#include <glib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>

gchar *
gdb_util_remove_white_spaces (const gchar * text)
{
	guint src_count, dest_count, tab_count;
	gchar buff[2048];	/* Let us hope that it does not overflow */
	
	tab_count = 8;
	dest_count = 0;
	
	for (src_count = 0; src_count < strlen (text); src_count++)
	{
		if (text[src_count] == '\t')
		{
			gint j;
			for (j = 0; j < tab_count; j++)
				buff[dest_count++] = ' ';
		}
		else if (isspace (text[src_count]))
		{
			buff[dest_count++] = ' ';
		}
		else
		{
			buff[dest_count++] = text[src_count];
		}
	}
	buff[dest_count] = '\0';
	return g_strdup (buff);
}

GList *
gdb_util_remove_blank_lines (const GList * lines)
{
	GList *list, *node;
	gchar *str;

	if (lines)
		list = g_list_copy ((GList*)lines);
	else
		list = NULL;

	node = list;
	while (node)
	{
		str = node->data;
		node = g_list_next (node);
		if (!str)
		{
			list = g_list_remove (list, str);
			continue;
		}
		if (strlen (g_strchomp (str)) < 1)
			list = g_list_remove (list, str);
	}
	return list;
}

IAnjutaEditor*
dma_get_current_editor (AnjutaPlugin *plugin)
{
	IAnjutaDocumentManager *docman;
	IAnjutaEditor* editor = NULL;

	docman = IANJUTA_DOCUMENT_MANAGER (anjuta_shell_get_object (plugin->shell,
					"IAnjutaDocumentManager", NULL));
									   
	if (docman != NULL)
	{
		IAnjutaDocument* doc;
		
		doc = ianjuta_document_manager_get_current_document(docman, NULL);
		if (doc != NULL)
		{
			editor = IANJUTA_IS_EDITOR(doc) ? IANJUTA_EDITOR(doc) : NULL;
		}
	}
	
	return editor;
}
