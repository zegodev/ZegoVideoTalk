#include "ZegoBase.h"

/*
#warning 请开发者联系 ZEGO support 获取各自业务的 AppID 与 signKey
#warning Demo 默认使用 UDP 模式，请填充该模式下的 AppID 与 signKey,其他模式不需要可不用填
#warning AppID 填写样式示例：1234567890
#warning signKey 填写样式示例：{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
								  0x08,0x09,0x00,0x01,0x02,0x03,0x04,0x05,
								  0x06,0x07,0x08,0x09,0x00,0x01,0x02,0x03,
								  0x04,0x05,0x06,0x07,0x08,0x09,0x00,0x01}
*/
static unsigned long g_dwAppID_Udp = ;
static unsigned char g_bufSignKey_Udp[] = ;

static unsigned long g_dwAppID_International = 0;
static unsigned char g_bufSignKey_International[] =
{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

static unsigned long  g_dwAppID_Empty = 0;
static unsigned char g_bufSignKey_Empty[] =
{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

QZegoBase::QZegoBase(void)
{
	appIDs.push_back(g_dwAppID_Udp);
	appIDs.push_back(g_dwAppID_International);
	appIDs.push_back(g_dwAppID_Empty);

	appSigns.push_back(g_bufSignKey_Udp);
	appSigns.push_back(g_bufSignKey_International);
	appSigns.push_back(g_bufSignKey_Empty);

	m_pAVSignal = new QZegoAVSignal;

}

QZegoBase::~QZegoBase(void)
{
	UninitAVSDK();
	delete m_pAVSignal;
}

bool QZegoBase::InitAVSDK(SettingsPtr pCurSetting, QString userID, QString userName)
{
	if (!IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Init").arg(__FUNCTION__)));

		//Qstring对象.toLocal8Bit().data()用于将QString转为const char*
		LIVEROOM::SetLogDir(nullptr);
		LIVEROOM::SetVerbose(true);
		//LIVEROOM::SetBusinessType(2);
		LIVEROOM::SetUser(qtoc(userID), qtoc(userName));
		// ToDo: 需要通过代码获取网络类型
		LIVEROOM::SetNetType(2);

		//是否使用测试环境
		LIVEROOM::SetUseTestEnv(m_isTestEnv);

		//设置回调
		LIVEROOM::SetLivePublisherCallback(m_pAVSignal);
		LIVEROOM::SetLivePlayerCallback(m_pAVSignal);
		LIVEROOM::SetRoomCallback(m_pAVSignal);
		LIVEROOM::SetIMCallback(m_pAVSignal);
		LIVEROOM::SetDeviceStateCallback(m_pAVSignal);
		LIVEROOM::SetLiveEventCallback(m_pAVSignal);
		SOUNDLEVEL::SetSoundLevelCallback(m_pAVSignal);

		LIVEROOM::InitSDK(appIDs[key], appSigns[key], 32);
	}

	//为了调用OnUserUpdate
	LIVEROOM::SetRoomConfig(true, true);
	//实时视频低延迟模式
	LIVEROOM::SetLatencyMode(AV::ZegoAVAPILatencyMode::ZEGO_LATENCY_MODE_LOW3);

	m_sdkInited = true;
	return true;
}

bool QZegoBase::InitAVSDKofCustom(SettingsPtr pCurSetting, QString userID, QString userName, unsigned long appID, unsigned char *appSign, int signLen)
{
	if (!IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Init Custom.").arg(__FUNCTION__)));

		//Qstring对象.toLocal8Bit().data()用于将QString转为const char*
		LIVEROOM::SetLogDir(nullptr);
		LIVEROOM::SetVerbose(true);
		//LIVEROOM::SetBusinessType(2);
		LIVEROOM::SetUser(qtoc(userID), qtoc(userName));
		// ToDo: 需要通过代码获取网络类型
		LIVEROOM::SetNetType(2);

		//是否使用测试环境
		LIVEROOM::SetUseTestEnv(m_isTestEnv);

		//设置回调
		LIVEROOM::SetLivePublisherCallback(m_pAVSignal);
		LIVEROOM::SetLivePlayerCallback(m_pAVSignal);
		LIVEROOM::SetRoomCallback(m_pAVSignal);
		LIVEROOM::SetIMCallback(m_pAVSignal);
		LIVEROOM::SetDeviceStateCallback(m_pAVSignal);
		LIVEROOM::SetLiveEventCallback(m_pAVSignal);
		SOUNDLEVEL::SetSoundLevelCallback(m_pAVSignal);

		LIVEROOM::InitSDK(appID, appSign, signLen);
	}

	//为了调用OnUserUpdate
	LIVEROOM::SetRoomConfig(true, true);
	//实时视频低延迟模式
	LIVEROOM::SetLatencyMode(AV::ZegoAVAPILatencyMode::ZEGO_LATENCY_MODE_LOW3);

	m_sdkInited = true;
	return true;
}

void QZegoBase::UninitAVSDK(void)
{
	if (IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Uninit").arg(__FUNCTION__)));

		LIVEROOM::SetLivePublisherCallback(nullptr);
		LIVEROOM::SetLivePlayerCallback(nullptr);
		LIVEROOM::SetRoomCallback(nullptr);
		//LIVEROOM::SetIMCallback(nullptr);
		LIVEROOM::SetDeviceStateCallback(nullptr);
		LIVEROOM::SetLiveEventCallback(nullptr);
		SOUNDLEVEL::SetSoundLevelCallback(nullptr);

		LIVEROOM::UnInitSDK();

		m_sdkInited = false;
	}
}

bool QZegoBase::IsAVSdkInited(void)
{
	return m_sdkInited;
}

QZegoAVSignal* QZegoBase::GetAVSignal(void)
{
	return m_pAVSignal;
}

unsigned long QZegoBase::GetAppID(void)
{
	return appIDs[key];
}

unsigned long QZegoBase::GetAppIDwithKey(int tmpKey)
{
	return appIDs[tmpKey];
}

unsigned char* QZegoBase::GetAppSign()
{
	return appSigns[key];
}

void QZegoBase::setKey(int pKey)
{
	key = pKey;

}

int QZegoBase::getKey()
{
	return key;
}

void QZegoBase::setTestEnv(bool isTest)
{
	m_isTestEnv = isTest;
}

bool QZegoBase::getUseTestEnv()
{
	return m_isTestEnv;
}

void QZegoBase::setCustomAppID(unsigned long appid)
{
	appIDs[2] = appid;
}

void QZegoBase::setCustomAppSign(unsigned char *appsign)
{
	for (int i = 0; i < 32; i++)
	{
		appSigns[2][i] = appsign[i];
	}
}
