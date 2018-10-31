#include "ZegoVideoTalkDialog.h"
#include <QDebug>
#include "Base/ZegoVideoTalkDefines.h"

ZegoVideoTalkDialog::ZegoVideoTalkDialog(RoomPtr chatRoom, QDialog *lastDialog, bool isVideoCustom, QWidget *parent)
	: ZegoDialog(parent), 
	  m_pChatRoom(chatRoom), 
	  m_lastDialog(lastDialog), 
	  m_isVideoCustom(isVideoCustom)
{
	ui.setupUi(this);

	//通过sdk的信号连接到本类的槽函数中
	connect(GetAVSignal(), &QZegoAVSignal::sigLoginRoom, this, &ZegoVideoTalkDialog::OnLoginRoom);
	connect(GetAVSignal(), &QZegoAVSignal::sigStreamUpdated, this, &ZegoVideoTalkDialog::OnStreamUpdated);
	connect(GetAVSignal(), &QZegoAVSignal::sigPublishStateUpdate, this, &ZegoVideoTalkDialog::OnPublishStateUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigPlayStateUpdate, this, &ZegoVideoTalkDialog::OnPlayStateUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigDisconnect, this, &ZegoVideoTalkDialog::OnDisconnect);
	connect(GetAVSignal(), &QZegoAVSignal::sigKickOut, this, &ZegoVideoTalkDialog::OnKickOut);
	connect(GetAVSignal(), &QZegoAVSignal::sigPublishQualityUpdate, this, &ZegoVideoTalkDialog::OnPublishQualityUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigPlayQualityUpdate, this, &ZegoVideoTalkDialog::OnPlayQualityUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigUserUpdate, this, &ZegoVideoTalkDialog::OnUserUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigCaptureSoundLevelUpdate, this, &ZegoVideoTalkDialog::OnCaptureSoundLevelUpdate);
	connect(GetAVSignal(), &QZegoAVSignal::sigAVKitEvent, this, &ZegoVideoTalkDialog::OnAVKitEvent);
	//信号与槽同步执行
	connect(GetAVSignal(), &QZegoAVSignal::sigAuxInput, this, &ZegoVideoTalkDialog::OnAVAuxInput, Qt::DirectConnection);
	
	//ComboBox设备更改槽
	connect(ui.m_cbMircoPhone, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSwitchAudioDevice(int)));
	connect(ui.m_cbCamera, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSwitchVideoDevice(int)));
	
	m_device = new ZegoDeviceManager;
	connect(m_device, &ZegoDeviceManager::sigMicIdChanged, this, &ZegoVideoTalkDialog::OnMicIdChanged);
	connect(m_device, &ZegoDeviceManager::sigCameraIdChanged, this, &ZegoVideoTalkDialog::OnCameraIdChanged);
	connect(m_device, &ZegoDeviceManager::sigDeviceAdded, this, &ZegoVideoTalkDialog::OnDeviceAdded);
	connect(m_device, &ZegoDeviceManager::sigDeviceDeleted, this, &ZegoVideoTalkDialog::OnDeviceDeleted);

	//在VideoTalk中均以SettingsPtr操作
	m_pAVSettings = mConfig.GetVideoSettings();
	//读取配置中用户ID和用户名
	m_strCurUserID = mConfig.GetUserId();
	m_strCurUserName = mConfig.getUserName();

	this->installEventFilter(this);
	//初始化网格布局
	gridLayout = new QGridLayout();
}

ZegoVideoTalkDialog::~ZegoVideoTalkDialog()
{

}

