#include "ZegoTextButton.h"

ZegoTextButton::ZegoTextButton(QWidget  * parent) : QPushButton(parent)
{
	m_type = TEXT_NORNAL;
	this->setStyleSheet("QPushButton:!enabled{"
		"font-family: Î¢ÈíÑÅºÚ;"
		"font-size: 14px;"
		"border: 1px solid #cccccc;"
		"border-radius: 4px;"
		"background-color: #ffffff;"
		"color: #cccccc;}"
		"QPushButton:enabled:!hover{"
		"font-family: Î¢ÈíÑÅºÚ;"
		"font-size: 14px;"
		"border: 1px solid #0e88eb;"
		"border-radius: 4px;"
		"background-color: #ffffff;"
		"color: #0e88eb;}"
		"QPushButton:enabled:hover:!pressed{"
		"font-family: Î¢ÈíÑÅºÚ;"
		"font-size: 14px;"
		"border: 1px solid #0e88eb;"
		"border-radius: 4px;"
		"background-color: #0e88eb;"
		"color: #ffffff;}"
		"QPushButton:enabled:hover:pressed{"
		"font-family: Î¢ÈíÑÅºÚ;"
		"font-size: 14px;"
		"border: 1px solid #0d80de;"
		"border-radius: 4px;"
		"background-color: #0d80de;"
		"color: #ffffff;}");
}

ZegoTextButton::~ZegoTextButton()
{

}

void ZegoTextButton::SetTextButtonTitle(const QString& name)
{
	m_bName = name;
	this->setText(m_bName);
}

void ZegoTextButton::SetCustomButtonStyle(const QString& style)
{
	this->setStyleSheet(style);
}

void ZegoTextButton::SetTextButtonTpye(TextButtonType type)
{
	m_type = type;
	if (m_type == TEXT_NORNAL)
	{
		this->setStyleSheet("QPushButton:!enabled{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #cccccc;"
			"border-radius: 4px;"
			"background-color: #ffffff;"
			"color: #cccccc;}"
			"QPushButton:enabled:!hover{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0e88eb;"
			"border-radius: 4px;"
			"background-color: #ffffff;"
			"color: #0e88eb;}"
			"QPushButton:enabled:hover:!pressed{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0e88eb;"
			"border-radius: 4px;"
			"background-color: #0e88eb;"
			"color: #ffffff;}"
			"QPushButton:enabled:hover:pressed{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0d80de;"
			"border-radius: 4px;"
			"background-color: #0d80de;"
			"color: #ffffff;}");
	}
	else
	{
		this->setStyleSheet("QPushButton:!checked{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #cccccc;"
			"border-radius: 4px;"
			"background-color: #ffffff;"
			"color: #cccccc;}"
			"QPushButton:checked:!hover{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0e88eb;"
			"border-radius: 4px;"
			"background-color: #ffffff;"
			"color: #0e88eb;}"
			"QPushButton:checked:hover:!pressed{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0e88eb;"
			"border-radius: 4px;"
			"background-color: #0e88eb;"
			"color: #ffffff;}"
			"QPushButton:checked:hover:pressed{"
			"font-family: Î¢ÈíÑÅºÚ;"
			"font-size: 14px;"
			"border: 1px solid #0d80de;"
			"border-radius: 4px;"
			"background-color: #0d80de;"
			"color: #ffffff;}");
	}
}
