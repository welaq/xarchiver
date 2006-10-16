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

#include "config.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "new_dialog.h"
#include "callbacks.h"
#include "string_utils.h"
#include "main.h"

extern gboolean unrar;
gchar *current_new_directory = NULL;
gint new_combo_box = -1;

XArchive *xa_new_archive_dialog (gchar *path)
{
	XArchive *archive = NULL;
	GtkWidget *xa_file_chooser;
	GtkWidget *hbox = NULL;
	GtkWidget *combo_box = NULL;
	GtkWidget *add_extension_cb = NULL;
	GtkFileFilter *xa_new_archive_dialog_filter;
	GtkTooltips *filter_tooltip;
	GList *Suffix,*Name;
	gchar *my_path = NULL;
	gchar *my_path_ext = NULL;

	xa_file_chooser = gtk_file_chooser_dialog_new ( _("Create a new archive"),
							GTK_WINDOW (MainWindow),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							GTK_STOCK_CANCEL,
							GTK_RESPONSE_CANCEL,
							_("Cr_eate"),
							GTK_RESPONSE_ACCEPT,
							NULL);

	gtk_dialog_set_default_response (GTK_DIALOG (xa_file_chooser), GTK_RESPONSE_ACCEPT);

	xa_new_archive_dialog_filter = gtk_file_filter_new ();
	gtk_file_filter_set_name ( xa_new_archive_dialog_filter , _("All files") );
	gtk_file_filter_add_pattern ( xa_new_archive_dialog_filter, "*" );
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (xa_file_chooser), xa_new_archive_dialog_filter);

	xa_new_archive_dialog_filter = gtk_file_filter_new ();
	gtk_file_filter_set_name ( xa_new_archive_dialog_filter , _("Only archives") );

	Suffix = g_list_first ( ArchiveSuffix );

	while ( Suffix != NULL )
	{
		gtk_file_filter_add_pattern (xa_new_archive_dialog_filter, Suffix->data);
		Suffix = g_list_next ( Suffix );
	}
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (xa_file_chooser), xa_new_archive_dialog_filter);

	Suffix = g_list_first ( ArchiveSuffix );

	while ( Suffix != NULL )
	{
		if ( Suffix->data != "" )	/* To avoid double filtering when opening the archive */
		{
			xa_new_archive_dialog_filter = gtk_file_filter_new ();
			gtk_file_filter_set_name (xa_new_archive_dialog_filter, Suffix->data );
			gtk_file_filter_add_pattern (xa_new_archive_dialog_filter, Suffix->data );
			gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (xa_file_chooser), xa_new_archive_dialog_filter);
		}
		Suffix = g_list_next ( Suffix );
	}
	hbox = gtk_hbox_new (FALSE, 12);
	gtk_box_pack_start (GTK_BOX (hbox),gtk_label_new (_("Archive type:")),FALSE, FALSE, 0);

	combo_box = gtk_combo_box_new_text ();

	filter_tooltip = gtk_tooltips_new();
	gtk_tooltips_set_tip (filter_tooltip,combo_box, _("Choose the archive type to create") , NULL);
	Name = g_list_first ( ArchiveType );
	while ( Name != NULL )
	{
		if (Name->data == ".tgz" || Name->data == ".rpm" || Name->data == ".iso" || Name->data == ".gz" || Name->data == ".bz2" ||		(Name->data == ".rar" && unrar) )
			goto Next;
		else
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo_box), Name->data );
		Next:
			Name = g_list_next ( Name );
	}
	if (new_combo_box == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box) , 0 );
	else
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box) , new_combo_box );

	gtk_box_pack_start (GTK_BOX (hbox), combo_box, TRUE, TRUE, 0);

	add_extension_cb = gtk_check_button_new_with_label (_("Add the archive extension to the filename"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(add_extension_cb),TRUE);
	gtk_box_pack_start (GTK_BOX (hbox), add_extension_cb, TRUE, TRUE, 0);
	gtk_widget_show_all (hbox);
	gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (xa_file_chooser), hbox);

	if (path != NULL)
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (xa_file_chooser),path);

	gtk_window_set_modal (GTK_WINDOW (xa_file_chooser),TRUE);
	if (current_new_directory != NULL)
		gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER (xa_file_chooser) , current_new_directory );

	response = gtk_dialog_run (GTK_DIALOG (xa_file_chooser));
	current_new_directory = gtk_file_chooser_get_current_folder ( GTK_FILE_CHOOSER (xa_file_chooser) );

	if (response == GTK_RESPONSE_ACCEPT)
	{
		my_path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (xa_file_chooser) );
		ComboArchiveType = gtk_combo_box_get_active_text (GTK_COMBO_BOX (combo_box));

		if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (add_extension_cb) ) )
		{
			if ( ! g_str_has_suffix ( my_path , ComboArchiveType ) )
			{
				my_path_ext = g_strconcat ( my_path, ComboArchiveType , NULL);
				g_free (my_path);
				my_path = my_path_ext;
			}
		}
		if ( g_file_test ( my_path , G_FILE_TEST_EXISTS ) )
		{
			gchar *utf8_path;
			gchar  *msg;

			utf8_path = g_filename_to_utf8 (my_path, -1, NULL, NULL, NULL);
			msg = g_strdup_printf (_("The archive \"%s\" already exists!"), utf8_path);
			response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_QUESTION,
							GTK_BUTTONS_YES_NO,
							msg,
							_("Do you want to overwrite it?")
							);
			g_free (utf8_path);
			g_free (msg);
			if (response != GTK_RESPONSE_YES)
			{
				g_free (my_path);
				gtk_widget_destroy (xa_file_chooser);
			    return NULL;
			}
			/* The following to avoid to update the archive instead of adding to it since the filename exists */
			unlink ( my_path );
		}
		archive = xa_init_archive_structure (archive);
		new_combo_box = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_box));

		if (strcmp ( ComboArchiveType,".arj") == 0)
			archive->type = XARCHIVETYPE_ARJ;
		else if (strcmp ( ComboArchiveType,".rar") == 0)
			archive->type = XARCHIVETYPE_RAR;
		else if (strcmp ( ComboArchiveType,".tar") == 0)
			archive->type = XARCHIVETYPE_TAR;
		else if (strcmp ( ComboArchiveType,".tar.bz2") == 0)
			archive->type = XARCHIVETYPE_TAR_BZ2;
		else if (strcmp ( ComboArchiveType,".tar.gz") == 0)
			archive->type = XARCHIVETYPE_TAR_GZ;
		else if (strcmp ( ComboArchiveType,".jar") == 0 || strcmp ( ComboArchiveType,".zip") == 0 )
			archive->type = XARCHIVETYPE_ZIP;
		else if (strcmp ( ComboArchiveType,".rpm") == 0)
			archive->type = XARCHIVETYPE_RPM;
		else if (strcmp ( ComboArchiveType,".7z") == 0)
			archive->type = XARCHIVETYPE_7ZIP;
		else if (strcmp ( ComboArchiveType,".lzh") == 0)
			archive->type = XARCHIVETYPE_LHA;

		gtk_widget_destroy (xa_file_chooser);
		archive->path = g_strdup (my_path);
		archive->escaped_path = EscapeBadChars (archive->path , "$\'`\"\\!?* ()&|@#:;");
		g_free (my_path);
		return archive;
	}
	else if ( (response == GTK_RESPONSE_CANCEL) || (response == GTK_RESPONSE_DELETE_EVENT) )
		gtk_widget_destroy (xa_file_chooser);

	return NULL;
}