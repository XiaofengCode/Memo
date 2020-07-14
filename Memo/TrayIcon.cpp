#include "StdAfx.h"
#include "TrayIcon.h"

CTrayIcon::CTrayIcon(void)
{
	memset(&m_trayData, 0, sizeof(m_trayData));
	m_bEnabled		= false;
	m_bVisible		= false;
	m_bTwinkling	= false;
	//m_hWnd			= NULL;
	pIDuiTimer		= NULL;
}


CTrayIcon::~CTrayIcon(void)
{
	DeleteTrayIcon();
}

BOOL CTrayIcon::CreateTrayIcon(HWND hWnd,UINT uIconIDResource,LPCTSTR lpszToolTipText /*= NULL*/,UINT dwMessage)
{
	if(!hWnd || !uIconIDResource )
		return false;

	if (!dwMessage)
	{
		dwMessage = WM_TRAYICON_MESSAGE;
	}
	m_hIcon = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(uIconIDResource));

	m_trayData.cbSize = sizeof(NOTIFYICONDATA);
	m_trayData.hWnd	 = hWnd;
	m_trayData.uID	 = uIconIDResource;
	m_trayData.hIcon = m_hIcon;
	m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_trayData.uCallbackMessage = dwMessage;
	if(lpszToolTipText)
		_tcscpy(m_trayData.szTip,lpszToolTipText);

	if (Shell_NotifyIcon(NIM_ADD, &m_trayData))
	{
		m_bEnabled = true;
		return TRUE;
	}
	return FALSE;
}

BOOL CTrayIcon::CreateTrayIcon(HWND hWnd, LPCTSTR lpszIconPath,LPCTSTR lpszToolTipText /*= NULL*/,UINT dwMessage )
{
	if(!hWnd || !lpszIconPath )
		return FALSE;

	if (!dwMessage)
	{
		dwMessage = WM_TRAYICON_MESSAGE;
	}
	m_hIcon = (HICON)LoadImage(
		NULL,    //handle of the instance that contains //the image
		lpszIconPath,//name or identifier of image
		IMAGE_ICON,//type of image-can also be IMAGE_CURSOR or MAGE_ICON
		0,0,//desired width and height
		LR_LOADFROMFILE);//load flags

	m_trayData.cbSize = sizeof(NOTIFYICONDATA);
	m_trayData.hWnd	 = hWnd;
	m_trayData.uID	 = 0;
	m_trayData.hIcon = m_hIcon;
	m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_trayData.uCallbackMessage = dwMessage;
	if(lpszToolTipText)
		_tcscpy(m_trayData.szTip,lpszToolTipText);

	if (Shell_NotifyIcon(NIM_ADD, &m_trayData))
	{
		m_bEnabled = true;
		return TRUE;
	}
	return FALSE;
}

void CTrayIcon::DeleteTrayIcon()
{
	if(pIDuiTimer)
		StopTwinkling();

	Shell_NotifyIcon(NIM_DELETE, &m_trayData);
	m_bEnabled		= false;
	m_bVisible		= false;
	m_bTwinkling	= false;
	//m_hWnd			= NULL;
}

bool CTrayIcon::SetTooltipText( LPCTSTR _ToolTipText )
{
	if(_ToolTipText)
		_tcscpy(m_trayData.szTip,_ToolTipText);

	if (!m_bEnabled) return false;
	m_trayData.uFlags = NIF_TIP;
	return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
}

bool CTrayIcon::SetTooltipText( UINT _IDResource )
{
	TCHAR mbuf[64];
	LoadString(CPaintManagerUI::GetInstance(), _IDResource, mbuf, 64);

	return SetTooltipText(mbuf);
}

CDuiString CTrayIcon::GetTooltipText() const
{
	return m_trayData.szTip;
}

bool CTrayIcon::SetIcon( HICON _Hicon )
{
	if(_Hicon)
		m_hIcon = _Hicon;

	m_trayData.uFlags = NIF_ICON;
	m_trayData.hIcon = _Hicon;
		
	if (!m_bEnabled) return false;
	return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;

	return false;
}

bool CTrayIcon::SetIcon( LPCTSTR _IconFile )
{
	HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(),_IconFile);
	return SetIcon(hIcon);
}

bool CTrayIcon::SetIcon( UINT _IDResource )
{
	HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(_IDResource));
	return SetIcon(hIcon);
}

HICON CTrayIcon::GetIcon() const
{
	HICON hIcon = NULL;
	hIcon = m_trayData.hIcon;
	return hIcon;
}

void CTrayIcon::SetHideIcon()
{
	if (IsVisible()) {
		SetIcon((HICON)NULL);
		m_bVisible = TRUE;
	}
}

void CTrayIcon::SetShowIcon()
{
	if (!IsVisible()) {
		SetIcon(m_hIcon);
		m_bVisible = FALSE;
	}
}

void CTrayIcon::RemoveIcon()
{
	m_trayData.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, &m_trayData);
	m_bEnabled = false;
}

bool CTrayIcon::StartTwinkling()
{
	if(m_bTwinkling || !m_bEnabled || pIDuiTimer)
		return false;

	pIDuiTimer = MakeDuiTimer(this,&CTrayIcon::OnTimer,400);
	pIDuiTimer->SetDuiTimer();
	m_bTwinkling = true;
	return true;
}

void CTrayIcon::StopTwinkling()
{
	if(pIDuiTimer){
		pIDuiTimer->KillDuiTimer();
		delete pIDuiTimer;
		pIDuiTimer = NULL;
	}
	m_bTwinkling = false;
	SetShowIcon();
}

void CTrayIcon::OnTimer( IDuiTimer* pTimer )
{
	IsVisible()?SetHideIcon():SetShowIcon();
}

bool CTrayIcon::ShowMessage(LPCTSTR lpMsg, LPCTSTR lpTitle, UINT uTimeout)
{
	if (!m_bEnabled)
	{
		return false;
	}
	m_trayData.uFlags = NIF_INFO;
	m_trayData.uTimeout = uTimeout;
	m_trayData.dwInfoFlags = NIIF_INFO;
	lstrcpy( m_trayData.szInfo, lpMsg );
	lstrcpy( m_trayData.szInfoTitle, lpTitle ? lpTitle : TEXT( "" ) );
	return Shell_NotifyIcon( NIM_MODIFY, &m_trayData ) ? true : false;
}

bool CTrayIcon::OnTaskbarCreated()
{
	if (m_bEnabled)
	{
		m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		if (Shell_NotifyIcon(NIM_ADD, &m_trayData))
		{
			m_bEnabled = true;
			return true;
		}
	}
	return false;
}
