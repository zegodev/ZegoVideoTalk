#include "ZegoDeviceManager.h"
#include "Signal/ZegoSDKSignal.h"
#include "ZegoVideoTalkDemo.h"

ZegoDeviceManager::ZegoDeviceManager()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: device manager module create").arg(__FUNCTION__)));
	connect(GetAVSignal(), &QZegoAVSignal::sigAudioDeviceChanged, this, &ZegoDeviceManager::OnAudioDeviceStateChanged);
	connect(GetAVSignal(), &QZegoAVSignal::sigVideoDeviceChanged, this, &ZegoDeviceManager::OnVideoDeviceStateChanged);

	//初始化时麦克风、摄像头、扬声器默认开启
	LIVEROOM::EnableMic(m_micEnabled);
	LIVEROOM::EnableCamera(m_CameraEnabled);
	LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
}

ZegoDeviceManager::~ZegoDeviceManager()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: device manager module destroy").arg(__FUNCTION__)));
}

//设备初始化
void ZegoDeviceManager::EnumAudioDeviceList()
{
	int nDeviceCount = 0;
	AV::DeviceInfo* pDeviceList(NULL);

	m_audioDeviceList.clear();
	//获取音频输入设备
	pDeviceList = LIVEROOM::GetAudioDeviceList(AV::AudioDeviceType::AudioDevice_Input, nDeviceCount);
	log_string_notice(qtoc(QStringLiteral("[%1]: get audio input device list, device count: %2")
		.arg(__FUNCTION__)
		.arg(nDeviceCount)
	));

	for (int i = 0; i < nDeviceCount; i++)
	{
		QDeviceInfo info;
		info.deviceId = pDeviceList[i].szDeviceId;
		info.deviceName = pDeviceList[i].szDeviceName;

		m_audioDeviceList.append(info);
	}

	if (m_audioDeviceList.size() > 0)
	{
		m_micListIndex = 0;
		
		unsigned int *id_len = new unsigned int;
		*id_len = 1024;
		char *mic_id = new char[*id_len];
		LIVEROOM::GetDefaultAudioDeviceId(AV::AudioDeviceType::AudioDevice_Input, mic_id, id_len);
		QString defaultId = mic_id;

		//设置默认麦克风与系统一致
		for (int i = 0; i < m_audioDeviceList.size(); i++)
		{
			if (m_audioDeviceList.at(i).deviceId == defaultId)
			{
				m_micListIndex = i;
				break;
			}
		}

		m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
		
		emit sigMicIdChanged(m_audioDeviceId);
	}
	else
	{
		m_audioDeviceId = "";
	}

	LIVEROOM::FreeDeviceList(pDeviceList);
	pDeviceList = NULL;

}

//设备初始化
void ZegoDeviceManager::EnumVideoDeviceList()
{
	int nDeviceCount = 0;
	AV::DeviceInfo* pDeviceList(NULL);

	m_videoDeviceList.clear();
	//获取视频设备
	pDeviceList = LIVEROOM::GetVideoDeviceList(nDeviceCount);
	for (int i = 0; i < nDeviceCount; i++)
	{
		QDeviceInfo info;
		info.deviceId = pDeviceList[i].szDeviceId;
		info.deviceName = pDeviceList[i].szDeviceName;

		m_videoDeviceList.append(info);
	}
	//设置摄像头1
	if (m_videoDeviceList.size() > 0)
	{
		m_cameraListIndex = 0;

		unsigned int *id_len = new unsigned int;
		*id_len = 1024;
		char *camera_id = new char[*id_len];
		LIVEROOM::GetDefaultVideoDeviceId(camera_id, id_len);
		QString defaultId = camera_id;
		//设置默认摄像头与系统一致
		for (int i = 0; i < m_videoDeviceList.size(); i++)
		{
			if (m_videoDeviceList.at(i).deviceId == defaultId)
			{
				m_cameraListIndex = i;
				break;
			}
		}

		m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
		//emit sigCameraIdChanged(m_videoDeviceId);
	}
	else
	{
		m_videoDeviceId = "";
	}


	LIVEROOM::FreeDeviceList(pDeviceList);
	pDeviceList = NULL;
}