void ZegoVideoTalkDialog::initDialog()
{
	//初始化麦克风摄像头的ComboBox
	initComboBox();
	initButtonIcon();

	ui.m_lbRetryPublish->setVisible(false);

	//成员列表初始化
	m_memberModel = new QStringListModel(this);
	ui.m_listMember->setModel(m_memberModel);
	ui.m_listMember->setItemDelegate(new NoFocusFrameDelegate(this));
	ui.m_listMember->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//日志列表初始化
	m_logModel = new QStandardItemModel(this);
	ui.m_listLog->setModel(m_logModel);
	ui.m_listLog->horizontalHeader()->setVisible(false);
	ui.m_listLog->verticalHeader()->setVisible(false);
	ui.m_listLog->verticalHeader()->setDefaultSectionSize(21);
	ui.m_listLog->setColumnWidth(0, 300);
	//读取标题内容
	QString strTitle = QString(tr("当前房间号：%1")).arg(m_pChatRoom->getRoomId());
	ui.m_lbRoomName->setText(strTitle);

	ui.m_edInput->setEnabled(false);
	//剩余能用的AVView
	for (int i = MAX_VIEW_COUNT; i >= 0; i--)
		m_avaliableView.push_front(i);

	// 推流成功前不能开混音、声音采集
	//ui.m_bAux->setEnabled(false);
	ui.m_bCapture->setEnabled(false);

	//枚举音视频设备
	GetAudioAndVideoList();

	SOUNDLEVEL::StartSoundLevelMonitor();
	int role = LIVEROOM::ZegoRoomRole::Audience;
	
	if (!LIVEROOM::LoginRoom(m_pChatRoom->getRoomId().toStdString().c_str(), role))
	{
		QMessageBox::information(NULL, tr("提示"), tr("进入房间失败"));
	}

	addLogString(tr("开始登陆房间"));

}

void ZegoVideoTalkDialog::initButtonIcon()
{
	//imageButton
	ui.m_bMin->setButtonIcon("min");
	ui.m_bMax->setButtonIcon("max");
	ui.m_bClose->setButtonIcon("close");

	//switchButton
	ui.m_bSpeaker->setButtonIcon("sound");
	ui.m_bCamera->setButtonIcon("camera");
}

void ZegoVideoTalkDialog::GetAudioAndVideoList()
{
	//获取音频设备
	m_device->EnumAudioDeviceList();
	//获取视频设备
	m_device->EnumVideoDeviceList();

	m_vecAudioDeviceList = m_device->GetAudioDeviceList();
	m_vecVideoDeviceList = m_device->GetVideoDeviceList();

	for (int i = 0; i < m_vecAudioDeviceList.size(); i++)
	{
		insertStringListModelItem(m_cbMircoPhoneModel, m_vecAudioDeviceList.at(i).deviceName, m_cbMircoPhoneModel->rowCount());
	}

	for (int j = 0; j < m_vecVideoDeviceList.size(); j++)
	{
		insertStringListModelItem(m_cbCameraModel, m_vecVideoDeviceList.at(j).deviceName, m_cbCameraModel->rowCount());
	}

	int mic_index = m_device->GetAudioDeviceIndex();
	int camera_index = m_device->GetVideoDeviceIndex();

	if (mic_index >= 0)
		ui.m_cbMircoPhone->setCurrentIndex(m_device->GetAudioDeviceIndex());
	if (camera_index >= 0)
		ui.m_cbCamera->setCurrentIndex(m_device->GetVideoDeviceIndex());


}

void ZegoVideoTalkDialog::StartPublishStream()
{
	QString strStreamId;
#ifdef Q_OS_WIN
	strStreamId = QString("s-windows-vt-%1").arg(m_strCurUserID);
#else
	strStreamId = QString("s-mac-vt-%1").arg(m_strCurUserID);
#endif
	m_strPublishStreamID = strStreamId;

	StreamPtr pPublishStream(new QZegoStreamModel(m_strPublishStreamID, m_strCurUserID, m_strCurUserName, "", true));

	m_pChatRoom->addStream(pPublishStream);

	//推流前调用双声道
	LIVEROOM::SetAudioChannelCount(2);

	if (m_avaliableView.size() > 0)
	{

		int nIndex = takeLeastAvaliableViewIndex();
		pPublishStream->setPlayView(nIndex);
		addAVView(nIndex);
		

		LIVEROOM::SetVideoFPS(m_pAVSettings->GetFps());
		LIVEROOM::SetVideoBitrate(m_pAVSettings->GetBitrate());
		LIVEROOM::SetVideoCaptureResolution(m_pAVSettings->GetResolution().cx, m_pAVSettings->GetResolution().cy);
		LIVEROOM::SetVideoEncodeResolution(m_pAVSettings->GetResolution().cx, m_pAVSettings->GetResolution().cy);

		//配置View
		LIVEROOM::SetPreviewView((void *)AVViews.last()->winId());
		LIVEROOM::SetPreviewViewMode(LIVEROOM::ZegoVideoViewModeScaleAspectFill);
		LIVEROOM::StartPreview();
		

		QString streamID = m_strPublishStreamID;
		m_anchorStreamInfo = pPublishStream;
		
		addLogString(tr("创建流成功, streamID: %1").arg(streamID));
		if (LIVEROOM::StartPublishing(m_pChatRoom->getRoomName().toStdString().c_str(), streamID.toStdString().c_str(), LIVEROOM::ZEGO_JOIN_PUBLISH, ""))
		{
			m_bIsPublishing = true;
			addLogString(tr("开始推流，流ID: %1").arg(streamID));
		}
	}
}

