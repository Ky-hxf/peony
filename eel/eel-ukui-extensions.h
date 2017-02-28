/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* eel-ukui-extensions.h - interface for new functions that operate on
                                 ukui classes. Perhaps some of these should be
  			         rolled into ukui someday.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.
   Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Darin Adler <darin@eazel.com>
*/

#ifndef EEL_UKUI_EXTENSIONS_H
#define EEL_UKUI_EXTENSIONS_H

#include <gtk/gtk.h>

/* Return a command string containing the path to a terminal on this system. */
char *        eel_ukui_make_terminal_command                         (const char               *command);

/* Open up a new terminal, optionally passing in a command to execute */
void          eel_ukui_open_terminal_on_screen                       (const char               *command,
        GdkScreen                *screen);

#endif /* EEL_UKUI_EXTENSIONS_H */
