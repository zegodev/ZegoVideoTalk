#pragma once

#include <QObject>
#include <QVector>
#include "Base/IncludeZegoLiveRoomApi.h"

using namespace ZEGO;

struct QDeviceInfo
{
	QString deviceId;
	QString deviceName;
};

enum QDeviceType
{
	TYPE_AUDIO = 0,
	TYPE_VIDEO
};

enum QDeviceState
{
	STATE_ERROR = -1,
	STATE_NORMAL,
	STATE_SWAP
};

class ZegoDeviceManager : public QObject
{
	Q_OBJECT

signals:
	void sigDeviceAdded(int type, QString deviceName);
	void sigDeviceDeleted(int type, int index);

	void sigDeviceNone(int type);
	void sigDeviceExist(int type);

public:
	ZegoDeviceManager();
	~ZegoDeviceManager();

	void EnumAudioDeviceList();
	void EnumVideoDeviceList();
	QDeviceState SetMicrophoneIdByIndex(int index);
	QDeviceState SetCameraIdByIndex(int index);

	int GetAudioDeviceIndex();
	int GetVideoDeviceIndex();

	void SetMicVolume(int volume);
	int GetMicVolume();
	void SetMicEnabled(bool isUse);
	bool GetMicEnabled();

	void SetPlayVolume(int volume);
	int GetPlayVolume();
	void SetSpeakerEnabled(bool isUse);
	bool GetSpeakerEnabled();

	void SetCameraEnabled(bool isUse);
	bool GetCameraEnabled();

	QVector<QDeviceInfo> GetAudioDeviceList();
	QVector<QDeviceInfo> GetVideoDeviceList();

	const QString& GetAudioDeviceId();
	const QString& GetVideoDeviceId();
signals:
	void sigMicIdChanged(QString deviceId);
	void sigCameraIdChanged(QString deviceId);

protected slots:
	void OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state);
	void OnVideoDeviceStateChanged(const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state);

private:
	void RefreshCameraIndex();
	void RefreshMicIndex();

private:
	QVector<QDeviceInfo> m_audioDeviceList;
	QVector<QDeviceInfo> m_videoDeviceList;

	QString m_audioDeviceId;
	QString m_videoDeviceId;

	int m_micVolume = 100;
    bool m_micEnabled = true;

	int m_playVolume = 100;
	bool m_SpeakerEnabled = true;

	int m_micListIndex = -1;
	int m_cameraListIndex = -1;

	bool m_CameraEnabled = true;
};
