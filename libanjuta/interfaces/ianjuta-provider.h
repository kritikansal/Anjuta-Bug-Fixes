/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *  ianjuta-provider.h -- Autogenerated from libanjuta.idl
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

#ifndef _IANJUTA_PROVIDER_H_
#define _IANJUTA_PROVIDER_H_

#include <glib-object.h>
#include <libanjuta/interfaces/ianjuta-iterable.h>

G_BEGIN_DECLS

#define IANJUTA_TYPE_PROVIDER (ianjuta_provider_get_type ())
#define IANJUTA_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IANJUTA_TYPE_PROVIDER, IAnjutaProvider))
#define IANJUTA_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IANJUTA_TYPE_PROVIDER))
#define IANJUTA_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), IANJUTA_TYPE_PROVIDER, IAnjutaProviderIface))

#define IANJUTA_PROVIDER_ERROR ianjuta_provider_error_quark()

typedef struct _IAnjutaProvider IAnjutaProvider;
typedef struct _IAnjutaProviderIface IAnjutaProviderIface;


struct _IAnjutaProviderIface {
	GTypeInterface g_iface;
	

	void (*activate) (IAnjutaProvider *obj, IAnjutaIterable* iter,  gpointer data, GError **err);
	const gchar* (*get_name) (IAnjutaProvider *obj, GError **err);
	IAnjutaIterable* (*get_start_iter) (IAnjutaProvider *obj, GError **err);
	void (*populate) (IAnjutaProvider *obj, IAnjutaIterable* iter, GError **err);

};


GQuark ianjuta_provider_error_quark     (void);
GType  ianjuta_provider_get_type        (void);

void ianjuta_provider_activate (IAnjutaProvider *obj, IAnjutaIterable* iter,  gpointer data, GError **err);

const gchar* ianjuta_provider_get_name (IAnjutaProvider *obj, GError **err);

IAnjutaIterable* ianjuta_provider_get_start_iter (IAnjutaProvider *obj, GError **err);

void ianjuta_provider_populate (IAnjutaProvider *obj, IAnjutaIterable* iter, GError **err);


G_END_DECLS

#endif