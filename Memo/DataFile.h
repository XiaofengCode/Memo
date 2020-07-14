#pragma once
#pragma pack(1)
typedef struct  
{
	RECT rcPos;
	SYSTEMTIME stAlarm;
	DWORD dwContentLen;
}DF_ITEM, *PDF_ITEM;
#pragma pack()

typedef struct
{
	DF_ITEM dfItem;
	CString strContent;
}MEMO_ITEM, *PMEMO_ITEM;
class CDataFile
{
public:
	CDataFile(void);
	~CDataFile(void);
	bool Init(LPCTSTR lpszFileName);
	bool Save();
	bool ModifyItem(WORD nItem, RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent);
	bool RemoveItem(WORD nItem);
	WORD AddItem(RECT rcPos, SYSTEMTIME stAlarm, LPCTSTR lpszContent);
	CMapWordToPtr m_mapItem;
	WORD m_wNewKey;
	CString m_strFile;
	Zutil::CCriticalSection m_sec;
};

