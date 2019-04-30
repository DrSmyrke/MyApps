#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QWidget>
#include <QMouseEvent>

struct Ramka{
	unsigned long width;
	unsigned long height;
	long x;
	long y;
};

class ImageEditor : public QWidget
{
	Q_OBJECT
public:
	explicit ImageEditor(QWidget *parent = 0);
	void setImage(const QString &file);
	QImage getImage();
	void setNewSize(const QSize &size);
	QSize getImageSize() { return m_img.size(); }
protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
private:
	QImage m_img;
	Ramka m_ramka;
	bool m_ramkaMove=false;
	QPoint m_pressPos;
	QSize m_resizeBox;
	int m_mx;
	int m_my;
	bool m_resizeLeftTop=false;
	bool m_resizeLeftBottom=false;
	bool m_resizeRightBottom=false;
	bool m_resizeRightTop=false;
	float m_r;

	void chkResise();
};

#endif // IMAGEEDITOR_H
