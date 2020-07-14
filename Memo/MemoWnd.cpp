#include "StdAfx.h"
#include "MemoWnd.h"
#include "Ini.h"
#include "DateTimeWnd.h"


DUI_BEGIN_MESSAGE_MAP(CMemoWnd, WindowImplBase)
	DUI_ON_MSGTYPE_CTRNAME(DUI_MSGTYPE_KILLFOCUS, _T("redit_Content"), OnContentChanged)
	DUI_ON_CLICK_CTRNAME(_T("btn_Add"), OnButtonAdd)
	DUI_ON_CLICK_CTRNAME(_T("btn_Delete"), OnButtonDelete)
	DUI_ON_CLICK_CTRNAME(_T("btn_Alarm"), OnButtonAlarm)
	DUI_ON_CLICK_CTRNAME(_T("btn_Tomorrow"), OnButtonTomorrowAlarm)
	DUI_ON_CLICK_CTRNAME(_T("btn_DelayRemind"), OnButtonDelayAlarm)
	DUI_ON_CLICK_CTRNAME(_T("btn_NoRemind"), OnButtonNoAlarm)
	DUI_ON_CLICK_CTRNAME(_T("btn_Hide"), OnButtonHide)
	DUI_ON_SELECTCHANGED_CTRNAME(_T("cmb_AlarmMode"), OnAlarmModeSelected)
	DUI_ON_TIMER()
DUI_END_MESSAGE_MAP()


CMemoWnd::CMemoWnd()
{
	m_bStartSavePos =FALSE;
	memset(&m_stAlarm, 0, sizeof(m_stAlarm));
	m_wID = 0;
	memset(&m_rcWnd, 0, sizeof(m_rcWnd));
	m_bAlarm = FALSE;
	m_bNoZOrder = TRUE;
	m_uTimerID = 0;
	m_pWindow = NULL;
}

CMemoWnd::CMemoWnd(WORD wID, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent)
{
	m_wID = wID;
	m_bStartSavePos =FALSE;
	m_stAlarm = stAlarm;
	m_rcWnd = rcPos;
	m_strContent = lpszContent;
	m_bAlarm = FALSE;
	m_bNoZOrder = TRUE;
	m_uTimerID = 0;
	m_pWindow = NULL;
}

void CMemoWnd::SetAlarmTime(const SYSTEMTIME& st)
{
	GetDlgItem(_T("hlyt_Remind"))->SetVisible(false);
	m_bNoZOrder = FALSE;
	SetWindowPos(GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	m_bNoZOrder = TRUE;
	GetDlgItem(_T("vlyt_Main"))->SetBkColor(m_bkColor.dwColor);
	g_Memo.m_timer.DelTimer(m_uTimerID);
	CLabelUI* pLbl = (CLabelUI*)GetDlgItem(_T("lbl_Alarm"));
	if (!pLbl)
	{
		return;
	}
	CString sText;
	if (st.wHour)
	{
		SYSTEMTIME stNextAlart = st;
		if (st.wYear)
		{
			sText.Format(_T("%04d-%02d-%02d %2d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
		else
		{
			sText.Format(_T("每天 %2d:%02d:%02d"),
				st.wHour, st.wMinute, st.wSecond);
			COleDateTime dtCur = COleDateTime::GetCurrentTime();
			COleDateTime dtNextAlarm(dtCur.GetYear(), dtCur.GetMonth(), dtCur.GetDay(), stNextAlart.wHour, stNextAlart.wMinute, stNextAlart.wSecond);
			if (dtCur > dtNextAlarm)
			{
				COleDateTimeSpan span(1, 0, 0, 0);
				dtNextAlarm += span;
			}
			dtNextAlarm.GetAsSystemTime(stNextAlart);
		}
		m_uTimerID = g_Memo.m_timer.AddTimer(GetHWND(), stNextAlart, 0);
	}
	pLbl->SetText(sText);

	if (m_stAlarm.wYear == st.wYear &&
		m_stAlarm.wMonth == st.wMonth &&
		m_stAlarm.wDay == st.wDay &&
		m_stAlarm.wHour == st.wHour &&
		m_stAlarm.wMinute == st.wMinute &&
		m_stAlarm.wSecond == st.wSecond)
	{
		return;
	}
	m_stAlarm = st;
	Save();
}

void CMemoWnd::Active()
{
	m_bNoZOrder = FALSE;
	ShowWindow();
	//SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	BringWindowToTop(GetHWND());
	m_bNoZOrder = TRUE;
}

void CMemoWnd::InitWindow()
{
	m_bkColor.dwColor = GetDlgItem(_T("vlyt_Main"))->GetBkColor();
	SetDlgItemText(_T("redit_Content"), m_strContent);
	//GetDlgItem(_T("redit_Content"))->SetAttribute(_T("textcolor"), _T("#FF000000"));
	if (!m_rcWnd.left &&
		!m_rcWnd.top &&
		!m_rcWnd.right &&
		!m_rcWnd.bottom)
	{
		CenterWindow();
	}
	else
	{
		::SetWindowPos(m_PaintManager.GetPaintWindow(), HWND_NOTOPMOST, m_rcWnd.left, m_rcWnd.top, m_rcWnd.Width(), m_rcWnd.Height(), SWP_NOACTIVATE);
	}
	BKCOLOR alarm, normal;
	alarm.dwColor = BK_CLOLOR_ALARM;
	normal.dwColor = BK_CLOLOR_NORMAL;
	m_cR = (char)alarm.R - (char)normal.R;
	m_cG = (char)alarm.G - (char)normal.G;
	m_cB = (char)alarm.B - (char)normal.B;
	m_PaintManager.SetTimer(m_PaintManager.GetRoot(), TIMER_SAVE, 1000);
	SetDlgItemText(_T("lbl_Alarm"), _T(""));
	SetAlarmTime(m_stAlarm);
	m_bStartSavePos =TRUE;
}

LRESULT CMemoWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pWindow)
		delete m_pWindow;
	return WindowImplBase::OnClose(uMsg, wParam, lParam, bHandled);
}

void CMemoWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CMemoWnd::SaveWindowPos()
{
	GetWindowRect(m_PaintManager.GetPaintWindow(), m_rcWnd);
	Save();
}

LRESULT CMemoWnd::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

CControlUI* CMemoWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("ProjectItem")) == 0)
	{

	}
	return WindowImplBase::CreateControl(pstrClass);
}

