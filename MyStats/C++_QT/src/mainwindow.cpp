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
	QIcon appIco = QIcon("://monitor.png");

	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(250);
	m_pHWMonitorWidget = new HWMonitorWidget(this);
		m_pHWMonitorWidget->setMouseTracking(true);
	m_pTrayIcon = new QSystemTrayIcon(this);
		m_pTrayIcon->setIcon( appIco );
		m_pTrayIcon->show();

	setCentralWidget(m_pHWMonitorWidget);
	//setWindowFlags(Qt::WindowStaysOnBottomHint | Qt::CustomizeWindowHint | Qt::Tool);
	setWindowFlags(Qt::Popup | Qt::ToolTip);
	setBaseSize( 270, 240 );
	setFixedWidth( 270 );
	setAttribute(Qt::WA_TranslucentBackground);

	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_heightChangeRequest,this,&MainWindow::setFixedHeight);
	connect(m_pTimer,&QTimer::timeout,m_pHWMonitorWidget,&HWMonitorWidget::slot_update);
	connect(QApplication::desktop(),&QDesktopWidget::screenCountChanged,this,[this]( const int num ){
		//FIXME: 23
		//setWindowState( app::conf.windowState );
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
	connect(m_pTrayIcon,&QSystemTrayIcon::activated,this,[this](){
		if( this->isHidden() ){
			this->show();
			app::conf.showData = true;
			setWindowAction();
		}else{
			this->hide();
			app::conf.showData = false;
		}
	});

	m_pTimer->start();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setWindowAction()
{
	int primaryScreenNum = QApplication::desktop()->primaryScreen();
	auto primaryScreen = QApplication::desktop()->screenGeometry( primaryScreenNum );
	//m_screen = QApplication::desktop()->screenGeometry(this);

	this->move( primaryScreen.width() - this->width(), 0 );

	/*
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
	}
	*/
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
	m_pHWMonitorWidget->mouseClickToObject();
	/*
	m_mouseClick = false;
	if( !m_mouseMove ){
		if( ! ){
			app::conf.fixed = !app::conf.fixed;
			app::conf.saveSettings = true;
		}
	}
	m_mouseMove = false;
	*/

	QMainWindow::mouseReleaseEvent(event);
}
/*
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
			this->move( m_screen.x(), this->pos().y() );
		}
		if( screenTY >= 0 && screenTY <= app::conf.borderSize ){
			this->move( this->pos().x(), m_screen.y() );
		}
		if( screenBX <= m_screen.width() && screenBX >= m_screen.width() - app::conf.borderSize ){
			uint16_t rx = m_screen.width() - screenBX;
			this->move( this->pos().x() + rx, this->pos().y() );
		}
		if( screenBY <= m_screen.height() && screenBY >= m_screen.height() - app::conf.borderSize ){
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
*/
