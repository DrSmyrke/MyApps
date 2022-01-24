#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QMainWindow>
#include <QSerialPort>
#include <QTime>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void slot_readyRead();
	void slot_rescanPorts();
private:
	Ui::MainWindow *ui;
	QSerialPort* m_pSPort;
	QTimer* m_pTimer;
	bool m_working;
	QByteArray m_buff;
	QDate m_curDate;
	QTime m_curTime;

	bool checkPort(const QString &port, bool close = true);
	void sendData(const QByteArray &data);
};
#endif // MAINWINDOW_H
