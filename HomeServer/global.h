#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>

class global : public QObject
{
	Q_OBJECT
public:
	explicit global(QObject *parent = 0);

signals:

public slots:

};

#endif // GLOBAL_H
