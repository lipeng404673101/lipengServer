// DialogManagerConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "DialogManagerConfig.h"
#include "afxdialogex.h"


// CDialogManagerConfig 对话框

IMPLEMENT_DYNAMIC(CDialogManagerConfig, CDialogEx)

CDialogManagerConfig::CDialogManagerConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MANAGER_CONFIG, pParent)
	, m_Id(0)
	, m_Ip(_T(""))
	, m_Name(_T(""))
	, m_Port(0)
	, m_bClub(FALSE)
{

}

CDialogManagerConfig::~CDialogManagerConfig()
{
}

void CDialogManagerConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MANAGER_ID, m_Id);
	DDX_Text(pDX, IDC_EDIT_MANAGER_IP, m_Ip);
	DDX_Text(pDX, IDC_EDIT_MANAGER_NAME, m_Name);
	DDX_Text(pDX, IDC_EDIT_MANAGER_PORT, m_Port);
	DDX_Radio(pDX, IDC_RADIO_MANAGER_TYPE_PERSONAL, m_bClub);
}


BEGIN_MESSAGE_MAP(CDialogManagerConfig, CDialogEx)
END_MESSAGE_MAP()


// CDialogManagerConfig 消息处理程序
