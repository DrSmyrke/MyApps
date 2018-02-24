#define Q_UNUSED(x) (void)x;
#include <vector>
#include <string>

std::string logDir;
std::string appDir;
static char* confFile;
bool winAutohide=true;
int winWidth=270;
int winHeight=300;
int winUpdateTime=500;
bool saveLogs=false;
int yi=15;

std::vector<int> backgroundColor={0,0,0};
float backgroundColorAlpha=0.8;

const char* clockFont="Sans";
std::vector<int> clockColor={229,191,14};

const char* defaultTextFont="Sans";
cairo_font_slant_t defaultTextStyle=CAIRO_FONT_SLANT_NORMAL;
cairo_font_weight_t defaultTextWeight=CAIRO_FONT_WEIGHT_NORMAL;
int defaultTextSize=12;

std::vector<int> paramColor={225,175,0};
float paramColorAlpha=0.9;

std::vector<int> valueColor={138,186,56};
float valueColorAlpha=0.9;

std::vector<int> progressBarColor={100,100,100};
float progressBarColorAlpha=0.5;
int progressBarBlockWidth=3;
int progressBarBlockHeight=12;
int progressBarBlockIntery=1;

bool supports_alpha = FALSE;

//mouse data
int mx,my;

//mouse onHover disk
std::string hoverDisk;
std::string ejectDist;
int counterDiskMouse=0;

//display data
int dispWidth,dispHeight;

//window data
bool showWindow=false;
int winX,winY;

//content
int contextHeight;

int updCounter=0;
int saveLogCounter=0;
