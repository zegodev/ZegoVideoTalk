#pragma once

#include <QLabel>
#include <QPixmap>

class ZegoLabel : public QLabel
{
public:
	 ZegoLabel(QWidget * parent);
    ~ZegoLabel();

	void setButtonIcon(const QString& path);

private:

};