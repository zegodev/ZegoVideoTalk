#pragma execution_character_set("utf-8")
#pragma once

#include <QtWidgets/QDialog>
#include <QShowEvent>
#include "frameless_helper.h"

class ZegoDialog : public QDialog
{
	Q_OBJECT

public:
	ZegoDialog(QWidget *parent);
	~ZegoDialog();

	void SetWidgetBorderless(const QWidget *widget);

protected:
	virtual void showEvent(QShowEvent *event);

private:
	FramelessHelper *m_pHelper = nullptr;

};