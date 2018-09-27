#include "hwmonitorwidget.h"
#include <QPainter>
#include <QProcess>

HWMonitorWidget::HWMonitorWidget(QWidget *parent) : QWidget(parent)
{
	m_pHWMonitor = new HWMonitor(this);
}

void HWMonitorWidget::slot_show()
{
	m_show = true;
}

void HWMonitorWidget::slot_hide()
{
	m_show = false;
}

void HWMonitorWidget::slot_update()
{
	if( !m_show ) return;
	m_pHWMonitor->slot_timer();
	if( m_heightChangeF ) emit signal_heightChangeRequest( m_prewY );
	this->update();
}

void HWMonitorWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter p(this);

	QFont font = p.font();

	p.setPen(Qt::NoPen);
	p.setBrush(QBrush(QColor(25,25,25,240)));
	p.drawRect(0,0,this->width(),this->height());

	if( app::conf.fixed ){
		p.setPen(Qt::red);
		font.setPixelSize(16);
		font.setBold(true);
		p.setFont(font);
		p.drawText(210,5,50,15,Qt::AlignVCenter,"FIXED");
	}


	p.setPen(Qt::yellow);
	font.setBold(true);
	font.setPixelSize(48);
	p.setFont(font);
	p.drawText(m_xParamOffset,0,200,50,Qt::AlignVCenter,m_pHWMonitor->getData().time);

	p.setPen(Qt::green);
	font.setPixelSize(m_defFontSize);
	p.setFont(font);
	p.drawText(155,50,100,15,Qt::AlignVCenter,m_pHWMonitor->getData().date);

	/*
	 * TOP
	*/
	uint16_t y = 50;
	drawParam(p,m_xParamOffset,y,"Uptime:");
	drawValue(p,m_xVallOffset,y,m_pHWMonitor->getData().uptime);
	y += m_defFontSize + m_yOffset;

	drawParam(p,m_xParamOffset,y,"CPU:");
	drawBar(p,m_xVallOffset,y,m_pHWMonitor->getData().cpu);
	y += m_defFontSize + m_yOffset;

	drawParam(p,m_xParamOffset,y,"RAM:");
	drawBar(p,m_xVallOffset,y,m_pHWMonitor->getData().mem,15);
	drawValue(p,m_xVallOffset + 80,y,m_pHWMonitor->getData().memUsed + " / " + m_pHWMonitor->getData().memTotal);
	y += m_defFontSize + m_yOffset;

	if( m_swapParam.hover ){
		p.setBrush(QBrush(QColor(100,100,100,73)));
		p.setPen(Qt::NoPen);
		p.drawRect(m_xParamOffset,y + 2,this->width() - m_xParamOffset - 5,14);
	}
	drawParam(p,m_xParamOffset,y,"SWAP:");
	m_swapParam.clear = ( m_pHWMonitor->getData().swap >= 0 )?true:false;
	m_swapParam.y = y;
	drawBar(p,m_xVallOffset,y,m_pHWMonitor->getData().swap,15);
	drawValue(p,m_xVallOffset + 80,y,m_pHWMonitor->getData().swapUsed + " / " + m_pHWMonitor->getData().swapTotal);
	y += m_defFontSize + m_yOffset;

	drawParam(p,m_xParamOffset,y,"Apps:");
	drawValue(p,m_xVallOffset,y,m_pHWMonitor->getData().procCount);
	y += m_defFontSize + m_yOffset;
	/*
	 * IFACES
	*/
	for(auto iface:m_pHWMonitor->getData().ifaces){
		// Записываем координату У интерфейса
		m_ifaces[iface.name].y = y;
		// Если мышка наведена рисуем бокс
		if( m_ifaces[iface.name].hover ){
			p.setBrush(QBrush(QColor(100,100,100,73)));
			p.setPen(Qt::NoPen);
			p.drawRect(m_xParamOffset,y + 2,this->width() - m_xParamOffset - 5,14);
		}
		// Рисуем шапку
		p.setPen(QPen(QColor(138,186,56)));
		p.drawRect(m_xParamOffset,y+7,45,2);
		drawValue(p,m_xVallOffset,y,iface.name);
		p.drawRect(m_xVallOffset + (iface.name.length() * (m_defFontSize-3)),y+7, ( this->width() - m_xVallOffset - (iface.name.length() * (m_defFontSize-3)) - 7 ) ,2);
		y += m_defFontSize + m_yOffset;

		if(m_ifaces[iface.name].hideData) continue;

		drawParam(p,m_xParamOffset,y,"Down:");
		drawValue(p,m_xVallOffset,y,mf::getSize(iface.downloadSpeed) + " / s");
		drawValue(p,m_xVallOffset + 130,y,"+ " + mf::getSize(iface.download));
		y += m_defFontSize + m_yOffset;
		drawParam(p,m_xParamOffset,y,"Up:");
		drawValue(p,m_xVallOffset,y,mf::getSize(iface.uploadSpeed) + " / s");
		drawValue(p,m_xVallOffset + 130,y,"+ " + mf::getSize(iface.upload));
		y += m_defFontSize + m_yOffset;
		drawParam(p,m_xParamOffset,y,"IP:");
		drawValue(p,m_xVallOffset,y,iface.ip);
		drawValue(p,m_xVallOffset + 130,y,"= " + mf::getSize(iface.upload + iface.download));
		y += m_defFontSize + m_yOffset;
		drawParam(p,m_xParamOffset,y,"MAC:");
		drawValue(p,m_xVallOffset,y,iface.mac);
		y += m_defFontSize + m_yOffset;
	}
	/*
	 * DISKS
	*/
	y += m_defFontSize + m_yOffset;
	for(auto disk:m_pHWMonitor->getData().disks){
		QString name = disk.mount;
		if( name != "/" ){
			auto tmp = name.split("/");
			if( tmp.size() > 0 ) name = tmp[tmp.size() - 1];
			if( name.length() > 5 ) name = name.left(4) + "...";
		}

		if( name != "/" ){
			// Записываем координату У интерфейса
			m_disks[disk.mount].y = y;
			// Если мышка наведена рисуем бокс
			if( m_disks[disk.mount].hover ){
				p.setBrush(QBrush(QColor(100,100,100,73)));
				p.setPen(Qt::NoPen);
				p.drawRect(m_xParamOffset,y + 2,this->width() - m_xParamOffset - 5,28);
			}
		}

		drawParam(p,m_xParamOffset,y,"[" + name + "]");
		drawBar(p,m_xVallOffset,y,disk.usedPrz);
		y += m_defFontSize + m_yOffset;
		drawParam(p,m_xParamOffset,y,"Free:");
		drawValue(p,m_xVallOffset,y,mf::getSize(disk.avail) + " / " + mf::getSize(disk.size));
		y += m_defFontSize + m_yOffset;
	}

	m_heightChangeF = ( m_prewY != y ) ? true : false;
	m_prewY = y;
}

