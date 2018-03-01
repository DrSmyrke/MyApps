#ifndef HWMONITORWIDGET_H
#define HWMONITORWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimer>
#include "hwmonitor.h"

struct IfaceParam{
	uint16_t y;
	bool hover = false;
	bool hideData = false;
};

class HWMonitorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit HWMonitorWidget(QWidget *parent = 0);
public slots:
	void slot_show();
	void slot_hide();
	void slot_mouseReleaseEvent(QMouseEvent *event);
signals:
	void signal_clickObject(const uint8_t obj, bool state);
	void signal_heightChangeRequest(const uint16_t newValue);
protected:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
private:
	QTimer* m_pTimer;
	HWMonitor* m_pHWMonitor;
	uint8_t m_yOffset = 5;
	uint8_t m_xVallOffset = 60;
	uint8_t m_xParamOffset = 5;	//offset to left border
	uint8_t m_defFontSize = 12;
	QPoint m_mouse;
	bool m_fixed = false;
	std::map<QString,IfaceParam> m_ifaces;

	void drawParam(QPainter &p, const uint16_t x, const uint16_t y, const QString &param);
	void drawValue(QPainter &p, const uint16_t x, const uint16_t y, const QString &value);
	void drawBar(QPainter &p, const uint16_t x, const uint16_t y, const float value, const uint8_t blockCount = 40);
	bool chkHoverIface(const uint16_t y);
};

#endif // HWMONITORWIDGET_H
