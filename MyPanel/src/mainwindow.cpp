#include "mainwindow.h"
#include <QLayout>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QDesktopWidget>
#include <QThread>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(10,10);

	m_pHWMonitorWindow = new HWMonitorWindow(this);
	m_pSerialMonitor = new SerialMonitor(this);
	m_pSettings = new Settings(this);
	m_pExecWindow = new ExecWindow(this);

	m_pProcCount = new QAction(this);
		m_pProcCount->setIcon( QIcon("://img/terminal.png") );
		m_pProcCount->setEnabled(false);
	m_pSSHMenu = new QMenu("SSH",this);
		connect(m_pSSHMenu,&QMenu::aboutToShow,this,&MainWindow::slot_sshMenuUpdate);
	m_pMainMenu = new QMenu(this);
		QAction* settingsM = new QAction(tr("Settings"), this);
		connect(settingsM,&QAction::triggered,m_pSettings,&Settings::open);
	m_pMainMenu->addAction(settingsM);
//		QAction* clearErrM = new QAction(tr("Clear Errors"), this);
//		connect(clearErrM,&QAction::triggered,m_pTaskListWidget,&TaskListWidget::clear_errors);
//	m_pMainMenu->addAction(clearErrM);
	m_pMainMenu->addSeparator();
	m_pMainMenu->addMenu(m_pSSHMenu);
	m_pMainMenu->addSeparator();
	m_pMainMenu->addAction(m_pProcCount);

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
	for(auto elem:app::conf.autostartList){
		if( elem.isEmpty() ) continue;
		auto tmp = elem.split(" ");
		if( tmp.size() > 0 ){
			tmp.pop_front();
			startDetached( tmp[0], tmp );
		}
	}

	QTimer::singleShot(1000,this,&MainWindow::slot_syncInit);

	QTimer::singleShot(3000,this,[this](){
		m_pMonitorB->click();
	});

	//RSYNC

}

