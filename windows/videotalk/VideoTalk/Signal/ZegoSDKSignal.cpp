#include "ZegoSDKSignal.h"

#include <QMetaType>
#include <QDebug>

QZegoAVSignal::QZegoAVSignal()
{
	qRegisterMetaType< QVector<StreamPtr> >("QVector<StreamPtr>");
	qRegisterMetaType<StreamPtr>("StreamPtr");
	qRegisterMetaType<LIVEROOM::ZegoStreamUpdateType>("LIVEROOM::ZegoStreamUpdateType");
	qRegisterMetaType<LIVEROOM::ZegoUserInfo>("LIVEROOM::ZegoUserInfo");
	qRegisterMetaType<LIVEROOM::ZegoUserUpdateType>("LIVEROOM::ZegoUserUpdateType");
	qRegisterMetaType< QVector<QString> >(" QVector<QString> ");
	qRegisterMetaType< QVector<int> >(" QVector<int> ");
	qRegisterMetaType<AV::ZegoMixStreamResult>("AV::ZegoMixStreamResult");
	qRegisterMetaType< AV::AudioDeviceType >("AV::AudioDeviceType");
	qRegisterMetaType< AV::DeviceState >("AV::DeviceState");
}

QZegoAVSignal::~QZegoAVSignal()
{

}

void QZegoAVSignal::OnLoginRoom(int errorCode, const char *pszRoomID, const LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: errorCode: %2, roomID: %3").arg(__FUNCTION__).arg(errorCode).arg(pszRoomID)));
	QString strRoomID = pszRoomID ? pszRoomID : "";

	QVector<StreamPtr> vStreamList;

	for (int i = 0; i < streamCount; i++)
	{
		LIVEROOM::ZegoStreamInfo zegoStreamInfo = pStreamInfo[i];
		StreamPtr pStream(new QZegoStreamModel(zegoStreamInfo.szStreamId, zegoStreamInfo.szUserId, zegoStreamInfo.szUserName, zegoStreamInfo.szExtraInfo));
		vStreamList.push_back(pStream);
	}

	emit sigLoginRoom(errorCode, strRoomID, vStreamList);
	
}

void QZegoAVSignal::OnLogoutRoom(int errorCode, const char *pszRoomID)
{
	QString strRoomID = pszRoomID ? pszRoomID : "";

	emit sigLogoutRoom(errorCode, strRoomID);
	
}

void QZegoAVSignal::OnDisconnect(int errorCode, const char *pszRoomID)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: errorCode: %2, roomID: %3").arg(__FUNCTION__).arg(errorCode)));
	QString strRoomID = pszRoomID ? pszRoomID : "";

	emit sigDisconnect(errorCode, strRoomID);
	
}

void QZegoAVSignal::OnKickOut(int reason, const char *pszRoomID)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: reason: %2, roomID: %3").arg(__FUNCTION__).arg(reason)));
	QString strRoomID = pszRoomID ? pszRoomID : "";

	emit sigKickOut(reason, strRoomID);
	
}

/*void QZegoAVSignal::OnSendRoomMessage(int errorCode, const char *pszRoomID, int sendSeq, unsigned long long messageId)
{
	QString strRoomID = pszRoomID ? pszRoomID : "";

	emit sigSendRoomMessage(errorCode, strRoomID, sendSeq, messageId);
	
}

void QZegoAVSignal::OnRecvRoomMessage(ROOM::ZegoRoomMessage *pMessageInfo, unsigned int messageCount, const char *pszRoomID)
{
	if (pMessageInfo == nullptr || messageCount == 0)
	{
		return;
	}

	QString strRoomID = pszRoomID ? pszRoomID : "";

	QVector<RoomMsgPtr> vRoomMsgList;
	for (int i = 0; i < messageCount; i++)
	{
		ROOM::ZegoRoomMessage zegoRoomMessage = pMessageInfo[i];
		RoomMsgPtr pRoomMsg(new QZegoRoomMsgModel(zegoRoomMessage.szUserId, zegoRoomMessage.szUserName,
			zegoRoomMessage.szContent, zegoRoomMessage.messageId, zegoRoomMessage.type, zegoRoomMessage.priority, zegoRoomMessage.category));
		vRoomMsgList.push_back(pRoomMsg);
	}

	emit sigRecvRoomMessage(strRoomID, vRoomMsgList);
	
}*/

