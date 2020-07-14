// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once


#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <afx.h>
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include <afxtempl.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#define LINK_DUILIB
#include "UIlib.h"
using namespace DuiLib;
// 
// #ifdef _DEBUG
// #   ifdef _UNICODE
// #       pragma comment(lib, "DuiLib_sud.lib")
// #   else
// #       pragma comment(lib, "DuiLib_sd.lib")
// #   endif
// #else
// #   ifdef _UNICODE
// #       pragma comment(lib, "DuiLib_su.lib")
// #   else
// #       pragma comment(lib, "DuiLib_s.lib")
// #   endif
// #endif

#include "Util.h"
#include "Memo.h"
#include "MemoWnd.h"
#include "DataFile.h"
#include "MainWnd.h"