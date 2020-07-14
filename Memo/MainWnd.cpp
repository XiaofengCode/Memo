#include "StdAfx.h"
#include "MainWnd.h"

#include "MemoWnd.h"
#include "DataFile.h"
#include "PlanTimer.h"


DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
	DUI_ON_CLICK_CTRNAME(_T("btn_Add"), OnButtonAdd)
	DUI_ON_CLICK_CTRNAME(_T("btn_Hide"), OnButtonHide)
	DUI_ON_MSGTYPE_CTRNAME(DUI_MSGTYPE_ITEMSELECT, _T("lst_Memos"), OnListItemClick)
	DUI_ON_SELECTCHANGED_CTRNAME(_T("chk_StartOnBoot"), OnCheckedStartOnBoot)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(void)
{
	m_pList = NULL;
}


CMainWnd::~CMainWnd(void)
{
}

void CMainWnd::InitWindow()
{
	m_pList = (CListUI*)GetDlgItem(_T("lst_Memos"));
	CCheckBoxUI* pChk = (CCheckBoxUI*)GetDlgItem(_T("chk_StartOnBoot"));
	if (pChk)
	{
		TCHAR szCurFile[MAX_PATH];
		GetModuleFileName(NULL, szCurFile, MAX_PATH);

		TCHAR szRegFile[MAX_PATH];
		DWORD dwSize = sizeof(szRegFile);
		DWORD dwType;
		LSTATUS lRet = RegGetValue(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
			AUTORUN_NAME, RRF_RT_REG_SZ, &dwType, szRegFile, &dwSize);
		if (ERROR_SUCCESS == lRet)
		{
			if (_tcsicmp(szRegFile, szCurFile) == 0)
			{
				pChk->SetCheck(true);
			}
		}
	}

	InitMemos();
	m_trayIcon.CreateTrayIcon(GetHWND(), IDI_MEMO, _T("便笺"));
}

LRESULT CMainWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ShowWindow(false);
	return 0;
	return WindowImplBase::OnClose(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TRAYICON_MESSAGE)
	{
		OnTrayIconNotify(wParam, lParam);
	}
	if (uMsg == WM_COMMAND)
	{
		return OnCommand(uMsg, wParam, lParam, bHandled);
	}
	return 0;
}

LRESULT CMainWnd::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	switch (LOWORD(wParam))
	{
	case SWM_HIDE:
		ShowWindow(false);
		break;
	case SWM_SHOW:
		ShowWindow(true);
		break;
	case SWM_EXIT:
		m_trayIcon.DeleteTrayIcon();
		ExitProcess(0);
		//PostMessage(WM_CLOSE);
		break;
	default:
		bHandled = FALSE;
		break;
	}

	return 0;
}

void CMainWnd::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	static bool bNotify = false;
	if (bNotify)
	{
		return;
	}
	bNotify = true;
	switch (lParam)
	{
	case NIN_BALLOONUSERCLICK:
	case WM_LBUTTONUP:
		ShowWindow();
		SetForegroundWindow(m_hWnd);
		break;
	case WM_RBUTTONUP:
	case WM_CONTEXTMENU:
		ShowContextMenu(GetHWND());
		break;
	}
	bNotify = false;
}

void CMainWnd::ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		CString strItem;
		if (IsWindowVisible(hWnd))
		{
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_HIDE, _T("隐藏窗口"));
		}
		else
		{
			InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_SHOW, _T("显示窗口"));
		}
		
		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_EXIT, _T("退出"));


		// note:	must set window to the foreground or the
		//			menu won't disappear when it should
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN,
			pt.x, pt.y, 0, hWnd, NULL);
		DestroyMenu(hMenu);
	}
}

void CMainWnd::InitMemos()
{
	m_DataFile.Init(CPaintManagerUI::GetResourcePath() + _T("Data.dat"));
	POSITION pos = m_DataFile.m_mapItem.GetStartPosition();
	while (pos)
	{
		WORD wKey;
		PMEMO_ITEM pItem;
		m_DataFile.m_mapItem.GetNextAssoc(pos, wKey, (PVOID&)pItem);
		AddMemo(wKey, pItem->dfItem.rcPos, pItem->dfItem.stAlarm, pItem->strContent);
	}
	m_timer.StartTimer();
	
}

void CMainWnd::OnMemoSave(WORD wID, LPCTSTR lpszContent)
{
	CString strText(lpszContent);
	if (!strText.GetLength())
	{
		strText = _T("(无内容)");
	}
	CListLabelElementUI* pItem = (CListLabelElementUI*)GetListItemByID(wID);
	if (pItem)
	{
		pItem->SetText(strText);
	}
}

