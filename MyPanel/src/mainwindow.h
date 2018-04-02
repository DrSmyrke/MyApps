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
#include "windows/execwindow.h"
#include "windows/bookmarkswindow.h"
#include "nativeeventfilter.h"
#include "process.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	BookmarksWindow* m_pBookmarksWindow;
	ExecWindow* m_pExecWindow;
	HWMonitorWindow* m_pHWMonitorWindow;
	SerialMonitor* m_pSerialMonitor;
	Settings* m_pSettings;
	QPushButton* m_pComTermB;
	QPushButton* m_pMenuB;
	QPushButton* m_pMonitorB;
	NativeEventFilter* m_pNativeEventFilter;
	QMenu* m_pMainMenu;
	QMenu* m_pSSHMenu;
	QMenu* m_pBookmarksMenu;
	QAction* m_pProcCount;

	bool m_leave = false;
	QSize m_windowSize;
	std::vector<Process*> m_process;

	void getMainSize();
	void panelHide();
	void startDetached(const QString &cmd, const QStringList &args = QStringList());
	void changeProcCounter() { m_pProcCount->setText( tr("Running threads: ") + QString::number( m_process.size() ) ); }
	void reloadBookmarks();
signals:
	void signal_stopAll();
private slots:
	void slot_GlobalHotkey(const uint8_t mode, const uint16_t key);
	void slot_syncSave();
	void slot_sshMenuUpdate();
protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
