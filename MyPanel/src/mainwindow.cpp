#include "mainwindow.h"
#include <QLayout>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	resize(10,10);

	m_pThread = new QThread(this);

	m_pSettings = new Settings(this);
	m_pExecWindow = new ExecWindow(this);
	m_pBookmarksWindow = new BookmarksWindow(this);

	m_pDataUpdate = new DataUpdate();

	m_pSSHMenu = new QMenu("SSH",this);
		connect(m_pSSHMenu,&QMenu::aboutToShow,this,&MainWindow::slot_sshMenuUpdate);
	m_pBookmarksMenu = new QMenu(tr("Bookmarks"), this);
		m_pBookmarksMenu->setIcon( QIcon("://img/folder-remote.png") );
	m_pHomeDirMenu = new QMenu(tr("HOME"), this);
		m_pHomeDirMenu->setIcon( QIcon("://img/folder.png") );
	m_pMainMenu = new QMenu(this);
		QAction* settingsM = new QAction(tr("Settings"), this);
		connect(settingsM,&QAction::triggered,m_pSettings,&Settings::open);
	m_pMainMenu->addAction(settingsM);
//		QAction* clearErrM = new QAction(tr("Clear Errors"), this);
//		connect(clearErrM,&QAction::triggered,m_pTaskListWidget,&TaskListWidget::clear_errors);
//	m_pMainMenu->addAction(clearErrM);
	m_pMainMenu->addSeparator();
	m_pMainMenu->addMenu(m_pSSHMenu);
	m_pMainMenu->addMenu(m_pBookmarksMenu);
	m_pMainMenu->addSeparator();
	m_pMainMenu->addMenu(m_pHomeDirMenu);

	m_pMenuB = new QPushButton(QIcon("://img/system.png"),"",this);
	QPushButton* trashB = new QPushButton(QIcon("://img/trash.png"),"",this);
	QPushButton* winKillerB = new QPushButton(QIcon("://img/exit.png"),"",this);
	m_pNativeEventFilter = new NativeEventFilter(this);
	qApp->installNativeEventFilter(m_pNativeEventFilter);  // Устанавилваем его на приложение
	QPushButton* rsyncB = new QPushButton(QIcon("://img/save.png"),"",this);

	QWidget* centrWidget = new QWidget(this);
		QHBoxLayout* hBox = new QHBoxLayout();
			hBox->setSpacing(3);
			hBox->setMargin(1);
		hBox->addWidget(m_pMenuB);
		hBox->addWidget(trashB);
		hBox->addWidget(winKillerB);
		hBox->addWidget(rsyncB);
	centrWidget->setLayout(hBox);

	setCentralWidget(centrWidget);

	setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowOpacity( 0.5 );
	this->activateWindow();

	connect(m_pMenuB,&QPushButton::clicked,this,[this](){
		getMainSize();
		m_pMainMenu->move( this->mapToGlobal( m_pMenuB->pos() ).x(), 10 );
		m_pMainMenu->show();
		reloadBookmarks();
		drawDirMenu( m_pHomeDirMenu, QDir::homePath() );
	});
	connect(trashB,&QPushButton::clicked,this,[this](){
		QString path = QDir::homePath() + "/.local/share/Trash/files";
		mf::removeDir(path);
	});
	connect(winKillerB,&QPushButton::clicked,this,[this](){ app::startDetached("xkill"); });
	connect(m_pNativeEventFilter,&NativeEventFilter::activated,this,&MainWindow::slot_GlobalHotkey);
	m_pNativeEventFilter->setShortcut();

	connect(rsyncB,&QPushButton::clicked,this,&MainWindow::slot_syncSave);
	connect(m_pBookmarksWindow,&BookmarksWindow::signal_saveBookmarks,this,[this](){ reloadBookmarks(); });
