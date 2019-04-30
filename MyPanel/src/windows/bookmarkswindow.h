#ifndef BOOKMARKSWINDOW_H
#define BOOKMARKSWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "global.h"

class BookmarksWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit BookmarksWindow(QWidget *parent = 0);

signals:
	void signal_saveBookmarks();
public slots:
	void open();
private slots:
	void slot_save();
private:
	QTableWidget* m_pTable;

	void addRow(const QString &name,const QString &type, const QString &path, bool mount = false, const QString &mountDir = QString(), bool startup = false, const int rowNum = -1);
};

#endif // BOOKMARKSWINDOW_H
