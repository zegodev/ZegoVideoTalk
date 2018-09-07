#pragma execution_character_set("utf-8")

#ifndef ZEGOVIDEOTALKDIALOG_H
#define ZEGOVIDEOTALKDIALOG_H

#include <QtWidgets/QDialog>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QStringListModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include "ui_ZegoVideoTalkDialog.h"

#include "Config/ZegoUserConfig.h"
#include "Base/ZegoDialog.h"
#include "Delegate/NoFocusFrameDelegate.h"
#include "Model/ZegoSettingsModel.h"
#include "Model/ZegoRoomModel.h"
#include "Device/ZegoDeviceManager.h"
#include "ZegoVideoTalkDemo.h"
#include "ZegoLogLabel.h"
#include "ZegoAVView.h"

#define MAX_VIEW_COUNT 12

class ZegoVideoTalkDialog : public ZegoDialog
{
	Q_OBJECT

public:
	ZegoVideoTalkDialog(RoomPtr chatRoom, QDialog *lastDialog, bool isVideoCustom, QWidget *parent = 0);
	~ZegoVideoTalkDialog();
	void initDialog();

	//sdk回调槽
protected slots:
	void OnLoginRoom(int errorCode, const QString& roomId, QVector<StreamPtr> vStreamList);
	void OnStreamUpdated(const QString& roomId, QVector<StreamPtr> vStreamList, LIVEROOM::ZegoStreamUpdateType type);
	void OnPublishStateUpdate(int stateCode, const QString& streamId, StreamPtr streamInfo);
	void OnPlayStateUpdate(int stateCode, const QString& streamId);
	void OnUserUpdate(QVector<QString> userIDs, QVector<QString> userNames, QVector<int> userFlags, QVector<int> userRoles, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type);
	void OnDisconnect(int errorCode, const QString& roomId);
	void OnKickOut(int reason, const QString& roomId);
	void OnPlayQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS);
	void OnPublishQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS);
	void OnAVAuxInput(unsigned char *pData, int* pDataLen, int pDataLenValue, int *pSampleRate, int *pNumChannels);
	void OnCaptureSoundLevelUpdate(const QString& streamId, float soundlevel);
	void OnAVKitEvent(int event);

	//UI回调槽
private slots:
	//Button槽
	void on_m_bClose_clicked();
	void on_m_bMax_clicked();
	void on_m_bMin_clicked();
	void on_m_bProgMircoPhone_clicked();
	void on_m_bSpeaker_clicked();
	void on_m_bCamera_clicked();
	void on_m_bCapture_clicked();
	void on_m_bShowFullScreen_clicked();

	//ComboBox槽
	void OnSwitchAudioDevice(int id);
	void OnSwitchVideoDevice(int id);

private slots:
	void OnMicIdChanged(const QString& deviceId);
	void OnCameraIdChanged(const QString& deviceId);
	void OnDeviceAdded(int type, QString deviceName);
	void OnDeviceDeleted(int type, int index);

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual bool eventFilter(QObject *target, QEvent *event);

private:
	//推拉流
	void StartPublishStream();
	void StopPublishStream(const QString& streamID);
	void StartPlayStream(StreamPtr stream);
	void StopPlayStream(const QString& streamID);

	//初始化
	void initButtonIcon();
	void initComboBox();
	void GetAudioAndVideoList();
	//操作model增删的函数
	void insertStringListModelItem(QStringListModel * model, QString name, int size);
	void removeStringListModelItemByName(QStringListModel * model, QString name);
	void removeStringListModelItemByIndex(QStringListModel * model, int index);
	//退出前清理
	void cleanBeforeGetOut();
	//view布局函数
	int takeLeastAvaliableViewIndex();
	void initAVView(QZegoAVView *view);
	void addAVView(int addViewIndex);
	void removeAVView(int removeViewIndex);
	void updateViewLayout(int viewCount);
	void FreeAVView(StreamPtr stream);
	//混音
	//void BeginDefaultAux();
	//void EndAux();
	//成员列表增删函数
	void roomMemberAdd(QString userName);
	void roomMemberDelete(QString userName);
	//打印日志
	void addLogString(QString log);

private:
	Ui::ZegoVideoTalkDialog ui;
	//当前房间号
	QString m_roomID;

	bool m_isMax = false;
	QVector<unsigned int> m_avaliableView;

	bool m_bCKEnableMic = true;
	bool m_bCKEnableSpeaker = true;
	bool m_bCKEnableCamera = true;

	//bool m_isUseDefaultAux = false;
	bool m_bIsPublishing = false;
	bool m_isVideoCustom = false;
	bool m_isLiveFullScreen = false;
	SettingsPtr m_pAVSettings;
	RoomPtr m_pChatRoom;

	QString m_strPublishStreamID;
	QString m_strCurUserID;
	QString m_strCurUserName;

	QVector<QDeviceInfo> m_vecAudioDeviceList;
	QVector<QDeviceInfo> m_vecVideoDeviceList;

	QVector<QZegoAVView *> AVViews;

	//List
	QStringList m_MircoPhoneList;
	QStringList m_CameraList;

	//Model
	QStringListModel *m_cbMircoPhoneModel;
	QStringListModel *m_cbCameraModel;
	QStringListModel *m_memberModel;
	QStandardItemModel *m_logModel;

	//自定义的ComboBox下拉式页面
	QListView *m_cbMircoPhoneListView;
	QListView *m_cbCameraListView;

	//保存上一个界面的指针，用于退出该页面时显示它
	QDialog *m_lastDialog;

	//view的网格动态布局
	QGridLayout *gridLayout;

	//保留自己的流信息
	StreamPtr m_anchorStreamInfo;

	//设备管理器对象
	ZegoDeviceManager *m_device;
};

#endif // ZEGOVIDEOTALKDIALOG_H