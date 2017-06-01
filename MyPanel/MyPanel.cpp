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
using namespace std;
int apps=120;
GtkWidget *window,*table,*button;//,*buttonNM;
static void destroy(GtkWidget *widget,gpointer data){gtk_main_quit ();}
static void ent(GtkWidget *widget,gpointer data){
int w=gdk_screen_width();
int h=gdk_screen_height();
gtk_window_move(GTK_WINDOW (window),w/2-apps,0);
//gtk_button_set_image(GTK_BUTTON(buttonNM),gtk_image_new_from_file("bin/data/img/status.svg"));
//gtk_window_set_opacity(GTK_WINDOW (window),1);
}
static void out(GtkWidget *widget,gpointer data){
int w=gdk_screen_width();
int h=gdk_screen_height();
gtk_window_move(GTK_WINDOW (window),w/2-apps,-23);
//gtk_window_set_opacity(GTK_WINDOW (window),0);
}
static void callback(GtkWidget *widget,gpointer data){
char* cmd=(char *)data;
if(cmd=="MENU"){system("~/bin/MyMenu.Зефште");}
if(cmd=="Кор"){system("rm -r ~/.local/share/Trash/files/*");}
if(cmd=="Wall"){system("~/bin/AutoWallpapers.Зефште");}
if(cmd=="Apache"){system("~/bin/Apache.Зефште");}
if(cmd=="APPS"){system("MyApps.Зефште");}
//if(cmd=="App"){system("sdf=$(find ~/bin -name '*' -type f -executable -printf '%f\n' |  zenity  --list  --title  'Запускатор' --text 'Мой софт' --column 'Программы')\nsdf=$(find ~/bin -name \"$sdf\" -type f -executable)\necho $sdf\ngnome-terminal -e $sdf");}
//if(cmd=="WK"){system("wmctrl -c :SELECT:");}
if(cmd=="WK"){system("xkill");}
if(cmd=="NM"){system("echo '123'");}
}
int main(int argc,char *argv[]){
gtk_init (&argc, &argv);
gdk_init (&argc, &argv);
window = gtk_window_new (GTK_WINDOW_POPUP);
g_signal_connect(window,"destroy",G_CALLBACK (destroy),NULL);
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
//gtk_window_set_opacity(GTK_WINDOW (window),0);
// Прилепляем окно на все рабочие столы
gtk_window_stick(GTK_WINDOW (window));
table = gtk_table_new (8,1,FALSE);
gtk_container_add (GTK_CONTAINER (window), table);
// кнопки
button = gtk_button_new_with_label ("MENU");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "MENU");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 1, 0, 1);
gtk_widget_show (button);

button = gtk_button_new_with_label ("Кор");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "Кор");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 1, 2, 0, 1);
gtk_widget_show (button);

button = gtk_button_new_with_label ("Wall");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "Wall");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 3, 4, 0, 1);
gtk_widget_show (button);

button = gtk_button_new_with_label ("Apache");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "Apache");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 4, 5, 0, 1);
gtk_widget_show (button);

button = gtk_button_new_with_label ("APPS");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "APPS");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 5, 6, 0, 1);
gtk_widget_show (button);

button = gtk_button_new_with_label ("WK");
g_signal_connect(button,"clicked",G_CALLBACK(callback),(gpointer) "WK");
g_signal_connect(button,"enter",G_CALLBACK(ent),NULL);
g_signal_connect(button,"leave",G_CALLBACK(out),NULL);
gtk_table_attach_defaults (GTK_TABLE (table), button, 6, 7, 0, 1);
gtk_widget_show (button);

//buttonNM = gtk_button_new();
//gtk_button_set_image(GTK_BUTTON(buttonNM),gtk_image_new_from_file("bin/data/img/status.svg"));
//g_signal_connect(buttonNM,"clicked",G_CALLBACK(callback),(gpointer) "NM");
//g_signal_connect(buttonNM,"enter",G_CALLBACK(ent),NULL);
//g_signal_connect(buttonNM,"leave",G_CALLBACK(out),NULL);
//gtk_table_attach_defaults (GTK_TABLE (table), buttonNM, 6, 7, 0, 1);
//gtk_widget_show (buttonNM);
// end
gtk_widget_show (table);
gtk_widget_show (window);
int w=gdk_screen_width();
int h=gdk_screen_height();
// Перемещаем окно
gtk_window_move(GTK_WINDOW (window),w/2-apps,-23);
cout << w << "\n";
cout << h << "\n";
gtk_main ();
return 0;
}
