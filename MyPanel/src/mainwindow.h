#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QThread>
#include "global.h"
#include "windows/settings.h"
#include "windows/execwindow.h"
#include "windows/bookmarkswindow.h"
#include "nativeeventfilter.h"
#include "dataupdate.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
    void run();
private:
	BookmarksWindow* m_pBookmarksWindow;
	ExecWindow* m_pExecWindow;
	Settings* m_pSettings;
	QPushButton* m_pMenuB;
	NativeEventFilter* m_pNativeEventFilter;
	QMenu* m_pMainMenu;
	QMenu* m_pSSHMenu;
	QMenu* m_pBookmarksMenu;
	QMenu* m_pHomeDirMenu;
	DataUpdate* m_pDataUpdate;
	QThread* m_pThread;

	bool m_leave = false;
	QSize m_windowSize;

	void getMainSize();
	void panelHide();
	void reloadBookmarks();
	void drawDirMenu(QMenu* menu, const QString &path);
	void mount(const QString &type, const QString &remotePath, const QString &path);

	std::map<QString, QString> getMountList();
signals:
	void signal_stopThreads();
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
