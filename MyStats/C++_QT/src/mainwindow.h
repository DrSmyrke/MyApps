#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "hwmonitorwidget.h"
#include "global.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	HWMonitorWidget* m_pHWMonitorWidget;
	QTimer *m_pTimer;

	bool m_mouseClick = false;
	bool m_mouseMove = false;
	qreal m_xOffset = 0;
	qreal m_yOffset = 0;
	int m_prewWidth;

	void setWindowAction();
protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MAINWINDOW_H
