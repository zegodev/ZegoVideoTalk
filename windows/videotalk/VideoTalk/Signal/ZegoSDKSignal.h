#pragma once

#include "Model/ZegoRoomModel.h"
#include "Model/ZegoStreamModel.h"
#include "Base/IncludeZegoLiveRoomApi.h"

#include <QObject>
#include <QVector>
using namespace ZEGO;

class QZegoAVSignal : public QObject,
	public LIVEROOM::IRoomCallback,
	public LIVEROOM::ILivePublisherCallback,
	public LIVEROOM::ILivePlayerCallback,
	public LIVEROOM::IIMCallback,
	public SOUNDLEVEL::IZegoSoundLevelCallback,
	public AV::IZegoLiveEventCallback,
	public AV::IZegoDeviceStateCallback

{
	Q_OBJECT

public:
	QZegoAVSignal();
	~QZegoAVSignal();
	//继承回调对象后需要重载相应的回调函数
signals:
	void sigLoginRoom(int errorCode, const QString& roomId, QVector<StreamPtr> vStreamList);
	void sigLogoutRoom(int errorCode, const QString& roomId);
	void sigDisconnect(int errorCode, const QString& roomId);
	void sigKickOut(int reason, const QString& roomId);
	void sigStreamUpdated(const QString& roomId, QVector<StreamPtr> vStreamList, LIVEROOM::ZegoStreamUpdateType type);
	void sigPublishStateUpdate(int stateCode, const QString& streamId, StreamPtr streamInfo);
	void sigPlayStateUpdate(int stateCode, const QString& streamId);
	void sigPublishQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS);
	void sigPlayQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS);
	void sigAuxInput(unsigned char* pData, int* pDataLen, int pDataLenValue, int* pSampleRate, int* pNumChannels);
	void sigJoinLiveRequest(int seq, const QString& fromUserId, const QString& fromUserName, const QString& roomId);
	void sigJoinLiveResponse(int result, const QString& fromUserId, const QString& fromUserName, int seq);
	void sigAudioDeviceChanged(AV::AudioDeviceType deviceType, const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state);
	void sigVideoDeviceChanged(const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state);
	void sigUserUpdate(QVector<QString> userIDs, QVector<QString> userNames, QVector<int> userFlags, QVector<int> userRoles, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type);
	void sigMixStream(unsigned int errorCode, const QString& hlsUrl, const QString& rtmpUrl, const QString& mixStreamID, int seq);
	void sigRecvEndJoinLiveCommand(const QString& userId, const QString& userName, const QString& roomId);
	void sigCaptureSoundLevelUpdate(const QString& streamId, float soundlevel);
	void sigAVKitEvent(int event);

protected:
	//IRoomCallback
	void OnLoginRoom(int errorCode, const char *pszRoomID, const LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount);
	void OnLogoutRoom(int errorCode, const char *pszRoomID);
	void OnKickOut(int reason, const char *pszRoomID);
	void OnDisconnect(int errorCode, const char *pszRoomID);
	void OnStreamUpdated(LIVEROOM::ZegoStreamUpdateType type, LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID);
	void OnStreamExtraInfoUpdated(LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) {}
	void OnCustomCommand(int errorCode, int requestSeq, const char *pszRoomID) {}
	void OnRecvCustomCommand(const char *pszUserId, const char *pszUserName, const char *pszContent, const char *pszRoomID) {}

	//ILivePublisherCallback
	void OnPublishStateUpdate(int stateCode, const char* pszStreamID, const LIVEROOM::ZegoPublishingStreamInfo& oStreamInfo);
	void OnJoinLiveRequest(int seq, const char *pszFromUserId, const char *pszFromUserName, const char *pszRoomID);
	void OnInviteJoinLiveResponse(int result, const char *pszFromUserId, const char *pszFromUserName, int seq) {}
	void OnPublishQulityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS);
	void OnCaptureVideoSizeChanged(int nWidth, int nHeight) {}
	void OnPreviewSnapshot(void *pImage) {}
	void OnAuxCallback(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels);
	void OnMixStream(const AV::ZegoMixStreamResult& result, const char* pszMixStreamID, int seq);

	//ILivePlayerCallback
	void OnPlayStateUpdate(int stateCode, const char* pszStreamID);
	void OnPlayQualityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS);
	void OnJoinLiveResponse(int result, const char* pszFromUserId, const char* pszFromUserName, int seq);
	void OnInviteJoinLiveRequest(int seq, const char* pszFromUserId, const char* pszFromUserName, const char* pszRoomID) {};
	void OnVideoSizeChanged(const char* pStreamID, int nWidth, int nHeight) {};
	void OnSnapshot(void *pImage, const char* pszStreamID) {};
	void OnEndJoinLive(int result, int seq, const char *pszRoomID){};
	void OnRecvEndJoinLiveCommand(const char *pszFromUserId, const char *pszFromUserName, const char* pszRoomID);
	void OnSendRoomMessage(int errorCode, const char *pszRoomID, int sendSeq, unsigned long long messageId) {}
	void OnCreateConversation(int errorCode, const char *pszRoomID, int sendSeq, const char *pszConversationID) {};
	void OnGetConversationInfo(int errorCode, const char *pszRoomID, const char *pszConverID, ROOM::ZegoConverInfo *pConverInfo, ROOM::ZegoUser *pMemberList, unsigned int memberCount) {};
	void OnSendConversationMessage(int errorCode, const char *pszRoomID, const char *pszConverID, int sendSeq, unsigned long long messageId) {};
	void OnRecvRoomMessage(ROOM::ZegoRoomMessage *pMessageInfo, unsigned int messageCount, const char *pszRoomID) {}
	void OnRecvConversationMessage(ROOM::ZegoConversationMessage *pMessageInfo, const char *pszConversationID, const char *pszRoomID) {};

	//IZegoDeviceStateCallback
	void OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, AV::DeviceInfo *deviceInfo, AV::DeviceState state);
	void OnVideoDeviceStateChanged(AV::DeviceInfo *deviceInfo, AV::DeviceState state);
	void OnDeviceError(const char* deviceName, int errorCode) {};

	//IZegoLiveEventCallback
	void OnAVKitEvent(int event, AV::EventInfo* pInfo);

	//IZegoSoundLevelCallback
	void OnSoundLevelUpdate(SOUNDLEVEL::ZegoSoundLevelInfo *pSoundLevelList, unsigned int soundLevelCount) {}
	void OnCaptureSoundLevelUpdate(SOUNDLEVEL::ZegoSoundLevelInfo *pCaptureSoundLevel);

	//IMCallback
	void OnUserUpdate(const LIVEROOM::ZegoUserInfo *pUserInfo, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type);
};