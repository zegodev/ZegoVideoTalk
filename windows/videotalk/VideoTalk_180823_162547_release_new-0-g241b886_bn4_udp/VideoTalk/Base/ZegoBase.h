#pragma once

#include <QObject>
#include "Model/ZegoSettingsModel.h"
#include "Signal/ZegoSDKSignal.h"
#include "IncludeZegoLiveRoomApi.h"
#include "ZegoVideoTalkDefines.h"

class QZegoBase
{
public :
	QZegoBase();
	~QZegoBase();

	bool InitAVSDK(SettingsPtr pCurSetting, QString userID, QString userName);
	void UninitAVSDK(void);
	bool InitAVSDKofCustom(SettingsPtr pCurSetting, QString userID, QString userName, unsigned long appID, unsigned char *appSign, int signLen);
	bool IsAVSdkInited(void);

	void setTestEnv(bool isTest);
	bool getUseTestEnv();

	QZegoAVSignal* GetAVSignal(void);
	unsigned long GetAppID(void);
	unsigned long GetAppIDwithKey(int key);
	unsigned char* GetAppSign();
	void setKey(int pKey);
	int getKey();

	void setCustomAppID(unsigned long appid);
	void setCustomAppSign(unsigned char *appsign);
private :
	//SDK是否已经初始化
	bool m_sdkInited = false;
	//是否为测试环境
	bool m_isTestEnv = false;

	//appid、appsign默认为UDP版本
	int key = Version::ZEGO_PROTOCOL_UDP;

	QVector <unsigned int> appIDs;
	QVector <unsigned char *> appSigns;

	QZegoAVSignal* m_pAVSignal;
	
};