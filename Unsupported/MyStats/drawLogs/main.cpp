#include <gtk/gtk.h>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <vector>
#include <map>

int imgWidth=0;
int imgHeight=0;
int setka=0;
//int ifaceChagTraff=0;
//int ifaceChagPx=0;

static bool chkDir(const char* path);
void drawSetka(cairo_t *cr);
void OX(cairo_t *cr,int chagX,int chagChis);
void OY(cairo_t *cr,int chagY,int chagChis);
std::vector<std::string> split(const std::string& text,const std::string& delims);
static double parsFromRGB(int val);
void img(cairo_t *cr);
//void imgIface(cairo_t *cr);

int main(int argc, char **argv)
{
	char* logFile=(char*)(argc > 1 ? argv[1]:nullptr);
	if(logFile==nullptr){
		printf("USAGE: %s <logfile>\n",argv[0]);
		return 0;
	}
	std::string homeDir(getenv("HOME"));
	std::string appDir=std::string(homeDir+"/.MyStats");
	if(!chkDir(appDir.c_str())) return 0;
	std::string imageDir=std::string(appDir+"/images");
	if(!chkDir(imageDir.c_str())) return 0;
	std::string imageDisksDir=std::string(imageDir+"/disks");
	if(!chkDir(imageDisksDir.c_str())) return 0;
	std::string logDir=std::string(appDir+"/logs");
	if(access(logFile,0)){
		printf("NO FILE [%s]\n",logFile);
		std::string fileToDir=logDir+"/"+std::string(logFile);
		if(access(fileToDir.c_str(),0)){
			printf("NO FILE [%s]\n",fileToDir.c_str());
			return 0;
		}else{
			logFile=strdup(fileToDir.c_str());
		}
	}
	imgWidth=1024;	// Размер изображения по горизонтали
	imgHeight=768;	// Размер изображения по вертикали
	setka=7;		// Резмер сетки в пикселях
//	ifaceChagTraff=200;
//	ifaceChagPx=70;
	float chas=((float)imgWidth-25.0)/24.0;
	std::ifstream file;
	std::string str;
	char ch;
	std::map<std::string,cairo_surface_t*> disks;
	std::map<cairo_surface_t*,cairo_t*> surfaces;
	std::map<cairo_surface_t*,std::string> images;
	//подготовка изображений
	cairo_surface_t *surfaceCpu=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,imgWidth,imgHeight);
	cairo_surface_t *surfaceRam=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,imgWidth,imgHeight);
	cairo_surface_t *surfaceSwap=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,imgWidth,imgHeight);
	cairo_t *crCpu=cairo_create(surfaceCpu);
	cairo_t *crRam=cairo_create(surfaceRam);
	cairo_t *crSwap=cairo_create(surfaceSwap);

	surfaces[surfaceCpu]=crCpu;
	surfaces[surfaceRam]=crRam;
	surfaces[surfaceSwap]=crSwap;
	images[surfaceCpu]=imageDir+"/cpu.png";
	images[surfaceRam]=imageDir+"/ram.png";
	images[surfaceSwap]=imageDir+"/swap.png";

	img(crCpu);
	img(crRam);
	img(crSwap);
	cairo_set_source_rgba(crCpu,parsFromRGB(254),parsFromRGB(75),parsFromRGB(0),0.9);
	cairo_set_source_rgba(crRam,parsFromRGB(56),parsFromRGB(252),parsFromRGB(7),0.9);
	cairo_set_source_rgba(crSwap,parsFromRGB(248),parsFromRGB(252),parsFromRGB(7),0.9);



	bool first=true;
	int i=0;
	file.open(logFile);
	while (file.get(ch)){
		if(file.eof()) break;
		//parsing line from log file
		if(ch=='\n'){
			bool time=false;
			std::string hour;
			std::string min;
			float x,y;
			for(auto elem:split(str,"	")){
				auto array=split(elem,":");
				//parsing time
				if(!time){
					if(array.size()>1){
						hour=array[0];
						min=array[1];
					}
					time=true;
					continue;
				}
				if(array[0]=="cpu"){
					x=25+(chas*std::stof(hour))+((chas/60)*std::stof(min));
					y=((float)imgHeight-25.0)-((((float)imgHeight-25.0)/100.0)*std::stof(array[1]));
					if(!first){cairo_line_to(crCpu,x,y);}
					cairo_move_to(crCpu,x,y);
					continue;
				}
				if(array[0]=="ram"){
					x=25+(chas*std::stof(hour))+((chas/60)*std::stof(min));
					y=((float)imgHeight-25.0)-((((float)imgHeight-25.0)/100.0)*std::stof(array[1]));
					if(!first){cairo_line_to(crRam,x,y);}
					cairo_move_to(crRam,x,y);
					continue;
				}
				if(array[0]=="swap"){
					x=25+(chas*std::stof(hour))+((chas/60)*std::stof(min));
					y=((float)imgHeight-25.0)-((((float)imgHeight-25.0)/100.0)*std::stof(array[1]));
					if(!first){cairo_line_to(crSwap,x,y);}
					cairo_move_to(crSwap,x,y);
					continue;
				}
				if(array[0]=="disks"){
					for(auto elem:array){
						if(elem=="disks") continue;
						auto disksData=split(elem,"===");
						std::string diskName;
						if(disksData.size()>3){
							auto tmp=split(disksData[0],"/");
							diskName=tmp[tmp.size()-1];
							if(first){
								cairo_surface_t *surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,imgWidth,imgHeight);
								disks[diskName]=surface;
								cairo_t *cr=cairo_create(surface);
								surfaces[surface]=cr;
								images[surface]=imageDisksDir+"/"+diskName+".png";
								img(cr);
								cairo_set_source_rgba(cr,parsFromRGB(44),parsFromRGB(97),parsFromRGB(255),0.9);
							}
							x=25+(chas*std::stof(hour))+((chas/60)*std::stof(min));
							y=((float)imgHeight-25.0)-((((float)imgHeight-25.0)/100.0)*std::stof(disksData[3]));
							if(!first){cairo_line_to(surfaces[disks[diskName]],x,y);}
							cairo_move_to(surfaces[disks[diskName]],x,y);
						}
					}
					continue;
				}
//				if(array[0]=="ifaces"){
//					for(auto ifElem:array){
//						if(ifElem=="ifaces") continue;
//						auto ifData=split(ifElem,"===");
//						std::string ifName;
//						if(ifData.size()>0){
//							ifName=ifData[0];
//							if(first){
//								cairo_surface_t *surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,imgWidth,imgHeight);
//								ifaces[ifName]=surface;
//								cairo_t *cr=cairo_create(surface);
//								surfaces[surface]=cr;
//								images[surface]=imageIfacesDir+"/"+ifName+".png";
//								imgIface(cr);
//								cairo_set_source_rgba(cr,parsFromRGB(41),parsFromRGB(167),parsFromRGB(2),0.9);
//							}
//							float h=((float)imgHeight-25.0)/(float)ifaceChagPx;
//							x=25+(chas*std::stof(hour))+((chas/60)*std::stof(min));
//							y=((float)imgHeight-25.0)-((h/(float)ifaceChagTraff)*(std::stof(ifData[1])/10));
//							if(!first){cairo_line_to(surfaces[ifaces[ifName]],x,y);}
//							cairo_move_to(surfaces[ifaces[ifName]],x,y);
//						}
//					}
//					continue;
//				}
				printf("[%s]\n",elem.c_str());
			}
			printf("======\n");
			if(first){first=false;}
			str="";
			i++;
			continue;
		}
		str+=ch;
	}
	file.close();

	// close all and save files
	printf("==============================\n");
	for(auto surface:surfaces){
		printf(">>[%s]\n",images[surface.first].c_str());
		cairo_stroke(surface.second);
		cairo_surface_write_to_png(surface.first,images[surface.first].c_str());
		cairo_destroy(surface.second);
	}
	printf("==============================\n");
	return 0;
}



