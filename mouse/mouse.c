// Any original DrSmyrke code is licensed under the BSD license
//
// All code written since the fork of DrSmyrke is licensed under the GPL
//
//
// Copyright (c) 2013 Prokofiev Y. <Smyrke2005@yandex.ru>
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
#include "X11/Xlib.h"
#include "stdio.h"
int main(int argc, char *argv[]){
	Display *display;
	Window root_window;
	XEvent event;
	display=XOpenDisplay(0);
	root_window=XRootWindow(display, 0);
	//XSelectInput(display, root_window, KeyReleaseMask);
	//XWarpPointer(display, None, root_window, 0, 0, 0, 0, 100, 100);
	//XFlush(display);
	XWindowAttributes xwAttr;
	XSelectInput(display, root_window, PointerMotionMask );
	//while(1) {
		XQueryPointer(display, RootWindow(display,DefaultScreen(display)),&event.xbutton.root, &event.xbutton.window,&event.xbutton.x_root, &event.xbutton.y_root,&event.xbutton.x, &event.xbutton.y,&event.xbutton.state);
		XGetWindowAttributes(display,root_window, &xwAttr );
		printf("Mouse Coordinates: %d %d Display: %d %d\n", event.xbutton.x, event.xbutton.y,xwAttr.width,xwAttr.height);
		//}
//	while(1){
//		XNextEvent(display, &event );
//		switch(event.type){
//			case MotionNotify:
//			printf("x %d y %d\n", event.xmotion.x, event.xmotion.y );
//			break;
//	   	}
//	}
	XCloseDisplay(display); 
	return 0;
}
