/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-print.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_PRINT_H_
#define _IANJUTA_PRINT_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_PRINT (ianjuta_print_get_type ())
#define IANJUTA_PRINT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_PRINT, IAnjutaPrint))
#define IANJUTA_IS_PRINT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_PRINT))
#define IANJUTA_PRINT_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_PRINT, IAnjutaPrintIface))

#define IANJUTA_PRINT_ERROR ianjuta_print_error_quark()

typedef struct _IAnjutaPrint IAnjutaPrint;
typedef struct _IAnjutaPrintIface IAnjutaPrintIface;


struct _IAnjutaPrintIface {
	GTypeInterface g_iface;
	

	void (*print) (IAnjutaPrint *obj, GError **err);
	void (*print_preview) (IAnjutaPrint *obj, GError **err);

};


GQuark ianjuta_print_error_quark     (void);
GType  ianjuta_print_get_type        (void);

void ianjuta_print_print (IAnjutaPrint *obj, GError **err);

void ianjuta_print_print_preview (IAnjutaPrint *obj, GError **err);


G_END_DECLS

#endif
