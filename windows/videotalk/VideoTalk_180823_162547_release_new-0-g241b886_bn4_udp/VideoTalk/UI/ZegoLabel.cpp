#include "ZegoLabel.h"
#include <QDebug>
ZegoLabel::ZegoLabel(QWidget * parent) : QLabel(parent)
{

}

ZegoLabel::~ZegoLabel()
{
	
}

void ZegoLabel::setButtonIcon(const QString& path)
{
	QString icon_path = path;
	if (devicePixelRatio() >= 2.0)
		icon_path += "_2x";

	icon_path = QStringLiteral(":/%1").arg(icon_path);

	QPixmap map(icon_path);

	this->setPixmap(map);
	this->setScaledContents(true);
}