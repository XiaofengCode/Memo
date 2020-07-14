#pragma once
#include <ATLComTime.h>
#include <list>
#include <map>
using namespace std;
typedef bool (*PLANTIMER_CALLBACK)(LPVOID lpContext, UINT uID);

#define WM_PLANTIMER		(WM_USER + 'PT')
typedef struct  
{
	HWND hWnd;
	UINT uID;
	COleDateTime dt;
	WPARAM wParam;
}PT_ITEM, *PPT_ITEM;

class CPlanTimer
{
public:
	CPlanTimer(void);
	~CPlanTimer(void);
	UINT AddTimer(HWND hWnd, SYSTEMTIME st, WPARAM wParam);
	bool DelTimer(UINT uID);
	bool StartTimer();
	void StopTimer();
	void WorkThread();
protected:
	UINT m_uID;
	Zutil::CCriticalSection m_secItems;
	CList<PT_ITEM> m_lstItems;
	bool m_bQuit;
	HANDLE m_hWorkThread;
};