void CMainWnd::AddMemo()
{
	SYSTEMTIME stAlarm = {0};
	RECT rcPos = {0};
	WORD wID = m_DataFile.AddItem(rcPos, stAlarm, _T(""));
	if (!AddMemo(wID, rcPos, stAlarm, _T("")))
	{
		m_DataFile.RemoveItem(wID);
		return;
	}
}

CMemoWnd* CMainWnd::AddMemo(WORD wID, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent)
{
	HWND hWnd = ::FindWindow(_T("Progman"), NULL);
	if(NULL == hWnd)
	{
		return NULL;
	}
	hWnd = ::FindWindowEx(hWnd, NULL, _T("SHELLDLL_DefView"), NULL);
	if(NULL == hWnd)
	{
		return NULL;
	}
	hWnd = ::FindWindowEx(hWnd, NULL, _T("SysListView32"), NULL);
	if(NULL == hWnd)
	{
		return NULL;
	}
	CMemoWnd* pWnd = new CMemoWnd(wID, rcPos, stAlarm, lpszContent);
	//::SetParent(GetSafeHwnd(), hWnd);
	pWnd->Create(hWnd,
		NULL, 
		WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_OVERLAPPED,
		WS_EX_TOOLWINDOW|WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR/*UI_WNDSTYLE_EX_DIALOG*/);
	pWnd->ShowWindow(true);
	CListLabelElementUI* pItem = new CListLabelElementUI();
	pItem->SetTag((UINT_PTR)pWnd);
	m_pList->Add(pItem);
	m_nCount++;
	OnMemoSave(wID, lpszContent);
	return pWnd;
}

void CMainWnd::DelMemo(WORD wID)
{
	if (m_DataFile.RemoveItem(wID))
	{
		m_nCount--;
	}
	m_DataFile.Save();
	CControlUI* pItem = GetListItemByID(wID);
	m_pList->Remove(pItem);
// 	if (m_nCount == 0)
// 	{
// 		AddMemo();
// 	}
}

DuiLib::CControlUI* CMainWnd::GetListItemByID(WORD wID)
{
	for (int i = 0; i < m_pList->GetCount(); i++)
	{
		CControlUI* pItem = m_pList->GetItemAt(i);
		CMemoWnd* pWnd = (CMemoWnd*)pItem->GetTag();
		if (wID == pWnd->GetID())
		{
			return pItem;
		}
	}
	return NULL;
}

void CMainWnd::OnButtonAdd(TNotifyUI& msg)
{
	AddMemo();
}

void CMainWnd::OnButtonHide(TNotifyUI& msg)
{
	ShowWindow(false);
}

void CMainWnd::OnCheckedStartOnBoot(TNotifyUI& msg)
{
	CCheckBoxUI* pChk = (CCheckBoxUI*)msg.pSender;
	HKEY hKey;
	LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),0,KEY_SET_VALUE, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		//OutputError("Add %s to auto startup failed:%d (0x%08X)", szCurFile, lRet, lRet);
		return;
	}
	if (pChk->GetCheck())
	{
		TCHAR szCurFile[MAX_PATH];
		GetModuleFileName(NULL, szCurFile, MAX_PATH);
		lRet = RegSetValueEx(hKey, AUTORUN_NAME, 0, REG_SZ, (LPBYTE)szCurFile, _tcslen(szCurFile)*sizeof(TCHAR));
		if (lRet != ERROR_SUCCESS)
		{
			//OutputError("Add %s to auto startup failed:%d(0x%08X)", szCurFile, lRet, lRet);
		}
	}
	else
	{
		lRet = RegDeleteKey(hKey, AUTORUN_NAME);
		if (lRet != ERROR_SUCCESS)
		{
			//OutputError("Add %s to auto startup failed:%d(0x%08X)", szCurFile, lRet, lRet);
		}
		
	}
	//关闭注册表:
	RegCloseKey(hKey);
}

void CMainWnd::OnListItemClick(TNotifyUI& msg)
{
	int nSel = m_pList->GetCurSel();
	if (nSel == -1)
	{
		return;
	}
	CControlUI* pItem = m_pList->GetItemAt(nSel);
	if (!pItem)
	{
		return;
	}
	CMemoWnd* pWnd = (CMemoWnd*)pItem->GetTag();
	if (!pWnd)
	{
		return;
	}
	pWnd->Active();
	m_pList->SelectItem(-1);
}
