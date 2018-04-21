#ifndef EXECWINDOW_H
#define EXECWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include "global.h"
#include "wget.h"

enum{
	wget_operation_getKotirovki = 1,
};

class ExecWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit ExecWindow(QWidget *parent = 0);
	void open();
signals:
	void signal_start(const QString &cmd, const QStringList &args);
private slots:
	void slot_textChanged(const QString &text);
	void slot_highlighted(const QString &text) { m_flag = true;Q_UNUSED(text); }
	void slot_returnPressed();
private:
	QCompleter* m_pCompleter;
	QLineEdit* m_pExecLine;
	QStringListModel* m_pStringListModel;
	Wget* m_pWget;
	bool m_flag = false;
	float m_usd = 0;
	float m_eur = 0;
	uint8_t m_wgetOperation;

	void resizeMonitor();
	void updateValuta();
	void parsingValuta(const QString &data);
};

#endif // EXECWINDOW_H
