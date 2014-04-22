/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    start.h
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

#ifndef _START_H_
#define _START_H_

#include "plugin.h"

#include <libanjuta/interfaces/ianjuta-debugger.h>

typedef struct _DmaStart DmaStart;

DmaStart *dma_start_new (DebugManagerPlugin *plugin);
void dma_start_free (DmaStart *this);

void dma_add_source_path (DmaStart *self);
void dma_attach_to_process (DmaStart *this);
gboolean dma_run_target (DmaStart *this, const gchar* target);
gboolean dma_run_remote_target (DmaStart *this, const gchar *remote, const gchar *target);
gboolean dma_rerun_target (DmaStart *this);
gboolean dma_quit_debugger (DmaStart *this);

#endif
