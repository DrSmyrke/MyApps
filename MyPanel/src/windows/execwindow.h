#ifndef EXECWINDOW_H
#define EXECWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include "global.h"

class ExecWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit ExecWindow(QWidget *parent = 0);
	void open();
private slots:
	void slot_textChanged(const QString &text);
	void slot_highlighted(const QString &text) { m_flag = true;Q_UNUSED(text); }
	void slot_returnPressed();
private:
	QCompleter* m_pCompleter;
	QLineEdit* m_pExecLine;
	QStringListModel* m_pStringListModel;
	bool m_flag = false;
	float m_usd = 0;
	float m_eur = 0;
};

#endif // EXECWINDOW_H