void ZegoVideoTalkDialog::StopPublishStream(const QString& streamID)
{
	if (streamID.size() == 0){ return; }

	LIVEROOM::SetPreviewView(nullptr);
	LIVEROOM::StopPreview();
	
	removeAVView(m_anchorStreamInfo->getPlayView());
	LIVEROOM::StopPublishing();
	m_bIsPublishing = false;
	StreamPtr pStream = m_pChatRoom->removeStream(streamID);
	FreeAVView(pStream);

	m_strPublishStreamID = "";
}

void ZegoVideoTalkDialog::StartPlayStream(StreamPtr stream)
{
	if (stream == nullptr) { return; }

	m_pChatRoom->addStream(stream);

	if (m_avaliableView.size() > 0)
	{
		int nIndex = takeLeastAvaliableViewIndex();
		qDebug() << "playStream nIndex = " << nIndex << " play stream id is " << stream->getStreamId();
		stream->setPlayView(nIndex);
		addAVView(nIndex);
		//配置View
		LIVEROOM::SetViewMode(LIVEROOM::ZegoVideoViewModeScaleAspectFill, stream->getStreamId().toStdString().c_str());
		LIVEROOM::StartPlayingStream(qtoc(stream->getStreamId()), (void *)AVViews.last()->winId());
	}
}

void ZegoVideoTalkDialog::StopPlayStream(const QString& streamID)
{
	if (streamID.size() == 0) { return; }

	StreamPtr curStream;
	for (auto stream : m_pChatRoom->getStreamList())
	{
		if (streamID == stream->getStreamId())
			curStream = stream;
	}

    if (curStream)
    {
        qDebug() << "stop play view index = " << curStream->getPlayView();
        removeAVView(curStream->getPlayView());
    }

	LIVEROOM::StopPlayingStream(qtoc(streamID));

	StreamPtr pStream = m_pChatRoom->removeStream(streamID);
	FreeAVView(pStream);
}

void ZegoVideoTalkDialog::addLogString(QString log)
{
	QDateTime dateTime;
	QTime time;
	QDate date;
	dateTime.setTime(time.currentTime());
	dateTime.setDate(date.currentDate());
	QString strDate = dateTime.toString("[hh-mm-ss-zzz]: ");

	QStandardItem *item = new QStandardItem;
	m_logModel->insertRow(0, item);
	QModelIndex index = m_logModel->indexFromItem(item);

	ZegoLogLabel *label = new ZegoLogLabel;
	label->setTextContent(strDate, log);
	
	
	ui.m_listLog->setIndexWidget(index, label);

	if (label->getHeightNum() > 1)
	    ui.m_listLog->resizeRowToContents(0);

	qDebug() << log;
	
}

void ZegoVideoTalkDialog::initComboBox()
{

	m_cbMircoPhoneModel = new QStringListModel(this);

	m_cbMircoPhoneModel->setStringList(m_MircoPhoneList);

	m_cbMircoPhoneListView = new QListView(this);
	ui.m_cbMircoPhone->setView(m_cbMircoPhoneListView);
	ui.m_cbMircoPhone->setModel(m_cbMircoPhoneModel);
	ui.m_cbMircoPhone->setItemDelegate(new NoFocusFrameDelegate(this));

	m_cbCameraModel = new QStringListModel(this);

	m_cbCameraModel->setStringList(m_CameraList);

	m_cbCameraListView = new QListView(this);
	ui.m_cbCamera->setView(m_cbCameraListView);
	ui.m_cbCamera->setModel(m_cbCameraModel);
	ui.m_cbCamera->setItemDelegate(new NoFocusFrameDelegate(this));

}

