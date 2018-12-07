#include <gtk/gtk.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>


#include "main.h"
#include "iniFile.cpp"
#include "functions.h"
#include "settings.h"
#include "gui.h"


#define APP_VERSION "0.3"

#include "settings.cpp"
#include "gui.cpp"
#include "functions.cpp"
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer user_data);
static gboolean clicked(GtkWindow *win, GdkEventButton *event, gpointer user_data);
static guint time_handler(GtkWidget* widget);
static gboolean enter_notify_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean leave_notify_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);

int main(int argc, char **argv)
{
	printf("=== MyStats v%s ===\n",APP_VERSION);
	std::string homeDir(getenv("HOME"));
	appDir=std::string(homeDir+"/.MyStats");
	logDir=std::string(appDir+"/logs");
	if(!chkDir(appDir.c_str())) return 0;
	confFile=strdup(std::string(appDir+"/config").c_str());
	if(access(confFile,0)){printf("NO FILE [%s]\n",confFile);createConfig();}
	loadConfig();



	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_POPUP);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window),winWidth,winHeight);
	gtk_window_set_title(GTK_WINDOW(window), "MyStats");
	g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);
	g_signal_connect(G_OBJECT(window), "enter-notify-event", G_CALLBACK(enter_notify_event), NULL);
	g_signal_connect(G_OBJECT(window), "leave-notify-event", G_CALLBACK(leave_notify_event), NULL);
	g_signal_connect(G_OBJECT(window), "motion-notify-event", G_CALLBACK(motion_notify_event), NULL);
	//g_signal_connect(G_OBJECT(window), "draw", G_CALLBACK(draw), NULL);
	g_signal_connect(G_OBJECT(window), "screen-changed", G_CALLBACK(screen_changed), NULL);
	g_signal_connect(G_OBJECT(window), "button-release-event", G_CALLBACK(clicked), NULL);

	gtk_widget_set_app_paintable(window, TRUE);

	gtk_widget_add_events(window, GDK_BUTTON_RELEASE_MASK
						  | GDK_POINTER_MOTION_MASK);
	screen_changed(window, NULL, NULL);

	g_timeout_add(winUpdateTime,(GSourceFunc) time_handler, (gpointer) window);


	gtk_widget_show_all(window);

	//start app loop
	gtk_main();

	return 0;
}






static gboolean motion_notify_event(GtkWidget *widget,GdkEventMotion  *event,gpointer   user_data)
{
	Q_UNUSED(widget);
	Q_UNUSED(user_data);
	mx=(int)event->x;
	my=(int)event->y;
	return true;
}
static gboolean leave_notify_event(GtkWidget *widget,GdkEvent  *event,gpointer   user_data)
{
	Q_UNUSED(widget);
	Q_UNUSED(event);
	Q_UNUSED(user_data);
	if(showWindow) showWindow=false;
	return true;
}
static gboolean enter_notify_event(GtkWidget *widget,GdkEvent  *event,gpointer   user_data)
{
	Q_UNUSED(widget);
	Q_UNUSED(event);
	Q_UNUSED(user_data);
	if(!showWindow) showWindow=true;
	return true;
}
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
	Q_UNUSED(old_screen);
	Q_UNUSED(userdata);
	GdkScreen *screen = gtk_widget_get_screen(widget);
	GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
	//GdkColormap* colorMap;
	if (!visual){
		printf("Your screen does not support alpha channels!\n");
		visual = gdk_screen_get_system_visual(screen);
		supports_alpha = FALSE;
		//colorMap=gdk_screen_get_rgba_colormap(screen);
	}else{
		printf("Your screen supports alpha channels!\n");
		supports_alpha = TRUE;
		//colorMap=gdk_screen_get_rgb_colormap(screen);
	}
	gtk_widget_set_visual(widget, visual);
	//gdk_screen_set_default_colormap(screen,colorMap);
}
static guint time_handler(GtkWidget* widget)
{
	int rUpd=1000/winUpdateTime;
	if(rUpd>0){
		if(updCounter<rUpd){
			updCounter++;
		}else{
			updCounter=0;
			upd();
		}
	}else{
		upd();
	}
	gtk_window_get_position(GTK_WINDOW(widget),&winX,&winY);
	gtk_window_get_size(GTK_WINDOW(widget),&winWidth,&winHeight);
	dispWidth=gdk_screen_get_width(gdk_window_get_screen(gtk_widget_get_window(widget)));
	dispHeight=gdk_screen_get_height(gdk_window_get_screen(gtk_widget_get_window(widget)));
	if(!winAutohide) showWindow=true;

	if(showWindow) draw(widget);
	if(contextHeight==0) contextHeight=winHeight;
	if(winHeight!=contextHeight) gtk_window_resize(GTK_WINDOW(widget),winWidth,contextHeight);
	if(!showWindow){
		if(winX!=dispWidth-4 or winY!=0) gtk_window_move(GTK_WINDOW(widget),dispWidth-4,0);
	}else{
		if(winX!=dispWidth-winWidth or winY!=0) gtk_window_move(GTK_WINDOW(widget),dispWidth-winWidth,0);
	}
	//gtk_main_iteration();
	//gtk_events_pending();
	//gtk_widget_queue_draw_area(window,0,0,winwidth,wintotcontentheight);
	return 1;
}

static gboolean clicked(GtkWindow *win, GdkEventButton *event, gpointer user_data)
{
	Q_UNUSED(win);
	Q_UNUSED(user_data);
	int button=event->button;
	//printf(">>[%i]\n",event->button);
	bool action=false;
	// action from disks
	//printf("CLICK: [%i:%i][%i,%i][%i]	[%s][%s]\n",mx,my,mstate,GDK_BUTTON1_MASK,FirsState,ejectDist.c_str(),hoverDisk.c_str());
	//open disk
	if(hoverDisk.length()>0 and ejectDist.length()==0 and button==1){
		std::string cmd="xdg-open "+disksMount[hoverDisk];
		system(cmd.c_str());
		action=true;
	}
	//eject disk
	if(hoverDisk.length()>0 and ejectDist.length()>0 and button==1){
		std::string cmd;
		std::string partition=ejectDist;
		std::string dev=ejectDist.substr(0,ejectDist.length()-1);
		cmd="udisks --unmount "+partition+" || udisksctl unmount -b "+partition;
		system(cmd.c_str());sleep(1);
		cmd="udisks --detach "+dev+" || udisksctl power-off -b "+dev;
		system(cmd.c_str());sleep(1);
		cmd="notify-send -i gnome-terminal 'Операции с устройствами' 'Устройство успешно извлечено!'";
		system(cmd.c_str());sleep(1);
		ejectDist="";
		hoverDisk="";
		action=true;
	}
	if(!action and button==1) winAutohide=!winAutohide;
	//change logs state
	if(button==3) saveLogs=!saveLogs;
	return true;
}
