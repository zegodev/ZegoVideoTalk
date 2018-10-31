#include "ZegoSettingsDialog.h"
#include <QDebug>

ZegoSettingsDialog::ZegoSettingsDialog(QWidget *parent)
	: ZegoDialog(parent)
{
	ui.setupUi(this);

	//只要更改了设置，均触发该信号槽
	connect(this, &ZegoSettingsDialog::sigChangedSettingsConfig, this, &ZegoSettingsDialog::OnChangedSettingsConfig);
	//拖动条按钮
	connect(ui.m_bResolutionDown, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	connect(ui.m_bResolutionUp, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	connect(ui.m_bBitrateDown, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	connect(ui.m_bBitrateUp, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	connect(ui.m_bFPSDown, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	connect(ui.m_bFPSUp, &QPushButton::clicked, this, &ZegoSettingsDialog::OnButtonSliderValueChange);
	
	//安装键盘事件过滤器
	this->installEventFilter(this);

	//注册自定义信号的参数
	qRegisterMetaType< QZegoUserConfig >("QZegoUserConfig");
	m_isUseTestEnv = mBase.getUseTestEnv();
}

ZegoSettingsDialog::~ZegoSettingsDialog()
{

}

void ZegoSettingsDialog::initDialog()
{
	initButtonIcon();

	//获取sdk版本号
	ui.m_lbSdkVersion->setText(QString(QLatin1String(LIVEROOM::GetSDKVersion())));
	ui.m_bSaveSettings->setEnabled(false);

	m_pCurSettings = mConfig.GetVideoSettings();
	if (m_pCurSettings == nullptr)
	{
		QMessageBox::warning(NULL, tr("警告"), tr("设置页面初始化失败"));
		return;
	}

	m_tmpCurSettings = SettingsPtr::create();
	copySettings(m_tmpCurSettings, m_pCurSettings);

	m_strEdUserId = mConfig.GetUserId();
	m_strEdUserName = mConfig.getUserName();

	ui.m_strEdUserId->setText(m_strEdUserId);
	ui.m_strEdUserName->setText(m_strEdUserName);
	//Account输入框
	connect(ui.m_strEdUserId, &QLineEdit::textChanged, this, &ZegoSettingsDialog::OnChangedSettingsConfig);
	connect(ui.m_strEdUserName, &QLineEdit::textChanged, this, &ZegoSettingsDialog::OnChangedSettingsConfig);

	m_versionMode = mConfig.getAppVersion().m_versionMode;
	mBase.setKey(m_versionMode);

	m_tmpVersionMode = m_versionMode;
	m_tmpUseTestEnv = mBase.getUseTestEnv();

	//高级设置app版本为UDP,RTMP，国际版时，不用设置appid和appsign，默认为UDP
	if (m_versionMode == Version::ZEGO_PROTOCOL_UDP || m_versionMode == Version::ZEGO_PROTOCOL_UDP_INTERNATIONAL)
	{
		ui.m_strEdAPPID->setText(QString("%1").arg(mBase.GetAppID()));
		ui.m_strEdAPPSign->setText(tr("AppSign 已设置"));
		ui.m_strEdAPPID->setEnabled(false);
		ui.m_strEdAPPSign->setEnabled(false);
	}
	else if (m_versionMode == Version::ZEGO_PROTOCOL_CUSTOM)
	{
		unsigned long customAppID = mConfig.getAppVersion().m_strAppID;
		QString customAppSign = mConfig.getAppVersion().m_strAppSign;
		if (customAppID == 0 && customAppSign.isEmpty())
		{
			ui.m_strEdAPPID->setText("");
			ui.m_strEdAPPSign->setText("");

		}
		else
		{
			ui.m_strEdAPPID->setText(QString::number(customAppID, 10));
			ui.m_strEdAPPSign->setText(customAppSign);
		}
	}

	connect(ui.m_strEdAPPID, &QLineEdit::textChanged, this, &ZegoSettingsDialog::OnChangedSettingsConfigAndReinstallSDK);
	connect(ui.m_strEdAPPSign, &QLineEdit::textChanged, this, &ZegoSettingsDialog::OnChangedSettingsConfigAndReinstallSDK);

	if (mBase.getUseTestEnv())
		ui.m_switchTestEnv->setChecked(true);


	qDebug() << "当前App版本为：" << m_versionMode << " AppID: " << theApp.GetBase().GetAppID()<<"是否使用测试环境："<<theApp.GetBase().getUseTestEnv();
	
	//初始化app版本的ComboBox
	ui.m_cbAppVersion->addItem(tr("国内版"));
	ui.m_cbAppVersion->addItem(tr("国际版"));
	ui.m_cbAppVersion->addItem(tr("自定义"));
	ui.m_cbAppVersion->setCurrentIndex(m_versionMode);
	//ComboBox选中视频质量
	connect(ui.m_cbAppVersion, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxCheckAppVersion(int)));

	//初始化视频质量的ComboBox
	ui.m_cbVideoQuality->addItem(tr("超低质量"));
	ui.m_cbVideoQuality->addItem(tr("低质量"));
	ui.m_cbVideoQuality->addItem(tr("标准质量"));
	ui.m_cbVideoQuality->addItem(tr("高质量"));
	ui.m_cbVideoQuality->addItem(tr("超高质量"));
	ui.m_cbVideoQuality->addItem(tr("极高质量"));
	ui.m_cbVideoQuality->addItem(tr("自定义"));
	ui.m_cbVideoQuality->setCurrentIndex(m_pCurSettings->GetQuality(true));
	//ComboBox选中视频质量
	connect(ui.m_cbVideoQuality, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxCheckVideoQuality(int)));

	setDefalutVideoQuality();
}

void ZegoSettingsDialog::initButtonIcon()
{
	//ImageButton
	ui.m_bClose->setButtonIcon("close");
	ui.m_bMin->setButtonIcon("min");

	//SwitchButton
	ui.m_switchTestEnv->setButtonIcon("switch");

	//StateButton
	ui.m_bResolutionDown->setButtonIcon("sub");
	ui.m_bBitrateDown->setButtonIcon("sub");
	ui.m_bFPSDown->setButtonIcon("sub");
	ui.m_bResolutionUp->setButtonIcon("add");
	ui.m_bBitrateUp->setButtonIcon("add");
	ui.m_bFPSUp->setButtonIcon("add");
}

//视频参数拖动条Slider初始化
void ZegoSettingsDialog::setDefalutVideoQuality()
{

	Size defResolution = m_pCurSettings->GetResolution();

	for (int i = 0; i < sizeof(g_Resolution) / sizeof(g_Resolution[0]); ++i)
	{
		QString strResolution;
		strResolution = QString("%1×%2").arg(g_Resolution[i].cx).arg(g_Resolution[i].cy);
		m_vecResolution.push_back(strResolution);

		if (defResolution.cx == g_Resolution[i].cx && defResolution.cy == g_Resolution[i].cy)
		{
			//为了vec的索引值与slider的数组对应，采取反序
			ui.m_sliderResolution->setValue(g_Resolution_length - i);
			ui.m_lbValueResolution->setText(strResolution);
		}
	}

	connect(ui.m_sliderResolution, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChange(int)));
	connect(ui.m_sliderResolution, &QSlider::sliderPressed, this, &ZegoSettingsDialog::OnCheckSliderPressed);
	connect(ui.m_sliderResolution, &QSlider::sliderReleased, this, &ZegoSettingsDialog::OnCheckSliderReleased);

	int defBitrate = m_pCurSettings->GetBitrate();

	for (int i = 0; i < sizeof(g_Bitrate) / sizeof(g_Bitrate[0]); ++i)
	{
		QString strBitrate;
		int m = g_Bitrate[i] / (1000 * 1000);
		strBitrate = QString("%1k").arg(g_Bitrate[i] / 1000);
		m_vecBitrate.push_back(strBitrate);

		if (defBitrate == g_Bitrate[i])
		{
			ui.m_sliderBitrate->setValue(g_Bitrate_length - i);
			ui.m_lbValueBitrate->setText(strBitrate);
		}
	}

	connect(ui.m_sliderBitrate, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChange(int)));
	connect(ui.m_sliderBitrate, &QSlider::sliderPressed, this, &ZegoSettingsDialog::OnCheckSliderPressed);
	connect(ui.m_sliderBitrate, &QSlider::sliderReleased, this, &ZegoSettingsDialog::OnCheckSliderReleased);

	int defFPS = m_pCurSettings->GetFps();

	for (int i = 0; i < sizeof(g_Fps) / sizeof(g_Fps[0]); ++i)
	{
		QString strFPS;
		strFPS = QString("%1").arg(g_Fps[i]);
		m_vecFPS.push_back(strFPS);

		if (defFPS == g_Fps[i])
		{
			ui.m_sliderFPS->setValue(g_FPS_length - i);
			ui.m_lbValueFPS->setText(strFPS);
		}
	}

	connect(ui.m_sliderFPS, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChange(int)));
	connect(ui.m_sliderFPS, &QSlider::sliderPressed, this, &ZegoSettingsDialog::OnCheckSliderPressed);
	connect(ui.m_sliderFPS, &QSlider::sliderReleased, this, &ZegoSettingsDialog::OnCheckSliderReleased);

}

