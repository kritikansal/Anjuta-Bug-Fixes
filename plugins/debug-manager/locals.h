/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    locals.h
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

#ifndef _LOCALS_H_
#define _LOCALS_H_

#include "plugin.h"

#include <libanjuta/interfaces/ianjuta-debugger.h>
#include <libanjuta/anjuta-plugin.h>

#include <gtk/gtk.h>

typedef struct _Locals Locals;

Locals *locals_new (DebugManagerPlugin *plugin);
void locals_free (Locals *l);

gchar* locals_find_variable_value (Locals *l, const gchar *name);

#endif
