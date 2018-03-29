#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>
#include "global.h"

class Process : public QObject
{
	Q_OBJECT
public:
	explicit Process(const QString &cmd, const QStringList &args, QObject *parent = 0);
	~Process();
public slots:
	void start();
	void stop();
signals:
	void signal_finished(Process* proc);
private:
	QProcess* m_pProcess;
	int m_status;
	bool m_secondStart = false;
};

#endif // PROCESS_H