void HWMonitorWidget::mouseMoveEvent(QMouseEvent *event)
{
	m_mouse = event->pos();

	for(auto &elem:m_ifaces){
		elem.second.hover = false;
		elem.second.hover = chkHoverIface(elem.second.y + 3);
	}
	for(auto &elem:m_disks){
		elem.second.hover = false;
		elem.second.hover = chkHoverDisk(elem.second.y + 3);
	}

	if( m_swapParam.clear ) m_swapParam.hover = chkHoverSwap( m_swapParam.y );

	QWidget::mouseMoveEvent(event);
}

bool HWMonitorWidget::mouseClickToObject()
{
	bool find = false;

	for(auto &elem:m_ifaces){
		if( chkHoverIface(elem.second.y + 3) ){
			elem.second.hideData = !elem.second.hideData;
			find = true;
			break;
		}
	}
	for(auto &elem:m_disks){
		if( chkHoverDisk(elem.second.y + 3) ){
			QProcess::startDetached("xdg-open " + elem.first);
			find = true;
			break;
		}
	}
	if( m_swapParam.clear ){
		if( chkHoverSwap( m_swapParam.y ) ){
			QProcess::startDetached("gksu \"swapspace -e\" || gksu \"swapoff -a && swapon -a\" || ksudo \"swapoff -a && swapon -a\" || xterm -e \"sudo swapoff -a && sudo swapon -a\"");
			m_swapParam.hover = false;
			find = true;
		}
	}

	return find;
}

