#pragma once
#include <QLabel>

class ZegoLogLabel : public QLabel
{
	Q_OBJECT
public:
	ZegoLogLabel();
	~ZegoLogLabel();

	void setTextContent(QString user, QString content);
	QString handleChatStringContent(QString content);
	int getHeightNum();

private:
	int heightNum;
};