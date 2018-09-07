#include "ZegoAVView.h"
#include <QDebug>

QZegoAVView::QZegoAVView(QWidget * parent) : QGraphicsView(parent)
{
	//quality = new QLabel(this);
	//scene = new QZegoAVScene(this);
	//scene->setSceneRect(0, 0, dynamic_cast<QZegoAVView *>(scene->parent())->size().width(), dynamic_cast<QZegoAVView *>(scene->parent())->size().height());
    //setScene(scene);

	m_nAVQuality = -1;
	this->setUpdatesEnabled(false);
}

QZegoAVView::~QZegoAVView()
{

}

void QZegoAVView::setCurrentQuality(int quality)
{
	m_nAVQuality = quality;
}

int QZegoAVView::getCurrentQuality()
{
	return m_nAVQuality;
}

void QZegoAVView::setViewIndex(int index)
{
	viewIndex = index;
}

int QZegoAVView::getViewIndex()
{
	return viewIndex;
}

/*void QZegoAVView::resizeEvent(QResizeEvent *event)
{
	scene->setSceneRect(0 , 0 , dynamic_cast<QZegoAVView *>(scene->parent())->size().width() , dynamic_cast<QZegoAVView *>(scene->parent())->size().height());
}*/

/*void QZegoAVView::paintEvent(QPaintEvent *event)
{
	QGraphicsView::paintEvent(event);
	
	QPainter painter(this->viewport());
	painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
	
	QColor color;
	switch (m_nAVQuality)
	{
	case 0:  //优
		color.setRed(0);
		color.setGreen(255);
		color.setBlue(0);
		painter.setPen(QPen(color, 0, Qt::SolidLine));
		painter.setBrush(QBrush(color, Qt::SolidPattern));

		painter.drawEllipse(10, 10, 8, 8);
		break;
	case 1:  //良
		color.setRed(255);
		color.setGreen(255);
		color.setBlue(0);
		painter.setPen(QPen(color, 0, Qt::SolidLine));
		painter.setBrush(QBrush(color, Qt::SolidPattern));

		painter.drawEllipse(10, 10, 8, 8);
		break;
	case 2:  //中
		color.setRed(255);
		color.setGreen(0);
		color.setBlue(0);
		painter.setPen(QPen(color, 0, Qt::SolidLine));
		painter.setBrush(QBrush(color, Qt::SolidPattern));

		painter.drawEllipse(10, 10, 8, 8);
		break;
	case 3:  //差
		color.setRed(211);
		color.setGreen(211);
		color.setBlue(211);
		painter.setPen(QPen(color, 0, Qt::SolidLine));
		painter.setBrush(QBrush(color, Qt::SolidPattern));

		painter.drawEllipse(10, 10, 8, 8);
		break;
	default:
		break;
	}
}*/

QZegoAVScene::QZegoAVScene(QWidget * parent)
	: QGraphicsScene(parent)
{
	
}

QZegoAVScene::~QZegoAVScene()
{

}
