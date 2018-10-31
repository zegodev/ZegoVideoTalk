#pragma execution_character_set("utf-8")

#ifndef ZEGOSETTINGSDIALOG_H
#define ZEGOSETTINGSDIALOG_H

#include <QtWidgets/QDialog>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMetaType>
#include "ui_ZegoSettingsDialog.h"
#include "Base/ZegoDialog.h"
#include "Config/ZegoUserConfig.h"
#include "ZegoVideoTalkDemo.h"
#include "Model/ZegoSettingsModel.h"


class ZegoSettingsDialog : public ZegoDialog
{
	Q_OBJECT

public:
	ZegoSettingsDialog(QWidget *parent = 0);
	~ZegoSettingsDialog();
	void initDialog();

signals:
	void sigChangedSettingsConfig();
	void sigReturnConfigToMainDialog(QZegoUserConfig userConfig);

private slots:
    //Button槽
	void on_m_bClose_clicked();
	void on_m_bMin_clicked();
	void on_m_bSaveSettings_clicked();
	void on_m_bUploadLog_clicked();
	void on_m_switchTestEnv_clicked();
	//void OnButtonSwtichSurfaceMerge();
	//Slider槽
	void OnCheckSliderPressed();
	void OnCheckSliderReleased();
	void OnSliderValueChange(int value);
	void OnButtonSliderValueChange();
	//全局槽
	void OnChangedSettingsConfig();
	void OnChangedSettingsConfigAndReinstallSDK();

	//ComboBox槽
	void OnComboBoxCheckVideoQuality(int id);
	void OnComboBoxCheckAppVersion(int id);

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);

private:
	void initButtonIcon();
	void setDefalutVideoQuality();
	QVector<QString> handleAppSign(QString appSign);
	void copySettings(SettingsPtr dst, SettingsPtr src);

private:
	Ui::ZegoSettingsDialog ui;

	//处理是否保存了设置的逻辑
	bool m_isConfigChanged = false;
	bool m_isSaveConfig = false;

	//用户配置
	//QZegoUserConfig m_userConfig;
	QString m_strEdUserId;
	QString m_strEdUserName;
	//直播属性为UDP,RTMP,国际版或自定义（0,1,2,3）
	int m_versionMode = Version::ZEGO_PROTOCOL_UDP;

	//VideoQuality
	QVector<QString> m_vecResolution;
	QVector<QString> m_vecBitrate;
	QVector<QString> m_vecFPS;

	//Slider状态
	bool m_sliderPressed = false;
	//视频质量ComboBox状态
	bool m_isVideoCustom = false;
	//是否使用surfaceMerge
	//bool m_isUseSurfaceMerge;
	//是否使用测试环境，默认不使用
	bool m_isUseTestEnv;
	//当前配置参数
	SettingsPtr m_pCurSettings;
	//用户更改配置后是否需要重新InitSDK，默认否
	bool m_isNeedReInstallSDK = false;

	//暂时保存当前设置的App版本(不一定保存)
	int m_tmpVersionMode;
	//暂时保存切换测试环境(不一定保存)
	bool m_tmpUseTestEnv = false;
	//暂时保存切换SurfaceMerge(不一定保存)
	bool m_tmpUseSurfaceMerge = false;
	//暂时保存VideoSettings(不一定保存)
	SettingsPtr m_tmpCurSettings;

	bool m_isCustomAppTextChanged = false;
};

#endif // ZEGOSETTINGSDIALOG_H