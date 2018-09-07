#pragma execution_character_set("utf-8")

#ifndef ZEGOMAINDIALOG_H
#define ZEGOMAINDIALOG_H

#include <QtWidgets/QDialog>
#include <QDesktopServices>
#include "ui_ZegoMainDialog.h"
#include "ZegoSettingsDialog.h"
#include "ZegoVideoTalkDialog.h"
#include "Base/ZegoDialog.h"

class ZegoMainDialog : public ZegoDialog
{
	Q_OBJECT

public:
	ZegoMainDialog(QWidget *parent = 0);
	~ZegoMainDialog();
	void initDialog();

signals:
	void sigCheckEnterRoom();

private slots:
    void on_m_bEnterRoom_clicked();
    void on_m_bSettings_clicked();
	void on_m_bClose_clicked();
	void on_m_bMin_clicked();
	void on_m_bJumpToNet_clicked();

	void OnCheckEnterRoom();
	//void OnReturnConfigFromSettingsDialog(QZegoUserConfig userConfig);

private:
	void initButtonIcon();
	void updateAppVersionTitle();
	QVector<QString> handleAppSign(QString appSign);

protected:
	virtual bool eventFilter(QObject *target, QEvent *event);

private:
	Ui::ZegoMainDialog ui;

	//是否使用SurfaceMerge
	bool m_isUseSurfaceMerge = false;
	
	//用户ID和用户名
	QString m_strEdUserId;
	QString m_strEdUserName;

	bool m_isVideoCustom = false;
};

#endif // ZEGOMAINDIALOG_H
