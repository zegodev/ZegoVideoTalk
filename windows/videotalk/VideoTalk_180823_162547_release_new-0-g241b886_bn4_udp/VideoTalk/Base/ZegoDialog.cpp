#include "ZegoDialog.h"
#include <QGraphicsDropShadowEffect>

#ifdef Q_OS_WIN
    #include <Windows.h>
    #include <dwmapi.h>

#endif

ZegoDialog::ZegoDialog(QWidget *parent) : QDialog(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);

	m_pHelper = new FramelessHelper(this);
	m_pHelper->activateOn(this);
	m_pHelper->setTitleHeight(40);
	m_pHelper->setWidgetMovable(true); //ÎÞ±ß¿òÍÏ¶¯

	//SetWidgetBorderless(this);
}

ZegoDialog::~ZegoDialog()
{
	m_pHelper->removeFrom(this);
	delete m_pHelper;
	m_pHelper = nullptr;
}

void ZegoDialog::SetWidgetBorderless(const QWidget *widget)
{
/*#ifdef Q_OS_WIN
	HWND hwnd = reinterpret_cast<HWND>(widget->winId());

	const LONG style = (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN);
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	const MARGINS shadow = { 1, 1, 1, 1 };
	DwmExtendFrameIntoClientArea(hwnd, &shadow);

	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
#endif*/
/*#ifdef Q_OS_WIN 
	BOOL bEnable = false;
	::DwmIsCompositionEnabled(&bEnable);
	if (bEnable)
	{
		DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
		::DwmSetWindowAttribute((HWND)winId(), DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		MARGINS margins = { -1 };
		::DwmExtendFrameIntoClientArea((HWND)winId(), &margins);
	}
#endif  */
}

void ZegoDialog::showEvent(QShowEvent *event)
{
	this->setAttribute(Qt::WA_Mapped);

	QDialog::showEvent(event);
}
