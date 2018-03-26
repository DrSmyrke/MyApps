#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QPushButton>
#include <QMenu>
#include "global.h"
#include "windows/serialmonitor.h"
#include "windows/settings.h"
#include "windows/hwmonitorwindow.h"
#include "nativeeventfilter.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	HWMonitorWindow* m_pHWMonitorWindow;
	SerialMonitor* m_pSerialMonitor;
	Settings* m_pSettings;
	QPushButton* m_pComTermB;
	QPushButton* m_pMenuB;
	QPushButton* m_pMonitorB;
	QPushButton* m_pNetFoldersB;
	NativeEventFilter* m_pNativeEventFilter;
	QMenu* m_pMainMenu;
	QMenu* m_pSSHMenu;

	bool m_leave = false;
	QSize m_windowSize;

	void getMainSize();
	void panelHide();
private slots:
	void slot_GlobalHotkey(const uint8_t mode, const uint16_t key);
protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
