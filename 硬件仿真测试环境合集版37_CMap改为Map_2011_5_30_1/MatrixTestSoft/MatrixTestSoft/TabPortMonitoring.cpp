// PortMonitoring.cpp : 实现文件
//

#include "stdafx.h"
#include "MatrixTestSoft.h"
#include "TabPortMonitoring.h"


// CPortMonitoring 对话框

IMPLEMENT_DYNAMIC(CTabPortMonitoring, CDialog)

CTabPortMonitoring::CTabPortMonitoring(CWnd* pParent /*=NULL*/)
	: CDialog(CTabPortMonitoring::IDD, pParent)
	, m_uiRcvPort1(0)
	, m_uiSendPort1(0)
	, m_uiRcvPort2(0)
	, m_uiSendPort2(0)
	, m_csIPRec(_T(""))
	, m_csIPSend(_T(""))
	, m_iSaveSize(0)
	, m_bPortDistribution(FALSE)
	, m_pLogFile(NULL)
{

}

CTabPortMonitoring::~CTabPortMonitoring()
{
}

void CTabPortMonitoring::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PORTMONITORING, m_cTabCtrlItems);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPctrlRec);
	DDX_Control(pDX, IDC_IPADDRESS2, m_IPctrlSend);
	DDX_Check(pDX, IDC_CHECK_PORTDISTRIBUTION, m_bPortDistribution);
}


BEGIN_MESSAGE_MAP(CTabPortMonitoring, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PORTMONITORING, &CTabPortMonitoring::OnTcnSelchangeTabPortmonitoring)
	ON_BN_CLICKED(IDC_BUTTON_PORTMONITORING_OPEN, &CTabPortMonitoring::OnBnClickedButtonPortmonitoringOpen)
	ON_BN_CLICKED(IDC_BUTTON_PORTMONITORING_CLOSE, &CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose)
	ON_BN_CLICKED(IDC_BUTTON_STARTSAVE, &CTabPortMonitoring::OnBnClickedButtonStartsave)
	ON_BN_CLICKED(IDC_BUTTON_STOPSAVE, &CTabPortMonitoring::OnBnClickedButtonStopsave)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILEPATH, &CTabPortMonitoring::OnBnClickedButtonSavefilepath)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTabPortMonitoring::OnBnClickedButtonReset)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_SHOWMESSAGE, &CTabPortMonitoring::OnBnClickedCheckShowmessage)
END_MESSAGE_MAP()


// 初始化选项卡控件
//************************************
// Method:    InitTabControlItems
// FullName:  CTabPortMonitoring::InitTabControlItems
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabPortMonitoring::InitTabControlItems(void)
{
	CRect rs = 0;
	CString str = _T("");
	// 创建选项卡
	m_oPortMonitorRec.Create(IDD_DIALOG_PORTMONITORING_REC, GetDlgItem(IDC_TAB_PORTMONITORING));
	m_oPortMonitorSend.Create(IDD_DIALOG_PORTMONITORING_SEND,GetDlgItem(IDC_TAB_PORTMONITORING));
	

	//获得IDC_TABTEST客户区大小
	m_cTabCtrlItems.GetClientRect(&rs);
	//调整子对话框在父窗口中的位置
	rs.top += 20; 

	//设置子对话框尺寸并移动到指定位置
	m_oPortMonitorRec.MoveWindow(&rs);
	m_oPortMonitorSend.MoveWindow(&rs);

	m_oPortMonitorRec.ScreenToClient(&rs);
	m_oPortMonitorSend.ScreenToClient(&rs);

	//分别设置隐藏和显示
	m_oPortMonitorRec.ShowWindow(TRUE);
	m_oPortMonitorSend.ShowWindow(FALSE);
	
	//设置默认的选项卡
	m_cTabCtrlItems.SetCurSel(TabPortMonitoringRecCtl); 

	str = _T("接收帧");
	m_cTabCtrlItems.InsertItem(TabPortMonitoringRecCtl, str);		// 添加参数一选项卡
	str = _T("发送帧");
	m_cTabCtrlItems.InsertItem(TabPortMonitoringSndCtl, str);		// 添加参数二选项卡
}

