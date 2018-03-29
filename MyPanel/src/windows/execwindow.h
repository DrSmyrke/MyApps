#ifndef EXECWINDOW_H
#define EXECWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QCompleter>
#include "global.h"

class ExecWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit ExecWindow(QWidget *parent = 0);
	void open();
private slots:
	void slot_textChanged(const QString &text);
private:
	QCompleter* m_pCompleter;
	QLineEdit* m_pExecLine;
	QPushButton* m_pExecB;

	void resizeMonitor();
};

#endif // EXECWINDOW_H