void HWMonitorWidget::drawParam(QPainter &p, const uint16_t x, const uint16_t y, const QString &param)
{
	p.setPen(QPen(QColor(255,175,0)));
	QFont font = p.font();
	font.setPixelSize(m_defFontSize);
	p.setFont(font);
	p.drawText(x,y,100,15,Qt::AlignLeft,param);
}

void HWMonitorWidget::drawValue(QPainter &p, const uint16_t x, const uint16_t y, const QString &value)
{
	p.setPen(QPen(QColor(138,186,56)));
	QFont font = p.font();
	font.setPixelSize(m_defFontSize);
	p.setFont(font);
	p.drawText(x,y,value.length() * m_defFontSize,15,Qt::AlignLeft,value);
}

void HWMonitorWidget::drawBar(QPainter &p, const uint16_t x, const uint16_t y, const float value, const uint8_t blockCount)
{
	uint8_t barBlockWidth = 4;
	uint8_t barBlockHeight = 12;
	uint8_t barBlockIntery = 1;
	uint8_t yOffset = 3;
	p.setPen(Qt::NoPen);
	p.setBrush(QBrush(QColor(100,100,100,190)));

	uint16_t pbarWidth = ( barBlockWidth * blockCount ) + ( barBlockIntery * ( blockCount - 1 ) );
	float prz = ( (float)pbarWidth / 100.0 ) * value;

	uint16_t rx = x;
	for(uint8_t i = 0; i < blockCount; i++){
		p.drawRect(rx,y+yOffset,barBlockWidth,barBlockHeight);
		rx = rx + barBlockIntery + barBlockWidth;
	}

	QLinearGradient gradient;
	gradient.setStart(x,y+yOffset);
	gradient.setFinalStop(x+pbarWidth,y+yOffset-barBlockHeight);
	QColor color1(16,208,32,150);
	QColor color2(255,27,0,150);
	gradient.setColorAt((qreal)0, color1 );
	gradient.setColorAt((qreal)1, color2 );
	QBrush brush(gradient);
	p.setBrush(brush);
	rx = x;
	for(uint8_t i = 0; i < blockCount; i++){
		if( ( rx - x ) >= prz ) break;
		p.drawRect(rx,y+yOffset,barBlockWidth,barBlockHeight);
		rx = rx + barBlockIntery + barBlockWidth;
	}

	//draw 3d
	p.setBrush(QBrush(QColor(0,0,0,40)));
	rx = x;
	for(uint8_t i = 0; i < blockCount; i++){
		p.drawRect(rx,y+yOffset+(barBlockHeight/2),barBlockWidth,barBlockHeight-(barBlockHeight/2));
		rx = rx + barBlockIntery + barBlockWidth;
	}

	QString val = QString::number(value);
	val = val.left(5) + "%";
	p.setPen(QPen(QColor(229,191,14,225)));
	p.drawText(x,y+1,pbarWidth,15,Qt::AlignHCenter | Qt::AlignVCenter,val);
}

bool HWMonitorWidget::chkHoverIface(const uint16_t y)
{
	return (m_mouse.y() >= y && m_mouse.y() < y + 14) ? true : false;
}

bool HWMonitorWidget::chkHoverDisk(const uint16_t y)
{
	return (m_mouse.y() >= y && m_mouse.y() < y + 28 ) ? true : false;
}

bool HWMonitorWidget::chkHoverSwap(const uint16_t y)
{
	return (m_mouse.y() >= y && m_mouse.y() < y + 14) ? true : false;
}
