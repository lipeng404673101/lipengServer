// DialogLogicServerConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "DialogLogicServerConfig.h"
#include "afxdialogex.h"


// CDialogLogicServerConfig 对话框

IMPLEMENT_DYNAMIC(CDialogLogicServerConfig, CDialogEx)

CDialogLogicServerConfig::CDialogLogicServerConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOGIC_SERVER, pParent)
	, m_nServerCount(0)
	, m_nServerIdStart(0)
	, m_strServerIP(_T(""))
	, m_nServerPortStart(0)
	, m_nLogicServerType(0)
{

}

CDialogLogicServerConfig::~CDialogLogicServerConfig()
{
}

void CDialogLogicServerConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIC_COUNT, m_nServerCount);
	DDX_Text(pDX, IDC_EDIT_LOGIC_ID, m_nServerIdStart);
	DDX_Text(pDX, IDC_EDIT_LOGIC_IP, m_strServerIP);
	DDX_Text(pDX, IDC_EDIT_LOGIC_PORT, m_nServerPortStart);
	DDX_Radio(pDX, IDC_RADIO_LOGIC_TYPE, m_nLogicServerType);
}


BEGIN_MESSAGE_MAP(CDialogLogicServerConfig, CDialogEx)
END_MESSAGE_MAP()


// CDialogLogicServerConfig 消息处理程序
