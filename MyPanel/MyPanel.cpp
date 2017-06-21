/* Small launcher, based on DrSmyrke
#
# Any original DrSmyrke code is licensed under the BSD license
#
# All code written since the fork of DrSmyrke is licensed under the GPL
#
#
# Copyright (c) 2015 Prokofiev Y. <Smyrke2005@yandex.ru>
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
*/
#include <iostream>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <unistd.h>

int winWidth;
int winHeight;
GtkWidget* cmdField;

static gboolean enter_notify_event(GtkWidget *widget,GdkEvent *event,gpointer user_data)
{
	gtk_window_get_size(GTK_WINDOW(widget),&winWidth,&winHeight);
	int w=gdk_screen_width();
	int h=gdk_screen_height();
	gtk_window_move(GTK_WINDOW (widget),w/2-winWidth/2,0);
	//gtk_button_set_image(GTK_BUTTON(buttonNM),gtk_image_new_from_file("bin/data/img/status.svg"));
	gtk_window_set_opacity(GTK_WINDOW (widget),1);
}

static gboolean leave_notify_event(GtkWidget *widget,GdkEvent *event,gpointer user_data)
{
	gtk_window_get_size(GTK_WINDOW(widget),&winWidth,&winHeight);
	int w=gdk_screen_width();
	int h=gdk_screen_height();
	gtk_window_move(GTK_WINDOW (widget),w/2-winWidth/2,-23);
	gtk_window_set_opacity(GTK_WINDOW (widget),0.2);
}

static void callback(GtkWidget *widget,gpointer data){
	char* cmd=(char *)data;
	if(cmd=="PASSWD"){system("~/bin/passwd.3eфште");return;}
	if(cmd=="OPENWIN"){system("~/bin/myexec");return;}
	if(cmd=="MENU"){system("~/bin/mymenu");return;}
	if(cmd=="Кор"){system("rm -r ~/.local/share/Trash/files/*");return;}
	if(cmd=="Wall"){system("~/bin/AutoWallpapers.Зефште");return;}
//if(cmd=="App"){system("sdf=$(find ~/bin -name '*' -type f -executable -printf '%f\n' |  zenity  --list  --title  'Запускатор' --text 'Мой софт' --column 'Программы')\nsdf=$(find ~/bin -name \"$sdf\" -type f -executable)\necho $sdf\ngnome-terminal -e $sdf");}
//if(cmd=="WK"){system("wmctrl -c :SELECT:");}
	if(cmd=="WK"){system("xkill");return;}
}
int main(int argc,char *argv[])
{
	gtk_init (&argc, &argv);
	gdk_init (&argc, &argv);

	GtkWidget* window = gtk_window_new (GTK_WINDOW_POPUP);
	g_signal_connect(G_OBJECT(window),"destroy",gtk_main_quit,NULL);
	g_signal_connect(G_OBJECT(window),"delete-event",gtk_main_quit, NULL);
	g_signal_connect(G_OBJECT(window), "leave-notify-event", G_CALLBACK(leave_notify_event), NULL);
	g_signal_connect(G_OBJECT(window), "enter-notify-event", G_CALLBACK(enter_notify_event), NULL);
	gtk_window_set_title (GTK_WINDOW (window), "MyPanel");
	// Изменение размера окна
	gtk_window_set_resizable(GTK_WINDOW (window),FALSE);
	// Скрываем окно в панели задач
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW (window),TRUE);
	// Скрываем окно в пейджере
	gtk_window_set_skip_pager_hint(GTK_WINDOW (window),TRUE);
	// Устанавливаем окно поверх остальных
	gtk_window_set_keep_above(GTK_WINDOW (window),TRUE);
	// Убираем рамку окна
	gtk_window_set_decorated(GTK_WINDOW (window),FALSE);
	// Задаем прозрачность окна
	gtk_window_set_opacity(GTK_WINDOW (window),0.2);
	// Прилепляем окно на все рабочие столы
	gtk_window_stick(GTK_WINDOW (window));
	GtkWidget* HBox=gtk_hbox_new(FALSE,0);
		GtkWidget* b1 = gtk_button_new_with_label("MENU");
			g_signal_connect(b1,"clicked",G_CALLBACK(callback),(gpointer) "MENU");
		gtk_box_pack_start(GTK_BOX(HBox),b1,FALSE,FALSE,0);
		GtkWidget* b2 = gtk_button_new();
			gtk_button_set_image(GTK_BUTTON(b2),gtk_image_new_from_stock(GTK_STOCK_CLEAR,GTK_ICON_SIZE_MENU));
			g_signal_connect(b2,"clicked",G_CALLBACK(callback),(gpointer) "Кор");
		gtk_box_pack_start(GTK_BOX(HBox),b2,FALSE,FALSE,0);
		GtkWidget* b3 = gtk_button_new_with_label("Wall");
			g_signal_connect(b3,"clicked",G_CALLBACK(callback),(gpointer) "Wall");
		gtk_box_pack_start(GTK_BOX(HBox),b3,FALSE,FALSE,0);
		GtkWidget* b4 = gtk_button_new();
			gtk_button_set_image(GTK_BUTTON(b4),gtk_image_new_from_stock(GTK_STOCK_QUIT,GTK_ICON_SIZE_MENU));
			g_signal_connect(b4,"clicked",G_CALLBACK(callback),(gpointer) "WK");
		gtk_box_pack_start(GTK_BOX(HBox),b4,FALSE,FALSE,0);
		GtkWidget* b5 = gtk_button_new();
			gtk_button_set_image(GTK_BUTTON(b5),gtk_image_new_from_stock(GTK_STOCK_APPLY,GTK_ICON_SIZE_MENU));
			g_signal_connect(b5,"clicked",G_CALLBACK(callback),(gpointer) "OPENWIN");
		gtk_box_pack_start(GTK_BOX(HBox),b5,FALSE,FALSE,0);
		GtkWidget* b6 = gtk_button_new();
			gtk_button_set_image(GTK_BUTTON(b6),gtk_image_new_from_stock(GTK_STOCK_DIALOG_AUTHENTICATION,GTK_ICON_SIZE_MENU));
			g_signal_connect(b6,"clicked",G_CALLBACK(callback),(gpointer) "PASSWD");
		gtk_box_pack_start(GTK_BOX(HBox),b6,FALSE,FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),HBox);

	
	gtk_widget_show_all (window);
	gtk_window_set_default_size(GTK_WINDOW (window),10,10);
	gtk_window_resize(GTK_WINDOW (window),10,10);
	int w=gdk_screen_width();
	int h=gdk_screen_height();
	// Перемещаем окно
	gtk_window_get_size(GTK_WINDOW(window),&winWidth,&winHeight);
	gtk_window_move(GTK_WINDOW (window),w/2-winWidth/2,-23);
	std::cout << w << "\n";
	std::cout << h << "\n";
	gtk_main ();
	return 0;
}
