#include "mainwindow.h"
#include <QLayout>
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QDesktopWidget>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(10,10);

	m_pHWMonitorWindow = new HWMonitorWindow(this);
	m_pSerialMonitor = new SerialMonitor(this);
	m_pSettings = new Settings(this);

	m_pSSHMenu = new QMenu("SSH",this);
	m_pMainMenu = new QMenu(this);
		QAction* settingsM = new QAction(tr("Settings"), this);
		connect(settingsM,&QAction::triggered,m_pSettings,&Settings::open);
	m_pMainMenu->addAction(settingsM);
//		QAction* clearErrM = new QAction(tr("Clear Errors"), this);
//		connect(clearErrM,&QAction::triggered,m_pTaskListWidget,&TaskListWidget::clear_errors);
//	m_pMainMenu->addAction(clearErrM);
	m_pMainMenu->addSeparator();
	m_pMainMenu->addMenu(m_pSSHMenu);

	m_pMenuB = new QPushButton(QIcon("://img/system.png"),"",this);
	QPushButton* trashB = new QPushButton(QIcon("://img/trash.png"),"",this);
	QPushButton* winKillerB = new QPushButton(QIcon("://img/exit.png"),"",this);
	m_pComTermB = new QPushButton(QIcon("://img/terminalCom.png"),"",this);
		m_pComTermB->setCheckable(true);
	m_pNativeEventFilter = new NativeEventFilter(this);
	qApp->installNativeEventFilter(m_pNativeEventFilter);  // Устанавилваем его на приложение
	QPushButton* rsyncB = new QPushButton(QIcon("://img/save.png"),"",this);
	m_pMonitorB = new QPushButton(QIcon("://img/monitor.png"),"",this);
		m_pMonitorB->setCheckable(true);
	m_pNetFoldersB = new QPushButton(QIcon("://img/folder-remote.png"),"",this);

	QWidget* centrWidget = new QWidget(this);
		QHBoxLayout* hBox = new QHBoxLayout();
			hBox->setSpacing(3);
			hBox->setMargin(1);
		hBox->addWidget(m_pMenuB);
		hBox->addWidget(trashB);
		hBox->addWidget(winKillerB);
		hBox->addWidget(m_pComTermB);
		hBox->addWidget(rsyncB);
		hBox->addWidget(m_pMonitorB);
		hBox->addWidget(m_pNetFoldersB);
	centrWidget->setLayout(hBox);

	setCentralWidget(centrWidget);

	setWindowFlags(Qt::Popup | Qt::ToolTip);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowOpacity( 0.5 );

	connect(m_pMenuB,&QPushButton::clicked,this,[this](){
		getMainSize();
		auto x = m_pMenuB->pos().x() + (app::screen.width()/2)-(m_windowSize.width()/2);
		m_pMainMenu->show();
		m_pMainMenu->move( x, m_windowSize.height() + 5 );

		m_pSSHMenu->clear();
		qDebug()<<QDir::homePath();
//		FILE* f = fopen("/proc/uptime","r");
//		QByteArray buff;
//		char ch;
//		uint8_t n;
//		while( (n = fread(&ch,1,1,f) ) > 0 ){
//			if(ch == ' ' or ch == '	') break;
//			buff.append(ch);
//		}
//		fclose(f);
	});
	connect(trashB,&QPushButton::clicked,this,[this](){
		QString path = QDir::homePath() + "/.local/share/Trash/files";
		mf::removeDir(path);
	});
	connect(winKillerB,&QPushButton::clicked,this,[this](){ startDetached("xkill"); });
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

	connect(rsyncB,&QPushButton::clicked,this,&MainWindow::slot_syncSave);
	connect(m_pMonitorB,&QPushButton::clicked,this,[this](bool checked){
		if(checked){
			m_pHWMonitorWindow->open();
		}else{
			m_pHWMonitorWindow->close();
		}
	});
	connect(m_pMainMenu,&QMenu::aboutToHide,this,[this](){ if( m_leave ) panelHide(); });

	/*
	#ifdef QT_DEBUG
		m_pMonitorB->click();
	#endif
	#ifdef QT_RELEASE
		m_pMonitorB->click();
	#endif
	*/
	// AUTOSTART
	for(auto elem:app::conf.autostartList) startDetached( elem );

	QTimer::singleShot(1000,this,&MainWindow::slot_syncInit);

	QTimer::singleShot(3000,this,[this](){
		m_pMonitorB->click();
	});

	//RSYNC

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

	//qDebug()<<QApplication::desktop()->screenGeometry();
	//qDebug()<<QApplication::desktop()->availableGeometry();
	//qDebug()<<QApplication::desktop()->screenCount();
	//qDebug()<<QApplication::desktop()->screenNumber();
	//qDebug()<<QApplication::desktop()->screenGeometry(1);
}