void ZegoVideoTalkDialog::insertStringListModelItem(QStringListModel * model, QString name, int size)
{
	if (model == nullptr)
		return;

	int row = size;
	model->insertRows(row, 1);
	QModelIndex index = model->index(row);
	model->setData(index, name);

}

void ZegoVideoTalkDialog::removeStringListModelItemByName(QStringListModel * model, QString name)
{
	if (model == nullptr)
		return;

	if (model->rowCount() > 0)
	{
		int curIndex = -1;
		QStringList list = model->stringList();
		for (int i = 0; i < list.size(); i++)
		{
			if (list[i] == name)
				curIndex = i;
		}

		model->removeRows(curIndex, 1);
	}

}

void ZegoVideoTalkDialog::removeStringListModelItemByIndex(QStringListModel * model, int index)
{
	if (model == nullptr)
		return;

	if (model->rowCount() > 0)
	{
		model->removeRows(index, 1);
	}

}

int ZegoVideoTalkDialog::takeLeastAvaliableViewIndex()
{
	int min = m_avaliableView[0];
	int minIndex = 0;
	for (int i = 1; i < m_avaliableView.size(); i++)
	{
		if (m_avaliableView[i] < min)
		{
			min = m_avaliableView[i];
			minIndex = i;
		}
	}

	m_avaliableView.takeAt(minIndex);
	return min;
}

void ZegoVideoTalkDialog::initAVView(QZegoAVView *view)
{
	view->setMinimumSize(QSize(240, 0));
	view->setStyleSheet(QLatin1String("border: none;\n"
		"background-color: #383838;"));
}

void ZegoVideoTalkDialog::addAVView(int addViewIndex)
{
	QZegoAVView *newAVView = new QZegoAVView;
	initAVView(newAVView);
	newAVView->setViewIndex(addViewIndex);
	AVViews.push_back(newAVView);

	updateViewLayout(AVViews.size());
}

void ZegoVideoTalkDialog::removeAVView(int removeViewIndex)
{
	int viewIndex = -1;
	for (int i = 0; i < AVViews.size(); i++)
	{
		if (AVViews[i]->getViewIndex() == removeViewIndex)
		{
			viewIndex = i;
			break;
		}
	}

	QZegoAVView *popView = AVViews.takeAt(viewIndex);
	popView->deleteLater();

	updateViewLayout(AVViews.size());
}

void ZegoVideoTalkDialog::updateViewLayout(int viewCount)
{

	for (int i = 0; i < viewCount; i++)
		gridLayout->removeWidget(AVViews[i]);

	gridLayout->deleteLater();

	gridLayout = new QGridLayout();
	gridLayout->setSpacing(0);
	gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
	ui.zoneLiveViewHorizontalLayout->addLayout(gridLayout);

	for (int i = 0; i < viewCount; i++)
	{
		int row, col;
		if (viewCount >= 1 && viewCount <= 4)
		{
			row = i / 2;
			col = i % 2;
		}
		else if (viewCount >= 5 && viewCount <= 9)
		{
			row = i / 3;
			col = i % 3;
		}
		else if (viewCount >= 10 && viewCount <= 12)
		{
			row = i / 4;
			col = i % 4;
		}
		qDebug() << "current row = " << row << " col = " << col;
		gridLayout->addWidget(AVViews[i], row, col, 1, 1);
		gridLayout->setRowStretch(row, 1);
		gridLayout->setColumnStretch(col, 1);
	}

}

void ZegoVideoTalkDialog::FreeAVView(StreamPtr stream)
{
	if (stream == nullptr)
	{
		return;
	}

	int nIndex = stream->getPlayView();

	m_avaliableView.push_front(nIndex);

	//刷新可用的view页面
	update();
}