//	connect(m_pExecWindow,&ExecWindow::signal_start,this,[this](const QString &cmd, const QStringList &args){
//		QString str = cmd;
//		if( args.size() > 0 ) str += " " + args.join(" ");
//		app::startDetached(str);
//	});

	/*
	#ifdef QT_DEBUG_pBookmarksWindow,&BookmarksWindow::open
		m_pMonitorB->click();
	#endif
	#ifdef QT_RELEASE
		m_pMonitorB->click();
	#endif
	*/


	reloadBookmarks();


	connect(m_pThread,&QThread::finished,m_pThread,&QThread::deleteLater);
	connect(m_pThread,&QThread::started,m_pDataUpdate,&DataUpdate::updateValuta);

	connect(m_pDataUpdate,&DataUpdate::signal_finished,m_pThread,&QThread::quit);
	connect(this,&MainWindow::signal_stopThreads,m_pDataUpdate,&DataUpdate::slot_stop);
	connect(m_pDataUpdate,&DataUpdate::signal_finished,m_pDataUpdate,&DataUpdate::deleteLater);

	m_pDataUpdate->moveToThread(m_pThread);
	m_pThread->start();
}

MainWindow::~MainWindow()
{
	emit signal_stopThreads();
}

void MainWindow::run()
{
	//automount bookmarks
	auto list = getMountList();
	for(auto elem:app::conf.bookmarks){
		if( elem.mount and !elem.mountDir.isEmpty() and elem.mountOnStart ){
			QString path = QDir::homePath() + "/mnt/" + elem.mountDir;
			bool mountF = false;
			if( list.find(elem.path) != list.end() ){
				if( list.at(elem.path) == path ) mountF = true;
			}
			if( !mountF ) mount(elem.type, elem.path, path);
		}
	}
}

void MainWindow::getMainSize()
{
	int primaryScreenNum = QApplication::desktop()->primaryScreen();
	app::screen = QApplication::desktop()->screenGeometry( primaryScreenNum );

	m_windowSize.setWidth( this->size().width() );
	m_windowSize.setHeight( this->size().height() );

//	qDebug()<<QApplication::desktop()->screenGeometry();
//	qDebug()<<QApplication::desktop()->availableGeometry();
//	qDebug()<<QApplication::desktop()->screenCount();
//	qDebug()<<QApplication::desktop()->screenNumber();
//	qDebug()<<QApplication::desktop()->screenGeometry(1);
}

void MainWindow::panelHide()
{
	getMainSize();
	this->move( app::screen.x() + (app::screen.width()/2)-(m_windowSize.width()/2) , app::screen.y() - this->height() + 5);
	setWindowOpacity( 0.5 );
	m_pExecWindow->move( app::screen.x() + (app::screen.width()/2)-(m_pExecWindow->width()/2), this->pos().y() + this->height() + 5);
}

void MainWindow::reloadBookmarks()
{
	m_pBookmarksMenu->clear();
		QAction* configM = new QAction(QIcon("://img/system.png"),tr("Config"), this);
		connect(configM,&QAction::triggered,m_pBookmarksWindow,&BookmarksWindow::open);
	m_pBookmarksMenu->addAction(configM);
	m_pBookmarksMenu->addSeparator();

	auto list = getMountList();

	//drawBookmarks
	for(auto elem:app::conf.bookmarks){
		if( elem.mount ){
			QString path = QDir::homePath() + "/mnt/" + elem.mountDir;
			bool mountF = false;
			if( list.find(elem.path) != list.end() ){
				if( list.at(elem.path) == path ) mountF = true;
			}
			QMenu* dirM  = new QMenu(elem.name, this);
				dirM->setIcon( QIcon("://img/folder-remote.png") );
				if( !mountF ){
					QAction* dirAction = new QAction( tr("Mount"), this);
					connect(dirAction,&QAction::triggered,this,[this, elem, path](){
						//TDOD: make alert
						if( elem.mountDir.isEmpty() ) return;
						mount(elem.type, elem.path, path);
					});
					dirM->addAction( dirAction );
				}else{
					QAction* dirAction = new QAction( tr("Umount"), this);
					connect(dirAction,&QAction::triggered,this,[this, elem, path](){
						//TDOD: make alert
						if( !QDir( path ).exists() ) return;
						app::startDetached("fusermount",QStringList()<<"-u"<<path);
					});
					dirM->addAction( dirAction );
				}
			//if( mountF ){
			//	connect(dirM,&QMenu::triggered,this,[this,elem](){ startDetached("xdg-open",QStringList()<<elem.path); });
			//}
			m_pBookmarksMenu->addMenu( dirM );
		}else{
			QAction* dirM = new QAction(QIcon("://img/folder-remote.png"),elem.name, this);
			connect(dirM,&QAction::triggered,this,[this,elem](){ app::startDetached("exo-open",QStringList()<<elem.type + "://" + elem.path); });
			m_pBookmarksMenu->addAction(dirM);
		}
	}
}

