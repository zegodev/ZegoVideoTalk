//
//  ZegoLiveRoom-mobile.h
//  ZegoLiveRoom
//
//  Created by Strong on 24/10/2017.
//

#ifndef ZegoLiveRoom_mobile_h
#define ZegoLiveRoom_mobile_h

#include "audio_in_output.h"
#include "RoomDefines.h"

#include <memory>
#include "LiveRoomDefines.h"

namespace AVE {
    class VideoCaptureFactory;
    class IAudioDataInOutput;
}


namespace ZEGO
{
    namespace LIVEROOM
    {        
        ZEGO_API bool SetAppOrientation(int nOrientation, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
        /// \brief 前摄像头开关
        /// \param bFront true 前摄像头, false 后摄像头
        /// \return true 成功，false 失败
        ZEGO_API bool SetFrontCam(bool bFront, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
        /// \brief 手机手电筒开关
        /// \param bEnable 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableTorch(bool bEnable, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
        ZEGO_API bool EnableCaptureMirror(bool bEnable, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        ZEGO_API bool EnableRateControl(bool bEnable, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
        ZEGO_API bool InitSDKAsync(unsigned int uiAppID, unsigned char* pBufAppSignature, int nSignatureSize);
        
        ZEGO_API void PauseModule(int moduleType);
        ZEGO_API void ResumeModule(int moduleType);
        
#if defined(ANDROID)
        ZEGO_API bool SetBluetoothOn(bool bEnable);
#endif

#ifndef WIN32
        ZEGO_API void EnableAECWhenHeadsetDetected(bool bEnable);
        ZEGO_API bool StartPlayingStream(const char* pszStreamID, std::shared_ptr<void> pView, const char* pszParams = 0);
        ZEGO_API bool StartPlayingStream2(const char* pszStreamID, std::shared_ptr<void> pView, ZegoStreamExtraPlayInfo* info);
        ZEGO_API bool UpdatePlayView(std::shared_ptr<void> pView, const char* pszStreamID);
        ZEGO_API bool SetPreviewView(std::shared_ptr<void> pView, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
        /// \brief 设置外部采集模块
        /// \param factory 工厂
        /// \note 必须在 InitSDK 前调用，并且不能置空
        ZEGO_API void SetVideoCaptureFactoryAsync(AVE::VideoCaptureFactory* factory, AV::PublishChannelIndex idx = AV::PUBLISH_CHN_MAIN);
        
#endif // !WIN32
    }
}

#endif /* ZegoLiveRoom_mobile_h */