void ZegoVideoTalkDialog::cleanBeforeGetOut()
{
	
	if (ui.m_bCapture->text() == tr("停止采集"))
#ifdef Q_OS_WIN
		LIVEROOM::EnableMixSystemPlayout(false);
#endif
	
	SOUNDLEVEL::StopSoundLevelMonitor();

	for (auto& stream : m_pChatRoom->getStreamList())
	{
		if (stream != nullptr){
			if (stream->isCurUserCreated())
			{
				StopPublishStream(stream->getStreamId());
			}
			else
			{
				StopPlayStream(stream->getStreamId());
			}
		}
	}

	roomMemberDelete(m_strCurUserName);
	LIVEROOM::LogoutRoom();
	//if (timer != nullptr)
		//timer->stop();

	//释放堆内存
	delete m_cbMircoPhoneListView;
	delete m_cbCameraListView;
	delete m_memberModel;
	delete m_cbMircoPhoneModel;
	delete m_cbCameraModel;
	//delete timer;
	delete gridLayout;
	//指针置为空
	m_cbMircoPhoneListView = nullptr;
	m_cbCameraListView = nullptr;
	m_memberModel = nullptr;
	m_cbMircoPhoneModel = nullptr;
	m_cbCameraModel = nullptr;
	//timer = nullptr;
	gridLayout = nullptr;
}

void ZegoVideoTalkDialog::roomMemberAdd(QString userName)
{
	if (m_memberModel == nullptr)
		return;

	insertStringListModelItem(m_memberModel, userName, m_memberModel->rowCount());
	ui.m_tabCommonAndUserList->setTabText(1, QString(tr("成员(%1)").arg(m_memberModel->rowCount())));
}

void ZegoVideoTalkDialog::roomMemberDelete(QString userName)
{
	if (m_memberModel == nullptr)
		return;

	removeStringListModelItemByName(m_memberModel, userName);
	ui.m_tabCommonAndUserList->setTabText(1, QString(tr("成员(%1)").arg(m_memberModel->rowCount())));
}

//-----------------------------------------------UI回调----------------------------------------------------------
void ZegoVideoTalkDialog::on_m_bClose_clicked()
{
	this->close();
}

void ZegoVideoTalkDialog::on_m_bMax_clicked()
{
	if (m_isMax)
	{
		this->showNormal();
		m_isMax = false;
	}
	else
	{
		this->showMaximized();
		m_isMax = true;
	}
}

void ZegoVideoTalkDialog::on_m_bMin_clicked()
{
	this->showMinimized();
}

void ZegoVideoTalkDialog::OnSwitchAudioDevice(int id)
{
	if (id < 0)
		return;

	if (id < m_vecAudioDeviceList.size())
	{
		m_device->SetMicrophoneIdByIndex(id);
		QString microphoneId = m_device->GetAudioDeviceId();

		LIVEROOM::SetAudioDevice(AV::AudioDevice_Input, qtoc(microphoneId));
		m_pAVSettings->SetMicrophoneId(microphoneId);
	}
}

void ZegoVideoTalkDialog::OnSwitchVideoDevice(int id)
{
	if (id < 0)
		return;

	if (id < m_vecVideoDeviceList.size())
	{
		m_device->SetCameraIdByIndex(id);
		QString cameraId = m_device->GetVideoDeviceId();

		LIVEROOM::SetVideoDevice(qtoc(cameraId));
		m_pAVSettings->SetCameraId(cameraId);

	}
}

void ZegoVideoTalkDialog::OnMicIdChanged(const QString& deviceId)
{
	QString curDeviceId = deviceId;

	if (curDeviceId.isEmpty())
	{
		LIVEROOM::SetAudioDevice(ZEGO::AV::AudioDeviceType::AudioDevice_Input, nullptr);
	}
	else
	{
		LIVEROOM::SetAudioDevice(ZEGO::AV::AudioDeviceType::AudioDevice_Input, qtoc(curDeviceId));
	}

	m_pAVSettings->SetMicrophoneId(curDeviceId);
}

void ZegoVideoTalkDialog::OnCameraIdChanged(const QString& deviceId)
{
	QString curDeviceId = deviceId;

	if (curDeviceId.isEmpty())
	{
		LIVEROOM::SetVideoDevice(nullptr);
	}
	else
	{
		LIVEROOM::SetVideoDevice(qtoc(curDeviceId));
	}

	m_pAVSettings->SetCameraId(curDeviceId);


	update();
}

