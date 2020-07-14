#include "StdAfx.h"
#include "PlanTimer.h"


CPlanTimer::CPlanTimer(void)
{
	m_uID = 1;
	m_bQuit = false;
	m_hWorkThread = nullptr;
}


CPlanTimer::~CPlanTimer(void)
{
}

UINT CPlanTimer::AddTimer(HWND hWnd, SYSTEMTIME st, WPARAM wParam)
{
	PT_ITEM item;
	item.hWnd = hWnd;
	item.dt = st;
	item.wParam = wParam;;
	//Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_secItems);
	m_secItems.Lock();
	item.uID = m_uID++;
	POSITION pos = m_lstItems.GetHeadPosition();
	if (pos)
	{
		bool bInsert = false;
		while (pos)
		{
			PT_ITEM& i  = m_lstItems.GetAt(pos);
			if (i.dt > item.dt)
			{
				m_lstItems.InsertBefore(pos, item);
				bInsert = true;
				break;
			}
			m_lstItems.GetNext(pos);
		}
		if (!bInsert)
		{
			m_lstItems.AddTail(item);
		}
	}
	else
	{
		m_lstItems.AddHead(item);
	}
	m_secItems.Unlock();
	return item.uID;
}

bool CPlanTimer::DelTimer(UINT uID)
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_secItems);
	POSITION pos = m_lstItems.GetHeadPosition();
	while (pos)
	{
		PT_ITEM& i  = m_lstItems.GetAt(pos);
		if (i.uID == uID)
		{
			m_lstItems.RemoveAt(pos);
			return true;
		}
		m_lstItems.GetNext(pos);
	}
	return false;
}

UINT CALLBACK ThreadTimer(LPVOID lpContext)
{
	CPlanTimer* pThis = (CPlanTimer*)lpContext;
	pThis->WorkThread();
	return 0;
}

bool CPlanTimer::StartTimer()
{
	m_hWorkThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadTimer, this, 0, nullptr);
	if (m_hWorkThread)
	{
		return true;
	}
	return false;
}

void CPlanTimer::StopTimer()
{
	m_bQuit = true;
	WaitForSingleObject(m_hWorkThread, INFINITE);
}

void CPlanTimer::WorkThread()
{
	while (!m_bQuit)
	{
		Sleep(500);
		m_secItems.Lock();
		while (m_lstItems.GetCount())
		{
			PT_ITEM& item = m_lstItems.GetHead();
			if (item.dt.GetStatus() == COleDateTime::invalid)
			{
				break;
			}
			if (item.dt < COleDateTime::GetCurrentTime())
			{
				PostMessage(item.hWnd, WM_PLANTIMER, item.wParam, 0);
				m_lstItems.RemoveHead();
			}
			else
			{
				break;
			}
		}
		m_secItems.Unlock();
	}
}
