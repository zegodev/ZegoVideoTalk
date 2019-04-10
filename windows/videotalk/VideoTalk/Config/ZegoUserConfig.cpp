#include "ZegoUserConfig.h"
#include <random>
#include <QSharedPointer>

QZegoUserConfig::QZegoUserConfig()
{
    //生成ini文件,用于在本地保存用户配置信息
	m_strIniPath =  QStringLiteral("Config/ZegoUserConfig.ini");

}

QZegoUserConfig::~QZegoUserConfig()
{

}

void QZegoUserConfig::LoadConfig(void)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: load config enter.").arg(__FUNCTION__)));

	if (LoadConfigInternal())
	{
		return;
	}

	//随机生成编号为10000000-99999999的用户ID
	std::random_device rd;
	std::uniform_int_distribution<int> dist(10000000, 99999999);
	//int to QString
	m_strUserId = QString::number(dist(rd), 10);
#ifdef Q_OS_WIN32
	m_strUserName = QStringLiteral("windows-vt-") + m_strUserId;
#else
	m_strUserName = QStringLiteral("mac-vt-") + m_strUserId;
#endif

	m_isUseTestEnv = true;

	m_appVersion.m_versionMode = ZEGO_PROTOCOL_UDP;
	m_appVersion.m_strAppID = 0;
	m_appVersion.m_strAppSign = "";

	if (m_pVideoSettings == nullptr)
	{
		m_pVideoSettings = SettingsPtr::create();
	}

	m_pVideoSettings->SetQuality(true, VQ_Middle);

	log_string_notice(qtoc(QStringLiteral("[%1]: new user. user id: %1, user name: %2").arg(__FUNCTION__).arg(m_strUserId).arg(m_strUserName)));

	SaveConfig();
}

bool QZegoUserConfig::LoadConfigInternal(void)
{
	QSettings *configIni = new QSettings(m_strIniPath, QSettings::IniFormat);
	if (configIni == nullptr)
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: load user failed. config file is not exists.")));
		return false;
	}

	QString strUserId = configIni->value("/sUserRecords/kUserId").toString();
	QString strUserName = configIni->value("/sUserRecords/kUserName").toString();

	
	if (strUserId.isEmpty() || strUserName.isEmpty())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: load user failed. user id or user name is empty.")));
		return false;
	}

	Size sizeResolution;
	sizeResolution.cx = configIni->value("/sUserRecords/kResolutionX").toLongLong();
	sizeResolution.cy = configIni->value("/sUserRecords/kResolutionY").toLongLong();
	int nBitrate = configIni->value("/sUserRecords/kBitrate").toInt();
	int nFps = configIni->value("/sUserRecords/kFps").toInt();

	bool nIsUseTest = configIni->value("/sUserRecords/kIsTestEnv").toBool();

	int nAppVer = configIni->value("/sUserRecords/kAppVersion").toInt();
	unsigned int nAppId = configIni->value("/sUserRecords/kAppId").toLongLong();
	QString nAppSign = configIni->value("/sUserRecords/kAppSign").toString();

	//读ini文件完毕后释放指针
	delete configIni;

	if (sizeResolution.cx == 0 || sizeResolution.cy == 0 || nBitrate == 0 || nFps == 0)
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: load user failed. video quality prams incorrect.")));
		return false;
	}

	m_strUserId = strUserId;
	m_strUserName = strUserName;
	
	m_isUseTestEnv = nIsUseTest;

	m_appVersion.m_versionMode = nAppVer;
	m_appVersion.m_strAppID = nAppId;
	m_appVersion.m_strAppSign = nAppSign;

	if (m_pVideoSettings == nullptr)
	{
		m_pVideoSettings = SettingsPtr::create();
	}
	m_pVideoSettings->SetResolution(sizeResolution);
	m_pVideoSettings->SetBitrate(nBitrate);
	m_pVideoSettings->SetFps(nFps);

	log_string_notice(qtoc(QStringLiteral("[%1]: load user success. user id: %1, user name: %2")
		.arg(__FUNCTION__)
		.arg(strUserId)
		.arg(strUserName)
	));

	return true;
}
void QZegoUserConfig::SaveConfig()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: save config enter.").arg(__FUNCTION__)));

	QSettings *configIni = new QSettings(m_strIniPath, QSettings::IniFormat);
	if (m_strUserId.isEmpty() || m_strUserName.isEmpty() || m_pVideoSettings == nullptr)
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: save config error. user config or video quality prams incorrect").arg(__FUNCTION__)));
		return;
	}

	configIni->setValue("/sUserRecords/kUserId", m_strUserId);
	configIni->setValue("/sUserRecords/kUserName", m_strUserName);

	configIni->setValue("/sUserRecords/kResolutionX", m_pVideoSettings->GetResolution().cx);
	configIni->setValue("/sUserRecords/kResolutionY", m_pVideoSettings->GetResolution().cy);
	configIni->setValue("/sUserRecords/kBitrate", m_pVideoSettings->GetBitrate());
	configIni->setValue("/sUserRecords/kFps", m_pVideoSettings->GetFps());

	configIni->setValue("/sUserRecords/kIsTestEnv", m_isUseTestEnv);

	configIni->setValue("/sUserRecords/kAppVersion", m_appVersion.m_versionMode);

	if (m_appVersion.m_versionMode == ZEGO_PROTOCOL_CUSTOM)
	{
		configIni->setValue("/sUserRecords/kAppId", (qlonglong)m_appVersion.m_strAppID);
		configIni->setValue("/sUserRecords/kAppSign", m_appVersion.m_strAppSign);
	}

	delete configIni;

	log_string_notice(qtoc(QStringLiteral("[%1]: save user config success.").arg(__FUNCTION__)));
}

