#pragma once

#include <QPainter>
#include <QColor>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#define USE_SURFACE_MERGE

class QZegoAVScene;
class QZegoAVView : public QGraphicsView
{
	Q_OBJECT
public:
	QZegoAVView(QWidget * parent = 0);
	~QZegoAVView();

	void setCurrentQuality(int quality);
	int getCurrentQuality();

	void setViewIndex(int index);
	int getViewIndex();

protected:
	//virtual void paintEvent(QPaintEvent *event);
	//virtual void resizeEvent(QResizeEvent *event);

private:
	//QZegoAVScene *scene;
	int m_nAVQuality;
	bool isSurfaceMergeView = false;
	int viewIndex;

	//QLabel *quality;
};

class QZegoAVScene : public QGraphicsScene
{
	Q_OBJECT

public:
	QZegoAVScene(QWidget * parent = 0);
	~QZegoAVScene();

};