void ZegoDeviceManager::RefreshMicIndex()
{
	m_micListIndex = -1;
	for (int i = 0; i < m_audioDeviceList.size(); i++)
		if (m_audioDeviceList.at(i).deviceId == m_audioDeviceId)
			m_micListIndex = i;
}

void ZegoDeviceManager::RefreshCameraIndex()
{
	m_cameraListIndex = -1;
	for (int i = 0; i < m_videoDeviceList.size(); i++)
		if (m_videoDeviceList.at(i).deviceId == m_videoDeviceId)
			m_cameraListIndex = i;
}

//音频设备切换
QDeviceState ZegoDeviceManager::SetMicrophoneIdByIndex(int index)
{
	if (index >= m_audioDeviceList.size())
		return STATE_ERROR;

	m_micListIndex = index;
	m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
	emit sigMicIdChanged(m_audioDeviceId);

	log_string_notice(qtoc(QStringLiteral("[%1]: set microphone deviceId: %2 by index %3")
		.arg(__FUNCTION__)
		.arg(m_audioDeviceId)
		.arg(index)
	));

	return STATE_NORMAL;
}

QDeviceState ZegoDeviceManager::SetCameraIdByIndex(int index)
{
	if (index >= m_videoDeviceList.size())
		return STATE_ERROR;

	m_cameraListIndex = index;
	m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
	emit sigCameraIdChanged(m_videoDeviceId);

	log_string_notice(qtoc(QStringLiteral("[%1]: set speaker deviceId: %2 by index %3")
		.arg(__FUNCTION__)
		.arg(m_videoDeviceId)
		.arg(index)
	));

	return STATE_NORMAL;
}

int ZegoDeviceManager::GetAudioDeviceIndex()
{
	return m_micListIndex;
}

int ZegoDeviceManager::GetVideoDeviceIndex()
{
	return m_cameraListIndex;
}

//音量切换
void ZegoDeviceManager::SetMicVolume(int volume)
{
	m_micVolume = volume;

    //log_string_notice(tr("SetMicVolume: %1").arg(volume).toStdString().c_str());
	LIVEROOM ::SetMicDeviceVolume(qtoc(m_audioDeviceId), volume);
    if (volume == 0 && m_micEnabled)
    {
        m_micEnabled = false;

		LIVEROOM::EnableMic(m_micEnabled);
		//emit sigMicVolumeMute(!m_micEnabled);
    }
    else if (!m_micEnabled && volume > 0)
    {
        m_micEnabled = true;

		LIVEROOM::EnableMic(m_micEnabled);
		//emit sigMicVolumeMute(!m_micEnabled);
    }
    
	
}

int ZegoDeviceManager::GetMicVolume()
{
	return m_micVolume;
}

void ZegoDeviceManager::SetMicEnabled(bool isUse)
{
	m_micEnabled = isUse;
	LIVEROOM::EnableMic(isUse);
}

bool ZegoDeviceManager::GetMicEnabled()
{
	return m_micEnabled;
}

//拉流音量切换
void ZegoDeviceManager::SetPlayVolume(int volume)
{
	m_playVolume = volume;

	LIVEROOM::SetPlayVolume(m_playVolume);
	if (volume == 0 && m_SpeakerEnabled)
	{
		m_SpeakerEnabled = false;

		LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
		//emit sigSpeakerVolumeMute(!m_SpeakerEnabled);
	}
	else if (volume > 0 && !m_SpeakerEnabled)
	{
		m_SpeakerEnabled = true;

		LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
		//emit sigSpeakerVolumeMute(!m_SpeakerEnabled);
	}
}

int ZegoDeviceManager::GetPlayVolume()
{
	return m_playVolume;
}

void ZegoDeviceManager::SetSpeakerEnabled(bool isUse)
{
	m_SpeakerEnabled = isUse;
	LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
}