MainWindow::~MainWindow()
{
	emit signal_stopAll();
	system("killall rsync");
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

void MainWindow::startDetached(const QString &cmd, const QStringList &args)
{
	if( cmd.isEmpty() ) return;
	//QProcess process;
	//process.setProgram( cmd );
	//QProcess::startDetached( cmd );
	//Process proc;
	//proc.start(cmd);
	//process.startDetached( cmd );
	//process.waitForStarted(5000);
	//qDebug()<<process.processId()<<cmd<<process.exitStatus()<<process.exitCode();
	Process* proc = new Process(cmd, args);
	QThread* thread = new QThread();
	proc->moveToThread(thread);
	connect(thread,&QThread::started,proc,&Process::start);
	/* при запуске потока будет вызван метод process(), который будет работать в новом потоке */
	connect(proc,&Process::signal_finished,thread,&QThread::quit);
	/* … и при завершении работы построителя отчетов, обертка построителя передаст потоку сигнал finished() , вызвав срабатывание слота quit()	*/
	connect(this,&MainWindow::signal_stopAll,proc,&Process::stop);
	/* … и MainWindow может отправить сигнал о срочном завершении работы обертке построителя, а она уже остановит построитель и направит сигнал finished() потоку */
	connect(proc,&Process::signal_finished,this,[this](Process* proc){
		for(auto iter = m_process.begin(); iter != m_process.end(); iter++){
			if( (*iter) == proc ){
				m_process.erase( iter );
				break;
			}
		}
		changeProcCounter();
	});
	connect(proc,&Process::signal_finished,proc,&Process::deleteLater);
	/* … и обертка пометит себя для удаления */
	connect(thread,&QThread::finished,thread,&QThread::deleteLater);
	/* … и поток пометит себя для удаления. Удаление будет произведено только после полной остановки потока. */
	thread->start();
	m_process.push_back(proc);
	changeProcCounter();
}

void MainWindow::slot_sshMenuUpdate()
{
	m_pSSHMenu->clear();

		QAction* configM = new QAction(QIcon("://img/system.png"),tr("Config"), this);
		connect(configM,&QAction::triggered,this,[this](){ startDetached("xdg-open",QStringList()<<app::conf.sshConfig); });
	m_pSSHMenu->addAction(configM);
		QAction* confiDirgM = new QAction(QIcon("://img/folder.png"),"~/.ssh", this);
		connect(confiDirgM,&QAction::triggered,this,[this](){ startDetached("xdg-open",QStringList()<<app::conf.sshConfDir); });
	m_pSSHMenu->addAction(confiDirgM);
	m_pSSHMenu->addSeparator();

	FILE* f = fopen(app::conf.sshConfig.toLatin1().data(),"r");
	QByteArray buff;
	char ch;
	uint8_t n;
	while( (n = fread(&ch,1,1,f) ) > 0 ) buff.append(ch);
	fclose(f);

	SSHConfig sshconf;
	std::vector<SSHConfig> array;

	for(auto str:buff.split('\n')){
		while( str.indexOf("\t") == 0 ) str.remove(0,1);
		while( str.indexOf(" ") == 0 ) str.remove(0,1);
		str.replace("	",QByteArray(" "));
		while( str.contains(QByteArray("  ")) ) str.replace("  ",QByteArray(" "));
		if( str == "" ) continue;
		auto tmp = str.split(' ');
		if( tmp.size() > 1 ){
			if( tmp[0].toLower() == "host" ){
				if( !sshconf.host.isEmpty() ){
					array.push_back(sshconf);
					sshconf.host.clear();
					sshconf.hostName.clear();
					sshconf.keyFile.clear();
					sshconf.port = sshconf.defaultPort;
					sshconf.kexAlgorithms.clear();
					sshconf.localForward.second.clear();
					sshconf.user.clear();
				}
				sshconf.host = tmp[1];
				continue;
			}
			if( tmp[0].toLower() == "hostname" ){
				sshconf.hostName = tmp[1];
				continue;
			}
			if( tmp[0].toLower() == "user" ){
				sshconf.user = tmp[1];
				continue;
			}
			if( tmp[0].toLower() == "identityfile" ){
				sshconf.keyFile = tmp[1];
				continue;
			}
			if( tmp[0].toLower() == "port" ){
				sshconf.port = tmp[1].toUInt();
				continue;
			}
			if( tmp[0].toLower() == "localforward" and tmp.size() == 3 ){
				sshconf.localForward.first = tmp[1].toUInt();
				sshconf.localForward.second = tmp[2];
				continue;
			}
			if( tmp[0].toLower() == "kexalgorithms" ){
				for( uint8_t i = 1; i < tmp.size(); i++ ) sshconf.kexAlgorithms.push_back(tmp[i]);
				continue;
			}
		}
	}

	for( auto elem:array ){
		QMenu* menu = new QMenu(elem.host,this);
				QAction* actionTerm = new QAction(QIcon("://img/terminal.png"),tr("Open in terminal"), this);
				connect(actionTerm,&QAction::triggered,this,[this,elem](){ startDetached("exo-open", QStringList()<<"--launch"<<"TerminalEmulator"<<"-e"<<"ssh"<<elem.hostName); });
			menu->addAction(actionTerm);
				QAction* actionDir = new QAction(QIcon("://img/folder.png"),tr("Open in filemanager"), this);
				connect(actionDir,&QAction::triggered,this,[this,elem](){ startDetached("xdg-open",QStringList()<<"ssh://"+elem.host); });
			menu->addAction(actionDir);
		m_pSSHMenu->addMenu(menu);
	}

	array.clear();
}

void MainWindow::slot_GlobalHotkey(const uint8_t mode, const uint16_t key)
{
	switch (key) {
		case key_type_E: if(mode == key_mode_ctrl) m_pExecWindow->open(); break;
		case key_type_Tilda: if(mode == key_mode_ctrl) startDetached("exo-open", QStringList()<<"--working-directory"<<QDir::homePath()<<"--launch"<<"TerminalEmulator"); break;
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
		QStringList args;
		args.append("-azpgtlF");
		args.append("--delete-excluded");
		args.append("--prune-empty-dirs");
		args.append("-e");
		args.append("ssh -p " + QString::number(app::conf.sync.port));
		args.append(dir);
		args.append(QDir::homePath());
		startDetached( "rsync", args );
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
		//QString cmd = "rsync -azpgtlF --delete-excluded --prune-empty-dirs -e \"ssh -p " + QString::number(app::conf.sync.port) + "\" " + QDir::homePath() + "/" + elem + " " + dir;
		//startDetached( cmd );
		QStringList args;
		args.append("-azpgtlF");
		args.append("--delete-excluded");
		args.append("--prune-empty-dirs");
		args.append("-e");
		args.append("ssh -p " + QString::number(app::conf.sync.port));
		args.append(QDir::homePath() + "/" + elem);
		args.append(dir);
		startDetached( "rsync", args );
	}

	if( app::conf.sync.personalDir.isEmpty() ) return;
	if( app::conf.sync.saveDirs.size() == 0 ) return;

	for(auto elem:app::conf.sync.saveDirs){
		QString dir = app::conf.sync.user + "@" + app::conf.sync.server + ":" + app::conf.sync.workDir + "/" + app::conf.sync.personalDir + "/";
		//QString cmd = "rsync -azpgtlF --delete-excluded --prune-empty-dirs -e \"ssh -p " + QString::number(app::conf.sync.port) + "\" " + QDir::homePath() + "/" + elem + " " + dir;
		//startDetached( cmd );
		QStringList args;
		args.append("-azpgtlF");
		args.append("--delete-excluded");
		args.append("--prune-empty-dirs");
		args.append("-e");
		args.append("ssh -p " + QString::number(app::conf.sync.port));
		args.append(QDir::homePath() + "/" + elem);
		args.append(dir);
		startDetached( "rsync", args );
	}
}

void MainWindow::enterEvent(QEvent *event)
{
	//Q_UNUSED(event);

	getMainSize();
	move( (app::screen.width()/2)-(m_windowSize.width()/2) ,0);
	m_pExecWindow->move( (app::screen.width()/2)-(this->width()/2) ,  this->pos().y() + this->height() + 5);
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
		m_pExecWindow->move( (app::screen.width()/2)-(this->width()/2) ,  this->pos().y() + this->height() + 5);
	}

	QMainWindow::leaveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	//Q_UNUSED(event);

	getMainSize();
	panelHide();
	m_pExecWindow->move( (app::screen.width()/2)-(this->width()/2) ,  this->pos().y() + this->height() + 5);

	QMainWindow::resizeEvent(event);
}



