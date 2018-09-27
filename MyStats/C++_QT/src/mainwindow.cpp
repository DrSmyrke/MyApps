#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QProcess>
//TODO: remove qDebug
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(250);
	m_pHWMonitorWidget = new HWMonitorWidget(this);
		m_pHWMonitorWidget->setMouseTracking(true);

	setCentralWidget(m_pHWMonitorWidget);
	setWindowFlags(Qt::Popup | Qt::ToolTip);
	setBaseSize( app::conf.windowFixedWidth, app::conf.windowRealHeight );
	setFixedWidth( app::conf.windowFixedWidth );
	setAttribute(Qt::WA_TranslucentBackground);
	move( app::conf.winPos );

	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_heightChangeRequest,this,[this]( const uint16_t newHeight ){
		app::conf.windowRealHeight = newHeight;
		if( app::conf.windowState == window_state_show ) this->setFixedHeight( app::conf.windowRealHeight );
	});
	connect(m_pTimer,&QTimer::timeout,this,[this](){
		if( app::conf.saveSettings ){
			if( app::conf.saveSettingsCounter++ >= 28 ){	// 7000 ms / 250 ms timer timeout = 28
				app::conf.saveSettings = false;
				app::saveSettings();
			}
		}
		m_pHWMonitorWidget->slot_update();
		if( !app::conf.fixed ){
			if( app::conf.hideWindowCounter == 0 ){
				setWindowState( window_state_hide );
				app::conf.hideWindowCounter = -1;
			}
			if( app::conf.hideWindowCounter > 0 ) app::conf.hideWindowCounter--;
		}
	});
	connect(QApplication::desktop(),&QDesktopWidget::screenCountChanged,this,[this]( const int num ){
		setWindowState( app::conf.windowState );
		Q_UNUSED(num);
	});
	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_diskClicked,this,[this]( const QString &path, const QString &name ){
		QMessageBox msgBox(this);
		msgBox.setText("You are selected device [" + name + "]");
		msgBox.setInformativeText("Device mount to: " + path);
		msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		msgBox.setButtonText(QMessageBox::Discard,"Eject");
		msgBox.setWindowFlags( Qt::ToolTip );
		int ret = msgBox.exec();
		QString dev;
		switch (ret) {
			case QMessageBox::Open: QProcess::startDetached("xdg-open " + path); break;
			case QMessageBox::Discard:
				dev = name.left( name.length() - 1 );
				QProcess::startDetached("udisks --unmount " + name);
				QProcess::startDetached("udisksctl unmount -b " + name);
				QProcess::startDetached("udisks --detach " + dev);
				QProcess::startDetached("udisksctl power-off -b " + dev);
			break;
			default: break;
		}
	});

	m_pTimer->start();

	setWindowState( app::conf.windowState );
}

MainWindow::~MainWindow()
{

}

