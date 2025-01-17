// MainDlg.h : interface of the flv_mergeDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "ThreadObject.h"
#include "flv_mergeSrc.h"
using namespace std;

class flv_mergeDlg : public SHostWnd
					, public CThreadObject	//线程对象
					, public TAutoEventMapReg<flv_mergeDlg>//通知中心自动注册
{
public:
	flv_mergeDlg();
	~flv_mergeDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	bool OnEventThread(EventArgs *e);

	bool m_bCnLang; //语言
	char m_outfileStr[256];
	SListBox *m_pflvlist;
	SProgress *m_pProgress;
protected:
	//Event Sender ID
	enum { SENDER_ID = 30000 };
	virtual int GetID() const { return SENDER_ID; }
	virtual UINT Run();

	BOOL BrowseDir(SStringT &path, SStringT title);
	BOOL OpenFile(LPCWSTR lpstrFilter, vector<SStringT> &fileNames, bool IsSave);
	void OnAddFlv();
	void RemoveBnClickedButton();
	void ClearBnClickedButton();
	void OutputBnClickedButton();
	void JoinBnClickedButton();
	void CancelBnClickedButton();
	void ExitBnClickedButton();
	void OnLanguage(int nID);
	bool OnSwitchLanguage(EventArgs *e);
	
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_ID_COMMAND(R.id.add_flv, OnAddFlv)
		EVENT_ID_COMMAND(R.id.remove_flv, RemoveBnClickedButton)
		EVENT_ID_COMMAND(R.id.clear_list, ClearBnClickedButton)	
		EVENT_ID_COMMAND(R.id.choose_path, OutputBnClickedButton)
		EVENT_ID_COMMAND(R.id.btn_generate, JoinBnClickedButton)
		EVENT_ID_COMMAND(R.id.btn_cancel, OutputBnClickedButton)
		EVENT_ID_COMMAND(R.id.btn_exit, ExitBnClickedButton)
		//<--通知中心事件
		EVENT_ID_HANDLER(SENDER_ID, EventThread::EventID, OnEventThread)
	EVENT_MAP_END()
		
	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(flv_mergeDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;	
};