bool ZegoDeviceManager::GetSpeakerEnabled()
{
	return m_SpeakerEnabled;
}

void ZegoDeviceManager::SetCameraEnabled(bool isUse)
{
	m_CameraEnabled = isUse;
	LIVEROOM::EnableCamera(m_CameraEnabled);
}

bool ZegoDeviceManager::GetCameraEnabled()
{
	return m_CameraEnabled;
}

QVector<QDeviceInfo> ZegoDeviceManager::GetAudioDeviceList()
{
	return m_audioDeviceList;
}

QVector<QDeviceInfo> ZegoDeviceManager::GetVideoDeviceList()
{
	return m_videoDeviceList;
}

const QString& ZegoDeviceManager::GetAudioDeviceId()
{
	return m_audioDeviceId;
}

const QString& ZegoDeviceManager::GetVideoDeviceId()
{
	return m_videoDeviceId;
}

void ZegoDeviceManager::OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state)
{
	if (deviceType == AV::AudioDeviceType::AudioDevice_Output)
		return;

	if (state == AV::DeviceState::Device_Added)
	{
		QDeviceInfo added_device;
		added_device.deviceId = strDeviceId;
		added_device.deviceName = strDeviceName;
		m_audioDeviceList.append(added_device);

		//从0到1
		if (m_audioDeviceList.size() == 1)
		{
			m_micListIndex = 0;
			m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
			emit sigMicIdChanged(m_audioDeviceId);

			//emit sigDeviceExist(TYPE_AUDIO);
		}

		emit sigDeviceAdded(TYPE_AUDIO, added_device.deviceName);
	}
	else if (state == AV::DeviceState::Device_Deleted)
	{
		
		for (int i = 0; i < m_audioDeviceList.size(); ++i)
		{
			if (m_audioDeviceList.at(i).deviceId != strDeviceId)
				continue;

			m_audioDeviceList.takeAt(i);

			if (m_micListIndex == i)
			{
				if (m_audioDeviceList.size() > 0)
				{
					m_audioDeviceId = m_audioDeviceList.at(0).deviceId;
					
				}
				else
				{
					m_audioDeviceId = "";
					emit sigDeviceNone(TYPE_AUDIO);
				}

				RefreshMicIndex();
				emit sigMicIdChanged(m_audioDeviceId);
			}

			emit sigDeviceDeleted(TYPE_AUDIO, i);
		}

	}

}

void ZegoDeviceManager::OnVideoDeviceStateChanged(const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state)
{
	if (state == AV::DeviceState::Device_Added)
	{
		QDeviceInfo added_device;
		added_device.deviceId = strDeviceId;
		added_device.deviceName = strDeviceName;
		m_videoDeviceList.append(added_device);

		if (m_videoDeviceList.size() == 1)
		{
			m_cameraListIndex = 0;
			m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
			emit sigCameraIdChanged(m_videoDeviceId);
			emit sigDeviceExist(TYPE_VIDEO);
		}

		emit sigDeviceAdded(TYPE_VIDEO, added_device.deviceName);
	}
	else if (state == AV::DeviceState::Device_Deleted)
	{
		for (int i = 0; i < m_videoDeviceList.size(); ++i)
		{
			if (m_videoDeviceList.at(i).deviceId != strDeviceId)
				continue;

			m_videoDeviceList.takeAt(i);

			if (m_cameraListIndex == i)
			{
				//若当前还有摄像头存在，且未被摄像头2使用，将未被使用的摄像头设置在1上
				if (m_videoDeviceList.size() > 0)
				{
					m_videoDeviceId = m_videoDeviceList.at(0).deviceId;
				}
				else
				{
					m_videoDeviceId = "";
					emit sigDeviceNone(TYPE_VIDEO);
				}

				//刷新index
				RefreshCameraIndex();
				emit sigCameraIdChanged(m_videoDeviceId);
			}

			emit sigDeviceDeleted(TYPE_VIDEO, i);
		}
	}
}
