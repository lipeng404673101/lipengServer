// DailogMainWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "DailogMainWnd.h"
#include "afxdialogex.h"


// CDailogMainWnd 对话框

IMPLEMENT_DYNAMIC(CDailogMainWnd, CDialogEx)

CDailogMainWnd::CDailogMainWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MAIN, pParent)
{

}

CDailogMainWnd::~CDailogMainWnd()
{
}

void CDailogMainWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SERVER_CONFIG, m_ctrlTab);
}


BEGIN_MESSAGE_MAP(CDailogMainWnd, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SERVER_CONFIG, &CDailogMainWnd::OnSelchangeTabServerConfig)	
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SXMAIN_LOAD_CONFIG, &CDailogMainWnd::OnBnClickedButtonSxmainLoadConfig)
	ON_BN_CLICKED(IDC_BUTTON_SXMAIN_SAVE_CONFIG, &CDailogMainWnd::OnBnClickedButtonSxmainSaveConfig)
END_MESSAGE_MAP()


// CDailogMainWnd 消息处理程序


BOOL CDailogMainWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_dlgLogicGateMap.Create(IDD_DIALOG_LOGIC_GATE_MAP, this);
	m_dlgLogicGateMap.ShowWindow(SW_HIDE);
	m_dlgServerConfig.Create(IDD_SXLAUNCHER_DIALOG, this);
	m_dlgServerConfig.ShowWindow(SW_HIDE);
	// TODO:  在此添加额外的初始化
	if (::IsWindow(m_ctrlTab.GetSafeHwnd()))
	{
		CRect rect;
		GetClientRect(&rect);
		m_ctrlTab.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), 25, SWP_NOZORDER);
		m_dlgServerConfig.SetWindowPos(NULL, rect.left, rect.top + 25, rect.Width(), rect.Height() - 25 - 40, SWP_NOZORDER);
		m_dlgLogicGateMap.SetWindowPos(NULL, rect.left, rect.top + 25, rect.Width(), rect.Height() - 25 - 40, SWP_NOZORDER);
				

		m_ctrlTab.InsertItem(0, L"服务器配置");	
		m_ctrlTab.InsertItem(1, L"棋牌室配置");
		m_ctrlTab.SetCurSel(0);

		m_dlgServerConfig.ShowWindow(SW_NORMAL);

		CWnd * pWnd = NULL;
		pWnd = GetDlgItem(IDC_BUTTON_SXMAIN_LOAD_CONFIG);		 
		pWnd->SetWindowPos(NULL, rect.left, rect.bottom - 40, 100, 40, SWP_NOZORDER);
		pWnd = GetDlgItem(IDC_BUTTON_SXMAIN_SAVE_CONFIG);
		pWnd->SetWindowPos(NULL, rect.left + 100, rect.bottom - 40, 100, 40, SWP_NOZORDER);
	}

	::GetCurrentDirectory(MAX_PATH, m_szCurrentWorkPath);
	m_strConfigFile.Format(L"%s\\launcher_config.ini", m_szCurrentWorkPath);
	
	m_dlgServerConfig.m_strLauncherConfigFile = m_strConfigFile;
	ServerConfigHelper::GetInstance().SetConfigFilePath(m_strConfigFile);

	OnBnClickedButtonSxmainLoadConfig();
	m_dlgServerConfig.UpdateData(FALSE);
	m_dlgLogicGateMap.UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDailogMainWnd::OnSelchangeTabServerConfig(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//获取所选择的页面  
	int nCurSel = m_ctrlTab.GetCurSel();
	if (nCurSel == 0)
	{
		//页面一显示，页面二隐藏  
		m_dlgServerConfig.ShowWindow(SW_NORMAL);
		m_dlgLogicGateMap.ShowWindow(SW_HIDE);
	}
	else if (nCurSel == 1)
	{
		////页面二显示，页面一隐藏  
		m_dlgLogicGateMap.ShowWindow(SW_NORMAL);
		m_dlgServerConfig.ShowWindow(SW_HIDE);
	}	
	*pResult = 0;
}



void CDailogMainWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_dlgServerConfig.OnClickedButtonSaveConfig();
	m_dlgLogicGateMap.OnBnClickedButtonMapSaveConfig();


	m_dlgServerConfig.SendMessage(WM_CLOSE);	
	CDialogEx::OnClose();
}


void CDailogMainWnd::OnBnClickedButtonSxmainLoadConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgServerConfig.OnClickedButtonLoadConfig();
	m_dlgLogicGateMap.OnBnClickedButtonMapLoadConfig();
}


void CDailogMainWnd::OnBnClickedButtonSxmainSaveConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dlgServerConfig.OnClickedButtonSaveConfig();
	m_dlgLogicGateMap.OnBnClickedButtonMapSaveConfig();
}
