#ifndef HWMONITORWINDOW_H
#define HWMONITORWINDOW_H

#include <QMainWindow>
#include "widgets/hwmonitorwidget.h"
#include "global.h"
#include <QTimer>

class HWMonitorWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit HWMonitorWindow(QWidget *parent = 0);
public slots:
	void open();
	void close();
private:
	HWMonitorWidget* m_pHWMonitorWidget;
	QTimer* m_pTimer;

	bool m_mouseClick = false;
	bool m_mouseMove = false;
	qreal m_xOffset = 0;
	qreal m_yOffset = 0;
	QSize m_windowSize;
	bool m_skrepka = false;

	void getMainSize();
protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void enterEvent(QEvent *event) override;
	void leaveEvent(QEvent *event) override;
};

#endif // HWMONITORWINDOW_H
