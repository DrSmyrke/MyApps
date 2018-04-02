
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
	QGroupBox* swapGroupBox = new QGroupBox(tr("Swap mode"),this);
	m_pStaticSwapB = new QRadioButton(tr("Static"),this);
	m_pDynamicSwapB = new QRadioButton(tr("Dynamic (use swapspace)"),this);

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
			QHBoxLayout* autostartBox = new QHBoxLayout();
			autostartBox->addWidget(new QLabel( tr("<b>Autostart:</b>") ));
			autostartBox->addWidget(m_pEditor);
		vBox->addLayout(autostartBox);
			QHBoxLayout* swapBox = new QHBoxLayout();
				swapBox->addWidget(m_pStaticSwapB);
				swapBox->addWidget(m_pDynamicSwapB);
			swapGroupBox->setLayout(swapBox);
		vBox->addWidget(swapGroupBox);



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

			app::conf.autostartList.clear();
			for(auto elem:m_pEditor->toPlainText().split("\n")) app::conf.autostartList.push_back( elem );

			app::conf.sync.saveDirs.clear();
			for(auto elem:m_pSyncSaveDirs->toPlainText().split("\n")) app::conf.sync.saveDirs.push_back( elem );

			app::conf.sync.server = m_pRemoteServer->text();
			app::conf.sync.user = m_pRUser->text();
			app::conf.sync.port = m_pRemotePort->value();
			app::conf.sync.workDir = m_pRDir->text();
			app::conf.sync.personalDir = m_pPersonalDir->text();

			if( m_pStaticSwapB->isChecked() ) app::conf.swapMode = swap_mode_static;
			if( m_pDynamicSwapB->isChecked() ) app::conf.swapMode = swap_mode_dynamic;

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

	m_pSyncSaveDirs->clear();
	for( auto elem:app::conf.sync.saveDirs ) m_pSyncSaveDirs->append( elem );

	m_pRemoteServer->setText( app::conf.sync.server );
	m_pRUser->setText( app::conf.sync.user );
	m_pRDir->setText( app::conf.sync.workDir );
	m_pRemotePort->setValue( app::conf.sync.port );
	m_pPersonalDir->setText( app::conf.sync.personalDir );

	switch (app::conf.swapMode) {
		case swap_mode_static: m_pStaticSwapB->setChecked(true); break;
		case swap_mode_dynamic: m_pDynamicSwapB->setChecked(true); break;
	}
}