QVector<QString> ZegoSettingsDialog::handleAppSign(QString appSign)
{
	QVector<QString> vecAppSign;
	appSign = appSign.simplified();
	appSign.remove(",");
	appSign.remove(" ");
	
	for (int i = 0; i < appSign.size(); i += 4)
	{
		QString hexSign = appSign.mid(i, 4);
		hexSign.remove("0x");
		hexSign.toUpper();
		vecAppSign.append(hexSign);
	}
	
	return vecAppSign;
}

void ZegoSettingsDialog::copySettings(SettingsPtr dst, SettingsPtr src)
{
	if (dst == nullptr || src == nullptr)
		return;

	dst->SetResolution(src->GetResolution());
	dst->SetBitrate(src->GetBitrate());
	dst->SetFps(src->GetFps());
	dst->SetMicrophoneId(src->GetMircophoneId());
	dst->SetCameraId(src->GetCameraId());
	dst->SetQuality(true, src->GetQuality(true));
}

//UI信号槽回调
void ZegoSettingsDialog::OnChangedSettingsConfig()
{
	ui.m_bSaveSettings->setEnabled(true);
	m_isConfigChanged = true;
}

void ZegoSettingsDialog::OnChangedSettingsConfigAndReinstallSDK()
{
	ui.m_bSaveSettings->setEnabled(true);
	m_isConfigChanged = true;
	m_isNeedReInstallSDK = true;
	m_isCustomAppTextChanged = true;
}

