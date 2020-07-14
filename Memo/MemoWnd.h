#pragma once
#include <xstring>
#include "Ini.h"

#define BK_CLOLOR_NORMAL	0xFFF0F0AA
#define BK_CLOLOR_ALARM		0xFFB72D44//0xff5b3327

typedef union
{
	DWORD dwColor;
	struct  
	{
		BYTE B;
		BYTE G;
		BYTE R;
		BYTE A;
	};
}BKCOLOR;

#define TIMER_SAVE	1
#define TIMER_TIP	2

class CDTWnd;
class CMemoWnd :
	public WindowImplBase
{
public:
	CMemoWnd();
	CMemoWnd(WORD wID, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent);
	~CMemoWnd()
	{
	}
	SYSTEMTIME GetAlarmTime(){return m_stAlarm;}
	void SetAlarmTime(const SYSTEMTIME& st);
	WORD GetID(){return m_wID;}
	void Active();
	CDTWnd* m_pWindow;
protected:
	DUI_DECLARE_MESSAGE_MAP()
	CDuiString m_strSkinFile;
	CIni m_ini;
	CDialogBuilder m_dlgBuilderItem;
	CString m_strContent;
	CRect m_rcWnd;
	SYSTEMTIME m_stAlarm;
	WORD m_wID;
	BOOL	m_bStartSavePos;
	BOOL	m_bAlarm;
	char	m_cR;
	char	m_cG;
	char	m_cB;
	UINT	m_uTimerID;
	BOOL	m_bNoZOrder;
	BKCOLOR m_bkColor;



	virtual void InitWindow();
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual void OnFinalMessage( HWND hWnd );

	void SaveWindowPos();

	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("MemoWnd");
	}

	virtual CDuiString GetSkinFile()
	{
		return _T("Memo.xml");
	}

	virtual CDuiString GetSkinFolder()
	{
		return _T("");
	}
	virtual UILIB_RESOURCETYPE GetResourceType() const
	{
		return UILIB_FILE;
	}
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

	void OnTimer(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnContentChanged(TNotifyUI& msg);

	void Save();

	void OnButtonAdd(TNotifyUI& msg);
	void OnButtonDelete(TNotifyUI& msg);
	void OnButtonAlarm(TNotifyUI& msg);
	void OnButtonTomorrowAlarm(TNotifyUI& msg);
	void OnButtonDelayAlarm(TNotifyUI& msg);
	void OnButtonNoAlarm(TNotifyUI& msg);
	void OnButtonHide(TNotifyUI& msg);
	void OnAlarmModeSelected(TNotifyUI& msg);
	//
};

