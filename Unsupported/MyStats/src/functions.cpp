#include <ctime>
#include <stdlib.h>
#include <stdio.h>

static double parsFromRGB(int val)
{
	if(val>255) val=255;
	if(val<0) val=0;
	double value=0.0;
	if(val>0) value=(val/2.55)/100;
	return value;
}
char* getSize(unsigned long int val)
{
	static char buffer[12];
	if(val<1024){
		sprintf(buffer,"%lu bytes",val);
	}else{
		if(val>1024 and val<1024000){
			sprintf(buffer,"%0.3f",(float)val/1024.0);
			std::string tmp(buffer);
			tmp=tmp.substr(0,5);
			char *buf=strdup(tmp.c_str());
			sprintf(buf,"%s Kb",buf);
			return buf;
		}else{
			if(val>1024000 and val<1024000000){
				sprintf(buffer,"%0.3f",(float)val/1048576.0);
				std::string tmp(buffer);
				tmp=tmp.substr(0,5);
				char *buf=strdup(tmp.c_str());
				sprintf(buf,"%s Mb",buf);
				return buf;
			}else{
				if(val>1024000000 and val<1024000000000){
					sprintf(buffer,"%0.3f",(float)val/1048576000.0);
					std::string tmp(buffer);
					tmp=tmp.substr(0,5);
					char *buf=strdup(tmp.c_str());
					sprintf(buf,"%s Gb",buf);
					return buf;
				}else{
					if(val>1024000000000 and val<1024000000000000){
						sprintf(buffer,"%0.3f",(float)val/1048576000000.0);
						std::string tmp(buffer);
						tmp=tmp.substr(0,5);
						char *buf=strdup(tmp.c_str());
						sprintf(buf,"%s Tb",buf);
						return buf;
					}else{
						if(val>1024000000000000 and val<1024000000000000000){
							sprintf(buffer,"%0.3f",(float)val/1048576000000000.0);
							std::string tmp(buffer);
							tmp=tmp.substr(0,5);
							char *buf=strdup(tmp.c_str());
							sprintf(buf,"%s Pb",buf);
							return buf;
						}else{
							if(val>1024000000000000000){
								sprintf(buffer,"doxera");
							}
						}
					}
				}
			}
		}
	}
	return buffer;
}
char* getTime()
{
	std::time_t seconds = std::time(nullptr);
	tm* timeinfo = localtime(&seconds);
	static char buffer[10];
	sprintf(buffer,"%.2i:%.2i:%.2i",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	return buffer;
}
char* getDate()
{
	std::time_t seconds = std::time(nullptr);
	tm* timeinfo = localtime(&seconds);
	static char buffer[20];
	std::string wday;
	switch (timeinfo->tm_wday) {
		case 1: wday="Пн"; break;
		case 2: wday="Вт"; break;
		case 3: wday="Ср"; break;
		case 4: wday="Чт"; break;
		case 5: wday="Пт"; break;
		case 6: wday="Сб"; break;
		case 0: wday="Вс"; break;
		default: wday="N/A"; break;
	}
	sprintf(buffer,"%s %.2i.%.2i.%i",wday.c_str(),timeinfo->tm_mday,timeinfo->tm_mon,timeinfo->tm_year+1900);
	return buffer;
}
char* getUptime()
{
	std::string str;
	char ch;
	ifstream file;
	file.open("/proc/uptime");
	while (file.get(ch)){
		if(file.eof()) break;
		if(ch=='.') break;
		str+=ch;
	}
	file.close();
	int uptime=atoi(str.c_str());
	int h=uptime/60/60;
	int m=((uptime-(h*60*60))/60);
	int s=(uptime-(h*60*60)-(m*60));
	static char buffer[30];
	sprintf(buffer,"%.2i:%.2i:%.2i",h,m,s);
	return buffer;
}
float getCpuPrz()
{
	int counter=0;
	int idle=0;
	int total=0;
	std::string str;
	char ch;
	ifstream file;
	file.open("/proc/stat");
	while (file.get(ch)){
		if(file.eof()) break;
		if(ch=='	') continue;
		if(ch=='\n') break;
		if(ch==' '){
			if(counter==5) idle=atoi(str.c_str());
			if(counter>1) total+=atoi(str.c_str());
			str="";
			counter++;
		}else{
			str+=ch;
		}
	}
	file.close();
	int diff_idle=idle-prev_idle;
	int diff_total=total-prev_total;
	float cpu=(1000.0*(diff_total-diff_idle)/diff_total+5.0)/10.0;
	prev_total=total;
	prev_idle=idle;
	return cpu;
}
void getMem()
{
	char buf[BUFSIZ];
	FILE *ptr;
	if ((ptr = popen("free -b", "r")) != NULL){
		int counter=0;
		 while (fgets(buf, BUFSIZ, ptr) != NULL){
			if(counter==0){
				counter++;
				continue;
			}
			if(counter==1){
				std::string tmp(buf);
				auto str=str_replace("  "," ",tmp);
				auto array=split(str," ");
				memTotal=stol(array[1]);
				memUsed=stol(array[2]);
			}
			if(counter==2){
				std::string tmp(buf);
				auto str=str_replace("  "," ",tmp);
				auto array=split(str," ");
				swapTotal=stol(array[1].c_str());
				swapUsed=stol(array[2].c_str());
			}
			counter++;
		}
		pclose(ptr);
	}
	memPrz=(float)memUsed/((float)memTotal/100);
	if(swapUsed==0 and swapTotal==0){
		swapPrz=0.0;
	}else{
		swapPrz=(float)swapUsed/((float)swapTotal/100);
	}
}
char* getProcCount()
{
	static char buf[BUFSIZ];
	FILE *ptr;
	if ((ptr = popen("ls /proc | grep \"[0-9]\" | wc -l", "r")) != NULL){
		if(!fgets(buf, BUFSIZ, ptr)) pclose(ptr);
		pclose(ptr);
	}
	std::string tmp(buf);
	auto str=str_replace("\n","",tmp);
	sprintf(buf,"%s",str.c_str());
	return buf;
}
void getIfacesInfo()
{
	int counter=0;
	ifaces.clear();
	std::string str;
	char ch;
	ifstream file;
	file.open("/proc/net/dev");
	while (file.get(ch)){
		if(file.eof()) break;
		if(ch=='	') continue;
		if(ch=='\n'){
			if(counter>1){
				if(str.find("lo:")>str.length()){
					str=str_replace("  "," ",str);
					auto array=split(str,": ");
					//printf("[%s][%s][%s]\n",str.c_str(),array[1].c_str(),array[9].c_str());
					std::string ifname=array[0];
					ifaces.push_back(ifname);
					ifaceRXprew[ifname]=ifaceRX[ifname];
					ifaceTXprew[ifname]=ifaceTX[ifname];

					ifaceRX[ifname]=stol(array[1]);
					ifaceTX[ifname]=stol(array[9]);
				}
			}
			str="";
			counter++;
			continue;
		}
		str+=ch;
	}
	file.close();
}
std::string getIfaceIp(const std::string& ip)
{
	std::string cm="ifconfig "+ip+" | grep \"inet addr\"";
	char buf[BUFSIZ];
	FILE *ptr;
	if ((ptr = popen(cm.c_str(), "r")) != NULL){
		if(!fgets(buf, BUFSIZ, ptr)) pclose(ptr);
		pclose(ptr);
	}
	std::string tmp(buf);
	auto str=str_replace("  "," ",tmp);
	auto array=split(str,":");
	array=split(array[1]," ");
	return array[0];
}
void getDisksInfo()
{
	disks.clear();
	int counter=0;
	static char buf[BUFSIZ];
	FILE *ptr;
	if ((ptr = popen("df -B 1", "r")) != NULL){
		while (fgets(buf, BUFSIZ, ptr) != NULL){
			if(counter==0){
				counter++;
				continue;
			}
			std::string tmp(buf);
			auto str=str_replace("  "," ",tmp);
			str=str_replace("\n","",str);
			auto array=split(str," ");
			if(array[0]=="none" or array[0]=="udev" or array[0]=="tmpfs" or array[0]=="cgmfs") continue;
			//printf("#%i [%s][%s][%s]\n",counter,str.c_str(),array[1].c_str(),array[2].c_str());
			disks.push_back(array[0]);
			disksMount[array[0]]=array[5];
			disksTotal[array[0]]=stol(array[1]);
			disksUsed[array[0]]=stol(array[2]);
		}
		pclose(ptr);
	}
}
void upd()
{
	date=getDate();
	uptime=getUptime();
	cpuPrz=getCpuPrz();
	procCount=getProcCount();
	getMem();
	getIfacesInfo();
	getDisksInfo();
	if(saveLogs){
		if(saveLogCounter<60 and winUpdateTime<=1000){
			saveLogCounter++;
		}else{
			saveLogCounter=0;
			saveLog();
		}

	}
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
void saveLog()
{
	if(!chkDir(logDir.c_str())) return;
	std::time_t seconds = std::time(nullptr);
	tm* timeinfo = localtime(&seconds);
	static char buffer[20];
	sprintf(buffer,"%i-%.2i-%.2i",timeinfo->tm_year+1900,timeinfo->tm_mon,timeinfo->tm_mday);
	std::string logFile;
	logFile=logDir+"/"+std::string(buffer)+".log";
	//printf("SAVE DATA to LOG [%s]\n",logFile.c_str());
	std::string str;
	sprintf(buffer,"%.2i:%.2i",timeinfo->tm_hour,timeinfo->tm_min);
	str=std::string(buffer)+"	cpu:"+std::to_string(cpuPrz)+"	ram:"+std::to_string(memPrz)+":"
			+std::to_string(memTotal)+":"+std::to_string(memUsed)+"	swap:"
			+std::to_string(swapPrz)+":"+std::to_string(swapTotal)+":"+std::to_string(swapUsed)
			+"	apps:"+std::string(procCount)+"	ifaces:";
	for(auto ifname:ifaces){
		long rxSpeed=ifaceRX[ifname]-ifaceRXprew[ifname];
		long txSpeed=ifaceTX[ifname]-ifaceTXprew[ifname];
		str+=ifname+"==="+std::to_string(rxSpeed)+"==="+std::to_string(ifaceRX[ifname])+"===";
		str+=std::to_string(txSpeed)+"==="+std::to_string(ifaceTX[ifname])+"==="+std::to_string(ifaceRX[ifname]+ifaceTX[ifname]);
		str+="==="+getIfaceIp(ifname)+":";
	}
	str+="	disks:";
	for(auto disk:disks){
		str+=disk+"==="+std::to_string(disksTotal[disk])+"==="+std::to_string(disksUsed[disk])+"===";
		float przUsed=(float)disksUsed[disk]/((float)disksTotal[disk]/100.0);
		str+=std::to_string(przUsed)+"==="+disksMount[disk]+":";
	}
	FILE *f;
	f=fopen(logFile.c_str(),"a+");
		fprintf(f,"%s\n",str.c_str());
	fclose(f);
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
std::vector<int> splitToInt(const std::string& text,const std::string& delims)
{
	std::vector<int> tokens;
	std::size_t start = text.find_first_not_of(delims), end = 0;

	while((end = text.find_first_of(delims, start)) != std::string::npos)
	{
		tokens.push_back(stoi(text.substr(start, end - start)));
		start = text.find_first_not_of(delims, end);
	}
	if(start != std::string::npos)
		tokens.push_back(stoi(text.substr(start)));

	return tokens;
}
std::string str_replace(const std::string& orig,const std::string& rep,std::string str)
{
	for (int pos = str.find(orig); pos != std::string::npos; pos = str.find(orig, pos)){
		str = str.replace(pos, orig.length(), rep);
	}
	return str;
}