LRESULT CMemoWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOVE && m_bStartSavePos)
	{
		SaveWindowPos();
	}
	else if (uMsg == WM_SYSCOMMAND)
	{
		if (wParam == SC_MINIMIZE)
		{
			return 0;
		}
	}
	else if (m_bNoZOrder && WM_WINDOWPOSCHANGING == uMsg)
	{
		LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;
		lpPos->flags |= SWP_NOZORDER;
	}
	else if (WM_PLANTIMER == uMsg)
	{
		Active();
		TCHAR szTitle[64] = {0};
		GetWindowText(g_Memo, szTitle, sizeof(szTitle)/sizeof(szTitle[0]));
 		g_Memo.m_trayIcon.ShowMessage(m_strContent,  szTitle);
		//定时已到
		m_PaintManager.SetTimer(m_PaintManager.GetRoot(), TIMER_TIP, 50);		
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CMemoWnd::OnContentChanged(TNotifyUI& msg)
{
	CString strContent = GetDlgItemText(_T("redit_Content"));
	if (strContent == m_strContent)
	{
		return;
	}
	m_strContent = strContent;
	Save();
}

void CMemoWnd::Save()
{
	PVOID pItem;
	if (g_Memo.m_DataFile.m_mapItem.Lookup(m_wID, pItem))
	{
		g_Memo.m_DataFile.ModifyItem(m_wID, m_rcWnd, m_stAlarm, (LPCTSTR)m_strContent);
	}
	else
	{
		m_wID = g_Memo.m_DataFile.AddItem(m_rcWnd, m_stAlarm, (LPCTSTR)m_strContent);
	}
	g_Memo.m_DataFile.Save();
	g_Memo.OnMemoSave(m_wID, m_strContent);
}

void CMemoWnd::OnButtonAdd(TNotifyUI& msg)
{
	g_Memo.AddMemo();
}

void CMemoWnd::OnButtonDelete(TNotifyUI& msg)
{
	PVOID pItem;
	DWORD dw = GetDlgItem(_T("redit_Content"))->GetTextColor();
	//GetDlgItem(_T("redit_Content"))->SetAttribute(_T("textcolor"), _T("#FFFFFFFF"));
	if (g_Memo.m_DataFile.m_mapItem.Lookup(m_wID, pItem))
	{
		if (m_strContent.GetLength())
		{
			if (MessageBox(m_hWnd, _T("确定删除？"), _T("删除便笺"), MB_YESNO) != IDYES)
			{
				return;
			}
		}
		g_Memo.DelMemo(m_wID);
		g_Memo.m_DataFile.RemoveItem(m_wID);
		g_Memo.m_DataFile.Save();
		Close(IDOK);
	}
}

void CMemoWnd::OnButtonAlarm(TNotifyUI& msg)
{
	if( m_pWindow ) return;
	m_pWindow = new CDTWnd();
	m_pWindow->Init(this, (CLabelUI*)GetDlgItem(_T("lbl_Alarm")));
	m_pWindow->ShowWindow();
}

void CMemoWnd::OnButtonTomorrowAlarm(TNotifyUI& msg)
{
	COleDateTime dt = COleDateTime::GetCurrentTime();
	COleDateTimeSpan span(1, 0, 0, 0);
	dt += span;
	dt.SetTime(m_stAlarm.wHour, m_stAlarm.wMinute, m_stAlarm.wSecond);
	SYSTEMTIME st;
	dt.GetAsSystemTime(st);
	SetAlarmTime(st);
}

void CMemoWnd::OnButtonDelayAlarm(TNotifyUI& msg)
{
	COleDateTime dt = COleDateTime::GetCurrentTime();
	COleDateTimeSpan span(0, 0, 10, 0);
	dt += span;
	SYSTEMTIME st;
	dt.GetAsSystemTime(st);
	SetAlarmTime(st);
}

void CMemoWnd::OnButtonNoAlarm(TNotifyUI& msg)
{
	SYSTEMTIME st = {0};
	SetAlarmTime(st);
}

void CMemoWnd::OnButtonHide(TNotifyUI& msg)
{
	ShowWindow(false);
}

void CMemoWnd::OnAlarmModeSelected(TNotifyUI& msg)
{
	CTabLayoutUI* pTab = (CTabLayoutUI*)GetDlgItem(_T("tab_Alarm"));
	if (!pTab)
	{
		return;
	}
	pTab->SelectItem(msg.wParam);
}

void CMemoWnd::OnTimer(TNotifyUI& msg)
{
	if (msg.wParam == TIMER_SAVE)
	{
		CString strContent = GetDlgItemText(_T("redit_Content"));
		if (strContent == m_strContent)
		{
			return;
		}
		m_strContent = strContent;
		Save();
	}
	else if (TIMER_TIP == msg.wParam)
	{
		//动画
		static int nTimes = 0;
		static bool bAdd = true;
		BKCOLOR bkColor;
		nTimes++;
		BKCOLOR alarm, normal;
		m_bNoZOrder = FALSE;
		RECT rc;
		m_bStartSavePos = FALSE;
		GetWindowRect(GetHWND(), &rc);
		if (bAdd)
		{
			rc.left += 5;
		}
		else
		{
			rc.left -= 5;
		}
		bAdd = !bAdd;

		SetWindowPos(GetHWND(), HWND_TOPMOST, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		BringWindowToTop(GetHWND());

		HWND hCurWnd = NULL;
		DWORD dwMyID;
		DWORD dwCurID;
		hCurWnd = ::GetForegroundWindow();
		dwMyID = ::GetCurrentThreadId();
		dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
		::AttachThreadInput(dwCurID, dwMyID, TRUE);
		::SetForegroundWindow(m_hWnd);
		::AttachThreadInput(dwCurID, dwMyID, FALSE);

		alarm.dwColor = BK_CLOLOR_ALARM;
		normal.dwColor = BK_CLOLOR_NORMAL;
		bkColor.A = 0xFF * nTimes / 10;
		int R = m_cR * nTimes / 10;
		bkColor.R = normal.R + R;
		bkColor.G = normal.G + m_cG * nTimes / 10;
		bkColor.B = normal.B + m_cB * nTimes / 10;
		if (nTimes == 10)
		{
			m_PaintManager.KillTimer(msg.pSender, msg.wParam);
			nTimes = 0;
			SetWindowPos(GetHWND(), HWND_TOPMOST, m_rcWnd.left, m_rcWnd.top, 0, 0, SWP_NOSIZE);
			m_bStartSavePos = TRUE;
			GetDlgItem(_T("hlyt_Remind"))->SetVisible(true);
			if (m_stAlarm.wYear == 0)
			{
				GetDlgItem(_T("btn_Tomorrow"))->SetVisible(false);
			}
			m_bNoZOrder = TRUE;
		}
 		GetDlgItem(_T("vlyt_Main"))->SetBkColor(bkColor.dwColor);
// 		m_PaintManager.KillTimer(msg.pSender, msg.wParam);
	}
}

