// Memo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"

CMainWnd g_Memo;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("Skin"));

	g_Memo.Create(NULL, NULL, UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
	g_Memo.CenterWindow();
	g_Memo.ShowModal();
	return 0;
}

