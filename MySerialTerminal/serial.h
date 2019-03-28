#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include "global.h"

class Serial : public QObject
{
	Q_OBJECT
public:
	explicit Serial(QObject *parent = 0);
	~Serial(){ close(); }
	void init( const SerialSettings &settings );
	void open();
	void close();
	void send( const QByteArray &data );
signals:
	void signal_close();
	void signal_open();
	void signal_readData( const QByteArray &data );
private slots:
	void slot_readData();
private:
	QSerialPort* m_pSerialPort;
};

#endif // SERIAL_H
