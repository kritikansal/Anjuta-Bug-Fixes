/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-debugger-memory.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_DEBUGGER_MEMORY_H_
#define _IANJUTA_DEBUGGER_MEMORY_H_

#include <glib-object.h>
#include <libanjuta/interfaces/ianjuta-debugger.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_DEBUGGER_MEMORY (ianjuta_debugger_memory_get_type ())
#define IANJUTA_DEBUGGER_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_DEBUGGER_MEMORY, IAnjutaDebuggerMemory))
#define IANJUTA_IS_DEBUGGER_MEMORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_DEBUGGER_MEMORY))
#define IANJUTA_DEBUGGER_MEMORY_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_DEBUGGER_MEMORY, IAnjutaDebuggerMemoryIface))

#define IANJUTA_DEBUGGER_MEMORY_ERROR ianjuta_debugger_memory_error_quark()

typedef struct _IAnjutaDebuggerMemory IAnjutaDebuggerMemory;
typedef struct _IAnjutaDebuggerMemoryIface IAnjutaDebuggerMemoryIface;

/**
 * IAnjutaDebuggerRegisterMemoryBlock:
 * @address: start address of memory block
 * @length: size of memory block
 * @data: memory block data
 *
 * Represent a block of memory.
 */
typedef struct _IAnjutaDebuggerMemoryBlock IAnjutaDebuggerMemoryBlock;
struct _IAnjutaDebuggerMemoryBlock {
	gulong address;
	guint length;
	gchar *data;
};

/**
 * IAnjutaDebuggerMemoryCallback:
 * @data: a #IAnjutaDebuggerMemoryBlock object
 * @user_data: user data passed to the function
 * @err: error
 *
 * This callback function is used to return a #IAnjutaDebuggerMemoryBlock.
 */
typedef void (*IAnjutaDebuggerMemoryCallback) (const IAnjutaDebuggerMemoryBlock *data, gpointer user_data, GError* err);



struct _IAnjutaDebuggerMemoryIface {
	IAnjutaDebuggerIface g_iface;
	

	gboolean (*inspect) (IAnjutaDebuggerMemory *obj, gulong address,  guint length,  IAnjutaDebuggerMemoryCallback callback,  gpointer user_data, GError **err);

};


GQuark ianjuta_debugger_memory_error_quark     (void);
GType  ianjuta_debugger_memory_get_type        (void);

gboolean ianjuta_debugger_memory_inspect (IAnjutaDebuggerMemory *obj, gulong address,  guint length,  IAnjutaDebuggerMemoryCallback callback,  gpointer user_data, GError **err);


G_END_DECLS

#endif
