#pragma once
class CDTWnd : public CWindowWnd
{
public:
	CDTWnd();

	void Init(CMemoWnd* pWnd, CLabelUI* pOwner);

	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CMemoWnd* m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;

	int        m_nDTUpdateFlag;
};