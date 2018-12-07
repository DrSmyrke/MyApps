#include <cstring>
#include <gtk/gtk.h>

void setText(cairo_t *cr, const char* font, cairo_font_slant_t style,cairo_font_weight_t type,double fontSize,int x,int y,int r,int g,int b,double alpha,const char* text)
{
	cairo_select_font_face(cr,font,style,type);
	cairo_set_font_size(cr,fontSize);
	cairo_move_to(cr,x,y);
	cairo_set_source_rgba(cr,parsFromRGB(r),parsFromRGB(g),parsFromRGB(b),alpha);
	cairo_show_text(cr,text);
}
void setParam(cairo_t *cr, int x, int y, const char* text, int &contHeight)
{
	setText(cr,defaultTextFont,defaultTextStyle,defaultTextWeight,defaultTextSize,x,y,paramColor[0],paramColor[1],paramColor[2],paramColorAlpha,text);
	contHeight+=yi;
}
void setValue(cairo_t *cr, int x, int y, const char* text)
{
	setText(cr,defaultTextFont,defaultTextStyle,defaultTextWeight,defaultTextSize,x,y,valueColor[0],valueColor[1],valueColor[2],valueColorAlpha,text);
}
void getBar(cairo_t *cr, int x, int y,float value,int blockCount)
{
	y-=9;
	int pbarWidth=(progressBarBlockWidth*blockCount)+(progressBarBlockIntery*(blockCount-1));
	int prz=(pbarWidth/100.0)*value;
	cairo_set_source_rgba(cr,parsFromRGB(progressBarColor[0]),parsFromRGB(progressBarColor[1]),parsFromRGB(progressBarColor[2]),progressBarColorAlpha);
	int rx=x;
	for(int i=0;i<blockCount;i++){
		cairo_rectangle(cr,rx,y,progressBarBlockWidth,progressBarBlockHeight);
		cairo_fill(cr);
		rx=rx+progressBarBlockIntery+progressBarBlockWidth;
	}
	cairo_pattern_t* pattern = cairo_pattern_create_linear(x,y+(progressBarBlockHeight/2),x+pbarWidth,y+(progressBarBlockHeight/2));
	cairo_pattern_add_color_stop_rgba(pattern,0,0,0.7,0,progressBarColorAlpha);
	cairo_pattern_add_color_stop_rgba(pattern,1,0.7,0,0,progressBarColorAlpha);
	cairo_set_source(cr,pattern);
	rx=x;
	for(int i=0;i<blockCount;i++){
		if((rx-x)<prz){
			cairo_rectangle(cr,rx,y,progressBarBlockWidth,progressBarBlockHeight);
			cairo_fill(cr);
			rx=rx+progressBarBlockIntery+progressBarBlockWidth;
		}
	}
	static char buff[10];
	int tmp=value;
	if(tmp==100){
		sprintf(buff,"%i%%",100);
	}else{
		int r=(value*100)-(tmp*100);
		if(r==0){
			sprintf(buff,"%i%%",tmp);
		}else{
			sprintf(buff,"%i.%i%%",tmp,r);
		}
	}
	cairo_move_to(cr,x+(pbarWidth/2)-(strlen(buff)*4),y+10);
	cairo_set_source_rgba(cr,parsFromRGB(clockColor[0]),parsFromRGB(clockColor[1]),parsFromRGB(clockColor[2]),1);
	cairo_show_text(cr,buff);
}
void drawDate(cairo_t *cr, int x, int y, const char* text)
{
	setText(cr,defaultTextFont,defaultTextStyle,defaultTextWeight,defaultTextSize,x,y,valueColor[0],valueColor[1]+155,valueColor[2],valueColorAlpha,text);
}








