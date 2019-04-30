#ifndef CLIENT_H
#define CLIENT_H

#include <QThread>
#include <QTcpSocket>

class Client : public QThread
{
	Q_OBJECT
public:
	explicit Client(qintptr descriptor, const QString &dest_addr,const int dest_port, QObject *parent = 0);
	void run() { slot_start(); }
signals:
	void signal_finished();
public slots:
	void slot_start();
	void slot_stop();
private:
	QTcpSocket* m_pClient;
	QTcpSocket* m_pRClient;
	int m_dist_port;
	QString m_dist_addr;

	QString& parsHead(const QString &data);
};

#endif // CLIENT_H