void ZegoSettingsDialog::on_m_bUploadLog_clicked()
{
	LIVEROOM::UploadLog();
	QMessageBox::information(NULL, tr("提示"), tr("日志上传成功"));
}

void ZegoSettingsDialog::on_m_bClose_clicked()
{
	this->close();
}

void ZegoSettingsDialog::on_m_bMin_clicked()
{
	this->showMinimized();
}

void ZegoSettingsDialog::on_m_bSaveSettings_clicked()
{
	//若无设置更改，直接返回
	if (!m_isConfigChanged)
		return;

	//先卸载SDK
	if (m_isNeedReInstallSDK)
		mBase.UninitAVSDK();

	m_strEdUserId = ui.m_strEdUserId->text();
	m_strEdUserName = ui.m_strEdUserName->text();

	mConfig.SetUserId(m_strEdUserId);
	mConfig.SetUserName(m_strEdUserName);

	
	//若更改了测试环境按钮设置,需要重新设置参数
	if (m_tmpUseTestEnv != m_isUseTestEnv)
	{
		mBase.setTestEnv(m_tmpUseTestEnv);
		m_isUseTestEnv = m_tmpUseTestEnv;

	}

	mConfig.SetUseTestEnv(m_isUseTestEnv);

	mConfig.SetVideoSettings(m_tmpCurSettings);
	copySettings(m_pCurSettings, m_tmpCurSettings);

	unsigned long appId = 0;
	unsigned char *appSign = NULL;
	int signLen = 0;
	//若更改了App版本，或者在本文中重新输入了自定义appid，则在保存时尝试重新InitSDK
	if (m_tmpVersionMode != m_versionMode || m_isCustomAppTextChanged)
	{
		m_isCustomAppTextChanged = false;
		AppVersion saveVersion = mConfig.getAppVersion();
		mBase.setKey(m_tmpVersionMode);

		if (m_tmpVersionMode == ZEGO_PROTOCOL_UDP || m_tmpVersionMode == ZEGO_PROTOCOL_UDP_INTERNATIONAL)
		{
			//saveVersion.m_strAppID = 0;
			//saveVersion.m_strAppSign = "已设置";	
		}
		else if (m_tmpVersionMode == ZEGO_PROTOCOL_CUSTOM)
		{
			appId = ui.m_strEdAPPID->text().toUInt();
			QString strAppSign = ui.m_strEdAPPSign->text();
			QVector<QString> vecAppSign = handleAppSign(strAppSign);

			int len = vecAppSign.size() > 32 ? 32 : vecAppSign.size();
			signLen = vecAppSign.size();

			appSign = new unsigned char[32];
			for (int i = 0; i < len; i++)
			{
				bool ok;
				appSign[i] = (unsigned char)vecAppSign[i].toInt(&ok, 16);
			}

			saveVersion.m_strAppID = appId;
			saveVersion.m_strAppSign = strAppSign;

			mBase.setCustomAppID(appId);
			mBase.setCustomAppSign(appSign);
		}
		saveVersion.m_versionMode = m_tmpVersionMode;
		mConfig.setAppVersion(saveVersion);

	}

	//根据用户所改变的设定重新InitSDK
	if (m_isNeedReInstallSDK)
	{
		if (m_tmpVersionMode == ZEGO_PROTOCOL_UDP || m_tmpVersionMode == ZEGO_PROTOCOL_UDP_INTERNATIONAL)
			mBase.InitAVSDK(mConfig.GetVideoSettings(), m_strEdUserId, m_strEdUserName);
		else if (m_tmpVersionMode == ZEGO_PROTOCOL_CUSTOM && signLen == 32)
		{
			mBase.InitAVSDKofCustom(mConfig.GetVideoSettings(), m_strEdUserId, m_strEdUserName, appId, appSign, signLen);

		}
	}

	mConfig.SaveConfig();

	m_isConfigChanged = false;
	m_versionMode = m_tmpVersionMode;
	m_isUseTestEnv = m_tmpUseTestEnv;
	ui.m_lbTitle->setText(tr("设置"));

	//释放临时堆空间
	if (appSign != NULL)
	{
		delete[]appSign;
		appSign = NULL;
	}

	QMessageBox::information(NULL, tr("提示"), tr("保存配置成功"));
	ui.m_bSaveSettings->setEnabled(false);
}

