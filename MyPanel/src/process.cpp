#include "process.h"

Process::Process(const QString &cmd, const QStringList &args, QObject *parent)
	: QObject(parent)
{
	m_pProcess = new QProcess(this);
		m_pProcess->setProgram( cmd );
		m_pProcess->setArguments( args );

	connect(m_pProcess,static_cast<void(QProcess::*)(int,QProcess::ExitStatus)>(&QProcess::finished),this,[this](int exitCode, QProcess::ExitStatus exitStatus){
		if( m_secondStart ){
			m_secondStart = false;
			emit signal_finished(this);
			return;
		}
		/*
		int m_status = proc_status_normal;

		if( exitStatus == QProcess::NormalExit ){
			if( exitCode == 0 ){
				m_status = proc_status_normal;
			}else{
				m_status = proc_status_exit;
			}
		}
		if( exitStatus == QProcess::CrashExit ) m_status = proc_status_crash;

		*/

		QString cmd = m_pProcess->program();
		m_secondStart = true;
		m_pProcess->setProgram("notify-send");
		m_pProcess->setArguments(QStringList()<<"-u"<<"normal"<<"-i"<<"dialog-information"<<cmd<<"Finished");
		m_pProcess->start(QIODevice::ReadOnly);
		m_pProcess->waitForStarted(50);
		m_pProcess->waitForFinished();
	});
	connect(m_pProcess,static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),this,[this](QProcess::ProcessError ProcessError){
		QString textError;
		switch (ProcessError) {
			case QProcess::FailedToStart: textError = tr("Failed To Start"); break;
			case QProcess::Crashed: textError = tr("Crashed"); break;
			case QProcess::Timedout: textError = tr("Timedout"); break;
			case QProcess::WriteError: textError = tr("Write Error"); break;
			case QProcess::ReadError: textError = tr("Read Error"); break;
			default: textError = tr("Unknown Error"); break;
		}

		m_pProcess->close();
		QString cmd = m_pProcess->program();
		m_secondStart = true;
		m_pProcess->setProgram("notify-send");
		m_pProcess->setArguments(QStringList()<<"-u"<<"critical"<<"-i"<<"dialog-warning"<<cmd<<textError + " >: " + m_pProcess->errorString());
		m_pProcess->start(QIODevice::ReadOnly);
		m_pProcess->waitForStarted(50);
		m_pProcess->waitForFinished();
	});
}

Process::~Process()
{
	if( m_pProcess->isOpen() ) m_pProcess->kill();
	m_pProcess->deleteLater();
}

void Process::start()
{
	m_pProcess->start(QIODevice::ReadOnly);
	m_pProcess->waitForStarted(150);
	m_pProcess->waitForFinished();
}

void Process::stop()
{
	if( m_pProcess->isOpen() ){
		m_pProcess->terminate();
		m_pProcess->waitForFinished(100);
		if( m_pProcess->isOpen() ) m_pProcess->kill();
	}
}

