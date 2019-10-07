// DialogGateServerConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "DialogGateServerConfig.h"
#include "afxdialogex.h"


// CDialogGateServerConfig 对话框

IMPLEMENT_DYNAMIC(CDialogGateServerConfig, CDialogEx)

CDialogGateServerConfig::CDialogGateServerConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_GATE_SERVER_CONFIG, pParent)
	, m_IdStart(0)
	, m_Ip(_T(""))
	, m_PortStart(0)
	, m_iGaoFangType(0)
	, m_nServerCount(0)
{

}

CDialogGateServerConfig::~CDialogGateServerConfig()
{
}

void CDialogGateServerConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GATE_ID, m_IdStart);
	DDX_Text(pDX, IDC_EDIT_GATE_IP, m_Ip);
	DDX_Text(pDX, IDC_EDIT_GATE_PORT, m_PortStart);
	DDX_Radio(pDX, IDC_RADIO_GATE_TYPE_NORMAL, m_iGaoFangType);
	DDX_Text(pDX, IDC_EDIT_GATE_NAME, m_nServerCount);
}


BEGIN_MESSAGE_MAP(CDialogGateServerConfig, CDialogEx)
END_MESSAGE_MAP()


// CDialogGateServerConfig 消息处理程序
