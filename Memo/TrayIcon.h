#ifndef __TRAYICON_H__
#define __TRAYICON_H__

#pragma once
#include <ShellAPI.h>
#define WM_TRAYICON_MESSAGE			(WM_USER + 0x250)
class CTrayIcon
{
public:
	CTrayIcon(void);
	~CTrayIcon(void);

public:
	BOOL CreateTrayIcon(HWND hWnd, UINT _IconIDResource, LPCTSTR _ToolTipText = NULL, UINT _Message = 0);
	BOOL CreateTrayIcon(HWND hWnd, LPCTSTR _IconPath, LPCTSTR _ToolTipText = NULL, UINT _Message =0);
	void DeleteTrayIcon();
	bool SetTooltipText(LPCTSTR _ToolTipText);
	bool SetTooltipText(UINT _IDResource);
	CDuiString GetTooltipText() const;

	bool SetIcon(HICON _Hicon);
	bool SetIcon(LPCTSTR _IconFile);
	bool SetIcon(UINT _IDResource);
	HICON GetIcon() const;
	void SetHideIcon();
	void SetShowIcon();
	void RemoveIcon();
	bool StartTwinkling();
	void StopTwinkling();

	bool Enabled(){return m_bEnabled;};
	bool IsVisible(){return !m_bVisible;};
	bool IsTwinkling(){return m_bTwinkling;};

	void OnTimer(IDuiTimer* pTimer);
	bool ShowMessage(LPCTSTR lpMsg, LPCTSTR lpTitle, UINT uTimeout = 3000);
	bool OnTaskbarCreated();
private:
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bTwinkling;
	//HWND m_hWnd;
	HICON m_hIcon;
	IDuiTimer* pIDuiTimer;
	NOTIFYICONDATA	m_trayData;
};
#endif // __TRAYICON_H__