void QZegoAVSignal::OnStreamUpdated(LIVEROOM::ZegoStreamUpdateType type, LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID)
{
	if (pStreamInfo == nullptr || streamCount == 0)
	{
		return;
	}

	QString strRoomID = pszRoomID ? pszRoomID : "";

	QVector<StreamPtr> vStreamList;
	for (int i = 0; i < streamCount; i++)
	{
		LIVEROOM::ZegoStreamInfo zegoStreamInfo = pStreamInfo[i];
		StreamPtr pStream(new QZegoStreamModel(zegoStreamInfo.szStreamId, zegoStreamInfo.szUserId, zegoStreamInfo.szUserName, zegoStreamInfo.szExtraInfo));
		vStreamList.push_back(pStream);
	}

	emit sigStreamUpdated(strRoomID, vStreamList, type);
	
}

void QZegoAVSignal::OnPublishStateUpdate(int stateCode, const char* pszStreamID, const LIVEROOM::ZegoPublishingStreamInfo& oStreamInfo)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: stateCode: %2, streamID: %3").arg(__FUNCTION__).arg(stateCode).arg(pszStreamID)));
	QString strStreamID = pszStreamID ? pszStreamID : "";

	StreamPtr pStream(new QZegoStreamModel(strStreamID, "", "", "", true));

	for (unsigned int i = 0; i < oStreamInfo.uiRtmpURLCount; i++)
	{
		pStream->m_vecRtmpUrls.push_back(oStreamInfo.arrRtmpURLs[i]);
	}

	for (unsigned int i = 0; i < oStreamInfo.uiFlvURLCount; i++)
	{
		pStream->m_vecFlvUrls.push_back(oStreamInfo.arrFlvURLs[i]);
	}

	for (unsigned int i = 0; i < oStreamInfo.uiHlsURLCount; i++)
	{
		pStream->m_vecHlsUrls.push_back(oStreamInfo.arrHlsURLs[i]);
	}

	emit sigPublishStateUpdate(stateCode, strStreamID, pStream);
	
}

void QZegoAVSignal::OnPlayStateUpdate(int stateCode, const char* pszStreamID)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: stateCode: %2, streamID: %3").arg(__FUNCTION__).arg(stateCode).arg(pszStreamID)));
	QString strStreamID = pszStreamID ? pszStreamID : "";

	emit sigPlayStateUpdate(stateCode, strStreamID);
	
}

void QZegoAVSignal::OnPublishQulityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS)
{
	QString strStreamID = pszStreamID ? pszStreamID : "";
	//去掉StreamId后面CDN的地址
	//int index = strStreamID.indexOf("?");
	//strStreamID = strStreamID.left(index);
	
	emit sigPublishQualityUpdate(strStreamID, quality, videoFPS, videoKBS);
	
}

void QZegoAVSignal::OnPlayQualityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS)
{
	QString strStreamID = pszStreamID ? pszStreamID : "";
	//去掉StreamId后面CDN的地址
	//int index = strStreamID.indexOf("?");
	//strStreamID = strStreamID.left(index);

	emit sigPlayQualityUpdate(strStreamID, quality, videoFPS, videoKBS);
	
}

void QZegoAVSignal::OnAuxCallback(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels)
{
	int pDataLenValue = *pDataLen;
	//qDebug() << "pdataLen = " << *pDataLen;
	emit sigAuxInput(pData, pDataLen, pDataLenValue, pSampleRate, pNumChannels);
}

void QZegoAVSignal::OnJoinLiveRequest(int seq, const char *pszFromUserId, const char *pszFromUserName, const char *pszRoomID)
{
	QString strFromUserID = pszFromUserId ? pszFromUserId : "";
	QString strFromUserName = pszFromUserName ? pszFromUserName : "";
	QString strRoomID = pszRoomID ? pszRoomID : "";

	emit sigJoinLiveRequest(seq, strFromUserID, strFromUserName, strRoomID);

}

void QZegoAVSignal::OnJoinLiveResponse(int result, const char* pszFromUserId, const char* pszFromUserName, int seq)
{
	QString strFromUserID = pszFromUserId ? pszFromUserId : "";
	QString strFromUserName = pszFromUserName ? pszFromUserName : "";

    emit sigJoinLiveResponse(result, strFromUserID, strFromUserName, seq);
	
}

