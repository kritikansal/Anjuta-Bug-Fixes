/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/* 
    anjuta-debug.h
    Copyright (C) 2003 Naba Kumar  <naba@gnome.org>

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

#ifndef __ANJUTA_DEBUG__
#define __ANJUTA_DEBUG__

/**
 * SECTION:anjuta-debug
 * @title: Debugging
 * @short_description: Debug functions 
 * @see_also: 
 * @stability: stable
 * @include: libanjuta/anjuta-debug.h
 * 
 * Anjuta debug messages are displayed using the g_debug() function from GLib.
 *
 * To display debug messages, Anjuta must have been compiled with
 * --enable-debug and the environment variable G_MESSAGES_DEBUG should be set
 * to "all" or to a list separated by whitespace of domains to display.
 *
 * By example
 *<programlisting>
 * G_MESSAGE_DEBUG=Gtk Anjuta libanjuta-gdb
 *</programlisting>
 * will display debug messages from Gtk, Anjuta and gdb plugin only.
 */

/**
 * DEBUG_PRINT:
 * 
 * Equivalent to g_debug() showing the FILE, the LINE and the FUNC,
 * except it has only effect when DEBUG is defined . Used for printing debug 
 * messages.
 */
#if defined (DEBUG)
	#if defined (__GNUC__) && (__GNUC__ >= 3) && !defined(__STRICT_ANSI__)
		#define DEBUG_PRINT(format, ...) g_debug ("%s:%d (%s) " format, __FILE__, __LINE__, G_STRFUNC, ##__VA_ARGS__)
	#else
		#define DEBUG_PRINT g_debug
	#endif
#else
	#define DEBUG_PRINT(...)
#endif

#endif /* _ANJUTA_DEBUG_H_ */
