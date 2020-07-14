// Ini.h: interface for the CIni class.
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2000.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_)
#define AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//
// INI file class
//
// Author:
// Bjarke Viksøe
// Description:
// Implements helper functions to access
// an .INI configuration file using
// conventional CString operations
//
#define  MAX_ALLSECTIONS 255
#define  MAX_SECTION 255
// Ini-file wrapper class
class CIni// : public CObject  
{
public:
	CIni();
	CIni( LPCTSTR IniFilename );
	virtual ~CIni();

	// Methods
public:
	// Sets the current Ini-file to use.
	BOOL OpenIniFile(LPCTSTR lpFileName);
	void Clear();
	POSITION GetSectionStartPosition();
	void GetNextSection(POSITION& pos, CString& strSection);
	static BOOL Write(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszFile);
	BOOL Write(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue);
	// Variables
	CMapStringToString& operator[](LPCTSTR lpszSection);
protected:
	CMapStringToPtr m_mapSectionToKeyValue;
	CMapStringToString m_mapEmpty;
	CString m_strFileName;
};

#endif // !defined(AFX_INI_H__2478E9E2_E904_11D1_93C1_241C08C10000__INCLUDED_)