void ZegoVideoTalkDialog::OnDeviceAdded(int type, QString deviceName)
{
	if (type == TYPE_AUDIO)
	{
		ui.m_cbMircoPhone->blockSignals(true);
		insertStringListModelItem(m_cbMircoPhoneModel, deviceName, m_cbMircoPhoneModel->rowCount());
		ui.m_cbMircoPhone->blockSignals(false);
	}

	if(type == TYPE_VIDEO)
	{
		ui.m_cbCamera->blockSignals(true);
		insertStringListModelItem(m_cbCameraModel, deviceName, m_cbCameraModel->rowCount());
		ui.m_cbCamera->blockSignals(false);
	}
}

void ZegoVideoTalkDialog::OnDeviceDeleted(int type, int index)
{
	if (type == TYPE_AUDIO)
	{
		ui.m_cbMircoPhone->blockSignals(true);
		removeStringListModelItemByIndex(m_cbMircoPhoneModel, index);
		ui.m_cbMircoPhone->blockSignals(false);
	}

	if (type == TYPE_VIDEO)
	{
		ui.m_cbCamera->blockSignals(true);
		removeStringListModelItemByIndex(m_cbCameraModel, index);
		ui.m_cbCamera->blockSignals(false);
	}
}

void ZegoVideoTalkDialog::on_m_bProgMircoPhone_clicked()
{

	if (ui.m_bProgMircoPhone->isChecked())
	{
		m_bCKEnableMic = true;
		ui.m_bProgMircoPhone->setMyEnabled(m_bCKEnableMic);

		m_device->SetMicEnabled(m_bCKEnableMic);
	}
	else
	{
		m_bCKEnableMic = false;
		//timer->stop();
		ui.m_bProgMircoPhone->setMyEnabled(m_bCKEnableMic);
		ui.m_bProgMircoPhone->update();

		m_device->SetMicEnabled(m_bCKEnableMic);
	}

}

void ZegoVideoTalkDialog::on_m_bSpeaker_clicked()
{

	if (ui.m_bSpeaker->isChecked())
	{

		m_bCKEnableSpeaker = true;
	}
	else
	{

		m_bCKEnableSpeaker = false;
	}

	//使用扬声器
	m_device->SetSpeakerEnabled(m_bCKEnableSpeaker);
}

void ZegoVideoTalkDialog::on_m_bShowFullScreen_clicked()
{
	//直播窗口总在最顶层
	ui.m_zoneLiveView_Inner->setWindowFlags(ui.m_zoneLiveView_Inner->windowFlags() | Qt::WindowStaysOnTopHint);
	ui.m_zoneLiveView_Inner->setParent(NULL);
	ui.m_zoneLiveView_Inner->showFullScreen();
	m_isLiveFullScreen = true;
	
}

void ZegoVideoTalkDialog::on_m_bCamera_clicked()
{
	if (ui.m_bCamera->isChecked())
	{
		m_bCKEnableCamera = true;
	}
	else
	{
		m_bCKEnableCamera = false;
	}

	//允许使用摄像头
	m_device->SetCameraEnabled(m_bCKEnableCamera);

	update();
}

void ZegoVideoTalkDialog::on_m_bCapture_clicked()
{
	if (ui.m_bCapture->text() == tr("声卡采集"))
	{
//#ifdef Q_OS_WIN
		LIVEROOM::EnableMixSystemPlayout(true);
//#endif
		ui.m_bCapture->setText(tr("停止采集"));
	}
	else
	{
//#ifdef Q_OS_WIN
		LIVEROOM::EnableMixSystemPlayout(false);
//#endif
		ui.m_bCapture->setText(tr("声卡采集"));
	}
}

void ZegoVideoTalkDialog::closeEvent(QCloseEvent *e)
{
	QDialog::closeEvent(e);
	cleanBeforeGetOut();
	//emit sigSaveVideoSettings(m_pAVSettings);
	m_lastDialog->show();
}

//-----------------------------------------------SDK回调-------------------------------------------------------------
void ZegoVideoTalkDialog::OnLoginRoom(int errorCode, const QString& strRoomID, QVector<StreamPtr> vStreamList)
{
	
	if (errorCode != 0)
	{
		addLogString(tr("登陆房间失败. error: %1").arg(errorCode));
		QMessageBox::information(NULL, tr("提示"), tr("登陆房间失败. error: %1").arg(errorCode));
		this->close();
		return;
	}

	addLogString(tr("登陆房间成功. roomID: %1").arg(strRoomID));

	//加入房间列表
	roomMemberAdd(m_strCurUserName);
	//登录房间成功即推流以及拉流
	StartPublishStream();

	for (auto& stream : vStreamList)
	{
		StartPlayStream(stream);
	}

}