//void imgIface(cairo_t *cr)
//{
//	drawSetka(cr);
//	cairo_select_font_face(cr,"Play",CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
//	cairo_set_font_size (cr, 10);
//	cairo_set_source_rgba(cr,0,0,0,0.8);
//	OX(cr,24,1);
//	OY(cr,ifaceChagPx,ifaceChagTraff);
//	cairo_stroke(cr);
//	cairo_set_line_width(cr,2);
//}
void img(cairo_t *cr)
{
	drawSetka(cr);
	cairo_select_font_face(cr,"Play",CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 10);
	cairo_set_source_rgba(cr,0,0,0,0.8);
	OX(cr,24,1);
	OY(cr,20,5);
	cairo_stroke(cr);
	cairo_set_line_width(cr,2);
}
static double parsFromRGB(int val)
{
	if(val>255) val=255;
	if(val<0) val=0;
	double value=0.0;
	if(val>0) value=(val/2.55)/100;
	return value;
}
std::vector<std::string> split(const std::string& text,const std::string& delims)
{
	std::vector<std::string> tokens;
	std::size_t start = text.find_first_not_of(delims), end = 0;

	while((end = text.find_first_of(delims, start)) != std::string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = text.find_first_not_of(delims, end);
	}
	if(start != std::string::npos)
		tokens.push_back(text.substr(start));

	return tokens;
}
void OX(cairo_t *cr,int chagX,int chagChis)
{
	int tmp=(imgWidth-25)/chagX;
	int ch=0;
	for(int i=25;i<imgWidth-25;i+=tmp){
		cairo_move_to(cr,i-5,imgHeight-15);
		cairo_show_text(cr,std::to_string(ch).c_str());
		cairo_move_to(cr,i,imgHeight-25);
		cairo_line_to(cr,i,0);
		ch+=chagChis;
	}
}
void OY(cairo_t *cr, int chagY, int chagChis)
{
	int tmp=(imgHeight-25)/chagY;
	int ch=0;
	for(int i=imgHeight-25;i>0;i-=tmp){
		cairo_move_to(cr,5,i+5);
		cairo_show_text(cr,std::to_string(ch).c_str());
		cairo_move_to(cr,25,i);
		cairo_line_to(cr,imgWidth,i);
		ch+=chagChis;
	}
}
void drawSetka(cairo_t *cr)
{
	cairo_set_source_rgba(cr,0,0,0,0.3);
	for(int i=0;i<imgWidth;i+=setka){
		cairo_move_to(cr,i,0);
		cairo_line_to(cr,i,imgHeight);
	}
	for(int i=0;i<imgHeight;i+=setka){
		cairo_move_to(cr,0,i);
		cairo_line_to(cr,imgWidth,i);
	}
	cairo_set_line_width(cr,0.5);
	cairo_stroke(cr);
}
static bool chkDir(const char* path){
	if(access(path,1)){
		int result=mkdir(path,0755);
		if(result==0){
			printf("DIR [%s] CREATED OK\n",path);
		}else{
			printf("DIR [%s] CREATED ERROR\n",path);
			return false;
		}
	}
	return true;
}