void MainWindow::setWindowState(const uint8_t state)
{
	app::conf.windowState = state;
	app::conf.saveSettings = true;

	int primaryScreenNum = QApplication::desktop()->primaryScreen();
	auto primaryScreen = QApplication::desktop()->screenGeometry( primaryScreenNum );
	m_screen = QApplication::desktop()->screenGeometry(this);

	// если окно ушло за область рабочих столов
	int screens = QApplication::desktop()->screenCount();
	QRect wholeDisplayGeometry;
	for(int i = 0; i < screens; ++i ){
		 QRect screenRect = QApplication::desktop()->screen(i)->geometry();
		 wholeDisplayGeometry = wholeDisplayGeometry.united(screenRect); //union
	}
	if( this->pos().x() > wholeDisplayGeometry.width() || this->pos().y() > wholeDisplayGeometry.height()
			|| this->pos().x() < 0 || this->pos().y() < 0 ){
		m_screen = primaryScreen;
		this->move( primaryScreen.x(), primaryScreen.y() );
		app::conf.windowState = window_state_show;
	}
	//



	int16_t screenTX = this->pos().x() - m_screen.x();
	int16_t screenTY = this->pos().y() - m_screen.y();
	int16_t screenBX = screenTX + this->width();
	int16_t screenBY = screenTY + this->height();

	uint16_t newWidth = app::conf.windowFixedWidth;
	uint16_t newHeight = this->size().height();

	switch ( app::conf.windowState ) {
		case window_state_hide:
			if( screenTX == 0 || screenBX == m_screen.width() ) newWidth = app::conf.showSize;
			if( screenTY == 0 || screenBY == m_screen.height() ) newHeight = app::conf.showSize;
			this->setFixedSize( newWidth, newHeight );
			if( screenBX >= m_screen.width() ) move( this->pos().x() + app::conf.windowFixedWidth - app::conf.showSize, this->pos().y() );
			if( screenBY >= m_screen.height() ) move( this->pos().x(), m_screen.height() - app::conf.showSize );
			m_pHWMonitorWidget->slot_hide();
		break;
		case window_state_show:
			this->setFixedSize( app::conf.windowFixedWidth, app::conf.windowRealHeight );
			if( screenTX == m_screen.width() - app::conf.showSize ) move( this->pos().x() - app::conf.windowFixedWidth + app::conf.showSize, this->pos().y() );
			if( screenTY == m_screen.height() - app::conf.showSize ) move( this->pos().x(), m_screen.height() - app::conf.windowRealHeight );
			m_pHWMonitorWidget->slot_show();
			app::conf.hideWindowCounter = -1;
			m_hideFlag = false;
		break;
	}

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	m_mouseClick = true;
	m_xOffset = event->windowPos().x();
	m_yOffset = event->windowPos().y();

	//Q_UNUSED(event);
	QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	m_mouseClick = false;
	if( !m_mouseMove ){
		if( !m_pHWMonitorWidget->mouseClickToObject() ){
			app::conf.fixed = !app::conf.fixed;
			app::conf.saveSettings = true;
		}
	}
	m_mouseMove = false;

	QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(m_mouseClick){
		if( app::conf.fixed ){
			m_mouseMove = true;
			return;
		}
		int screens = QApplication::desktop()->screenCount();
		QRect wholeDisplayGeometry;
		for(int i = 0; i < screens; ++i ){
			 QRect screenRect = QApplication::desktop()->screen(i)->geometry();
			 wholeDisplayGeometry = wholeDisplayGeometry.united(screenRect); //union
		}
		m_hideFlag = false;
		m_screen = QApplication::desktop()->screenGeometry(this);
		auto startPos = this->pos();
		this->move( event->screenPos().x() - m_xOffset, event->screenPos().y() - m_yOffset );
		if( this->pos().x() < wholeDisplayGeometry.x() ) this->move( wholeDisplayGeometry.x(), this->pos().y() );
		if( this->pos().y() < wholeDisplayGeometry.y() ) this->move( this->pos().x(), wholeDisplayGeometry.y() );
		if( this->pos().x() + this->width() > wholeDisplayGeometry.width() )  this->move( wholeDisplayGeometry.width() - this->width(), this->pos().y() );
		if( this->pos().y() + this->height() > wholeDisplayGeometry.height() )  this->move( this->pos().x(), wholeDisplayGeometry.height() - this->height() );
		int16_t screenTX = this->pos().x() - m_screen.x();
		int16_t screenTY = this->pos().y() - m_screen.y();
		int16_t screenBX = screenTX + this->width();
		int16_t screenBY = screenTY + this->height();
		if( screenTX >= 0 && screenTX <= app::conf.borderSize ){
			m_hideFlag = true;
			this->move( m_screen.x(), this->pos().y() );
		}
		if( screenTY >= 0 && screenTY <= app::conf.borderSize ){
			m_hideFlag = true;
			this->move( this->pos().x(), m_screen.y() );
		}
		if( screenBX <= m_screen.width() && screenBX >= m_screen.width() - app::conf.borderSize ){
			m_hideFlag = true;
			uint16_t rx = m_screen.width() - screenBX;
			this->move( this->pos().x() + rx, this->pos().y() );
		}
		if( screenBY <= m_screen.height() && screenBY >= m_screen.height() - app::conf.borderSize ){
			m_hideFlag = true;
			uint16_t ry = m_screen.height() - screenBY;
			this->move( this->pos().x(), this->pos().y() + ry );
		}
		if( startPos.x() != this->pos().x() or startPos.y() != this->pos().y() ){
			m_mouseMove = true;
			app::conf.winPos = this->pos();
			app::conf.saveSettings = true;
		}
	}

	QMainWindow::mouseMoveEvent(event);
}

void MainWindow::enterEvent(QEvent *event)
{
	if( app::conf.windowState == window_state_hide ) { setWindowState( window_state_show ); m_hideFlag = true; }

	QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
	if( m_hideFlag ) app::conf.hideWindowCounter = 2;

	QMainWindow::leaveEvent(event);
}
