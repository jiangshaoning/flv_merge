// MainDlg.cpp : implementation of the flv_mergeDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flv_mergeDlg.h"
#include <shlobj.h>
#include "../../controls.extend/FileHelper.h"

flv_mergeDlg::flv_mergeDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
	m_bCnLang = FALSE;
}

flv_mergeDlg::~flv_mergeDlg()
{
}

int flv_mergeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL flv_mergeDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	SComboBox *cbx_language = FindChildByID2<SComboBox>(R.id.cbx_language);
	cbx_language->GetEventSet()->subscribeEvent(EventCBSelChange::EventID, Subscriber(&flv_mergeDlg::OnSwitchLanguage, this));
	return 0;
}

bool flv_mergeDlg::OnSwitchLanguage(EventArgs *e)
{	
	EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
	SComboBox *cbx_language = (SComboBox*)e->sender;
	OnLanguage(cbx_language->GetCurSel());
	return true;
}
void flv_mergeDlg::OnLanguage(int nID)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);
	m_bCnLang = nID == 1;

	pugi::xml_document xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, m_bCnLang ? _T("lang_eg") : _T("lang_cn"), _T("translator")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
		pTransMgr->SetLanguage(lang->name());
		pTransMgr->InstallTranslator(lang);
		SDispatchMessage(UM_SETLANGUAGE,0,0);
	}
}

//TODO:消息映射
void flv_mergeDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void flv_mergeDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void flv_mergeDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void flv_mergeDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void flv_mergeDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

