#include "ZegoLogLabel.h"

ZegoLogLabel::ZegoLogLabel()
{
	this->setStyleSheet("font-family: Microsoft YaHei;\ncolor: #666666;\nfont-size: 14px;\nmargin: 0 0 0 0;");
}

ZegoLogLabel::~ZegoLogLabel()
{

}

void ZegoLogLabel::setTextContent(QString user, QString content)
{
	user += QStringLiteral("：");
	
	QString handleString = user + content;
	QString finalStringContent = handleChatStringContent(handleString);
	
	finalStringContent = finalStringContent.mid(user.size());
	
	finalStringContent = QString(QStringLiteral("<html> \
												<head> \
												<style> \
												font{color:#0e88eb;} #f{color: #333333;} \
												</style> \
												</head> \
												<body>\
												<font>%1</font><font id=\"f\">%2</font> \
												</body> \
												</html>")).arg(user).arg(finalStringContent);

	this->setText(finalStringContent);
	this->setFixedSize(QSize(300, 20 * this->getHeightNum()));
}

QString ZegoLogLabel::handleChatStringContent(QString content)
{
	QString str, tmp;
	QLabel *label = new QLabel;
	heightNum = 0;
	label->setStyleSheet("font-family: Microsoft YaHei;\ncolor: #666666;\nfont-size: 14px;");
	for (int i = 0; i < content.size(); i++)
	{

		label->setText(tmp);
		label->adjustSize();
		if (label->width() >= 288)
		{
			str += "<br>";
			tmp.clear();
			heightNum++;
		}
		if (content.at(i) == '\n')
		{
			str += "<br>";
			tmp.clear();
			heightNum++;
		}
		else
		{
			str.append(content.at(i));
			tmp.append(content.at(i));
		}
	}

	
	heightNum++;

	delete label;
	label = nullptr;
	return str;
}

int ZegoLogLabel::getHeightNum()
{
	return heightNum;
}