void ZegoSettingsDialog::on_m_switchTestEnv_clicked()
{
	if (ui.m_switchTestEnv->isChecked())
	{
		qDebug() << "TestEnv checked!";
		m_tmpUseTestEnv = true;

	}
	else
	{
		qDebug() << "TestEnv unChecked!";
		m_tmpUseTestEnv = false;

	}

	m_isNeedReInstallSDK = true;
	emit sigChangedSettingsConfig();
}

void ZegoSettingsDialog::OnComboBoxCheckAppVersion(int id)
{
	if (id == ZEGO_PROTOCOL_UDP || id == ZEGO_PROTOCOL_UDP_INTERNATIONAL)
	{
		ui.m_strEdAPPID->setText(QString("%1").arg(mBase.GetAppIDwithKey(id)));
		ui.m_strEdAPPSign->setText(tr("AppSign 已设置"));
		ui.m_strEdAPPID->setEnabled(false);
		ui.m_strEdAPPSign->setEnabled(false);

	}
	else
	{
		unsigned long customAppID = mConfig.getAppVersion().m_strAppID;
		QString customAppSign = mConfig.getAppVersion().m_strAppSign;
		if (customAppID == 0 && customAppSign.isEmpty())
		{
			ui.m_strEdAPPID->setText("");
			ui.m_strEdAPPSign->setText("");
		}
		else
		{
			ui.m_strEdAPPID->setText(QString::number(customAppID, 10));
			ui.m_strEdAPPSign->setText(customAppSign);
			ui.m_strEdAPPID->setFocus();
		}

		ui.m_strEdAPPID->setEnabled(true);
		ui.m_strEdAPPSign->setEnabled(true);
		ui.m_strEdAPPID->setFocus();
	}


	//暂时保存设置
	m_tmpVersionMode = (Version)id;

	m_isNeedReInstallSDK = true;
	emit sigChangedSettingsConfig();
}

