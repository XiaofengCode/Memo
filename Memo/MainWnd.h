#pragma once
#include "PlanTimer.h"
#include "TrayIcon.h"

#define SWM_HIDE		(WM_USER + 1)
#define SWM_SHOW		(WM_USER + 2)
#define SWM_EXIT		(WM_USER + 3)

#define AUTORUN_NAME		_T("XFMemo")
class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

	virtual void InitWindow();
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("CMainWnd");
	}

	virtual CDuiString GetSkinFile()
	{
		return _T("Main.xml");
	}

	virtual CDuiString GetSkinFolder()
	{
		return _T("");
	}
	virtual UILIB_RESOURCETYPE GetResourceType() const
	{
		return UILIB_FILE;
	}
	void OnMemoSave(WORD wID, LPCTSTR lpszContent);
	void ShowContextMenu(HWND hWnd);
	void InitMemos();
	void AddMemo();
	CMemoWnd* AddMemo(WORD wKey, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent);
	void DelMemo(WORD wID);
	CControlUI* GetListItemByID(WORD wID);

	void OnButtonAdd(TNotifyUI& msg);
	void OnButtonHide(TNotifyUI& msg);
	void OnCheckedStartOnBoot(TNotifyUI& msg);
	void OnListItemClick(TNotifyUI& msg);
	int m_nCount;
	//CArray<CMemoWnd*,CMemoWnd*> m_arDlgItems;
	CDataFile m_DataFile;
	CPlanTimer m_timer;
	CListUI* m_pList;
	CTrayIcon m_trayIcon;
	DUI_DECLARE_MESSAGE_MAP()
};
extern CMainWnd g_Memo;