void QZegoAVSignal::OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, AV::DeviceInfo *deviceInfo, AV::DeviceState state)
{
	if (deviceInfo == nullptr)
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: deviceType: %2, deviceID: null, deviceName: null, deviceState: %3")
			.arg(__FUNCTION__)
			.arg(deviceType)
			.arg(state)
		));

		return;
	}

	QString strDeviceId = deviceInfo->szDeviceId;
	QString strDeviceName = deviceInfo->szDeviceName;

	log_string_notice(qtoc(QStringLiteral("[%1]: deviceType: %2, deviceID: %3, deviceName: %4, deviceState: %5")
		.arg(__FUNCTION__)
		.arg(deviceType)
		.arg(strDeviceId)
		.arg(strDeviceName)
		.arg(state)
	));

	emit sigAudioDeviceChanged(deviceType, strDeviceId, strDeviceName, state);

}

void QZegoAVSignal::OnVideoDeviceStateChanged(AV::DeviceInfo *deviceInfo, AV::DeviceState state)
{
	if (deviceInfo == nullptr)
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: deviceID: null, deviceName: null, deviceState: %2")
			.arg(__FUNCTION__)
			.arg(state)
		));

		return;
	}

	

	QString strDeviceId = deviceInfo->szDeviceId;
	QString strDeviceName = deviceInfo->szDeviceName;

	log_string_notice(qtoc(QStringLiteral("[%1]: deviceID: %2, deviceName: %3, deviceState: %4")
		.arg(__FUNCTION__)
		.arg(strDeviceId)
		.arg(strDeviceName)
		.arg(state)
	));

	emit sigVideoDeviceChanged(strDeviceId, strDeviceName, state);
	
}

void QZegoAVSignal::OnUserUpdate(const LIVEROOM::ZegoUserInfo *pUserInfo, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type)
{
	QVector<QString> userIDs;
	QVector<QString> userNames;
	QVector<int> userFlags;
	QVector<int> userRoles;

	log_string_notice(qtoc(QStringLiteral("[%1]: user update count: %2").arg(__FUNCTION__).arg(userCount)));
	for (int i = 0; i < userCount; i++)
	{
		QString strUserId = pUserInfo[i].szUserId;
		QString strUserName = pUserInfo[i].szUserName;
		int userFlag = pUserInfo[i].udapteFlag;
		int userRole = pUserInfo[i].role;

		userIDs.push_back(strUserId);
		userNames.push_back(strUserName);
		userFlags.push_back(userFlag);
		userRoles.push_back(userRole);

		log_string_notice(qtoc(QStringLiteral("[%1]:updated user id: %2, user name: %3, user role: %4, user flag: %5")
			.arg(__FUNCTION__)
			.arg(strUserId)
			.arg(strUserName)
			.arg(userFlag)
			.arg(userRole)
		));
	}

	emit sigUserUpdate(userIDs, userNames, userFlags, userRoles, userCount, type);
}

void QZegoAVSignal::OnMixStream(const AV::ZegoMixStreamResult& result, const char* pszMixStreamID, int seq)
{
	unsigned int errorCode = result.uiErrorCode;
	QString mixStreamID = pszMixStreamID ? pszMixStreamID : "";
	QString hlsUrl = result.oStreamInfo.arrHlsURLs[0];
	QString rtmpUrl = result.oStreamInfo.arrRtmpURLs[0];

	emit sigMixStream(errorCode, hlsUrl, rtmpUrl, mixStreamID, seq);
}

void QZegoAVSignal::OnRecvEndJoinLiveCommand(const char* pszFromUserId, const char* pszFromUserName, const char *pszRoomID)
{
	QString userId = pszFromUserId;
	QString userName = pszFromUserName;
	QString roomId = pszRoomID;
	
	emit sigRecvEndJoinLiveCommand(userId, userName, roomId);
}

void QZegoAVSignal::OnAVKitEvent(int event, AV::EventInfo* pInfo)
{
	log_string_notice(qtoc(QStringLiteral("[%1]: avkit event: %2").arg(__FUNCTION__).arg(event)));
	emit sigAVKitEvent(event);
}

void QZegoAVSignal::OnCaptureSoundLevelUpdate(SOUNDLEVEL::ZegoSoundLevelInfo *pCaptureSoundLevel)
{
	if (pCaptureSoundLevel == nullptr)
		return;

	QString streamId = pCaptureSoundLevel->szStreamID ? pCaptureSoundLevel->szStreamID : "";
	float soundLevel = pCaptureSoundLevel->soundLevel;

	emit sigCaptureSoundLevelUpdate(streamId, soundLevel);
}