void ZegoSettingsDialog::OnComboBoxCheckVideoQuality(int id)
{
	if (id == VideoQuality::VQ_SelfDef)
	{
		m_isVideoCustom = true;
	}
	else
	{
		m_isVideoCustom = false;
		m_tmpCurSettings->SetQuality(true, (VideoQuality)id);

		IndexSet index = m_tmpCurSettings->getIndex();

		ui.m_lbValueResolution->setText(m_vecResolution[index.indexResolution]);
		ui.m_lbValueBitrate->setText(m_vecBitrate[index.indexBitrate]);
		ui.m_lbValueFPS->setText(m_vecFPS[index.indexFps]);

		ui.m_sliderResolution->setValue(g_Resolution_length - index.indexResolution);
		ui.m_sliderBitrate->setValue(g_Bitrate_length - index.indexBitrate);
		ui.m_sliderFPS->setValue(g_FPS_length - index.indexFps);
	}
}

void ZegoSettingsDialog::OnCheckSliderPressed()
{
	m_sliderPressed = true;
}

void ZegoSettingsDialog::OnCheckSliderReleased()
{
	m_sliderPressed = false;
}

void ZegoSettingsDialog::OnSliderValueChange(int value)
{
	//每次slider改变时将当前视频质量改为“自定义”，同时将每次改动写入ini文件中

	QSlider *slider = qobject_cast<QSlider *>(sender());

	if (!m_isVideoCustom && m_sliderPressed)
	{
		m_isVideoCustom = true;
		//暂时阻塞信号，不发送此时Index改变的信号
		ui.m_cbVideoQuality->blockSignals(true);
		ui.m_cbVideoQuality->setCurrentIndex(VideoQuality::VQ_SelfDef);
		ui.m_cbVideoQuality->blockSignals(false);
	}

	//SettingsPtr curSettings = m_userConfig.GetVideoSettings();
	if (slider == ui.m_sliderResolution)
	{
		slider->setValue(value);
		ui.m_lbValueResolution->setText(m_vecResolution[g_Resolution_length - value]);
		m_tmpCurSettings->SetResolution(g_Resolution[g_Resolution_length - value]);
	}
	else if (slider == ui.m_sliderBitrate)
	{
		slider->setValue(value);
		ui.m_lbValueBitrate->setText(m_vecBitrate[g_Bitrate_length - value]);
		m_tmpCurSettings->SetBitrate(g_Bitrate[g_Bitrate_length - value]);
	}
	else if (slider == ui.m_sliderFPS)
	{
		slider->setValue(value);
		ui.m_lbValueFPS->setText(m_vecFPS[g_FPS_length - value]);
		m_tmpCurSettings->SetFps(g_Fps[g_FPS_length - value]);
	}

	emit sigChangedSettingsConfig();

}

