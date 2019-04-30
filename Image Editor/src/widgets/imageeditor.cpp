#include "imageeditor.h"

#include <QPainter>
#include <QDebug>

ImageEditor::ImageEditor(QWidget *parent) : QWidget(parent)
{
	setNewSize(QSize(100,100));
	//
	m_resizeBox.setWidth(20);
	m_resizeBox.setHeight(20);
	this->setMouseTracking(true);
}
void ImageEditor::setImage(const QString &file)
{
	m_img.load(file);
	m_img=m_img.scaledToHeight(this->height());
	if(m_img.width()>this->width()){
		m_img=m_img.scaledToWidth(this->width());
	}
	chkResise();
}
QImage ImageEditor::getImage()
{
	QImage copy;
	int rx=(this->width()/2)-(m_img.width()/2);
	int ry=(this->height()/2)-(m_img.height()/2);
	copy=m_img.copy(m_ramka.x-rx,m_ramka.y-ry,m_ramka.width,m_ramka.height);
	return copy;
}

void ImageEditor::setNewSize(const QSize &size)
{
	m_r=(float)size.width()/(float)size.height();
	m_ramka.width = size.width();
	m_ramka.height = size.height();

	chkResise();

	m_ramka.x = (this->width()/2)-(m_ramka.width/2);
	m_ramka.y = (this->height()/2)-(m_ramka.height/2);
	update();
}
void ImageEditor::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter p(this);

	int rx=(this->width()/2)-(m_img.width()/2);
	int ry=(this->height()/2)-(m_img.height()/2);
	p.drawImage(rx,ry,m_img);

	//чтоб рамка за границы изображения не уползла
	if(m_ramka.x<rx) m_ramka.x = rx;
	if(m_ramka.y<ry) m_ramka.y = ry;
	if(m_ramka.y+m_ramka.height>m_img.height()+ry) m_ramka.y = m_img.height()-m_ramka.height+ry;
	if(m_ramka.x+m_ramka.width>m_img.width()+rx) m_ramka.x = m_img.width()-m_ramka.width+rx;

	//draw shadow
	p.setPen(Qt::NoPen);
	p.setBrush(QBrush(QColor(100,100,100,200)));
	p.drawRect(rx,ry,m_img.width(),m_ramka.y-ry);
	p.drawRect(rx,m_ramka.y+m_ramka.height,m_img.width(),m_img.height()-(m_ramka.y+m_ramka.height)+ry);
	p.drawRect(rx,m_ramka.y,m_ramka.x-rx,m_ramka.height);
	p.drawRect(m_ramka.x+m_ramka.width,m_ramka.y,m_img.width()-(m_ramka.x+m_ramka.width)+rx,m_ramka.height);

	//draw resize boxes
	int r=255;
	int g=255;
	int b=255;
	int a=100;
	int rBoxW2=m_resizeBox.width()/2;
	int rBoxH2=m_resizeBox.height()/2;
	p.setPen(Qt::NoPen);

	//отрисовка ресайз боксов
	p.setBrush(QBrush(QColor(r,g,b,a)));
	if(m_mx>m_ramka.x-rBoxW2 and m_mx<m_ramka.x+rBoxW2 and m_my>m_ramka.y-rBoxH2 and m_my<m_ramka.y+rBoxH2){
		p.setBrush(QBrush(QColor(255,147,0,50)));
	}
	p.drawRect(m_ramka.x-rBoxW2,m_ramka.y-rBoxH2,m_resizeBox.width(),m_resizeBox.height());

	p.setBrush(QBrush(QColor(r,g,b,a)));
	if(m_mx>m_ramka.x-rBoxW2 and m_mx<m_ramka.x+rBoxW2 and m_my>m_ramka.y+m_ramka.height-rBoxH2 and m_my<m_ramka.y+m_ramka.height+rBoxH2){
		p.setBrush(QBrush(QColor(255,147,0,50)));
	}
	p.drawRect(m_ramka.x-rBoxW2,m_ramka.y+m_ramka.height-rBoxH2,m_resizeBox.width(),m_resizeBox.height());

	p.setBrush(QBrush(QColor(r,g,b,a)));
	if(m_mx>m_ramka.x+m_ramka.width-rBoxW2 and m_mx<m_ramka.x+m_ramka.width+rBoxW2 and m_my>m_ramka.y+m_ramka.height-rBoxH2 and m_my<m_ramka.y+m_ramka.height+rBoxH2){
		p.setBrush(QBrush(QColor(255,147,0,50)));
	}
	p.drawRect(m_ramka.x+m_ramka.width-rBoxW2,m_ramka.y+m_ramka.height-rBoxH2,m_resizeBox.width(),m_resizeBox.height());

	p.setBrush(QBrush(QColor(r,g,b,a)));
	if(m_mx>m_ramka.x+m_ramka.width-rBoxW2 and m_mx<m_ramka.x+m_ramka.width+rBoxW2 and m_my>m_ramka.y-rBoxH2 and m_my<m_ramka.y+rBoxH2){
		p.setBrush(QBrush(QColor(255,147,0,50)));
	}
	p.drawRect(m_ramka.x+m_ramka.width-rBoxW2,m_ramka.y-rBoxH2,m_resizeBox.width(),m_resizeBox.height());

	//draw ramka
	p.setBrush(Qt::NoBrush);
	p.setPen(Qt::white);
	p.drawRect(m_ramka.x,m_ramka.y,m_ramka.width,m_ramka.height);

	if(m_ramka.width>90 and m_ramka.height>50){
		unsigned long x=m_ramka.x+(m_ramka.width/2)-40;
		unsigned long y=m_ramka.y+(m_ramka.height/2)-25;
		p.drawText(x,y,80,15,Qt::AlignCenter
							  ,"X:"+QString::number(m_ramka.x)+" "
							  +"Y:"+QString::number(m_ramka.y));
		p.drawText(x,y+25,80,15,Qt::AlignCenter
							  ,QString::number(m_ramka.width)+"x"
							  +QString::number(m_ramka.height));
	}
}
void ImageEditor::mousePressEvent(QMouseEvent *event)
{
	int mx=event->pos().x();
	int my=event->pos().y();
	int rx=m_ramka.x+m_ramka.width;
	int ry=m_ramka.y+m_ramka.height;
	if(mx>m_ramka.x and mx<rx and my>m_ramka.y and my<ry){
		m_ramkaMove=true;
		m_pressPos=event->pos();
	}
	int rBoxW2=m_resizeBox.width()/2;
	int rBoxH2=m_resizeBox.height()/2;
	if(m_mx>m_ramka.x-rBoxW2 and m_mx<m_ramka.x+rBoxW2 and m_my>m_ramka.y-rBoxH2 and m_my<m_ramka.y+rBoxH2){
		m_resizeLeftTop=true;
		m_ramkaMove=false;
		m_pressPos=event->pos();
	}
	if(m_mx>m_ramka.x-rBoxW2 and m_mx<m_ramka.x+rBoxW2 and m_my>m_ramka.y+m_ramka.height-rBoxH2 and m_my<m_ramka.y+m_ramka.height+rBoxH2){
		m_resizeLeftBottom=true;
		m_ramkaMove=false;
		m_pressPos=event->pos();
	}
	if(m_mx>m_ramka.x+m_ramka.width-rBoxW2 and m_mx<m_ramka.x+m_ramka.width+rBoxW2 and m_my>m_ramka.y+m_ramka.height-rBoxH2 and m_my<m_ramka.y+m_ramka.height+rBoxH2){
		m_resizeRightBottom=true;
		m_ramkaMove=false;
		m_pressPos=event->pos();
	}
	if(m_mx>m_ramka.x+m_ramka.width-rBoxW2 and m_mx<m_ramka.x+m_ramka.width+rBoxW2 and m_my>m_ramka.y-rBoxH2 and m_my<m_ramka.y+rBoxH2){
		m_resizeRightTop=true;
		m_ramkaMove=false;
		m_pressPos=event->pos();
	}
	QWidget::mousePressEvent(event);
}
void ImageEditor::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	m_ramkaMove=false;
	m_resizeLeftTop=false;
	m_resizeLeftBottom=false;
	m_resizeRightBottom=false;
	m_resizeRightTop=false;
}
void ImageEditor::mouseMoveEvent(QMouseEvent *event)
{
	m_mx=event->pos().x();
	m_my=event->pos().y();
	QPoint r(m_pressPos-event->pos());
	if(m_ramkaMove){
		if((m_ramka.x - r.x()) > -1) m_ramka.x -= r.x();
		if((m_ramka.y - r.y()) > -1) m_ramka.y -= r.y();
		m_pressPos=event->pos();
	}
	if(m_resizeLeftTop){
		int tmpW=m_ramka.width;
		int tmpH=m_ramka.height;
		m_ramka.width = m_ramka.width+r.y();
		chkResise();
		tmpW-=m_ramka.width;
		tmpH-=m_ramka.height;
		m_ramka.x = m_ramka.x+tmpW;
		m_ramka.y = m_ramka.y+tmpH;
		m_pressPos=event->pos();
	}
	if(m_resizeLeftBottom){
		int tmpW=m_ramka.width;
		m_ramka.width = m_ramka.width+r.x();
		chkResise();
		tmpW-=m_ramka.width;
		m_ramka.x = m_ramka.x+tmpW;
		m_pressPos=event->pos();
	}
	if(m_resizeRightTop){
		int tmp=m_ramka.height;
		m_ramka.width = m_ramka.width-r.x();
		chkResise();
		tmp-=m_ramka.height;
		m_ramka.y = m_ramka.y+tmp;
		m_pressPos=event->pos();
	}
	if(m_resizeRightBottom){
		m_ramka.width = m_ramka.width-r.x();
		chkResise();
		m_pressPos=event->pos();
	}
	update();
	QWidget::mouseMoveEvent(event);
}
void ImageEditor::chkResise()
{
	if(m_ramka.width < 25) m_ramka.width = 25;
	if(m_ramka.width > m_img.width()) m_ramka.width = m_img.width();
	m_ramka.height = m_ramka.width/m_r;

	if(m_ramka.height > m_img.height()){
		m_ramka.height = m_img.height();
		m_ramka.width = m_ramka.height*m_r;
	}
}
