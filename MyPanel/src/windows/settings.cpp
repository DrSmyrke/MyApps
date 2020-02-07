#include "settings.h"
#include <QLayout>
#include <QLabel>

Settings::Settings(QWidget *parent) : QMainWindow(parent)
{
	QPushButton* saveB = new QPushButton(tr("SAVE"),this);

	m_pRemoteServer = new QLineEdit(this);
	m_pRUser = new QLineEdit(this);
	m_pRDir = new QLineEdit(this);
	m_pPersonalDir = new QLineEdit(this);
	m_pRemotePort = new QSpinBox(this);
		m_pRemotePort->setRange(1,65535);
	m_pEditor = new QTextEdit(this);
	m_pSyncSaveDirs = new QTextEdit(this);

	QWidget* centrWidget = new QWidget(this);
		QVBoxLayout* vBox = new QVBoxLayout();
			QHBoxLayout* syncConnBox = new QHBoxLayout();
			syncConnBox->addWidget(new QLabel( tr("<b>Sync Server:</b>") ));
			syncConnBox->addWidget(m_pRemoteServer);
			syncConnBox->addWidget(new QLabel( tr("<b>Port:</b>") ));
			syncConnBox->addWidget(m_pRemotePort);
			syncConnBox->addWidget(new QLabel( tr("<b>User:</b>") ));
			syncConnBox->addWidget(m_pRUser);
		vBox->addLayout(syncConnBox);
			QHBoxLayout* syncConfBox = new QHBoxLayout();
			syncConfBox->addWidget(new QLabel( tr("<b>Work dir:</b>") ));
			syncConfBox->addWidget(m_pRDir);
			syncConfBox->addWidget(new QLabel( tr("<b>Personal dir:</b>") ));
			syncConfBox->addWidget(m_pPersonalDir);
		vBox->addLayout(syncConfBox);
			QHBoxLayout* syncDirsBox = new QHBoxLayout();
			syncDirsBox->addWidget(new QLabel( tr("<b>Sync save dirs:</b>") ));
			syncDirsBox->addWidget(m_pSyncSaveDirs);
		vBox->addLayout(syncDirsBox);



			QHBoxLayout* saveBox = new QHBoxLayout();
			saveBox->addWidget(saveB);
		vBox->addLayout(saveBox);
	centrWidget->setLayout(vBox);

	setCentralWidget(centrWidget);
	setMinimumSize(700,480);
	setWindowTitle(tr("Settings"));

	connect(m_pEditor,&QTextEdit::textChanged,this,[this]{ m_changeSettings = true; });
	connect(saveB,&QPushButton::clicked,this,[this]{
		if(m_changeSettings){
			app::conf.sync.saveDirs.clear();
			for(auto elem:m_pSyncSaveDirs->toPlainText().split("\n")) app::conf.sync.saveDirs.push_back( elem );

			app::conf.sync.server = m_pRemoteServer->text();
			app::conf.sync.user = m_pRUser->text();
			app::conf.sync.port = m_pRemotePort->value();
			app::conf.sync.workDir = m_pRDir->text();
			app::conf.sync.personalDir = m_pPersonalDir->text();


			app::saveSettings();
		}
		m_changeSettings = false;
		this->hide();
	});
}

void Settings::open()
{
	this->show();
	m_pEditor->clear();

	m_pSyncSaveDirs->clear();
	for( auto elem:app::conf.sync.saveDirs ) m_pSyncSaveDirs->append( elem );

	m_pRemoteServer->setText( app::conf.sync.server );
	m_pRUser->setText( app::conf.sync.user );
	m_pRDir->setText( app::conf.sync.workDir );
	m_pRemotePort->setValue( app::conf.sync.port );
	m_pPersonalDir->setText( app::conf.sync.personalDir );
}