void MainWindow::drawDirMenu(QMenu *menu, const QString &path)
{
	menu->clear();
		QAction* actionTerm = new QAction(QIcon("://img/terminal.png"),tr("Open in terminal"), this);
		connect(actionTerm,&QAction::triggered,this,[this,path](){ app::startDetached("x-terminal-emulator", QStringList(), path); });
	menu->addAction(actionTerm);
		QAction* actionDir = new QAction(QIcon("://img/folder.png"),tr("Open in filemanager"), this);
		connect(actionDir,&QAction::triggered,this,[this,path](){ app::startDetached("xdg-open",QStringList()<<path); });
	menu->addAction(actionDir);
	menu->addSeparator();

	QDir dir = QDir( path );
	for(auto elem:dir.entryList(QStringList() << "*",QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot)){
		QString newPath = path + "/" + elem;
		if( QDir( newPath ).exists() ){
			QMenu* newMenu = new QMenu( elem, this );
				newMenu->setIcon( QIcon("://img/folder.png") );
				connect(newMenu,&QMenu::aboutToShow,this,[this, newMenu, newPath](){ drawDirMenu( newMenu, newPath ); });
			menu->addMenu( newMenu );
		}
	}
}

void MainWindow::mount(const QString &type, const QString &remotePath, const QString &path)
{
	if( !QDir( path ).exists() ) QDir().mkpath( path );
	//TDOD: make alert
	if( !QDir( path ).exists() ) return;

	//qDebug()<<elem.path<<elem.type<<path;
	if(type == "sftp"){
		app::startDetached("sshfs",QStringList()<<remotePath<<path);
	}
}

std::map<QString, QString> MainWindow::getMountList()
{
	//read mount list
	FILE* f = fopen("/proc/self/mounts","r");
	QByteArray buff;
	char ch;
	uint8_t n;
	while( (n = fread(&ch,1,1,f) ) > 0 ) buff.append(ch);
	fclose(f);

	std::map<QString, QString> list;
	for(auto str:buff.split('\n')){
		str.replace("	", QByteArray(" "));
		while( str.contains( QByteArray("  ") ) ) str.replace("  ", QByteArray(" "));
		auto data = str.split(' ');
		if( data.size() < 2 ) continue;
		list[ data[0] ] = data[1];
	}

	return list;
}