void ZegoSettingsDialog::OnButtonSliderValueChange()
{
	//每次增减视频参数时将当前视频质量改为“自定义”，同时将每次改动写入ini文件中
	if (!m_isVideoCustom)
	{
		m_isVideoCustom = true;
		//暂时阻塞信号，不发送此时Index改变的信号
		ui.m_cbVideoQuality->blockSignals(true);
		ui.m_cbVideoQuality->setCurrentIndex(VideoQuality::VQ_SelfDef);
		ui.m_cbVideoQuality->blockSignals(false);
	}

	QPushButton *button = qobject_cast<QPushButton *>(sender());

	//SettingsPtr curSettings = m_userConfig.GetVideoSettings();

	if (button == ui.m_bResolutionDown)
	{
		int curValue = ui.m_sliderResolution->value() - 1;
		if (curValue >= 0){
			ui.m_sliderResolution->setValue(curValue);
			ui.m_lbValueResolution->setText(m_vecResolution[g_Resolution_length - curValue]);
			m_tmpCurSettings->SetResolution(g_Resolution[g_Resolution_length - curValue]);

		}

	}
	else if (button == ui.m_bResolutionUp)
	{
		int curValue = ui.m_sliderResolution->value() + 1;
		if (curValue <= g_Resolution_length){
			ui.m_sliderResolution->setValue(curValue);
			ui.m_lbValueResolution->setText(m_vecResolution[g_Resolution_length - curValue]);
			m_tmpCurSettings->SetResolution(g_Resolution[g_Resolution_length - curValue]);

		}
	}
	else if (button == ui.m_bBitrateDown)
	{
		int curValue = ui.m_sliderBitrate->value() - 1;
		if (curValue >= 0){
			ui.m_sliderBitrate->setValue(curValue);
			ui.m_lbValueBitrate->setText(m_vecBitrate[g_Bitrate_length - curValue]);
			m_tmpCurSettings->SetBitrate(g_Bitrate[g_Bitrate_length - curValue]);

		}
	}
	else if (button == ui.m_bBitrateUp)
	{
		int curValue = ui.m_sliderBitrate->value() + 1;
		if (curValue <= g_Bitrate_length){
			ui.m_sliderBitrate->setValue(curValue);
			ui.m_lbValueBitrate->setText(m_vecBitrate[g_Bitrate_length - curValue]);
			m_tmpCurSettings->SetBitrate(g_Bitrate[g_Bitrate_length - curValue]);

		}
	}
	else if (button == ui.m_bFPSDown)
	{
		int curValue = ui.m_sliderFPS->value() - 1;
		if (curValue >= 0)
		{
			ui.m_sliderFPS->setValue(curValue);
			ui.m_lbValueFPS->setText(m_vecFPS[g_FPS_length - curValue]);
			m_tmpCurSettings->SetFps(g_Fps[g_FPS_length - curValue]);

		}
	}
	else if (button == ui.m_bFPSUp)
	{
		int curValue = ui.m_sliderFPS->value() + 1;
		if (curValue <= g_FPS_length)
		{
			ui.m_sliderFPS->setValue(curValue);
			ui.m_lbValueFPS->setText(m_vecFPS[g_FPS_length - curValue]);
			m_tmpCurSettings->SetFps(g_Fps[g_FPS_length - curValue]);

		}
	}
	emit sigChangedSettingsConfig();
}

void ZegoSettingsDialog::closeEvent(QCloseEvent *event)
{
	if (m_isConfigChanged)
	{
		QMessageBox::StandardButton button;
		button = QMessageBox::question(this, tr("退出设置"),
			QString(tr("确认不保存设置吗?")),
			QMessageBox::Yes | QMessageBox::No);
		if (button == QMessageBox::No) {
			event->ignore();  //忽略退出信号，程序继续运行
		}
		else if (button == QMessageBox::Yes) {
			event->accept();  //接受退出信号，程序退出
		}
	}
	
}

bool ZegoSettingsDialog::eventFilter(QObject *target, QEvent *event)
{
	
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_S && (keyEvent->modifiers() & Qt::ControlModifier))
		{
			on_m_bSaveSettings_clicked();
			return true;
		}
	}
	return QDialog::eventFilter(target, event);
}