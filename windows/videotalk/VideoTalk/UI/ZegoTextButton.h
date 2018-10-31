#pragma once
#pragma execution_character_set("utf-8")

#include <QPushButton>

enum TextButtonType
{
	TEXT_NORNAL = 0,
	TEXT_CHECKABLE = 1
};

enum TextButtonColor
{
	COLOR_BLUE = 0,
	COLOR_YELLOW = 1,
	COLOR_RED
};

class ZegoTextButton : public QPushButton
{
	Q_OBJECT

public:
	ZegoTextButton(QWidget  * parent = 0);
	~ZegoTextButton();

	void SetTextButtonTpye(TextButtonType type);
	void SetTextButtonTitle(const QString& name);
	void SetCustomButtonStyle(const QString& style);
	void SetTextButtonColorType(TextButtonColor color);

private:
	QString m_bName;
	TextButtonType m_type;
};