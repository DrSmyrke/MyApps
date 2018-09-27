#ifndef HWMONITORWIDGET_H
#define HWMONITORWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include "hwmonitor.h"
#include "global.h"

struct IfaceParam{
	uint16_t y;
	bool hover = false;
	bool hideData = false;
};

struct DiskParam{
	uint16_t y;
	bool hover = false;
	QString name;
};
struct SwapParam{
	uint16_t y;
	bool hover = false;
	bool clear = false;
};

class HWMonitorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit HWMonitorWidget(QWidget *parent = 0);
	bool mouseClickToObject();
public slots:
	void slot_show();
	void slot_hide();
	void slot_update();
signals:
	void signal_heightChangeRequest(const uint16_t newValue);
	void signal_diskClicked( const QString &path, const QString &name );
protected:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
private:
	HWMonitor* m_pHWMonitor;
	uint8_t m_yOffset = 5;
	uint8_t m_xVallOffset = 60;
	uint8_t m_xParamOffset = 5;	//offset to left border
	uint8_t m_defFontSize = 12;
	QPoint m_mouse;
	uint16_t m_prewY;
	bool m_heightChangeF;
	bool m_fixed = false;
	std::map<QString,IfaceParam> m_ifaces;
	std::map<QString,DiskParam> m_disks;
	SwapParam m_swapParam;
	bool m_show = false;

	void drawParam(QPainter &p, const uint16_t x, const uint16_t y, const QString &param);
	void drawValue(QPainter &p, const uint16_t x, const uint16_t y, const QString &value);
	void drawBar(QPainter &p, const uint16_t x, const uint16_t y, const float value, const uint8_t blockCount = 40);
	bool chkHoverIface(const uint16_t y);
	bool chkHoverDisk(const uint16_t y);
	bool chkHoverSwap(const uint16_t y);
};

#endif // HWMONITORWIDGET_H
