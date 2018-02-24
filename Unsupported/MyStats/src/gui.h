void setText(cairo_t *cr,const char* font, cairo_font_slant_t style,cairo_font_weight_t type,double fontSize,int x,int y,int r,int g,int b,double alpha,const char* text);
void setParam(cairo_t *cr,int x,int y,const char* text,int &contHeight);
void setValue(cairo_t *cr,int x,int y,const char* text);
void getBar(cairo_t *cr, int x, int y,float value,int blockCount);
void drawDate(cairo_t *cr, int x, int y, const char* text);




void draw(GtkWidget *widget);