BOOL flv_mergeDlg::BrowseDir(SStringT &path, SStringT title)
{
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = ::GetActiveWindow();//父窗口  
	bInfo.lpszTitle = title;
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
		BIF_UAHINT/*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//单击了确定按钮  
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		path.Format(_T("%s"), szPathName);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL flv_mergeDlg::OpenFile(LPCWSTR lpstrFilter, vector<SStringT> &fileNames, bool IsSave)
{
	DWORD dwFlag = IsSave ? OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT : OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_READONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	TCHAR szFileName[MAX_PATH * 101 + 1] = _T("");
	if (IsSave) lstrcpy(szFileName, _T("join.flv"));

	OPENFILENAME openfilename = { 0 };

	ZeroMemory(&openfilename, sizeof(OPENFILENAME));

	SStringT s_title;

	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = ::GetActiveWindow();
	openfilename.hInstance = NULL;
	openfilename.lpstrFilter = lpstrFilter;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0L;
	openfilename.nFilterIndex = 1L;
	openfilename.lpstrFile = szFileName;
	openfilename.nMaxFile = MAX_PATH * 101 + 1;
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = NULL;
	openfilename.lpstrTitle = s_title;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	openfilename.lpstrDefExt = _T("*.*");
	openfilename.lCustData = 0;
	openfilename.Flags = dwFlag;

	// 弹出打开文件的对话框
	SStringT str;

	if (IsSave ? ::GetSaveFileName(&openfilename):(::GetOpenFileName(&openfilename)))
	{
		LPTSTR p = szFileName;
		SStringT TempPath;
		if (*p != NULL)
		{
			TempPath = p;
			p += TempPath.GetLength() + 1;
		}

		if (*p == NULL)
		{
			//	TempPath = TempPath.Left(TempPath.ReverseFind(L'\\'));
			fileNames.push_back(TempPath);
		}

		while (*p != NULL)
		{
			SStringT str = p;
			p += str.GetLength() + 1;
			fileNames.push_back(TempPath + _T("\\") + str);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void flv_mergeDlg::OnAddFlv()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<SStringT> filename;
	vector<SStringT>::iterator it;
	TCHAR szFilter[] = _T("flv文件(*.flv)\0*.flv\0所有文件(*.*)\0*.*\0\0");
	if (OpenFile(szFilter, filename, FALSE))
	{
		SListBox *pFL = FindChildByID2<SListBox>(R.id.flv_list);
		for (it = filename.begin(); it != filename.end(); it++)
		{
			int iItem = pFL->AddString(SStringT().Format(_T("%s"), *it));
			pFL->EnsureVisible(iItem);
		}
		pFL->UpdateWindow();
	}
}


void flv_mergeDlg::RemoveBnClickedButton()
{
	SListBox *pflvlist = FindChildByID2<SListBox>(R.id.flv_list);
	pflvlist->DeleteString(pflvlist->GetCurSel());
}

void flv_mergeDlg::ClearBnClickedButton()
{
	SListBox *pflvlist = FindChildByID2<SListBox>(R.id.flv_list);
	pflvlist->DeleteAll();
}

void flv_mergeDlg::JoinBnClickedButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//char outfileStr[256] = { 0 };
	//FLVContext *flvCtx;
	int len = 0;

	SEdit *pOutfile = FindChildByID2<SEdit>(R.id.outfile_path);
	m_pflvlist = FindChildByID2<SListBox>(R.id.flv_list);
	m_pProgress = FindChildByID2<SProgress>(R.id.merge_progress);
	SStringT outFilePath = pOutfile->GetWindowText();

	len = outFilePath.GetLength() + 1;

	if (len == 1)
	{
		MessageBoxEx(NULL, S_CW2T(TR(L"output_message", L"messagebox")), _T(""), MB_OK, m_bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) : NULL);
		return;
	}

	WideCharToMultiByte(CP_ACP, 0, outFilePath, -1, m_outfileStr, 256, NULL, NULL);
	SetTaskCancel(0);

	if (!m_pflvlist->GetCount())
	{
		MessageBoxEx(NULL, S_CW2T(TR(L"addflv_message", L"messagebox")), _T(""), MB_OK, m_bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) : NULL);
		return;
	}
	m_pProgress->SetRange(0, 1000);
	//HANDLE hThrd = CreateThread(NULL, 0, merge_thread, (LPVOID)this, 0, NULL);
	if (IsRunning()) return;
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventThread));

	EventThread evt(this);
	SNotifyCenter::getSingleton().FireEventSync(&evt);
	BeginThread();
}

void flv_mergeDlg::OutputBnClickedButton()
{
	vector<SStringT> filename;
	TCHAR szFilter[] = _T("flv文件(*.flv)\0*.flv\0所有文件(*.*)\0*.*\0\0");
	if (OpenFile(szFilter, filename, TRUE))
	{
		SEdit *edit_outfilepath = FindChildByID2<SEdit>(R.id.outfile_path);
		edit_outfilepath->SetWindowTextW(filename[0]);
	}
}

void flv_mergeDlg::CancelBnClickedButton()
{
	// TODO: 在此添加控件通知处理程序代码
	SetTaskCancel(1);
}

void flv_mergeDlg::ExitBnClickedButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBoxEx(NULL, S_CW2T(TR(L"exit_message", L"messagebox")), _T(""), MB_YESNO | MB_ICONQUESTION, m_bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US):NULL) == IDYES){
		this->SendMessage(WM_CLOSE);
	}
}

UINT flv_mergeDlg::Run()
{
	if (!IsStoped())
	{
		EventThread *pEvt = new EventThread(this);
		FLVContext *flvCtx;
		MergeTaskList mergeTaskList;
		int i;
		if (InitMergeTaskList(&mergeTaskList) != 0)
			return 0;

		pEvt->bCnLang = m_bCnLang;
		for (i = 0; i<m_pflvlist->GetCount(); i++)
		{
			char fileStr[256] = { 0 };
			SStringT strFilePath = m_pflvlist->GetText(i, FALSE);
			WideCharToMultiByte(CP_ACP, 0, strFilePath, -1, fileStr, 256, NULL, NULL);

			if (InitFLVContext(&flvCtx, fileStr, m_bCnLang) != 0)
			{
				fprintf(stderr, "failed when init file %s!\n", strFilePath);
				goto failed;
			}
			else
			{
				mergeTaskList.add_task(&mergeTaskList, flvCtx);
			}
		}
		if (mergeTaskList.do_merge_tasks(&mergeTaskList, m_outfileStr, pEvt) == 0) {
			fprintf(stderr, "-----congratulations, merge success!------\n");
		}
		else {
			fprintf(stderr, "failed!!!\n");
		}

	failed:
		pEvt->Release();
		mergeTaskList.clear_tasks(&mergeTaskList);
	}
	return 0;
}

bool flv_mergeDlg::OnEventThread(EventArgs *e)
{
	EventThread *pEvt = sobj_cast<EventThread>(e);

	m_pProgress->SetValue(pEvt->nData);
	m_pProgress->SetWindowTextW(pEvt->value);
	return true;
}