void draw(GtkWidget *widget)
{
	cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	if (supports_alpha){
		cairo_set_source_rgba(cr,parsFromRGB(backgroundColor[0]),parsFromRGB(backgroundColor[1]),parsFromRGB(backgroundColor[2]),0.7);
	}else{
		cairo_set_source_rgb(cr,parsFromRGB(backgroundColor[0]),parsFromRGB(backgroundColor[1]),parsFromRGB(backgroundColor[2]));
	}
	cairo_paint (cr);
	cr = gdk_cairo_create(gtk_widget_get_window(widget));


	contextHeight=50;
	//window state information
	if(!winAutohide){
		setText(cr,clockFont,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD,9,170,16,255,0,0,1,"FIXED");
		cairo_move_to(cr,168,5);
		cairo_rel_line_to(cr,35,0);
		cairo_rel_line_to(cr,0,15);
		cairo_rel_line_to(cr,-35,0);
		cairo_rel_line_to(cr,0,-16);
		cairo_stroke(cr);
	}
	if(saveLogs){
		setText(cr,clockFont,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD,9,210,16,255,0,0,1,"LOGING");
		cairo_move_to(cr,208,5);
		cairo_rel_line_to(cr,44,0);
		cairo_rel_line_to(cr,0,15);
		cairo_rel_line_to(cr,-44,0);
		cairo_rel_line_to(cr,0,-16);
		cairo_stroke(cr);
	}
	//time
	setText(cr,clockFont,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD,30,15,30,clockColor[0],clockColor[1],clockColor[2],1,getTime());
	drawDate(cr,150,contextHeight,date);
	//uptime
	setValue(cr,70,contextHeight,uptime);
	setParam(cr,10,contextHeight,"Uptime:",contextHeight);
	//cpu
	getBar(cr,70,contextHeight,cpuPrz,47);
	setParam(cr,10,contextHeight,"CPU:",contextHeight);
	//mem
	getBar(cr,70,contextHeight,memPrz,15);
	std::string memUse(getSize(memUsed));
	std::string memTot(getSize(memTotal));
	std::string strMem=memUse+" / "+memTot;
	setValue(cr,135,contextHeight,strMem.c_str());
	setParam(cr,10,contextHeight,"MEM:",contextHeight);
	//swap
	getBar(cr,70,contextHeight,swapPrz,15);
	std::string swapUse(getSize(swapUsed));
	std::string swapTot(getSize(swapTotal));
	std::string strSwap=swapUse+" / "+swapTot;
	setValue(cr,135,contextHeight,strSwap.c_str());
	setParam(cr,10,contextHeight,"SWAP:",contextHeight);
	//apps
	setValue(cr,70,contextHeight,procCount);
	setParam(cr,10,contextHeight,"Apps:",contextHeight);
	//interfaces
	for(auto ifname:ifaces){
		cairo_set_source_rgba(cr,0.9,0.9,0.9,0.8);
		cairo_move_to(cr,10,contextHeight-5);
		cairo_line_to(cr,20,contextHeight-5);
		cairo_move_to(cr,80,contextHeight-5);
		cairo_line_to(cr,winWidth-10,contextHeight-5);
		cairo_stroke(cr);
		setText(cr,defaultTextFont,defaultTextStyle,defaultTextWeight,defaultTextSize,25,contextHeight,clockColor[0],clockColor[1],clockColor[2],1.0,ifname.c_str());
		contextHeight+=15;

		//float rTime=(float)winUpdateTime/1000.0;
		//long rxSpeed=(ifaceRX[ifname]-ifaceRXprew[ifname])/rTime;
		long rxSpeed=ifaceRX[ifname]-ifaceRXprew[ifname];
		long txSpeed=ifaceTX[ifname]-ifaceTXprew[ifname];

		std::string rxSp=std::string(getSize(rxSpeed))+" / s";
		setValue(cr,70,contextHeight,rxSp.c_str());
		std::string rxTot="+ "+std::string(getSize(ifaceRX[ifname]));
		setValue(cr,190,contextHeight,rxTot.c_str());
		setParam(cr,10,contextHeight,"Down:",contextHeight);

		std::string txSp=std::string(getSize(txSpeed))+" / s";
		setValue(cr,70,contextHeight,txSp.c_str());
		std::string txTot="+ "+std::string(getSize(ifaceTX[ifname]));
		setValue(cr,190,contextHeight,txTot.c_str());
		setParam(cr,10,contextHeight,"Up:",contextHeight);

		std::string rxtxTot="= "+std::string(getSize(ifaceRX[ifname]+ifaceTX[ifname]));
		setValue(cr,190,contextHeight,rxtxTot.c_str());
		setValue(cr,70,contextHeight,getIfaceIp(ifname).c_str());
		setParam(cr,10,contextHeight,"IP:",contextHeight);
	}
	//disks
	for(auto disk:disks){
		bool drawEject=false;
		if(disksMount[disk].find("/media")<disksMount[disk].length()){
			if(my>=contextHeight-10 and my<contextHeight+yi){
				cairo_set_source_rgba(cr,0.9,0.9,0.9,0.1);
				if(disk==hoverDisk){counterDiskMouse++;}else{counterDiskMouse=0;}
				if(counterDiskMouse>1){
					drawEject=true;
					if(mx>=winWidth-37 and mx<winWidth-15){
						cairo_set_source_rgba(cr,1.0,0.0,0.0,0.1);
					}
				}

				cairo_rectangle(cr,3,contextHeight-12,winWidth-6,(yi*2));
				cairo_fill(cr);
				hoverDisk=disk;
			}else{
				hoverDisk="";
				counterDiskMouse=0;
			}
		}

		float przUsed=(float)disksUsed[disk]/((float)disksTotal[disk]/100.0);
		if(!drawEject){
			getBar(cr,70,contextHeight,przUsed,47);
		}else{
			getBar(cr,70,contextHeight,przUsed,37);
			// draw eject icon
			cairo_set_source_rgba(cr,0.9,0.9,0.9,0.2);
			if(mx>=winWidth-37 and mx<winWidth-15){
				cairo_set_source_rgba(cr,parsFromRGB(clockColor[0]),parsFromRGB(clockColor[1]),parsFromRGB(clockColor[2]),0.5);
				cairo_rectangle(cr,winWidth-38,contextHeight-7,30,20);
				cairo_stroke(cr);
				ejectDist=hoverDisk;
			}else{
				ejectDist="";
			}
			cairo_move_to(cr,winWidth-35,contextHeight+3);
			cairo_rel_line_to(cr,12,-7);
			cairo_rel_line_to(cr,12,7);
			cairo_rel_line_to(cr,-24,0);
			cairo_fill(cr);
			cairo_rectangle(cr,winWidth-35,contextHeight+5,24,3);
			cairo_fill(cr);

		}
		std::string mountPoint;
		if(disksMount[disk].length()>4){
			auto tmp=split(disksMount[disk],"/");
			auto name=tmp[tmp.size()-1];
			if(name.length()>5)	name=name.substr(0,5)+"...";
			mountPoint="["+name+"]";
		}else{
			mountPoint="["+disksMount[disk]+"]";
		}
		setParam(cr,10,contextHeight,mountPoint.c_str(),contextHeight);


		std::string freeSpace=std::string(getSize(disksTotal[disk]-disksUsed[disk]))+" / "+std::string(getSize(disksTotal[disk]));
		setValue(cr,70,contextHeight,freeSpace.c_str());
		setParam(cr,10,contextHeight,"Free:",contextHeight);
	}






	cairo_destroy(cr);
}
