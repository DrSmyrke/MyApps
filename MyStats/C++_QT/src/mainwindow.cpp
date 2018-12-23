#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QIcon appIco = QIcon("://img/monitor.png");

	m_pMenu = new QMenu(this);
		QAction* exitAction = new QAction(QIcon("://img/exit.png"),tr("Exit"), this);
		connect(exitAction,&QAction::triggered,this,&QApplication::exit);
		m_pMenu->addAction( exitAction );
	m_pTimer = new QTimer(this);
		m_pTimer->setInterval(250);
	m_pHWMonitorWidget = new HWMonitorWidget(this);
		m_pHWMonitorWidget->setMouseTracking(true);
	m_pTrayIcon = new QSystemTrayIcon(this);
		m_pTrayIcon->setIcon( appIco );
		m_pTrayIcon->setContextMenu( m_pMenu );
		m_pTrayIcon->show();

	setCentralWidget(m_pHWMonitorWidget);
	//setWindowFlags(Qt::WindowStaysOnBottomHint | Qt::CustomizeWindowHint | Qt::Tool);
	setWindowFlags(Qt::Popup | Qt::ToolTip);
	setBaseSize( 270, 240 );
	setFixedWidth( 270 );
	setAttribute(Qt::WA_TranslucentBackground);

	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_heightChangeRequest,this,[this](const uint16_t newValue){
		this->setFixedHeight( newValue );
		if( !this->isHidden() ) setWindowAction();
	});
	connect(m_pTimer,&QTimer::timeout,m_pHWMonitorWidget,&HWMonitorWidget::slot_update);
	connect(m_pHWMonitorWidget,&HWMonitorWidget::signal_diskClicked,this,[this]( const QString &path, const QString &name ){
		QMessageBox msgBox(this);
		msgBox.setText("You are selected device [" + name + "]");
		msgBox.setInformativeText(tr("Device mount to: ") + path);
		msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		msgBox.setButtonText(QMessageBox::Discard,tr("Eject"));
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
		if( !m_pMenu->isHidden() ){
			m_pTimer->stop();
			this->hide();
			app::conf.showData = false;
			return;
		}
		if( !this->isHidden() ){
			m_pTimer->stop();
			this->hide();
			app::conf.showData = false;
		}else{
			m_pTimer->start();
			this->show();
			app::conf.showData = true;
			setWindowAction();
		}
	});
}

MainWindow::~MainWindow()
{

}

void MainWindow::setWindowAction()
{
	int primaryScreenNum = QApplication::desktop()->primaryScreen();
	auto primaryScreen = QApplication::desktop()->screenGeometry( primaryScreenNum );
	//m_screen = QApplication::desktop()->screenGeometry(this);

	//auto y = QCursor::pos().y();
	//qDebug()<<QApplication::desktop()->geometry()<<primaryScreen;

	this->move( primaryScreen.width() - this->width() - 5, primaryScreen.height() - this->height() - 40 );
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

	QMainWindow::mouseReleaseEvent(event);
}