void ZegoVideoTalkDialog::OnStreamUpdated(const QString& roomId, QVector<StreamPtr> vStreamList, LIVEROOM::ZegoStreamUpdateType type)
{
	//在连麦模式下，有流更新直接处理
	for (auto& stream : vStreamList)
	{
		if (stream != nullptr){
			if (type == LIVEROOM::ZegoStreamUpdateType::StreamAdded)
			{
				StartPlayStream(stream);
				addLogString(tr("新增一条流，流ID: %1").arg(stream->getStreamId()));

			}
			else if (type == LIVEROOM::ZegoStreamUpdateType::StreamDeleted)
			{
				StopPlayStream(stream->getStreamId());
				addLogString(tr("删除一条流，流ID: %1").arg(stream->getStreamId()));
			}
		}
	}


}

void ZegoVideoTalkDialog::OnPublishStateUpdate(int stateCode, const QString& streamId, StreamPtr streamInfo)
{

	if (stateCode == 0)
	{
		
		if (streamInfo != nullptr)
		{

			QString strUrl;
			QString strRtmpUrl = (streamInfo->m_vecRtmpUrls.size() > 0) ?
				streamInfo->m_vecRtmpUrls[0] : "";

			if (!strRtmpUrl.isEmpty())
			{
				strUrl.append("1. ");
				strUrl.append(strRtmpUrl);
				strUrl.append("\r\n");
			}

			QString strFlvUrl = (streamInfo->m_vecFlvUrls.size() > 0) ?
				streamInfo->m_vecFlvUrls[0] : "";

			if (!strFlvUrl.isEmpty())
			{
				strUrl.append("2. ");
				strUrl.append(strFlvUrl);
				strUrl.append("\r\n");
			}

			QString strHlsUrl = (streamInfo->m_vecHlsUrls.size() > 0) ?
				streamInfo->m_vecHlsUrls[0] : "";

			if (!strHlsUrl.isEmpty())
			{
				strUrl.append("3. ");
				strUrl.append(strHlsUrl);
				strUrl.append("\r\n");
			}

			
			addLogString(QString("Rtp %1").arg(strRtmpUrl));
			addLogString(tr("推流成功，流ID: %1").arg(streamId));

		}

		//ui.m_bAux->setEnabled(true);
		ui.m_bCapture->setEnabled(true);

	}
	else
	{
		addLogString(tr("推流失败,流ID: %1, error: %2").arg(streamId).arg(stateCode));
		//EndAux();
		// 停止预览, 回收view
		//removeAVView(streamInfo->getPlayView());
		LIVEROOM::StopPreview();
		LIVEROOM::SetPreviewView(nullptr);
		StreamPtr pStream = m_pChatRoom->removeStream(streamId);
		//FreeAVView(pStream);
	}
}

void ZegoVideoTalkDialog::OnPlayStateUpdate(int stateCode, const QString& streamId)
{
	
	if (stateCode != 0)
	{
		addLogString(tr("播放流失败，流ID: %1, error: %2").arg(streamId).arg(stateCode));
		// 回收view
		StreamPtr pStream = m_pChatRoom->removeStream(streamId);
		removeAVView(pStream->getPlayView());
		FreeAVView(pStream);
		return;
	}

	addLogString(tr("播放流成功，流ID: %1").arg(streamId));
}

void ZegoVideoTalkDialog::OnUserUpdate(QVector<QString> userIDs, QVector<QString> userNames, QVector<int> userFlags, QVector<int> userRoles, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type)
{
	qDebug() << "onUserUpdate!";

	//全量更新
	if (type == LIVEROOM::ZegoUserUpdateType::UPDATE_TOTAL){
		//removeAll
		m_memberModel->removeRows(0, m_memberModel->rowCount());

		insertStringListModelItem(m_memberModel, m_strCurUserName, 0);
		for (int i = 0; i < userCount; i++)
		{
			insertStringListModelItem(m_memberModel, userNames[i], m_memberModel->rowCount());
		}
	}
	//增量更新
	else
	{

		for (int i = 0; i < userCount; i++)
		{

			if (userFlags[i] == LIVEROOM::USER_ADDED)
				insertStringListModelItem(m_memberModel, userNames[i], m_memberModel->rowCount());
			else
				removeStringListModelItemByName(m_memberModel, userNames[i]);
		}
	}

	ui.m_tabCommonAndUserList->setTabText(1, QString(tr("成员(%1)").arg(m_memberModel->rowCount())));
	ui.m_listMember->update();
}