void MainWindow::slot_sshMenuUpdate()
{
	m_pSSHMenu->clear();
	if( !QDir(app::conf.sshConfDir).exists() ) return;

	if( QFile(app::conf.sshConfig).exists() ){
			QAction* configM = new QAction(QIcon("://img/system.png"),tr("Config"), this);
			connect(configM,&QAction::triggered,this,[this](){ app::startDetached("xdg-open",QStringList()<<app::conf.sshConfig); });
		m_pSSHMenu->addAction(configM);
	}
	if( QDir(app::conf.sshConfDir).exists() ){
			QAction* confiDirgM = new QAction(QIcon("://img/folder.png"),"~/.ssh", this);
			connect(confiDirgM,&QAction::triggered,this,[this](){ app::startDetached("xdg-open",QStringList()<<app::conf.sshConfDir); });
		m_pSSHMenu->addAction(confiDirgM);
	}
	m_pSSHMenu->addSeparator();


	QByteArray buff;
	QFile f( app::conf.sshConfig );
	if( !f.exists() ) return;
	if( f.open( QIODevice::ReadOnly ) ){
		buff = f.readLine();
		f.close();
	}

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

	for( auto elem:array ){
		QMenu* menu = new QMenu(elem.host,this);
			if( !elem.host.isEmpty() ){
				QAction* actionTerm = new QAction(QIcon("://img/terminal.png"),tr("Open in terminal"), this);
				connect(actionTerm,&QAction::triggered,this,[this,elem](){
					app::startDetached("exo-open", QStringList()<<"--working-directory"<<QDir::homePath()<<"--launch"<<"TerminalEmulator"<<"ssh " + elem.host);
				});
				menu->addAction(actionTerm);
			}
			if( !elem.hostName.isEmpty() and elem.port and elem.localForward.second.isEmpty() ){
				QString path;
				if( !elem.user.isEmpty() ){
					path = "sftp://" + elem.user + "@" + elem.hostName + ":" + QString::number(elem.port);
				}else{
					path = "sftp://" + elem.hostName + ":" + QString::number(elem.port);
				}
				QAction* actionDir = new QAction(QIcon("://img/folder.png"),tr("Open in filemanager"), this);
				connect(actionDir,&QAction::triggered,this,[this,elem,path](){ app::startDetached("exo-open",QStringList()<<path); });
				menu->addAction(actionDir);
			}
		m_pSSHMenu->addMenu(menu);
	}

	array.clear();
}

void MainWindow::slot_GlobalHotkey(const uint8_t mode, const uint16_t key)
{
	switch (key) {
		case key_type_E: if(mode == key_mode_ctrl) m_pExecWindow->open(); break;
		//case key_type_Tilda: if(mode == key_mode_ctrl) startDetached("exo-open", QStringList()<<"--working-directory"<<QDir::homePath()<<"--launch"<<"TerminalEmulator"); break;
		case key_type_Tilda:
			if(mode == key_mode_ctrl){
				app::startDetached("x-terminal-emulator",QStringList());
			}
		break;
	}
}

/*
void MainWindow::slot_syncreloadBookmarksInit()
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
*/

void MainWindow::slot_syncSave()
{
	if( app::conf.sync.server.isEmpty() ) return;
	if( !app::conf.sync.port ) return;
	if( app::conf.sync.user.isEmpty() ) return;
	if( app::conf.sync.workDir.isEmpty() ) return;

	/*
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
	*/

	if( app::conf.sync.personalDir.isEmpty() ) return;
	if( app::conf.sync.saveDirs.size() == 0 ) return;

	for(auto elem:app::conf.sync.saveDirs){
		QStringList args;
		/*
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
		app::startDetached( "rsync", args );
		*/
		args.append( "s37" );
		args.append( QString::number(app::conf.sync.port) );
		args.append( QString( QDir::homePath() + "/" + elem ) );
		args.append( app::conf.sync.user );
		args.append( app::conf.sync.server );
		args.append( QString( app::conf.sync.workDir + "/" + app::conf.sync.personalDir + "/" ) );
		args.append( "y" );
		app::startDetached( "allinone", args );
	}
}

void MainWindow::enterEvent(QEvent *event)
{
	//Q_UNUSED(event);
	getMainSize();
	this->move( app::screen.x() + (app::screen.width()/2)-(m_windowSize.width()/2) , app::screen.y());
	m_pExecWindow->move( app::screen.x() + (app::screen.width()/2)-(m_pExecWindow->width()/2), this->pos().y() + this->height() + 5);
	setWindowOpacity( 1 );

	QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
	//Q_UNUSED(event);

	panelHide();

	QMainWindow::leaveEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	//Q_UNUSED(event);

	getMainSize();
	panelHide();

	QMainWindow::resizeEvent(event);
}