void MainWindow::panelHide()
{
	getMainSize();
	//move( (app::screen.width()/2)-(m_windowSize.width()/2) , 0 - m_windowSize.height() + 5);
	move( (app::screen.width()/2)-(m_windowSize.width()/2) , -30);
	setWindowOpacity( 0.5 );
	m_leave = false;
}

void MainWindow::startDetached(const QString &cmd)
{
	QProcess::startDetached( cmd  );
}

void MainWindow::slot_GlobalHotkey(const uint8_t mode, const uint16_t key)
{
	switch (key) {
		case key_type_E: if(mode == key_mode_ctrl) startDetached("myexec"); break;
		case key_type_Tilda: if(mode == key_mode_ctrl) startDetached("exo-open --working-directory " + QDir::homePath() + " --launch TerminalEmulator"); break;
	}
}

void MainWindow::slot_syncInit()
{
	if( !app::conf.sync.syncOnStart ) return;
	if( app::conf.sync.server.isEmpty() ) return;
	if( !app::conf.sync.port ) return;
	if( app::conf.sync.user.isEmpty() ) return;
	if( app::conf.sync.workDir.isEmpty() ) return;

	std::vector<QString> data;
	data.push_back("Images");
	data.push_back("Musik");

	for(auto elem:data){
		QString dir = app::conf.sync.user + "@" + app::conf.sync.server + ":" + app::conf.sync.workDir + "/" + elem;
		QString cmd = "rsync -azpgtlF --delete-excluded --prune-empty-dirs -e \"ssh -p " + QString::number(app::conf.sync.port) + "\" " + dir + " " + QDir::homePath();
		startDetached( cmd );
	}
	//	startDetached( "allinone s38 init " + app::conf.sync.server + " " + QString::number(app::conf.sync.port) + " " + app::conf.sync.user );
}

void MainWindow::slot_syncSave()
{
	if( !app::conf.sync.syncOnStart ) return;
	if( app::conf.sync.server.isEmpty() ) return;
	if( !app::conf.sync.port ) return;
	if( app::conf.sync.user.isEmpty() ) return;
	if( app::conf.sync.workDir.isEmpty() ) return;

	std::vector<QString> data;
	data.push_back("Images");
	data.push_back("Musik");

	for(auto elem:data){
		QString dir = app::conf.sync.user + "@" + app::conf.sync.server + ":" + app::conf.sync.workDir + "/";
		QString cmd = "rsync -azpgtlF --delete-excluded --prune-empty-dirs -e \"ssh -p " + QString::number(app::conf.sync.port) + "\" " + QDir::homePath() + "/" + elem + " " + dir;
		startDetached( cmd );
	}

	if( app::conf.sync.personalDir.isEmpty() ) return;
	if( app::conf.sync.saveDirs.size() == 0 ) return;

	for(auto elem:app::conf.sync.saveDirs){
		QString dir = app::conf.sync.user + "@" + app::conf.sync.server + ":" + app::conf.sync.workDir + "/" + app::conf.sync.personalDir + "/";
		QString cmd = "rsync -azpgtlF --delete-excluded --prune-empty-dirs -e \"ssh -p " + QString::number(app::conf.sync.port) + "\" " + QDir::homePath() + "/" + elem + " " + dir;
		startDetached( cmd );
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
	panelHide();

	QMainWindow::resizeEvent(event);
}