void ZegoVideoTalkDialog::OnDisconnect(int errorCode, const QString& roomId)
{
	if (m_pChatRoom->getRoomId() == roomId)
	{
		QMessageBox::information(NULL, tr("提示"), tr("连接失败，error: %1").arg(errorCode));
		this->close();
	}
}

void ZegoVideoTalkDialog::OnKickOut(int reason, const QString& roomId)
{
	if (m_pChatRoom->getRoomId() == roomId)
	{
		QMessageBox::information(NULL, tr("提示"), tr("您已被踢出房间"));
		this->close();
	}
}

void ZegoVideoTalkDialog::OnPlayQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS)
{
	StreamPtr pStream = m_pChatRoom->getStreamById(streamId);

	if (pStream == nullptr)
		return;

	int nIndex = pStream->getPlayView();

	if (nIndex < 0 || nIndex > 11)
		return;

	AVViews[nIndex]->setCurrentQuality(quality);

	//QVector<QString> q = { QStringLiteral("优"), QStringLiteral("良"), QStringLiteral("中"), QStringLiteral("差") };
	//qDebug() << QStringLiteral("当前窗口") << nIndex << QStringLiteral("的直播质量为") << q[quality];
}

void ZegoVideoTalkDialog::OnPublishQualityUpdate(const QString& streamId, int quality, double videoFPS, double videoKBS)
{
	StreamPtr pStream = m_pChatRoom->getStreamById(streamId);

	if (pStream == nullptr)
		return;

	int nIndex = pStream->getPlayView();

	if (nIndex < 0 || nIndex > 11)
		return;

	AVViews[nIndex]->setCurrentQuality(quality);

	//QVector<QString> q = { QStringLiteral("优"), QStringLiteral("良"), QStringLiteral("中"), QStringLiteral("差") };
	//qDebug() << QStringLiteral("当前窗口") << nIndex << QStringLiteral("的直播质量为") << q[quality];

}

void ZegoVideoTalkDialog::OnCaptureSoundLevelUpdate(const QString& streamId, float soundlevel)
{
	if (m_strPublishStreamID != streamId)
		return;

	ui.m_bProgMircoPhone->setProgValue(soundlevel);
	ui.m_bProgMircoPhone->update();
}

void ZegoVideoTalkDialog::OnAVKitEvent(int event)
{
	if (event == AV::Publish_TempDisconnected)
	{
		ui.m_lbRetryPublish->setVisible(true);

		if (!ui.m_ProgRetryPublish->isAnimated())
			ui.m_ProgRetryPublish->startAnimation();
	}

	if (event == AV::Publish_RetrySuccess)
	{
		if (ui.m_ProgRetryPublish->isAnimated())
			ui.m_ProgRetryPublish->stopAnimation();

		ui.m_lbRetryPublish->setVisible(false);
	}
}

void ZegoVideoTalkDialog::OnAVAuxInput(unsigned char *pData, int *pDataLen, int pDataLenValue, int *pSampleRate, int *pNumChannels)
{

}

bool ZegoVideoTalkDialog::eventFilter(QObject *target, QEvent *event)
{
	
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Escape && m_isLiveFullScreen)
			{
				qDebug() << "clicl esc";
				ui.m_zoneLiveView_Inner->setParent(ui.m_zoneLiveView);
				ui.horizontalLayout_ForAVView->addWidget(ui.m_zoneLiveView_Inner);
				m_isLiveFullScreen = false;
				//取消直播窗口总在最顶层
				ui.m_zoneLiveView_Inner->setWindowFlags(ui.m_zoneLiveView_Inner->windowFlags() &~Qt::WindowStaysOnTopHint);
				return true;
			}
			else if (keyEvent->key() == Qt::Key_Escape && !m_isLiveFullScreen)
			{
				this->close();
				return true;
			}
		}
	
	return QDialog::eventFilter(target, event);
}