#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>

static double parsFromRGB(int val);
char* getSize(unsigned long int val);
char* getTime();
char* getDate();
char* getUptime();
float getCpuPrz();
void getMem();
char* getProcCount();
void getIfacesInfo();
std::string getIfaceIp(const std::string& ip);
void getDisksInfo();

void upd();
static bool chkDir(const char* path);
void saveLog();

char* date;
char* uptime;
float cpuPrz=0.0;

int prev_idle=0;
int prev_total=0;

char* procCount;

unsigned long int memTotal=0;
unsigned long int memUsed=0;
unsigned long int swapTotal=0;
unsigned long int swapUsed=0;
float memPrz=0.0;
float swapPrz=0.0;
std::vector<std::string> ifaces;
std::map<std::string,unsigned long int> ifaceRX;
std::map<std::string,unsigned long int> ifaceTX;
std::map<std::string,unsigned long int> ifaceRXprew;
std::map<std::string,unsigned long int> ifaceTXprew;

std::vector<std::string> disks;
std::map<std::string,std::string> disksMount;
std::map<std::string,unsigned long int> disksTotal;
std::map<std::string,unsigned long int> disksUsed;


std::vector<std::string> split(const std::string& text,const std::string& delims);
std::vector<int> splitToInt(const std::string& text,const std::string& delims);
std::string str_replace(const std::string& orig,const std::string& rep,std::string str);
