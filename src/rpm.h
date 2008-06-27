/*
 *  Copyright (C) 2006 Giuseppe Torelli - <colossus73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#ifndef RPM_H
#define RPM_H

#include <gtk/gtk.h>
#include "window.h"
#include "interface.h"
#include "archive.h"

void xa_open_rpm ( XArchive *archive );
gboolean ExtractToDifferentLocation (GIOChannel *ioc, GIOCondition cond , gpointer data);
void xa_open_temp_file (gchar *tmp_dir,gchar *temp_path);
void xa_rpm_extract(XArchive *archive,GString *files);
void xa_get_cpio_line_content (gchar *line, gpointer data);
#endif
