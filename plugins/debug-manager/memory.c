/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    memory.c
    Copyright (C) 2006 Sebastien Granjoux

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
#include "config.h"
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdk.h>

/*#define DEBUG*/
#include <libanjuta/anjuta-debug.h>

#include "memory.h"

#include "data_view.h"
#include "plugin.h"
#include "queue.h"

/* Constants
 *---------------------------------------------------------------------------*/

/* Types
 *---------------------------------------------------------------------------*/

struct _DmaMemory
{
	DmaDebuggerQueue *debugger;
	AnjutaPlugin *plugin;
	GtkWidget *window;
	DmaDataView *dataview;
	DmaDataBuffer *buffer;
	GtkWidget *menu;
};

/* Private functions
 *---------------------------------------------------------------------------*/

static void
on_memory_block_read (const IAnjutaDebuggerMemoryBlock *block, DmaMemory *mem, GError *err)
{
	const gchar* tag;
	
	if (block == NULL)
	{
		/* Command has been cancelled */
		return;
	}
	
	guint length = block->length;
	gchar *data = block->data;
	gulong address = block->address;
	
	tag = data + length;
	while (length != 0)
	{
		const gchar* start;
		
		/* Skip undefined data */
		for (start = tag; *tag == 0; tag++)
		{
			length--;
			if (length == 0) return;
		}
		data += tag - start;
		address += tag - start;
		
		/* Compute length of defined data */
		for (start = tag; (length != 0) && (*tag != 0); tag++)
		{
			length--;
		}
		
		dma_data_buffer_set_data (mem->buffer, address, tag - start, data);
		data += tag - start;
		address += tag - start;
	}
}

static void
read_memory_block (gulong address, gulong length, gpointer user_data)
{
	DmaMemory *mem = (DmaMemory *)user_data;
	
	if (mem->debugger != NULL)
	{	
		dma_queue_inspect_memory (
				mem->debugger,
				address,
				(guint)length,
				(IAnjutaDebuggerCallback)on_memory_block_read,
				mem);
	}
}

static void
dma_memory_update (DmaMemory *mem)
{
	dma_data_buffer_invalidate(mem->buffer);
	dma_data_view_refresh(mem->dataview);
}

static void
on_program_stopped (DmaMemory *mem)
{
	dma_memory_update (mem);
}

static void
destroy_memory_gui (DmaMemory *mem)
{
	/* Destroy menu */
	if (mem->menu != NULL)
	{
		gtk_widget_destroy (mem->menu);
	}

	if (mem->window != NULL)
	{
		gtk_widget_destroy (mem->window);
		mem->window = NULL;
		mem->dataview = NULL;
		dma_data_buffer_remove_all_page (mem->buffer);
	}
	
	/* Remove buffer */
	if (mem->buffer != NULL)
	{	
		g_object_unref (mem->buffer);
		mem->buffer = NULL;
	}
}

static void
on_debugger_stopped (DmaMemory *mem)
{
	
	g_signal_handlers_disconnect_by_func (mem->plugin, G_CALLBACK (on_debugger_stopped), mem);
	g_signal_handlers_disconnect_by_func (mem->plugin, G_CALLBACK (on_program_stopped), mem);
	
	destroy_memory_gui (mem);
}

static gboolean
create_memory_gui (DmaMemory *mem)
{
	g_return_val_if_fail (mem->buffer == NULL, FALSE);
	g_return_val_if_fail (mem->window == NULL, FALSE);
	
	mem->buffer = dma_data_buffer_new (0x0000, 0xFFFFFFFFU, read_memory_block, NULL, mem);
	if (mem->buffer == NULL) return FALSE;
	
	mem->dataview = (DmaDataView*)dma_data_view_new_with_buffer (mem->buffer);

	mem->window = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (mem->window), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (mem->window),
	                   GTK_WIDGET (mem->dataview));

	gtk_widget_show_all (mem->window);
	anjuta_shell_add_widget (mem->plugin->shell,
							 mem->window,
                             "AnjutaDebuggerMemory", _("Memory"),
                             "debugger-memory", ANJUTA_SHELL_PLACEMENT_NONE,
							 NULL);
	
	return TRUE;
}

static void
on_debugger_started (DmaMemory *mem)
{
	/* Keep graphical interface if already existing, could happens after
	 * unloading a program in the debugger */
	if (mem->window) return;
	
	if (!dma_debugger_queue_is_supported (mem->debugger, HAS_MEMORY)) return;

	if (!create_memory_gui (mem)) return;

	/* Connect signals */
	g_signal_connect_swapped (mem->plugin, "debugger-stopped", G_CALLBACK (on_debugger_stopped), mem);
	g_signal_connect_swapped (mem->plugin, "program-stopped", G_CALLBACK (on_program_stopped), mem);
}

/* Constructor & Destructor
 *---------------------------------------------------------------------------*/

DmaMemory*
dma_memory_new(DebugManagerPlugin *plugin)
{
	DmaMemory* mem;
	
	mem = g_new0 (DmaMemory, 1);
	
	mem->debugger = dma_debug_manager_get_queue (plugin);
	mem->plugin = ANJUTA_PLUGIN (plugin);

	g_signal_connect_swapped (mem->plugin, "debugger-started", G_CALLBACK (on_debugger_started), mem);

	return mem;
}

void
dma_memory_free(DmaMemory* mem)
{
	g_return_if_fail (mem != NULL);

	g_signal_handlers_disconnect_matched (mem->plugin, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, mem);	
	
	destroy_memory_gui (mem);

	g_free(mem);
}


