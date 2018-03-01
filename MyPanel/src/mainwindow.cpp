#include "mainwindow.h"
#include <QLayout>
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QFile>

#include <QDebug>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(10,10);

	m_pHWMonitorWindow = new HWMonitorWindow(this);
	m_pSerialMonitor = new SerialMonitor(this);
	m_pSettings = new Settings(this);

	m_pMainMenu = new QMenu(this);
		QAction* settingsM = new QAction(tr("Settings"), this);
		connect(settingsM,&QAction::triggered,m_pSettings,&Settings::open);
	m_pMainMenu->addAction(settingsM);
	m_pMainMenu->addSeparator();

	m_pMenuB = new QPushButton(QIcon("://img/system.png"),"",this);
	QPushButton* trashB = new QPushButton(QIcon("://img/trash.png"),"",this);
	QPushButton* wallpB = new QPushButton(QIcon("://img/wallpapers.png"),"",this);
	QPushButton* winKillerB = new QPushButton(QIcon("://img/exit.png"),"",this);
	QPushButton* execB = new QPushButton(QIcon("://img/apply.png"),"",this);
	m_pComTermB = new QPushButton(QIcon("://img/terminalCom.png"),"",this);
		m_pComTermB->setCheckable(true);
	m_pNativeEventFilter = new NativeEventFilter(this);
	qApp->installNativeEventFilter(m_pNativeEventFilter);  // Устанавилваем его на приложение
	QPushButton* rsyncB = new QPushButton(QIcon("://img/save.png"),"",this);
	m_pMonitorB = new QPushButton(QIcon("://img/monitor.png"),"",this);
		m_pMonitorB->setCheckable(true);

	QWidget* centrWidget = new QWidget(this);
		QHBoxLayout* hBox = new QHBoxLayout();
			hBox->setSpacing(3);
			hBox->setMargin(1);
		hBox->addWidget(m_pMenuB);
		hBox->addWidget(trashB);
		hBox->addWidget(wallpB);
		hBox->addWidget(winKillerB);
		hBox->addWidget(execB);
		hBox->addWidget(m_pComTermB);
		hBox->addWidget(rsyncB);
		hBox->addWidget(m_pMonitorB);
	centrWidget->setLayout(hBox);

	setCentralWidget(centrWidget);

	setWindowFlags(Qt::Popup | Qt::ToolTip);
	// setStyleSheet("QMainWindow { color: rgb(255, 255, 0); background: rgb(0, 0, 0); }");
	setAttribute(Qt::WA_TranslucentBackground);
	//setAutoFillBackground(true);
	setWindowOpacity( 0.5 );

	connect(m_pMenuB,&QPushButton::clicked,this,[this](){
		getMainSize();
		auto x = m_pMenuB->pos().x() + (app::screen.width()/2)-(m_windowSize.width()/2);
		m_pMainMenu->show();
		m_pMainMenu->move( x, m_windowSize.height() + 5 );
	});
	connect(trashB,&QPushButton::clicked,this,[this](){
		QString path = QDir::homePath() + "/.local/share/Trash/files";
		mf::removeDir(path);
	});
	connect(wallpB,&QPushButton::clicked,this,[this](){ QProcess::startDetached("AutoWallpapers.Зефште"); });
	connect(winKillerB,&QPushButton::clicked,this,[this](){ QProcess::startDetached("xkill"); });
	connect(execB,&QPushButton::clicked,this,[this](){ slot_GlobalHotkey(key_mode_ctrl,key_type_E); });
	connect(m_pComTermB,&QPushButton::clicked,this,[this](bool checked){
		if(checked){
			getMainSize();
			m_pSerialMonitor->open();
			auto x = m_pComTermB->pos().x() + (app::screen.width()/2)-(m_windowSize.width()/2);
			m_pSerialMonitor->move( x, m_windowSize.height() + 5 );
		}else{
			m_pSerialMonitor->close();
		}
	});
	connect(m_pNativeEventFilter,&NativeEventFilter::activated,this,&MainWindow::slot_GlobalHotkey);
	m_pNativeEventFilter->setShortcut();

	connect(rsyncB,&QPushButton::clicked,this,[this](){ QProcess::startDetached("allinone s38 "+ mf::modeToStr(app::conf.sync.mode) +" save"); });
	connect(m_pMonitorB,&QPushButton::clicked,this,[this](bool checked){
		if(checked){
			m_pHWMonitorWindow->open();
		}else{
			m_pHWMonitorWindow->close();
		}
	});
	connect(m_pMainMenu,&QMenu::aboutToHide,this,[this](){ if( m_leave ) panelHide(); });

	// AUTOSTART
	for(auto elem:app::conf.autostartList) QProcess::startDetached( elem );

	if( !app::conf.sync.mode or !app::conf.sync.port or app::conf.sync.remoteDir.isEmpty() or app::conf.sync.server.isEmpty() or app::conf.sync.user.isEmpty() ) m_pSettings->open();

	QTimer::singleShot(3000,this,[this](){
		//QProcess cmd;

		//cmd.start();
		m_pMonitorB->click();
	});
}

MainWindow::~MainWindow()
{

}

void MainWindow::getMainSize()
{
	m_windowSize.setWidth( this->size().width() );
	m_windowSize.setHeight( this->size().height() );
	app::screen.setWidth( QApplication::desktop()->screenGeometry().width() );
	app::screen.setHeight( QApplication::desktop()->screenGeometry().height() );

	qDebug()<<QApplication::desktop()->screenGeometry();
	qDebug()<<QApplication::desktop()->availableGeometry();
	qDebug()<<QApplication::desktop()->screenCount();
	qDebug()<<QApplication::desktop()->screenNumber();
	qDebug()<<QApplication::desktop()->screenGeometry(1);
}

void MainWindow::panelHide()
{
	getMainSize();
	move( (app::screen.width()/2)-(m_windowSize.width()/2) , 0 - m_windowSize.height() + 5);
	setWindowOpacity( 0.5 );
	m_leave = false;
}

void MainWindow::slot_GlobalHotkey(const uint8_t mode, const uint16_t key)
{
	switch (key) {
		case key_type_E: if(mode == key_mode_ctrl) QProcess::startDetached("myexec"); break;
		case key_type_Tilda: if(mode == key_mode_ctrl) QProcess::startDetached("exo-open --working-directory " + QDir::homePath() + " --launch TerminalEmulator"); break;
	}
}

void MainWindow::enterEvent(QEvent *event)
{
	//Q_UNUSED(event);

	getMainSize();
	move( (app::screen.width()/2)-(m_windowSize.width()/2) ,0);
	setWindowOpacity( 1 );

	QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
	//Q_UNUSED(event);

	m_leave = true;

	if(!m_pMainMenu->isVisible()){
		// if show main menu
		panelHide();
	}

	QMainWindow::leaveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	//Q_UNUSED(event);

	getMainSize();
	move( (app::screen.width()/2)-(m_windowSize.width()/2) ,  0 - m_windowSize.height() + 5);

	QMainWindow::resizeEvent(event);
}



