#ifndef SERIALMONITOR_H
#define SERIALMONITOR_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QTextBrowser>
#include <QComboBox>
#include <QPushButton>
#include "global.h"

class SerialMonitor : public QMainWindow
{
	Q_OBJECT
public:
	explicit SerialMonitor(QWidget *parent = 0);

private slots:
	void slot_serialPortReadData();
public slots:
	void open();
	void close();
private:
	QSerialPort* m_pSerialPort;
	QTimer* m_pTimer;
	QTextBrowser* m_pConsole;
	QWidget* m_pConfWidget;
	QComboBox* m_pPort;
	QComboBox* m_pSpeed;
	QComboBox* m_pParity;
	QComboBox* m_pDataBits;
	QComboBox* m_pStopBits;
	QComboBox* m_pFolowControl;
	QComboBox* m_pResolution;
	bool m_hexmode = false;
	bool m_changeSettings = false;
	uint8_t m_colCounter = 0;
	uint8_t m_symCounter = 0;

	void reScanComPorts();
	QString printSym(const QString &data);
	void connecting();
	void resizeMonitor();
	/**
	 * @brief chkRow проверяет надо ли переходить на новую строку
	 * @param val кол-во колонок отображено
	 * @return
	 */
	bool chkRow(const uint8_t val);
	void printHex(QByteArray &data);
};

#endif // SERIALMONITOR_H