void CTabPortMonitoring::OnTcnSelchangeTabPortmonitoring(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	int CurSel = m_cTabCtrlItems.GetCurSel();
	switch(CurSel)
	{
	case TabPortMonitoringRecCtl:
		m_oPortMonitorSend.ShowWindow(FALSE);
		m_oPortMonitorRec.ShowWindow(TRUE); 
		Invalidate(TRUE);
		break;
	case TabPortMonitoringSndCtl:
		m_oPortMonitorRec.ShowWindow(FALSE);
		m_oPortMonitorSend.ShowWindow(TRUE);
		Invalidate(TRUE);
		break;
	default:
		;
	}    
	*pResult = 0;
}

void CTabPortMonitoring::OnBnClickedButtonPortmonitoringOpen()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CString str = _T("");
	m_IPctrlRec.GetWindowText(m_csIPRec);
	m_IPctrlSend.GetWindowText(m_csIPSend);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->GetWindowText(str);			// 0x8202
	_stscanf_s(str,_T("%x"), &m_uiRcvPort1);
	m_oThreadRec.m_iRecPort = m_uiRcvPort1;
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->GetWindowText(str);			// 0x9002
	_stscanf_s(str,_T("%x"), &m_uiSendPort1);

	m_oThreadRec.m_SendToAddr.sin_family = AF_INET;											// 填充套接字地址结构
	m_oThreadRec.m_SendToAddr.sin_port = htons(static_cast<unsigned short>(m_uiSendPort1));
	m_oThreadRec.m_SendToAddr.sin_addr.S_un.S_addr = inet_addr(ConvertCStringToConstCharPointer(m_csIPRec));

	m_oThreadRec.ResumeThread();
	m_oThreadRec.OnOpen();

	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->GetWindowText(str);			// 0x9001
	_stscanf_s(str,_T("%x"), &m_uiRcvPort2);
	m_oThreadSend.m_iRecPort = m_uiRcvPort2;
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->GetWindowText(str);			// 0x8201
	_stscanf_s(str,_T("%x"), &m_uiSendPort2);
	m_oThreadSend.m_iSendPort = m_uiSendPort2;
	m_oThreadSend.m_csIP = m_csIPSend;

	m_oThreadSend.m_bPortDistribution = m_bPortDistribution;

	m_oThreadSend.ResumeThread();
	m_oThreadSend.OnOpen();
	
	GetDlgItem(IDC_EDIT_SAVESIZE)->GetWindowText(str);
	m_iSaveSize = _tstoi(str);
	m_oSaveFile.m_iSaveSize = m_iSaveSize;
	
	m_IPctrlRec.EnableWindow(FALSE);
	m_IPctrlSend.EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_PORTDISTRIBUTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SAVESIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PORTMONITORING_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PORTMONITORING_CLOSE)->EnableWindow(TRUE);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonPortmonitoringOpen"), _T("打开端口监视！"), SuccessStatus);
}

void CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CString csErrorCodeReturnShow = _T("");
	unsigned int uiDataErrorCount[InstrumentMaxCount];
	unsigned int uiCmdErrorCount[InstrumentMaxCount];
	unsigned int uiDataError = 0;
	unsigned int uiCmdError = 0;
	m_oThreadRec.SuspendThread();
	m_oThreadRec.OnStop();

	m_oThreadSend.SuspendThread();
	m_oThreadSend.OnStop();

	m_IPctrlRec.EnableWindow(TRUE);
	m_IPctrlSend.EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_PORTDISTRIBUTION)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SAVESIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->EnableWindow(TRUE);
	// 调试用，在停止采集后显示误码查询结果
	m_Sec_PortMonitor.Lock();
	memcpy(uiDataErrorCount, m_oThreadSend.m_uiDataErrorCount, InstrumentMaxCount * (sizeof(unsigned int)));
	memcpy(uiCmdErrorCount, m_oThreadSend.m_uiCmdErrorCount, InstrumentMaxCount * (sizeof(unsigned int)));
//	csErrorCodeReturnShow = m_oThreadSend.m_csErrorCodeReturnShow;
	m_Sec_PortMonitor.Unlock();
	// 显示硬件错误码和码差
