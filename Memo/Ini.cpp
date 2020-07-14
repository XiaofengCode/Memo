// Ini.cpp: implementation of the CIni class.
// Author: Bjarke Viksøe
//
// Description:
// Thin wrapper around the Win32 Windows Profile (Ini-file configuration)
// interface.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni()
{
}

CIni::CIni(LPCTSTR IniFilename)
{
	OpenIniFile(IniFilename);
}

CIni::~CIni()
{
	Clear();
}

void CIni::Clear()
{
	POSITION pos = m_mapSectionToKeyValue.GetStartPosition();
	CString strKey;
	CMapStringToString* pValue;
	while (pos)
	{
		m_mapSectionToKeyValue.GetNextAssoc(pos, strKey, (PVOID&)pValue);
		delete pValue;
	}
	m_mapSectionToKeyValue.RemoveAll();
}

BOOL CIni::OpenIniFile( LPCTSTR lpFileName )
{
	Clear();
	BOOL bRet = TRUE;
// 	CStdioFile file;
// 	if (!file.Open(lpFileName, CStdioFile::modeRead, NULL, NULL))
// 		return FALSE;
	FILE* fp = _tfopen(lpFileName, _T("rt"));
	if (!fp)
	{
		return FALSE;
	}
	char szLine[MAX_PATH * 2];
	CString strLine;
	CMapStringToString* pMapKey2Value = NULL;
	while (!feof(fp))
	{
		strLine = fgets(szLine, sizeof(szLine), fp);
		//strLine = (char*)strLine.GetBuffer(0);
		int nComment = strLine.Find(';');
		if (nComment >= 0)
		{
			strLine.Delete(nComment, strLine.GetLength() - nComment);
		}
		strLine.TrimLeft();
		strLine.TrimRight();
		if (strLine.GetLength() == 0)
		{
			continue;
		}
		if (strLine[0] == '[' && strLine[strLine.GetLength() - 1] == ']')
		{
			strLine.Delete(0);
			strLine.Delete(strLine.GetLength() - 1);
			pMapKey2Value = new CMapStringToString();
			m_mapSectionToKeyValue[strLine] = pMapKey2Value;
			continue;
		}
		if (!pMapKey2Value)
		{
			bRet = FALSE;
			goto _exit;
		}
		int nPos = strLine.Find('=');
		if (nPos <= 0)
		{
			bRet = FALSE;
			goto _exit;
		}
		(*pMapKey2Value)[strLine.Left(nPos).TrimRight().MakeLower()] = strLine.Right(strLine.GetLength() - nPos - 1).TrimLeft();
	}
_exit:
	//file.Close();
	fclose(fp);
	m_strFileName = lpFileName;
	return bRet;
}

CMapStringToString& CIni::operator[]( LPCTSTR lpszSection )
{
	CMapStringToString* pMap = (CMapStringToString*)m_mapSectionToKeyValue[lpszSection];
	if (!pMap)
	{
		POSITION pos = GetSectionStartPosition();
		CString strSection;
		while (pos)
		{
			GetNextSection(pos, strSection);
			if (strSection.CompareNoCase(lpszSection) == 0)
			{
				pMap = (CMapStringToString*)m_mapSectionToKeyValue[strSection];
				if (pMap)
				{
					return *pMap;
				}
			}
		}
		return m_mapEmpty;
	}
	return *pMap;
}

POSITION CIni::GetSectionStartPosition()
{
	return m_mapSectionToKeyValue.GetStartPosition();
}

void CIni::GetNextSection( POSITION& pos, CString& strSection )
{
	PVOID pValue;
	m_mapSectionToKeyValue.GetNextAssoc(pos, strSection, pValue);
}

BOOL CIni::Write( LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszFile )
{
	return WritePrivateProfileString(lpszSection, lpszKey, lpszValue, lpszFile);
}

BOOL CIni::Write(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	CMapStringToString* pMap = (CMapStringToString*)m_mapSectionToKeyValue[lpszSection];
	if (!pMap)
	{
		(*pMap)[lpszKey] = lpszValue;
	}
	return Write(lpszSection, lpszKey, lpszValue, m_strFileName);
}

