
#include "settings.h"
#include <QLayout>
#include <QLabel>

Settings::Settings(QWidget *parent) : QMainWindow(parent)
{
	QPushButton* saveB = new QPushButton(tr("SAVE"),this);

	m_pRemoteServer = new QLineEdit(this);
	m_pRUser = new QLineEdit(this);
	m_pRDir = new QLineEdit(this);
	m_pRemotePort = new QSpinBox(this);
		m_pRemotePort->setRange(1,65535);
	m_pEditor = new QTextEdit(this);
	m_pSyncDirs = new QTextEdit(this);
	m_pSyncSaveDirs = new QTextEdit(this);
	m_pSyncMode = new QComboBox(this);
		m_pSyncMode->addItem( mf::modeToStr(sync_mode_work).toUpper(), sync_mode_work );
		m_pSyncMode->addItem( mf::modeToStr(sync_mode_home).toUpper(), sync_mode_home );

	QWidget* centrWidget = new QWidget(this);
		QVBoxLayout* vBox = new QVBoxLayout();
			QHBoxLayout* syncModeBox = new QHBoxLayout();
			syncModeBox->addWidget(new QLabel( tr("<b>Sync Mode:</b>") ));
			syncModeBox->addWidget(m_pSyncMode);
		vBox->addLayout(syncModeBox);
			QHBoxLayout* syncConnBox = new QHBoxLayout();
			syncConnBox->addWidget(new QLabel( tr("<b>Sync Server:</b>") ));
			syncConnBox->addWidget(m_pRemoteServer);
			syncConnBox->addWidget(new QLabel( tr("<b>Port:</b>") ));
			syncConnBox->addWidget(m_pRemotePort);
		vBox->addLayout(syncConnBox);
			QHBoxLayout* syncInfoBox = new QHBoxLayout();
			syncInfoBox->addWidget(new QLabel( tr("<b>Sync User:</b>") ));
			syncInfoBox->addWidget(m_pRUser);
			syncInfoBox->addWidget(new QLabel( tr("<b>Remote dir:</b>") ));
			syncInfoBox->addWidget(m_pRDir);
			syncInfoBox->setStretch(3,1);
		vBox->addLayout(syncInfoBox);
			QHBoxLayout* syncDirsBox = new QHBoxLayout();
			syncDirsBox->addWidget(new QLabel( tr("<b>Sync dirs:</b>") ));
			syncDirsBox->addWidget(m_pSyncDirs);
			syncDirsBox->addWidget(new QLabel( tr("<b>Sync save dirs:</b>") ));
			syncDirsBox->addWidget(m_pSyncSaveDirs);
		vBox->addLayout(syncDirsBox);
			QHBoxLayout* autostartBox = new QHBoxLayout();
			autostartBox->addWidget(new QLabel( tr("<b>Autostart:</b>") ));
			autostartBox->addWidget(m_pEditor);
		vBox->addLayout(autostartBox);
			QHBoxLayout* saveBox = new QHBoxLayout();
			saveBox->addWidget(saveB);
		vBox->addLayout(saveBox);
	centrWidget->setLayout(vBox);

	setCentralWidget(centrWidget);
	setMinimumSize(640,480);
	setWindowTitle(tr("Settings"));

	connect(m_pEditor,&QTextEdit::textChanged,this,[this]{ m_changeSettings = true; });
	connect(saveB,&QPushButton::clicked,this,[this]{
		if(m_changeSettings){

			app::conf.autostartList.clear();
			for(auto elem:m_pEditor->toPlainText().split("\n")) app::conf.autostartList.push_back( elem );

			app::conf.sync.dirs.clear();
			for(auto elem:m_pSyncDirs->toPlainText().split("\n")) app::conf.sync.dirs.push_back( elem );
			app::conf.sync.saveDirs.clear();
			for(auto elem:m_pSyncSaveDirs->toPlainText().split("\n")) app::conf.sync.saveDirs.push_back( elem );

			app::conf.sync.mode = m_pSyncMode->currentData(Qt::UserRole).toUInt();
			app::conf.sync.server = m_pRemoteServer->text();
			app::conf.sync.user = m_pRUser->text();
			app::conf.sync.remoteDir = m_pRDir->text();
			app::conf.sync.port = m_pRemotePort->value();

			app::saveSettings();
		}
		m_changeSettings = false;
		this->hide();
	});
}

void Settings::open()
{
	uint8_t i = 0;

	this->show();
	m_pEditor->clear();
	for( auto elem:app::conf.autostartList ) m_pEditor->append( elem );

	m_pSyncDirs->clear();
	for( auto elem:app::conf.sync.dirs ) m_pSyncDirs->append( elem );
	m_pSyncSaveDirs->clear();
	for( auto elem:app::conf.sync.saveDirs ) m_pSyncSaveDirs->append( elem );

	if(app::conf.sync.mode){
		for(i = 0; i < m_pSyncMode->count(); i++){
			if( m_pSyncMode->itemData(i, Qt::UserRole).toUInt() == app::conf.sync.mode ){
				m_pSyncMode->setCurrentIndex(i);
				break;
			}
		}
	}

	m_pRemoteServer->setText( app::conf.sync.server );
	m_pRUser->setText( app::conf.sync.user );
	m_pRDir->setText( app::conf.sync.remoteDir );
	m_pRemotePort->setValue( app::conf.sync.port );
}