QString QZegoUserConfig::GetUserId(void)
{
	return m_strUserId;
}

void QZegoUserConfig::SetUserId(QString strUserId)
{
	if (!strUserId.isEmpty())
	{
		m_strUserId = strUserId;
	}
}

QString QZegoUserConfig::getUserName(void)
{
	return m_strUserName;
}

void QZegoUserConfig::SetUserName(QString strUserName)
{
	if (!strUserName.isEmpty())
	{
		m_strUserName = strUserName;
	}
}

VideoQuality QZegoUserConfig::GetVideoQuality(void)
{
	if (m_pVideoSettings != nullptr)
	{
		return m_pVideoSettings->GetQuality(true);
	}
	return VQ_SelfDef;
}

void QZegoUserConfig::SetVideoQuality(VideoQuality quality)
{
	if (m_pVideoSettings != nullptr)
	{
		m_pVideoSettings->SetQuality(true, quality);
	}
}

SettingsPtr QZegoUserConfig::GetVideoSettings(void)
{
	return m_pVideoSettings;
}

void QZegoUserConfig::SetVideoSettings(SettingsPtr curSettings)
{
	m_pVideoSettings->SetResolution(curSettings->GetResolution());
	m_pVideoSettings->SetBitrate(curSettings->GetBitrate());
	m_pVideoSettings->SetFps(curSettings->GetFps());
	m_pVideoSettings->SetMicrophoneId(curSettings->GetMircophoneId());
	m_pVideoSettings->SetCameraId(curSettings->GetCameraId());
}

void QZegoUserConfig::setAppVersion(AppVersion appVersion)
{
	m_appVersion = appVersion;
}

AppVersion QZegoUserConfig::getAppVersion(void)
{
	return m_appVersion;
}

bool QZegoUserConfig::GetUseTestEnv()
{
	return m_isUseTestEnv;
}

void QZegoUserConfig::SetUseTestEnv(bool isUseTestEnv)
{
	m_isUseTestEnv = isUseTestEnv;
}