//	m_oPortMonitorRec.GetDlgItem(IDC_EDIT_ERRORCODERETURNSHOW)->SetWindowText(csErrorCodeReturnShow);
	GetDlgItem(IDC_BUTTON_PORTMONITORING_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PORTMONITORING_CLOSE)->EnableWindow(FALSE);
	for (int i=0; i<InstrumentMaxCount; i++)
	{
		csErrorCodeReturnShow.Format(_T("设备ID%d的数据误码总数为%d，命令误码总数为%d"), i, uiDataErrorCount[i], uiCmdErrorCount[i]);
		uiDataError += uiDataErrorCount[i];
		uiCmdError += uiCmdErrorCount[i];
		m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose"), csErrorCodeReturnShow, SuccessStatus);
	}
	csErrorCodeReturnShow.Format(_T("上线设备数据误码总数为%d，命令误码总数为%d"), uiDataError, uiCmdError);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose"), csErrorCodeReturnShow, SuccessStatus);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonPortmonitoringClose"), _T("关闭端口监视！"), SuccessStatus);
}

void CTabPortMonitoring::OnBnClickedButtonStartsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnSaveStart();
	GetDlgItem(IDC_BUTTON_STARTSAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOPSAVE)->EnableWindow(TRUE);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonStartsave"), _T("端口监视开始存储文件！"), SuccessStatus);
}

void CTabPortMonitoring::OnBnClickedButtonStopsave()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnSaveStop();
	GetDlgItem(IDC_BUTTON_STARTSAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOPSAVE)->EnableWindow(FALSE);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonStopsave"), _T("端口监视停止存储文件！"), SuccessStatus);
}

void CTabPortMonitoring::OnBnClickedButtonSavefilepath()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	CString str = _T("");
	m_oSaveFile.m_hWnd = this->m_hWnd;
	m_oSaveFile.OnSelectSaveFilePath();
	str = _T("端口监视选择文件存储路径为") + m_oSaveFile.m_csSaveFilePath;
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonSavefilepath"), str, SuccessStatus);
}

void CTabPortMonitoring::OnBnClickedButtonReset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	m_oSaveFile.OnReset();
	m_oThreadRec.OnReset();
	m_oThreadSend.OnReset();
	UpdateData(FALSE);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnBnClickedButtonReset"), _T("端口监视重置！"), SuccessStatus);
}

// 初始化
//************************************
// Method:    OnInit
// FullName:  CTabPortMonitoring::OnInit
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabPortMonitoring::OnInit(void)
{
	CString str = _T("");
	InitTabControlItems();

	m_oSaveFile.m_pWndTab = this;
	m_oSaveFile.OnInit();

	// 串口调试环境
	m_IPctrlRec.SetWindowText(m_csIPRec);
	m_IPctrlSend.SetWindowText(m_csIPSend);

	str.Format(_T("0x%04x"), m_uiRcvPort1);
	GetDlgItem(IDC_EDIT_AIMRCVPORT1)->SetWindowText(str);

	str.Format(_T("0x%04x"), m_uiSendPort1);
	GetDlgItem(IDC_EDIT_AIMSENDPORT1)->SetWindowText(str);

	str.Format(_T("0x%04x"), m_uiRcvPort2);
	GetDlgItem(IDC_EDIT_AIMRCVPORT2)->SetWindowText(str);

	str.Format(_T("0x%04x"), m_uiSendPort2);
	GetDlgItem(IDC_EDIT_AIMSENDPORT2)->SetWindowText(str);

	str.Format(_T("%d"), m_iSaveSize);
	GetDlgItem(IDC_EDIT_SAVESIZE)->SetWindowText(str);

	m_bPortDistribution = FALSE;
	UpdateData(FALSE);

	m_oPortMonitorSend.m_pRecThread = &m_oThreadRec;
	m_oPortMonitorRec.m_pSendThread = &m_oThreadSend;

	m_oThreadRec.m_pSaveFile = &m_oSaveFile;
	m_oThreadSend.m_pSaveFile = &m_oSaveFile;
	
	m_oThreadSend.m_pLogFile = m_pLogFile;
	m_oThreadRec.m_pLogFile = m_pLogFile;

	m_oThreadSend.OnInit();
	// 创建线程并挂起
	m_oThreadSend.CreateThread(CREATE_SUSPENDED, 0, 0);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnInit"), _T("端口监视发送线程创建成功！"), SuccessStatus);

	m_oThreadRec.OnInit();
	// 创建线程并挂起
	m_oThreadRec.CreateThread(CREATE_SUSPENDED, 0, 0);
	m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnInit"), _T("端口监视接收线程创建成功！"), SuccessStatus);

	GetDlgItem(IDC_BUTTON_PORTMONITORING_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PORTMONITORING_CLOSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STARTSAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOPSAVE)->EnableWindow(FALSE);
	OnShowMessage(FALSE);
}

