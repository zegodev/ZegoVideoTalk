#pragma once

#define mConfig theApp.GetConfig()
#define mBase theApp.GetBase()

//demo版本的枚举类型
typedef enum _Version
{
	ZEGO_PROTOCOL_UDP = 0,
	ZEGO_PROTOCOL_UDP_INTERNATIONAL = 1,
	ZEGO_PROTOCOL_CUSTOM = 2
}Version;