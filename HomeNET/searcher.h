#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>
#include <QUdpSocket>
#include "../HomeNET/myproto.h"
#include "../myfunctions.h"

class Searcher : public QObject
{
	Q_OBJECT
public:
	explicit Searcher(QObject *parent = 0);
	void start();
	void stop();
private slots:
	void slot_readyRead();
private:
	QUdpSocket* m_pSocket;

	void send( const QByteArray &data );
};

#endif // SEARCHER_H