//************************************
// Method:    OnClose
// FullName:  CTabPortMonitoring::OnClose
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void CTabPortMonitoring::OnClose()
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	int iResult = 0;
	CString str = _T("");
	// 恢复线程工作
	m_oThreadSend.ResumeThread();
	m_oThreadRec.ResumeThread();
	m_oThreadSend.OnClose();
	m_oThreadRec.OnClose();

	iResult = ::WaitForSingleObject(m_oThreadSend.m_hPortMonitoringSendThreadClose, WaitForThreadCloseTime);
	if (iResult != WAIT_OBJECT_0)
	{
		str.Format(_T("端口监视发送线程在%dms内未能正常结束！"), WaitForThreadCloseTime);
		m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnClose"), str, ErrorStatus);
		AfxMessageBox(str);
	}
	else
	{
		m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnClose"), _T("端口监视发送线程正常结束！"), SuccessStatus);
	}
	// 关闭事件句柄
	CloseHandle(m_oThreadSend.m_hPortMonitoringSendThreadClose);

	iResult = ::WaitForSingleObject(m_oThreadRec.m_hPortMonitoringRecThreadClose, WaitForThreadCloseTime);
	if (iResult != WAIT_OBJECT_0)
	{
		str.Format(_T("端口监视接收线程在%dms内未能正常结束！"), WaitForThreadCloseTime);
		m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnClose"), str, ErrorStatus);
		AfxMessageBox(str);
	}
	else
	{
		m_pLogFile->OnWriteLogFile(_T("CTabPortMonitoring::OnClose"), _T("端口监视接收线程正常结束！"), SuccessStatus);
	}
	// 关闭事件句柄
	CloseHandle(m_oThreadRec.m_hPortMonitoringRecThreadClose);
}
// 防止程序在循环体中运行无法响应消息
//************************************
// Method:    ProcessMessages
// FullName:  CTabPortMonitoring::ProcessMessages
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void CTabPortMonitoring::ProcessMessages(void)
{
	MSG msg;
	::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	::DispatchMessage(&msg);
}

void CTabPortMonitoring::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TabPortMonitoringFrameNumTimerNb)
	{
		CString strTemp = _T("");
		unsigned int uiSendFrameNum = 0;
		unsigned int uiRecFrameNum = 0;
	
		m_Sec_PortMonitor.Lock();
		uiSendFrameNum = m_oThreadRec.m_uiSendFrameNum;
		m_Sec_PortMonitor.Unlock();

		strTemp.Format(_T("%d"),uiSendFrameNum);
		GetDlgItem(IDC_STATIC_SENDFRAMENUM)->SetWindowText(strTemp);

		m_Sec_PortMonitor.Lock();
		uiRecFrameNum = m_oThreadSend.m_uiRecFrameNum;
		m_Sec_PortMonitor.Unlock();

		strTemp.Format(_T("%d"),uiRecFrameNum);
		GetDlgItem(IDC_STATIC_RECFRAMENUM)->SetWindowText(strTemp);
	}

	CDialog::OnTimer(nIDEvent);
}

void CTabPortMonitoring::OnBnClickedCheckShowmessage()
{
	// TODO: 在此添加控件通知处理程序代码
	// 得到CHECK控件当前状态
	BOOL bStatus = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_SHOWMESSAGE)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
	OnShowMessage(bStatus);
}

// 显示接收发送帧信息
void CTabPortMonitoring::OnShowMessage(BOOL bShow)
{
	if (bShow == TRUE)
	{
		KillTimer(TabPortMonitoringFrameNumTimerNb);
		SetTimer(TabPortMonitoringFrameNumTimerNb, TabPortMonitoringFrameNumTimerSet, NULL);
		m_oPortMonitorRec.KillTimer(PortMonitoringRecTimerNb);
		m_oPortMonitorRec.SetTimer(PortMonitoringRecTimerNb, PortMonitoringRecTimerSet, NULL);
		m_oPortMonitorSend.KillTimer(PortMonitoringSendTimerNb);
		m_oPortMonitorSend.SetTimer(PortMonitoringSendTimerNb, PortMonitoringSendTimerSet, NULL);
	}
	else
	{
		KillTimer(TabPortMonitoringFrameNumTimerNb);
		m_oPortMonitorRec.KillTimer(PortMonitoringRecTimerNb);
		m_oPortMonitorSend.KillTimer(PortMonitoringSendTimerNb);
	}
}
