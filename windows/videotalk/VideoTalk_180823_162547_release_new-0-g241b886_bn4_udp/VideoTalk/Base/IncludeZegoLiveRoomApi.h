#ifndef IncludeZegoLiveRoomApi_h
#define IncludeZegoLiveRoomApi_h

#include <QDebug>



#include "LiveRoom.h"
#include "LiveRoom-Publisher.h"
#include "LiveRoom-Player.h"
#include "LiveRoomDefines.h"
#include "LiveRoom-IM.h"
#include "RoomDefines.h"
#include "AVDefines.h"

#include "LiveRoomCallback.h"
#include "LiveRoomCallback-Player.h"
#include "LiveRoomCallback-Publisher.h"
#include "LiveRoomCallback-IM.h"
#include "zego-api-sound-level.h"

extern ZEGOAVKIT_API void ZegoExternalLogWithNotice(const char* content);

inline void log_string_notice(const char* content) { ZegoExternalLogWithNotice(content); qDebug() << content; }

#define qtoc(content) content.toStdString().c_str()


#endif /* IncludeZegoLiveRoomApi_h */