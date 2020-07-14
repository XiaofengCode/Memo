#include "StdAfx.h"
#include "DataFile.h"


CDataFile::CDataFile(void)
{
}


CDataFile::~CDataFile(void)
{
	Save();
}

bool CDataFile::Init(LPCTSTR lpszFileName)
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_sec);
	m_strFile = lpszFileName;
	CFile file;
	if (!file.Open(lpszFileName, CFile::modeReadWrite | CFile::typeBinary))
	{
		return false;
	}
	DF_ITEM item;
	m_wNewKey = 0;
	while (1)
	{
		if (file.Read(&item, sizeof(DF_ITEM)) != sizeof(DF_ITEM))
		{
			break;
		}
		PMEMO_ITEM pItem = new MEMO_ITEM();

// 		CString strDbg;
// 		strDbg.Format(_T("3.%p\n"), pItem);
// 		OutputDebugString(strDbg);

		memcpy(&pItem->dfItem, &item, sizeof(DF_ITEM));
		if (file.Read(pItem->strContent.GetBuffer(item.dwContentLen/sizeof(TCHAR)), item.dwContentLen) != item.dwContentLen)
		{
			delete pItem;
			break;
		}
		m_mapItem[m_wNewKey++] = pItem;
	}
	file.Close();
	return true;
}

bool CDataFile::Save()
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_sec);
	CFile file;
	if (!file.Open(m_strFile, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary))
	{
		return false;
	}
	POSITION pos = m_mapItem.GetStartPosition();
	while (pos)
	{
		WORD wKey;
		PVOID pValue;
		m_mapItem.GetNextAssoc(pos, wKey, pValue);
		PMEMO_ITEM pItem = (PMEMO_ITEM)pValue;
		if (pItem->strContent.GetLength() == 0)
		{
			continue;
		}
		try
		{
			file.Write(&pItem->dfItem, sizeof(DF_ITEM));
			file.Write(pItem->strContent.GetBuffer(0), pItem->dfItem.dwContentLen);
		}
		catch (CFileException* e)
		{
			TCHAR szErr[MAX_PATH];
			if (e->GetErrorMessage(szErr, MAX_PATH))
			{
				MessageBox(0, szErr, 0, 0);
			}
			else
			{
				MessageBox(0, _T("±£¥Ê ß∞‹£°"), 0, 0);
			}
			return false;
			
		}
		catch (CException* e)
		{
			TCHAR szErr[MAX_PATH];
			if (e->GetErrorMessage(szErr, MAX_PATH))
			{
				MessageBox(0, szErr, 0, 0);
			}
			else
			{
				MessageBox(0, _T("±£¥Ê ß∞‹£°"), 0, 0);
			}
			return false;
		}
	}
	file.Close();
	return true;
}

bool CDataFile::ModifyItem(WORD nItem, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent)
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_sec);
	PMEMO_ITEM pItem;
	if (!m_mapItem.Lookup(nItem, (void*&)pItem))
	{
		return false;
	}
// 	CString strDbg;
// 	strDbg.Format(_T("2.%p\n"), pItem);
// 	OutputDebugString(strDbg);
	pItem->dfItem.rcPos = rcPos;
	pItem->dfItem.stAlarm = stAlarm;
	pItem->dfItem.dwContentLen = (_tcslen(lpszContent) + 1) * sizeof(TCHAR);
	pItem->strContent = lpszContent;
	return true;
}

bool CDataFile::RemoveItem(WORD nItem)
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_sec);
	PMEMO_ITEM pItem;
	if (!m_mapItem.Lookup(nItem, (void*&)pItem))
	{
		return false;
	}
	delete pItem;
	return m_mapItem.RemoveKey(nItem) == TRUE;
}

WORD CDataFile::AddItem(RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent)
{
	Zutil::CAutoLock<Zutil::CCriticalSection> _lock(&m_sec);
	PMEMO_ITEM pItem = new MEMO_ITEM();
	m_mapItem[m_wNewKey] = pItem;

// 	CString strDbg;
// 	strDbg.Format(_T("1.%p\n"), p);
// 	OutputDebugString(strDbg);

	if (ModifyItem(m_wNewKey, rcPos, stAlarm, lpszContent))
	{
		return m_wNewKey++;
	}
	return (WORD) -1;
}
