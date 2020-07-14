/**********************************************************
 * @file Util.h 
 * @brief !!!No Simple Description!!!
 * !!!No Description!!!
 * 
 * @author Kevin 
 * @date 2015/6/3
 * 
 * @note 
 * 		2015/6/3	Kevin First Create.
 **********************************************************/
#ifndef _UTIL_H_14527_
#define _UTIL_H_14527_
#include <IO.H>
#include <string>
using namespace std;

#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
namespace Zutil{
class CRegister
{
public:
	CRegister():m_hKey(NULL), m_hRoot(NULL){}
	CRegister(LPCTSTR lpKey):m_hKey(NULL), m_hRoot(NULL)
	{
		Open(lpKey);
	}
	~CRegister()
	{
		if (m_hKey)
		{
			RegCloseKey(m_hKey);
			m_hKey = NULL;
		}
	}
	BOOL Open(LPCTSTR lpKey)
	{
		if (!lpKey)
		{
			return FALSE;
		}
		const TCHAR* p = _tcschr(lpKey, _T('\\'));
		if (!p || p == lpKey)
		{
			return FALSE;
		}
		TCHAR szRoot[MAX_PATH] = {0};
		_tcsncpy_s(szRoot, MAX_PATH, lpKey, p - lpKey);
		if (_tcsicmp(szRoot, _T("HKEY_LOCAL_MACHINE")) == 0 || _tcsicmp(szRoot, _T("HKLM")) == 0)
		{
			m_hRoot = HKEY_LOCAL_MACHINE;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_CLASSES_ROOT")) == 0 || _tcsicmp(szRoot, _T("HKCR")) == 0 )
		{
			m_hRoot = HKEY_CLASSES_ROOT;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_CURRENT_USER")) == 0 || _tcsicmp(szRoot, _T("HKCU")) == 0 )
		{
			m_hRoot = HKEY_CURRENT_USER;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_USERS")) == 0)
		{
			m_hRoot = HKEY_USERS;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_PERFORMANCE_DATA")) == 0)
		{
			m_hRoot = HKEY_PERFORMANCE_DATA;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_CURRENT_CONFIG")) == 0)
		{
			m_hRoot = HKEY_CURRENT_CONFIG;
		}
		else if (_tcsicmp(szRoot, _T("HKEY_DYN_DATA")) == 0)
		{
			m_hRoot = HKEY_DYN_DATA;
		}
		else
		{
			return FALSE;
		}
		return Open(m_hRoot, p + 1);
	}
	BOOL Open(HKEY hRoot, LPCTSTR lpKey)
	{
		if (m_hKey)
		{
			RegCloseKey(m_hKey);
			m_hKey = NULL;
		}
		LONG lRet = RegOpenKey(hRoot, lpKey, &m_hKey);
		if (lRet != ERROR_SUCCESS)
		{
			return FALSE;
		}
		m_hRoot = hRoot;
		return TRUE;
	}
	BOOL GetBOOL(LPCTSTR lpSubKey)
	{
		return GetDWORD(lpSubKey) != 0;
	}
	BOOL SetValue(LPCTSTR lpSubKey, BOOL bValue)
	{
		return SetValue(lpSubKey, (DWORD)(bValue ? 1 : 0));
	}
	DWORD GetDWORD(LPCTSTR lpSubKey)
	{
		DWORD dwType = 0, dwDataLen = 256;
        BYTE  bData[256] = {0};
        LPBYTE lpBuffer = bData;
		long lResult = RegQueryValueEx(m_hKey, lpSubKey, NULL, &dwType, lpBuffer, &dwDataLen);
		if (lResult == ERROR_BUFFER_OVERFLOW)
		{
			
		}
        if(dwType == REG_DWORD)
        {
            return *((LPDWORD)lpBuffer);
        }
		return 0;
	}
	BOOL SetValue(LPCTSTR lpSubKey, DWORD dwValue)
	{
		long lRet = RegSetValueEx(m_hKey, lpSubKey, 0, REG_DWORD, (LPBYTE)(&dwValue), sizeof(dwValue));
		return lRet == ERROR_SUCCESS;
	}
	tstring GetString(LPCTSTR lpSubKey)
	{
		DWORD dwType = 0, dwDataLen = 256;
        BYTE*  pData = new BYTE[256];
		long lResult = RegQueryValueEx(m_hKey, lpSubKey, NULL, &dwType, pData, &dwDataLen);
		if (lResult == ERROR_BUFFER_OVERFLOW)
		{
			delete[] pData;
			pData = new BYTE[dwDataLen];
			lResult = RegQueryValueEx(m_hKey, lpSubKey, NULL, &dwType, pData, &dwDataLen);
			if (lResult != ERROR_SUCCESS)
			{
				return _T("");
			}
		}
        switch(dwType)
        {
        case REG_SZ:
        case REG_EXPAND_SZ:
			tstring s((LPCTSTR)pData);
			delete[] pData;
			return s;
//         case REG_MULTI_SZ:
// 			{
// 				tstring ss;
// 				ss.append(dwDataLen, (LPTSTR)lpBuffer);
// 				return ss;
// 			}
// 			break;
        }
		delete[] pData;
		return _T("");
	}
	BOOL IsExistKey(LPCTSTR lpSubKey)
	{
		DWORD dwType = 0, dwDataLen = MAX_PATH;
		BYTE byTmp[MAX_PATH];
		LONG lRet = RegQueryValueEx(m_hKey, lpSubKey, NULL, &dwType, byTmp, &dwDataLen);
		return lRet == ERROR_SUCCESS || lRet == ERROR_BUFFER_OVERFLOW;
	}
	BOOL OpenSubKey(LPCTSTR lpSubKey, CRegister& regSub)
	{
		return regSub.Open(m_hKey, lpSubKey);
	}
	BOOL operator !()
	{
		if (m_hKey == INVALID_HANDLE_VALUE || m_hKey == NULL)
		{
			return TRUE;
		}
		return FALSE;
	}
	
	operator BOOL() const
	{
		if (m_hKey == INVALID_HANDLE_VALUE || m_hKey == NULL)
		{
			return FALSE;
		}
		return TRUE;
	}
protected:
	HKEY m_hKey;
	HKEY m_hRoot;
};
//////////////////////////////////////////////////////////////////////////
///确保正确创建一个新的目录
//
__inline BOOL xxEnsureCreateDir(tstring cPath)
{
	tstring strTemp;
	int i = cPath.length();
	TCHAR szTemp[MAX_PATH];
	memset(szTemp , 0 , MAX_PATH);
	_tcscpy_s(szTemp , MAX_PATH, cPath.c_str());
	
	if(_taccess(szTemp,0) == 0)
		return TRUE;
	
	if(CreateDirectory(cPath.c_str() , NULL))
	{
		return TRUE;
	}
	else
	{
		for(i -= 2 ; i > 0 ; i--)
		{
			if(szTemp[i] == '\\')
			{
				break;
			}
			else
			{
				szTemp[i] = 0;
			}
		}
		if(i == 0) return FALSE;
		
		strTemp = szTemp;
		if(xxEnsureCreateDir(strTemp))
		{
			if(CreateDirectory(cPath.c_str() , NULL))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}


__inline BOOL xxCopyFile2(LPCTSTR lpszSource, LPCTSTR lpszTarget, BOOL bFailedIfExist)
{
	if(CopyFile(lpszSource,lpszTarget,bFailedIfExist))
		return TRUE;
	
	if(!lpszSource || !lpszTarget)
		return FALSE;
	
	xxEnsureCreateDir(lpszTarget);
	
	TCHAR szObjFile[MAX_PATH] = {0};
	BOOL  bHasSep = (lpszTarget[lstrlen(lpszTarget)-1] == '\\' ? TRUE : FALSE);
	
	WIN32_FIND_DATA finder;
	HANDLE hFinder = FindFirstFile(lpszSource, &finder);
	while (hFinder != INVALID_HANDLE_VALUE)
	{
		//OutputDebugString(finder.cFileName);
		if (!FindNextFile(hFinder, &finder))
			break;
		if(finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			continue;
		
		if(bHasSep)
			wsprintf(szObjFile, _T("%s%s"),lpszTarget,_tcsrchr(finder.cFileName, '\\') + 1);
		else
			wsprintf(szObjFile,_T("%s\\%s"),lpszTarget, _tcsrchr(finder.cFileName, '\\') + 1);
		
		if(!CopyFile(finder.cFileName,szObjFile,bFailedIfExist))
			return FALSE;		
	}
	FindClose(hFinder);	
	return TRUE;
}

__inline void GetModuleDir(LPTSTR lpDir)
{
	TCHAR szBuff[MAX_PATH];
	GetModuleFileName(NULL, szBuff, sizeof(szBuff)); 
	
	int pos = _tcslen(szBuff) - 1;
	for (;pos;pos--)
	{
		if (szBuff[pos] == '\\')
		{
			break;
		}
	}
	
	if(pos)
	{
		szBuff[pos+1] = _T('\0');
	}
	_tcscpy_s(lpDir, MAX_PATH, szBuff);
}


// void LOGFILE(LPCTSTR lpszFormat, ...)
// {
// 	FILE* fp
// 	va_list va;
// 	va_start(va, lpszFormat);
// 	
// 	vfprintf()
// 	va_end(va);
// }

class CComInit
{
public:
	CComInit()
	{
		CoInitialize(NULL);
	}
	~CComInit()
	{
		CoUninitialize();
	}
};



class CFileHandle
{
public:
	CFileHandle(LPCTSTR szFile)
	{
		SECURITY_ATTRIBUTES sa;  
		SECURITY_DESCRIPTOR sd;  
		LPSECURITY_ATTRIBUTES lpSa = NULL;
		if (_taccess(szFile, 0) != 0)
		{
			//如果文件不存在则以低权限创建文件
			sa.nLength = sizeof(sa);  
			sa.bInheritHandle = FALSE;  
			sa.lpSecurityDescriptor = &sd;  
			lpSa = &sa;
			
			InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);  
			SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);  
		}
		
		m_hFile = CreateFile(szFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, lpSa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	~CFileHandle()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
		}
	}
	operator HANDLE() const
	{
		return m_hFile;
	}
	HANDLE m_hFile;
};

//作用域内自动修改某变量值
template<class T>
class CAutoChange
{
public:
	CAutoChange(T& t, const T value1, const T value2):m_t(t), m_value1(value1), m_value2(value2)
	{
		t = value1;
	}
	~CAutoChange()
	{
		m_t = m_value2;
	}
	T& m_t;
	const T m_value1;
	const T m_value2;
};

class CMutex
{
public:
	CMutex()
	{
		m_cs = CreateMutex(NULL, FALSE, NULL);
	}
	~CMutex()
	{
		CloseHandle(m_cs);
	}
	DWORD Wait(DWORD dwTime = INFINITE)
	{
		return WaitForSingleObject(m_cs, dwTime);
	}
	void Lock()
	{
		Wait(INFINITE);
	}
	void Unlock()
	{
		ReleaseMutex(m_cs);
	}
private:
	HANDLE m_cs;

};

class CEvent
{
public:
	CEvent()
	{
		m_cs = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	~CEvent()
	{
		CloseHandle(m_cs);
	}
	DWORD Wait(DWORD dwTime = INFINITE)
	{
		return WaitForSingleObject(m_cs, dwTime);
	}
	void ResetEvent()
	{
		::ResetEvent(m_cs);
	}
	void SetEvent()
	{
		::SetEvent(m_cs);
	}
private:
	HANDLE m_cs;

};

class CCriticalSection
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCriticalSection()
	{

	}
	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}
	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION m_cs;

};


template<class T>
class CAutoLock
{
public:
	CAutoLock(T* pCs)
	{
		if (pCs)
		{
			m_pCs = pCs;
			pCs->Lock();
		}
	}
	~CAutoLock()
	{
		if (m_pCs)
		{
			m_pCs->Unlock();
		}
	}
	T* m_pCs;
};

//原子操作类型
template<class T>
class CAtomValue
{
public:
	CAtomValue()
	{
		InitializeCriticalSection(&m_cs);
	}
	CAtomValue(T value)
	{
		InitializeCriticalSection(&m_cs);
		m_value = value;
	}
	operator T()
	{
		EnterCriticalSection(&m_cs);
		LeaveCriticalSection(&m_cs);
		return m_value;
	}
	T operator = (T value)
	{
		EnterCriticalSection(&m_cs);
		m_value = value;
		LeaveCriticalSection(&m_cs);
		return value;
	}
private:
	CRITICAL_SECTION m_cs;
	T	m_value;
	
};

typedef CAtomValue<BOOL>	CAtomBool;
#define AUTOCHANGE(type, t, v1, v2)	CAutoChange<type> __XXX(t, v1, v2)

__inline TCHAR charupr(TCHAR c)
{
	return c >= 'a' && c <= 'z' ? c -= 32 : c;
}
__inline int _tchricmp(TCHAR c1, TCHAR c2)
{
	return (int)charupr(c1) - (int)charupr(c2);
}
}
#endif //ifdef  _UTIL_H_14527_
