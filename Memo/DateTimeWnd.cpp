#include "stdafx.h"
#include "DateTimeWnd.h"

#define DT_NONE   0
#define DT_UPDATE 1
#define DT_DELETE 2
#define DT_KEEP   3
CDTWnd::CDTWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false), m_nDTUpdateFlag(DT_NONE)
{
}

void CDTWnd::Init(CMemoWnd* pWnd, CLabelUI* pOwner)
{
	m_pOwner = pWnd;
	RECT rcPos = pOwner->GetPos();
	if (m_hWnd == NULL)
	{
		UINT uStyle = 0;
		if(pOwner->GetManager()->IsBackgroundTransparent())
		{
			uStyle = WS_POPUP | WS_VISIBLE;
			RECT rcWnd={0};
			::GetWindowRect(pOwner->GetManager()->GetPaintWindow(), &rcWnd);
			rcPos.left += rcWnd.left;
			rcPos.right += rcWnd.left;
			rcPos.top += rcWnd.top;
			rcPos.bottom += rcWnd.top;
		}
		else
		{
			uStyle = WS_CHILD;
		}
		Create(pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		SetWindowFont(m_hWnd, pOwner->GetManager()->GetFontInfo(pOwner->GetFont())->hFont, TRUE);
		::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM)_T("yyyy-MM-dd HH:mm:ss"));
	}


	::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, 0, (LPARAM)&pWnd->GetAlarmTime());
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);

	m_bInit = true;    
}

LPCTSTR CDTWnd::GetWindowClassName() const
{
	return _T("DateTimeWnd");
}

LPCTSTR CDTWnd::GetSuperClassName() const
{
	return DATETIMEPICK_CLASS;
}

void CDTWnd::OnFinalMessage(HWND /*hWnd*/)
{
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CDTWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	if( uMsg == WM_KILLFOCUS )
	{
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	}
	else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		m_nDTUpdateFlag = DT_KEEP;
		PostMessage(WM_CLOSE);
		return lRes;
	}
	else bHandled = FALSE;
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CDTWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	if (m_nDTUpdateFlag == DT_NONE)
	{
		SYSTEMTIME st;
		::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
		m_pOwner->SetAlarmTime(st);
	}
	PostMessage(WM_CLOSE);
	return lRes;
}
