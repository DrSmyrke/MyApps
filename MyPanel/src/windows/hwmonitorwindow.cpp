#include "hwmonitorwindow.h"
#include <QLayout>
#include <QApplication>

HWMonitorWindow::HWMonitorWindow(QWidget *parent) : QMainWindow(parent)
{
	m_pTimer = new QTimer(this);
		m_pTimer->setSingleShot(true);
		m_pTimer->setInterval(500);
	m_pHWMonitorWidget = new HWMonitorWidget(this);
		m_pHWMonitorWidget->setMouseTracking(true);

	setCentralWidget(m_pHWMonitorWidget);
	setWindowFlags(Qt::Popup | Qt::ToolTip);
	setFixedSize(270,240);

	setAttribute(Qt::WA_TranslucentBackground);

	connect(m_pTimer,&QTimer::timeout,this,[this](){
		move( (app::screen.width()/2)-(m_windowSize.width()/2) ,  app::screen.height() - m_windowSize.height());
	});
	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_clickObject,this,[this](const uint8_t obj, bool state){
		switch (obj) {
			case ui_skrepka: m_skrepka = state; break;
		}
	});
	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_heightChangeRequest,this,[this](const uint16_t newValue){
		QPoint pos = this->pos();
		this->setFixedHeight(newValue);
		this->move(pos);
	});
}

void HWMonitorWindow::open()
{
	if( !this->isHidden() ) return;
	this->show();
	m_pHWMonitorWidget->slot_show();
}

void HWMonitorWindow::close()
{
	this->hide();
	m_pHWMonitorWidget->slot_hide();
}

void HWMonitorWindow::mousePressEvent(QMouseEvent *event)
{
	m_mouseClick = true;
	m_xOffset = event->windowPos().x();
	m_yOffset = event->windowPos().y();

	//Q_UNUSED(event);
	QMainWindow::mouseReleaseEvent(event);
}

void HWMonitorWindow::mouseReleaseEvent(QMouseEvent *event)
{
	m_mouseClick = false;

	if( !m_mouseMove ) m_pHWMonitorWidget->slot_mouseReleaseEvent(event);
	if( m_mouseMove ) m_mouseMove = false;
	QMainWindow::mouseReleaseEvent(event);
}

void HWMonitorWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(m_mouseClick){
		if( m_pTimer->isActive() ) m_pTimer->stop();
		uint8_t borderSize = 10;
		QPointF startPos = event->screenPos();
		this->move( event->screenPos().x() - m_xOffset, event->screenPos().y() - m_yOffset );
		uint16_t x = this->pos().x();
		uint16_t y = this->pos().y();
		if( this->pos().x() < borderSize ) this->move( 0, this->pos().y() );
		if( this->pos().y() < borderSize ) this->move( this->pos().x(), 0 );
		if( this->pos().x() > ( app::screen.width() - borderSize - this->width() ) ) this->move( app::screen.width() - this->width(), this->pos().y() );
		if( this->pos().y() > ( app::screen.height() - borderSize - this->height() ) ) this->move( this->pos().x(), app::screen.height() - this->height() );
		if( startPos.x() != this->pos().x() or startPos.y() != this->pos().y() ) m_mouseMove = true;
	}
}

void HWMonitorWindow::resizeEvent(QResizeEvent *event)
{
	getMainSize();
	move( (app::screen.width()/2)-(m_windowSize.width()/2) ,  app::screen.height() - 3);
	QMainWindow::resizeEvent(event);
}

void HWMonitorWindow::enterEvent(QEvent *event)
{
	if(!m_skrepka){
		move( (app::screen.width()/2)-(m_windowSize.width()/2) ,  app::screen.height() - 50);
		m_pTimer->start();
	}
	QMainWindow::enterEvent(event);
}

void HWMonitorWindow::leaveEvent(QEvent *event)
{
	if( m_pTimer->isActive() ) m_pTimer->stop();
	if(!m_skrepka) move( (app::screen.width()/2)-(m_windowSize.width()/2) ,  app::screen.height() - 3);
	QMainWindow::leaveEvent(event);
}

void HWMonitorWindow::getMainSize()
{
	m_windowSize.setWidth( this->size().width() );
	m_windowSize.setHeight( this->size().height() );
}
