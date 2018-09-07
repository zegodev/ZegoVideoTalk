#pragma once

#include "Base/ZegoBase.h"
#include "Config/ZegoUserConfig.h"

class QZegoVideoTalkDemoApp
{
public:
	QZegoVideoTalkDemoApp();

public:
	QZegoBase& GetBase();
	QZegoUserConfig& GetConfig();

private:
	QZegoBase m_base;
	QZegoUserConfig m_config;
};

extern QZegoVideoTalkDemoApp theApp;

QZegoAVSignal * GetAVSignal(void);