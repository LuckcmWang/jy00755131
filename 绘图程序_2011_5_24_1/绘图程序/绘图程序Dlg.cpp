// 绘图程序Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "绘图程序.h"
#include "绘图程序Dlg.h"
#include "HotSpotDlg.h"
#include <math.h>
#include <algorithm>
#include <fstream>
#include  <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 表示可改变大小的图标ID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// C绘图程序Dlg 对话框




C绘图程序Dlg::C绘图程序Dlg(CWnd* pParent /*=NULL*/)
: CDialog(C绘图程序Dlg::IDD, pParent)
, m_currentDuration(0)
, m_dateRange(0)
, m_minDuration(0)
, m_iClientWidth(0)
, m_iClientHeight(0)
, m_iMinWidth(0)
, m_iMinHeight(0)
, m_pControlArray(NULL)
, m_iControlNumber(0)
, m_bShowSizeIcon(TRUE)
, m_uiADCStartNum(0)
, m_uiADCDataCovNb(0)
, m_uiADCDataNum(0)
, m_uiADCDataFduNum(0)
, m_viewPortDataSeries(NULL)
, m_uiADCFileLineNum(0)
, m_uiInstrumentADCNum(0)
, m_bCheckYAxisFixed(FALSE)
, m_bCheckWaveRangeFixed(FALSE)
, m_bStartShow(FALSE)
, m_uiStartDrawPointsNum(0)
, m_uiOpenFileNb(0)
, m_uiOptType(0)
, m_uiEndDrawPointsNumMax(0)
, m_uiFindLineCount(0)
, m_uiADCLineBufNum(0)
, m_pLinebufRead(NULL)
, m_uiEditSampleRate(0)
, m_dbEditLineInterval(0)
, m_dbEditLineZoom(0)
, m_uiRecFrameBeginMaxNb(0)
, m_uiRecFrameEndMinNb(0)
, m_uiDrawFrameNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
C绘图程序Dlg::~C绘图程序Dlg()
{
	delete m_ChartViewer.getChart();
	// 记录X轴坐标点信息
	m_DrawPoint_X.clear();
	m_FileInfo.clear();
	m_ADCDataInfo.clear();
	if (m_dbFduData != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduData[i].clear();
		}
		delete[] m_dbFduData;
	}
	if (m_dbFduShow != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduShow[i].clear();
		}
		delete[] m_dbFduShow;
	}
	if (m_pLinebufRead != NULL)
	{
		delete[] m_pLinebufRead;
	}
}
void C绘图程序Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
	DDX_Control(pDX, IDC_PointerPB, m_PointerPB);
	DDX_Control(pDX, IDC_XZoomPB, m_XZoomPB);
	DDX_Control(pDX, IDC_HScrollBar, m_HScrollBar);
	DDX_Control(pDX, IDC_VScrollBar, m_VScrollBar);
	DDX_Control(pDX, IDC_Duration, m_Duration);
	DDX_Text(pDX, IDC_EDIT_SAMPLINGRATE, m_uiEditSampleRate);
	DDX_Text(pDX, IDC_EDIT_LINEINTERVAL, m_dbEditLineInterval);
	DDX_Text(pDX, IDC_EDIT_LINEZOOM, m_dbEditLineZoom);
}

BEGIN_MESSAGE_MAP(C绘图程序Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PointerPB, &C绘图程序Dlg::OnBnClickedPointerpb)
	ON_BN_CLICKED(IDC_ZoomInPB, &C绘图程序Dlg::OnBnClickedZoominpb)
	ON_BN_CLICKED(IDC_ZoomOutPB, &C绘图程序Dlg::OnBnClickedZoomoutpb)
	ON_BN_CLICKED(IDC_XZoomPB, &C绘图程序Dlg::OnBnClickedXzoompb)
	ON_BN_CLICKED(IDC_XYZoomPB, &C绘图程序Dlg::OnBnClickedXyzoompb)
	ON_BN_CLICKED(IDC_ChartViewer, &C绘图程序Dlg::OnBnClickedChartViewer)
	ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, &C绘图程序Dlg::OnViewPortChanged)
	ON_CBN_SELCHANGE(IDC_Duration, &C绘图程序Dlg::OnCbnSelchangeDuration)
	ON_CBN_KILLFOCUS(IDC_Duration, &C绘图程序Dlg::OnCbnKillfocusDuration)
	ON_BN_CLICKED(IDC_BUTTON_SAVECHART, &C绘图程序Dlg::OnBnClickedButtonSavechart)
	ON_BN_CLICKED(IDC_BUTTON_OPENADCFILE, &C绘图程序Dlg::OnBnClickedButtonOpenadcfile)
	ON_BN_CLICKED(IDC_YZoomPB, &C绘图程序Dlg::OnBnClickedYzoompb)
	ON_BN_CLICKED(IDC_BUTTON_REDRAW, &C绘图程序Dlg::OnBnClickedButtonRedraw)
	ON_BN_CLICKED(IDC_BUTTON_START, &C绘图程序Dlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &C绘图程序Dlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_CHECK_YAXISFIXED, &C绘图程序Dlg::OnBnClickedCheckYaxisfixed)
	ON_BN_CLICKED(IDC_BUTTON_OPENADCFOLDER, &C绘图程序Dlg::OnBnClickedButtonOpenadcfolder)
	ON_LBN_DBLCLK(IDC_LIST_FILE, &C绘图程序Dlg::OnLbnDblclkListFile)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETERSET, &C绘图程序Dlg::OnBnClickedButtonParameterset)
	ON_BN_CLICKED(IDC_CHECK_WAVERANGEFIXED, &C绘图程序Dlg::OnBnClickedCheckWaverangefixed)
END_MESSAGE_MAP()


// C绘图程序Dlg 消息处理程序

BOOL C绘图程序Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_oParameterSet.LoadXmlFile();
	// 载入图标到鼠标用法按钮
	LoadButtonIcon(IDC_PointerPB, IDI_PointerPB, 100, 20);  
	LoadButtonIcon(IDC_ZoomInPB, IDI_ZoomInPB, 100, 20);    
	LoadButtonIcon(IDC_ZoomOutPB, IDI_ZoomOutPB, 100, 20);

	// 载入图标到变焦/滚动位置控制按钮
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		LoadButtonIcon(IDC_XZoomPB, IDI_YZoomPB, 105, 20);
		LoadButtonIcon(IDC_YZoomPB, IDI_XZoomPB, 105, 20);
	}
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		LoadButtonIcon(IDC_XZoomPB, IDI_XZoomPB, 105, 20);
		LoadButtonIcon(IDC_YZoomPB, IDI_YZoomPB, 105, 20);
	}
	LoadButtonIcon(IDC_XYZoomPB, IDI_XYZoomPB, 105, 20);

	// 将对话框及其控件设为尺寸可变的
	OnSiteSizeBox();

	// 初始设置鼠标为横向拖动滚动条模式
	// 	m_PointerPB.SetCheck(1);
	// 	m_XZoomPB.SetCheck(1);
	m_uiEditSampleRate = 1;
	m_dbEditLineZoom = 1;
	m_dbEditLineInterval = 1;
	UpdateData(FALSE);
	// 采集站ADC数据存储
	m_dbFduData = NULL;
	// 采集站ADC数据绘图
	m_dbFduShow = NULL;


	m_oSocketADCDataRec.m_pParameterSet = &m_oParameterSet;


	// 输入License
	if (false == LoadChartDirLicense())
	{
		AfxMessageBox(_T("Please Input correct License!"));
		return FALSE;
	} 

	GetClientRect(&m_oRectWindowClient);
	GetDlgItem(IDC_STATIC_CONTROL)->GetClientRect(&m_oRectWindowCtrl);
	m_oRectWindowGraph.left = m_oRectWindowCtrl.right + SetPlotCtrlXInterval;
	m_oRectWindowGraph.right = m_oRectWindowClient.right - SetVScrollWidth - SetPlotCtrlXInterval;
	m_oRectWindowGraph.top = m_oRectWindowCtrl.top + SetPlotCtrlYInterval;
	m_oRectWindowGraph.bottom = m_oRectWindowCtrl.bottom;
	GetDlgItem(IDC_ChartViewer)->MoveWindow(&m_oRectWindowGraph);

	// @@@@提交用户时修改
	GetDlgItem(IDC_BUTTON_PARAMETERSET)->EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C绘图程序Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void C绘图程序Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C绘图程序Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
// 将对话框及其控件设为尺寸可变的
void C绘图程序Dlg::OnSiteSizeBox(void)
{
	// 设置对话框为可变大小的
	ModifyStyle(0, WS_SIZEBOX);

	// 以对话框的初始大小作为对话框的宽度和高度的最小值
	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_iMinWidth = rectDlg.Width();
	m_iMinHeight = rectDlg.Height();

	// 得到对话框client区域的大小 
	CRect rectClient;
	GetClientRect(rectClient);
	m_iClientWidth = rectClient.Width();
	m_iClientHeight = rectClient.Height();

	// Load图标
	m_bmpSizeIcon.LoadOEMBitmap(OBM_SIZE);
	m_bmpSizeIcon.GetBitmap(&m_bitmap);

	// 创建显示图标的静态控件并放在对话框右下角
	m_wndSizeIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight), this, 0);
	m_wndSizeIcon.SetBitmap(m_bmpSizeIcon);
	m_wndSizeIcon.MoveWindow(m_iClientWidth - m_bitmap.bmWidth, m_iClientHeight - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// 显示图标
	m_wndSizeIcon.ShowWindow(m_bShowSizeIcon);
	static DLGCTLINFO dcMenuGroup[] = 
	{
		{IDC_STATIC_CONTROL, ELASTICY,	100},
		{IDC_STATIC_CONTROL, ELASTICX,	5},
		{IDC_STATIC_MOUSEMODE, ELASTICY, 16},
		{IDC_STATIC_MOUSEMODE, ELASTICX, 5},
		{IDC_PointerPB, MOVEY, 2},
		{IDC_PointerPB, ELASTICY, 5},
		{IDC_PointerPB, ELASTICX, 2},
		{IDC_ZoomInPB, MOVEX, 3},
		{IDC_ZoomInPB, MOVEY, 2},
		{IDC_ZoomInPB, ELASTICY, 5},
		{IDC_ZoomInPB, ELASTICX, 2},
		{IDC_ZoomOutPB, MOVEY, 9},
		{IDC_ZoomOutPB, ELASTICY, 5},
		{IDC_ZoomOutPB, ELASTICX, 2},
		{IDC_STATIC_ZOOMMODE, MOVEY, 18},
		{IDC_STATIC_ZOOMMODE, ELASTICY, 16},
		{IDC_STATIC_ZOOMMODE, ELASTICX, 5},
		{IDC_XZoomPB, MOVEY, 20},
		{IDC_XZoomPB, ELASTICY, 5},
		{IDC_XZoomPB, ELASTICX, 2},
		{IDC_YZoomPB, MOVEX, 3},
		{IDC_YZoomPB, MOVEY, 20},
		{IDC_YZoomPB, ELASTICY, 5},
		{IDC_YZoomPB, ELASTICX, 2},
		{IDC_XYZoomPB, MOVEY, 27},
		{IDC_XYZoomPB, ELASTICY, 5},
		{IDC_XYZoomPB, ELASTICX, 2},
		{IDC_STATIC_GRAPHSHOWNUM, MOVEY, 35},
		{IDC_Duration, MOVEY, 35},
		{IDC_Duration, ELASTICX, 5},
		{IDC_BUTTON_OPENADCFILE, MOVEY, 37},
		{IDC_BUTTON_OPENADCFILE, ELASTICY, 5},
		{IDC_BUTTON_OPENADCFILE, ELASTICX, 5},
		{IDC_BUTTON_OPENADCFOLDER, MOVEY, 43},
		{IDC_BUTTON_OPENADCFOLDER, ELASTICY, 5},
		{IDC_BUTTON_OPENADCFOLDER, ELASTICX, 5},
		{IDC_STATIC_FILELIST, MOVEY, 48},
		{IDC_LIST_FILE, MOVEY, 48},
		{IDC_LIST_FILE, ELASTICX, 5},
		{IDC_LIST_FILE, ELASTICY, 5},
		{IDC_STATIC_SAMPLINGRATE, MOVEY, 48},
		{IDC_EDIT_SAMPLINGRATE, MOVEY, 48},
		{IDC_STATIC_LINEINTERVAL, MOVEY, 53},
		{IDC_STATIC_LINEINTERVAL, ELASTICX, 2},
		{IDC_EDIT_LINEINTERVAL, MOVEY, 53},
		{IDC_EDIT_LINEINTERVAL, ELASTICX, 2},
		{IDC_STATIC_LINEZOOM, MOVEY, 53},
		{IDC_STATIC_LINEZOOM, MOVEX, 3},
		{IDC_STATIC_LINEZOOM, ELASTICX, 2},
		{IDC_EDIT_LINEZOOM, MOVEY, 53},
		{IDC_EDIT_LINEZOOM, MOVEX, 3},
		{IDC_EDIT_LINEZOOM, ELASTICX, 2},
		{IDC_CHECK_YAXISFIXED, MOVEY, 53},
		{IDC_CHECK_YAXISFIXED, ELASTICY, 5},
		{IDC_CHECK_WAVERANGEFIXED, MOVEY, 53},
		{IDC_CHECK_WAVERANGEFIXED, MOVEX, 3},
		{IDC_CHECK_WAVERANGEFIXED, ELASTICY, 5},
		{IDC_BUTTON_REDRAW, MOVEY, 58},
		{IDC_BUTTON_REDRAW, ELASTICY, 5},
		{IDC_BUTTON_REDRAW, ELASTICX, 2},
		{IDC_BUTTON_SAVECHART, MOVEY, 63},
		{IDC_BUTTON_SAVECHART, ELASTICY, 5},
		{IDC_BUTTON_SAVECHART, ELASTICX, 2},
		{IDC_BUTTON_PARAMETERSET, MOVEY, 68},
		{IDC_BUTTON_PARAMETERSET, ELASTICY, 5},
		{IDC_BUTTON_PARAMETERSET, ELASTICX, 2},
		{IDC_BUTTON_START, MOVEY, 58},
		{IDC_BUTTON_START, MOVEX, 3},
		{IDC_BUTTON_START, ELASTICY, 5},
		{IDC_BUTTON_START, ELASTICX, 2},
		{IDC_BUTTON_STOP, MOVEY, 63},
		{IDC_BUTTON_STOP, MOVEX, 3},
		{IDC_BUTTON_STOP, ELASTICY, 5},
		{IDC_BUTTON_STOP, ELASTICX, 2},
		{IDC_HScrollBar, MOVEX, 5},
		{IDC_HScrollBar, MOVEY, 100},
		{IDC_HScrollBar, ELASTICX, 100},
		{IDC_VScrollBar, MOVEX, 100},
		{IDC_VScrollBar, MOVEY, 5},
		{IDC_VScrollBar, ELASTICY, 100},
		{IDC_ChartViewer, MOVEX, 5},
		{IDC_ChartViewer, ELASTICX, 95},
		{IDC_ChartViewer, ELASTICY, 100},
	};
	// 设置控件信息
	SetControlProperty(dcMenuGroup, sizeof(dcMenuGroup)/sizeof(DLGCTLINFO));
}
// 载入一个图标资源到按钮
void C绘图程序Dlg::LoadButtonIcon(int buttonId, int iconId, int width, int height)
{
	GetDlgItem(buttonId)->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)::LoadImage(
		AfxGetResourceHandle(), MAKEINTRESOURCE(iconId), IMAGE_ICON, width, height, 
		LR_DEFAULTCOLOR));  
}

// 得到默认的背景颜色
int C绘图程序Dlg::getDefaultBgColor(void)
{
	LOGBRUSH LogBrush; 
	HBRUSH hBrush = (HBRUSH)SendMessage(WM_CTLCOLORDLG, (WPARAM)CClientDC(this).m_hDC, 
		(LPARAM)m_hWnd); 
	::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush); 
	int ret = LogBrush.lbColor;
	return ((ret & 0xff) << 16) | (ret & 0xff00) | ((ret & 0xff0000) >> 16);
}
void C绘图程序Dlg::OnBnClickedPointerpb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
}

void C绘图程序Dlg::OnBnClickedZoominpb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomIn);
}

void C绘图程序Dlg::OnBnClickedZoomoutpb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomOut);
}

void C绘图程序Dlg::OnBnClickedXzoompb()
{
	// TODO: 在此添加控件通知处理程序代码
	// cxm 2011.10.14
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		m_ChartViewer.setZoomDirection(Chart::DirectionVertical); 
		m_ChartViewer.setScrollDirection(Chart::DirectionVertical);
	} 
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		m_ChartViewer.setZoomDirection(Chart::DirectionHorizontal); 
		m_ChartViewer.setScrollDirection(Chart::DirectionHorizontal);
	}

	// Update chart to auto-scale axis
	m_ChartViewer.updateViewPort(true, true);
}

void C绘图程序Dlg::OnBnClickedXyzoompb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChartViewer.setZoomDirection(Chart::DirectionHorizontalVertical); 
	m_ChartViewer.setScrollDirection(Chart::DirectionHorizontalVertical);  
}

void C绘图程序Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		// User is still scrolling
		// Set the view port based on the scroll bar
		m_ChartViewer.setViewPortLeft(moveScrollBar(nSBCode, nPos, pScrollBar));
		// Update the chart image only, but no need to update the image map.
		m_ChartViewer.updateViewPort(true, false);
	}
	else
	{
		// Scroll bar has stoped moving. Can update image map now.
		m_ChartViewer.updateViewPort(false, true);
	}
}

void C绘图程序Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		// User is still scrolling
		// Set the view port based on the scroll bar
		m_ChartViewer.setViewPortTop(moveScrollBar(nSBCode, nPos, pScrollBar));
		// Update the chart image only, but no need to update the image map.
		m_ChartViewer.updateViewPort(true, false);
	}
	else
	{
		// Scroll bar has stoped moving. Can update image map now.
		m_ChartViewer.updateViewPort(false, true);
	}
}
//
// CChartViewer ViewPortChanged event
//
void C绘图程序Dlg::OnViewPortChanged()
{
	//
	// Set up the scroll bars to reflect the current position and size of the view port
	//
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	info.nMin = 0;
	info.nMax = 0x1fffffff;

	m_HScrollBar.EnableWindow(m_ChartViewer.getViewPortWidth() < 1);
	if (m_ChartViewer.getViewPortWidth() < 1)
	{
		info.nPage = (int)ceil(m_ChartViewer.getViewPortWidth() * (info.nMax - info.nMin));
		info.nPos = (int)(0.5 + m_ChartViewer.getViewPortLeft() * (info.nMax - info.nMin))
			+ info.nMin;
		m_HScrollBar.SetScrollInfo(&info);
	}

	m_VScrollBar.EnableWindow(m_ChartViewer.getViewPortHeight() < 1);
	if (m_ChartViewer.getViewPortHeight() < 1)
	{
		info.nPage = (int)ceil(m_ChartViewer.getViewPortHeight() * (info.nMax - info.nMin));
		info.nPos = (int)(0.5 + m_ChartViewer.getViewPortTop() * (info.nMax - info.nMin))
			+ info.nMin;
		m_VScrollBar.SetScrollInfo(&info);
	}

	// Compute the start date (in chartTime) and duration (in seconds) of the view port
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		m_currentDuration = (int)(0.5 + m_ChartViewer.getViewPortHeight() * m_dateRange);
	} 
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		m_currentDuration = (int)(0.5 + m_ChartViewer.getViewPortWidth() * m_dateRange);
	}

	// Set the duration combo box to reflect the duration (in days)
	CString buffer;
	buffer.Format(_T("%d"), (int)(0.5 + m_currentDuration));
	m_Duration.SetWindowText(buffer);
	//
	// Update chart and image map if necessary
	//
	if (m_ChartViewer.needUpdateChart())
	{
		if (m_uiOptType == OptTypeNetwork)
		{
			drawChartNetwork(&m_ChartViewer);
		}
		else
		{
			drawChart(&m_ChartViewer);
		}	
	}
	if (m_ChartViewer.needUpdateImageMap())
	{
		updateImageMap(&m_ChartViewer);
	}	
}
//
// User clicks on the CChartViewer
//
void C绘图程序Dlg::OnBnClickedChartViewer() 
{
	ImageMapHandler *handler = m_ChartViewer.getImageMapHandler();
	if (0 != handler)
	{
		//
		// Query the ImageMapHandler to see if the mouse is on a clickable hot spot. We 
		// consider the hot spot as clickable if its href ("path") parameter is not empty.
		//
		const char *path = handler->getValue("path");
		if ((0 != path) && (0 != *path))
		{
			// In this sample code, we just show all hot spot parameters.
			CHotSpotDlg hs;
			hs.SetData(handler);
			hs.DoModal();
		}
	}
}

//
// Handle scroll bar events
//
double C绘图程序Dlg::moveScrollBar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//
	// Get current scroll bar position
	//
	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	pScrollBar->GetScrollInfo(&info);

	//
	// Compute new position based on the type of scroll bar events
	//
	int newPos = info.nPos;
	switch (nSBCode)
	{
	case SB_LEFT:
		newPos = info.nMin;
		break;
	case SB_RIGHT:
		newPos = info.nMax;
		break;
	case SB_LINELEFT:
		newPos -= (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_LINERIGHT:
		newPos += (info.nPage > 10) ? info.nPage / 10 : 1;
		break;
	case SB_PAGELEFT:
		newPos -= info.nPage;
		break;
	case SB_PAGERIGHT:
		newPos += info.nPage;
		break;
	case SB_THUMBTRACK:
		newPos = info.nTrackPos;
		break;
	}
	if (newPos < info.nMin) newPos = info.nMin;
	if (newPos > info.nMax) newPos = info.nMax;

	// Update the scroll bar with the new position
	pScrollBar->SetScrollPos(newPos);

	// Returns the position of the scroll bar as a ratio of its total length
	return ((double)(newPos - info.nMin)) / (info.nMax - info.nMin);
}
//
// Validate the contents of the duration combo box and update ViewPortWidth accordingly
//
void C绘图程序Dlg::validateDuration(const CString &text)
{
	// Parse the duration text
	double newDuration = _tcstod(text, 0);

	// Duration too short or not numeric?
	if (newDuration < m_minDuration) 
		newDuration = m_minDuration;

	if (newDuration != m_currentDuration)
	{
		// Set the ViewPortWidth according to the new duration
		m_currentDuration = newDuration;
		if (m_oParameterSet.m_uiShowDirection == 1)
		{
			m_ChartViewer.setViewPortHeight(newDuration / m_dateRange);
		}
		else if (m_oParameterSet.m_uiShowDirection == 2)
		{
			m_ChartViewer.setViewPortWidth(newDuration / m_dateRange);
		}
		TRACE(_T("1\r\n"));
		// Update the chart
		m_ChartViewer.updateViewPort(true, true);
	}
}
//
// Draw the chart and display it in the given viewer
//
void C绘图程序Dlg::drawChart(CChartViewer *viewer)
{
	//
	// In this demo, we copy the visible part of the data to a separate buffer for chart
	// plotting. 
	//
	// Note that if you only have a small amount of data (a few hundred data points), it
	// may be easier to just plot all data in any case (so the following copying code is 
	// not needed), and let ChartDirector "clip" the chart to the plot area. 
	//

	// Using ViewPortLeft and ViewPortWidth, get the start and end dates of the view port.
	double viewPortStartDate = 0.0;
	double viewPortEndDate = 0.0;
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		viewPortStartDate = m_minData + (__int32)(viewer->getViewPortTop() * 
			m_dateRange + 0.5);
		viewPortEndDate = viewPortStartDate + (__int32)(viewer->getViewPortHeight() * 
			m_dateRange + 0.5);
	} 
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		viewPortStartDate = m_minData + (__int32)(viewer->getViewPortLeft() * 
			m_dateRange + 0.5);
		viewPortEndDate = viewPortStartDate + (__int32)(viewer->getViewPortWidth() * 
			m_dateRange + 0.5);
	}

	if (m_uiOptType == OptTypeLoadFile)
	{
		// 载入单个文件
		if (FALSE == FraseDataToDraw((unsigned int)(viewPortStartDate), (unsigned int)(viewPortEndDate)))
		{
			return;
		}
		if (m_uiADCDataFduNum == 0)
		{
			return;
		}
	}
	else if (m_uiOptType == OptTypeLoadFolder)
	{
		// 载入文件夹
		LoadADCDataFromFile((unsigned int)(viewPortStartDate), (unsigned int)(viewPortEndDate));
		if (m_uiADCDataFduNum == 0)
		{
			return;
		}
	}
	// Get the starting index of the array using the start date
	int startIndex = 0;
	int endIndex = 0;
	startIndex = (int)(std::lower_bound(&m_DrawPoint_X[0], &m_DrawPoint_X[0] + m_DrawPoint_X.size(), 
		viewPortStartDate) - &m_DrawPoint_X[0]);
	if ((startIndex > 0) && (m_DrawPoint_X[startIndex] != viewPortStartDate)) 
		--startIndex;

	// Get the ending index of the array using the end date
	endIndex = (int)(std::upper_bound(&m_DrawPoint_X[0], &m_DrawPoint_X[0] + m_DrawPoint_X.size(), 
		viewPortEndDate) - &m_DrawPoint_X[0]);
	if (endIndex >= ((int)(m_DrawPoint_X.size()) - 1))
		endIndex = m_DrawPoint_X.size() - 1;

	// Get the length
	int noOfPoints = endIndex - startIndex + 1;
	if (noOfPoints == 1)
	{
		return;
	}
	// We copy the visible data from the main arrays to separate data arrays
	double* viewPortXStamps = new double[noOfPoints];
	int arraySizeInBytes = noOfPoints * sizeof(double);
	memcpy(viewPortXStamps, &m_DrawPoint_X[startIndex], arraySizeInBytes);

	// 一条线在界面显示点数超过网络接收缓冲区大小时采用抽点显示
	if (noOfPoints > ShowLinePointsNumNow)
	{
		//
		// Zoomable chart with high zooming ratios often need to plot many thousands of 
		// points when fully zoomed out. However, it is usually not needed to plot more
		// data points than the resolution of the chart. Plotting too many points may cause
		// the points and the lines to overlap. So rather than increasing resolution, this 
		// reduces the clarity of the chart. So it is better to aggregate the data first if
		// there are too many points.
		//
		// In our current example, the chart only has 520 pixels in width and is using a 2
		// pixel line width. So if there are more than 520 data points, we aggregate the 
		// data using the ChartDirector aggregation utility method.
		//
		// If in your real application, you do not have too many data points, you may 
		// remove the following code altogether.
		//

		// Set up an aggregator to aggregate the data based on regular sized slots
		ArrayMath m(DoubleArray(viewPortXStamps, noOfPoints));
		m.selectRegularSpacing(noOfPoints / (ShowLinePointsNumNow / 2));

		// For the timestamps, take the first timestamp on each slot
		int aggregatedNoOfPoints = m.aggregate(DoubleArray(viewPortXStamps, noOfPoints), 
			Chart::AggregateFirst).len;

		// For the data values, aggregate by taking the averages
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m.aggregate(DoubleArray(&m_dbFduShow[i][startIndex], noOfPoints), Chart::AggregateAvg);
		}
		noOfPoints = aggregatedNoOfPoints;
	}

	//
	// Now we have obtained the data, we can plot the chart. 
	//

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 1 - Configure overall chart appearance. 
	///////////////////////////////////////////////////////////////////////////////////////

	// Create an XYChart object 600 x 300 pixels in size, with pale blue (0xf0f0ff) 
	// background, black (000000) border, 1 pixel raised effect, and with a rounded frame.

	XYChart *c = new XYChart(m_oRectWindowGraph.Width(), m_oRectWindowGraph.Height(), SetPlotCtrlBackgroundColor, 0, 1);
	c->setRoundedFrame(m_extBgColor);

	// Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white
	// background. Turn on both horizontal and vertical grid lines with light grey
	// color (0xcccccc). Set clipping mode to clip the data lines to the plot area.
	int iWidth = 0;
	int iHeight = 0;
	iWidth = m_oRectWindowGraph.Width() - SetLegendWidth - m_uiIntervalNum * SetPlotAreaXInterval - SetLegendXInterval;
	iHeight = m_oRectWindowGraph.Height() - SetPlotAreaYInterval - SetPlotAreaBottomInterval;
	c->setPlotArea(m_uiIntervalNum * SetPlotAreaXInterval, SetPlotAreaYInterval, iWidth, iHeight, SetPlotAreaBackgroundColor, -1, -1, SetHGridColor, SetVGridColor);
	//	c->setClipping();

	// Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9
	// pts Arial Bold font. Set the background and border color to Transparent.

	// cxm 2011.10.12不显示标签栏
	//	c->addLegend(rectWindow.Width() - SetLegendWidth, SetLegendYInterval, false, "arialbd.ttf", SetLegnedBoxFontSize)->setBackground(Chart::Transparent);

	// Add a title box to the chart using 15 pts Times Bold Italic font, on a light
	// blue (CCCCFF) background with glass effect. white (0xffffff) on a dark red
	// (0x800000) background, with a 1 pixel 3D border.
	c->addTitle("Hitech Matrix428", "timesbi.ttf", SetTitleBoxFontSize)->setBackground(
		SetTitleBackgroundColor, SetTitleEdgeColor, Chart::glassEffect());

	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		// Swap the x and y axis to become a rotated chart
		c->swapXY();

		// Set the y axis on the top side (right + rotated = top)
		c->setYAxisOnRight(false);

		// Reverse the x axis so it is pointing downwards
		c->xAxis()->setReverse();

	}
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		c->setXAxisOnTop(true);
		c->yAxis()->setReverse();
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 2 - Add data to chart
	///////////////////////////////////////////////////////////////////////////////////////

	// 
	// In this example, we represent the data by lines. You may modify the code below if 
	// you want to use other representations (areas, scatter plot, etc).
	//

	// Add a line layer to the chart
	LineLayer *layer = c->addLineLayer();

	// Set the default line width to 1 pixels
	layer->setLineWidth(SetLineWidth);

	// Set the axes width to 2 pixels
	c->xAxis()->setWidth(SetXAxisWidth);
	c->yAxis()->setWidth(SetYAxisWidth);

	// Now we add the 3 data series to a line layer, using the color red (ff0000), green
	// (00cc00) and blue (0000ff)
	// 载入X轴数据
	layer->setXData(DoubleArray(viewPortXStamps, noOfPoints));
	// 载入标签并设置标签和绘线的颜色
	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	{
		char buffer[1024];
		sprintf_s(buffer, sizeof(buffer), "FDU #%d", m_uiInstrumentNb[i]);
		layer->addDataSet(DoubleArray(&m_dbFduShow[i][startIndex], noOfPoints), RedColor, buffer);

		// Chart::Transparent按照原来的背景颜色
		Mark *m = c->yAxis()->addMark(i * m_dbEditLineInterval + 1, Chart::Transparent);
		// 曲线值小于Mark线的部分用原背景色填充，大于Mark线的部分用0x80ff8888颜色填充
		c->addInterLineLayer(layer->getLine(i), m->getLine(), m_oParameterSet.m_iMarkHighColor, m_oParameterSet.m_iMarkLowColor);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 3 - Set up x-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	// Set x-axis date scale to the view port date range. 
	c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 4 - Set up y-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	if (((m_minValue == 0) && (m_maxValue == 0)))
	{
		// y-axis is auto-scaled - save the chosen y-axis scaled to support xy-zoom mode
		c->layout();
		m_minValue = c->yAxis()->getMinValue();
		m_maxValue = c->yAxis()->getMaxValue();
	}
	else
	{
		// xy-zoom mode - compute the actual axis scale in the view port 
		double axisLowerLimit = 0.0;
		double axisUpperLimit = 0.0;
		if (m_oParameterSet.m_uiShowDirection == 1)
		{
			axisLowerLimit = m_minValue + (m_maxValue - m_minValue) * viewer->getViewPortLeft();
			axisUpperLimit = m_minValue + (m_maxValue - m_minValue) * (viewer->getViewPortLeft() + viewer->getViewPortWidth());
		} 
		else if (m_oParameterSet.m_uiShowDirection == 2)
		{
			axisLowerLimit =  m_maxValue - (m_maxValue - m_minValue) * 
				(viewer->getViewPortTop() + viewer->getViewPortHeight());
			axisUpperLimit =  m_maxValue - (m_maxValue - m_minValue) * 
				viewer->getViewPortTop();
		}
		// use the zoomed-in scale
		if (m_oParameterSet.m_uiShowDirection == 1)
		{
			c->yAxis()->setLinearScale(axisLowerLimit, axisUpperLimit);
		} 
		else if (m_oParameterSet.m_uiShowDirection == 2)
		{
			c->yAxis()->setLinearScale(axisUpperLimit, axisLowerLimit);
		}
		// 为true则坐标最大或最小值显示到最近的栅格
		c->yAxis()->setRounding(false, false);
		c->xAxis()->setRounding(false, false);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// Step 5 - Display the chart
	///////////////////////////////////////////////////////////////////////////////////////

	// Set the chart image to the WinChartViewer
	delete viewer->getChart();
	viewer->setChart(c);

	// 释放资源
	delete[] viewPortXStamps;
}
// 网络传输绘图
void C绘图程序Dlg::drawChartNetwork(CChartViewer *viewer)
{
	//
	// In this demo, we copy the visible part of the data to a separate buffer for chart
	// plotting. 
	//
	// Note that if you only have a small amount of data (a few hundred data points), it
	// may be easier to just plot all data in any case (so the following copying code is 
	// not needed), and let ChartDirector "clip" the chart to the plot area. 
	//

	// Using ViewPortLeft and ViewPortWidth, get the start and end dates of the view port.
	double viewPortStartDate = 0.0;
	double viewPortEndDate = 0.0;
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		viewPortStartDate = m_minData + (__int32)(viewer->getViewPortTop() * 
			m_dateRange + 0.5);
		viewPortEndDate = viewPortStartDate + (__int32)(viewer->getViewPortHeight() * 
			m_dateRange + 0.5);
	} 
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		viewPortStartDate = m_minData + (__int32)(viewer->getViewPortLeft() * 
			m_dateRange + 0.5);
		viewPortEndDate = viewPortStartDate + (__int32)(viewer->getViewPortWidth() * 
			m_dateRange + 0.5);
	}
	// Get the starting index of the array using the start date
	int startIndex = 0;
	int endIndex = 0;
	startIndex = (int)(std::lower_bound(&m_oSocketADCDataRec.m_DrawPoint_X[0], &m_oSocketADCDataRec.m_DrawPoint_X[0] + ShowLinePointsNumNow, 
		viewPortStartDate) - &m_oSocketADCDataRec.m_DrawPoint_X[0]);
	if ((startIndex > 0) && (m_oSocketADCDataRec.m_DrawPoint_X[startIndex] != viewPortStartDate)) 
		--startIndex;

	// Get the ending index of the array using the end date
	endIndex = (int)(std::upper_bound(&m_oSocketADCDataRec.m_DrawPoint_X[0], &m_oSocketADCDataRec.m_DrawPoint_X[0] + ShowLinePointsNumNow, 
		viewPortEndDate) - &m_oSocketADCDataRec.m_DrawPoint_X[0]);
	if (endIndex >= (ShowLinePointsNumNow - 1))
		endIndex = ShowLinePointsNumNow - 1;

	// Get the length
	int noOfPoints = endIndex - startIndex + 1;
	if (noOfPoints == 1)
	{
		return;
	}
	// We copy the visible data from the main arrays to separate data arrays
	int arraySizeInBytes = noOfPoints * sizeof(double);
	memcpy(m_viewPortXStamps, &m_oSocketADCDataRec.m_DrawPoint_X[startIndex], arraySizeInBytes);
	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	{
		memcpy(m_viewPortYStamps[i], &m_oSocketADCDataRec.m_dbFduShow[i][startIndex], arraySizeInBytes);
	}

	// 一条线在界面显示点数超过网络接收缓冲区大小时采用抽点显示
	if (noOfPoints >= ShowLinePointsNumNow)
	{
		//
		// Zoomable chart with high zooming ratios often need to plot many thousands of 
		// points when fully zoomed out. However, it is usually not needed to plot more
		// data points than the resolution of the chart. Plotting too many points may cause
		// the points and the lines to overlap. So rather than increasing resolution, this 
		// reduces the clarity of the chart. So it is better to aggregate the data first if
		// there are too many points.
		//
		// In our current example, the chart only has 520 pixels in width and is using a 2
		// pixel line width. So if there are more than 520 data points, we aggregate the 
		// data using the ChartDirector aggregation utility method.
		//
		// If in your real application, you do not have too many data points, you may 
		// remove the following code altogether.
		//

		// Set up an aggregator to aggregate the data based on regular sized slots
		ArrayMath m(DoubleArray(m_viewPortXStamps, noOfPoints));
		m.selectRegularSpacing(noOfPoints / (ShowLinePointsNumNow / 2));

		// For the timestamps, take the first timestamp on each slot
		int aggregatedNoOfPoints = m.aggregate(DoubleArray(m_viewPortXStamps, noOfPoints), 
			Chart::AggregateFirst).len;

		// For the data values, aggregate by taking the averages
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m.aggregate(DoubleArray(m_viewPortYStamps[i], noOfPoints), Chart::AggregateAvg);
		}
		noOfPoints = aggregatedNoOfPoints;
	}

	//
	// Now we have obtained the data, we can plot the chart. 
	//

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 1 - Configure overall chart appearance. 
	///////////////////////////////////////////////////////////////////////////////////////

	// Create an XYChart object 600 x 300 pixels in size, with pale blue (0xf0f0ff) 
	// background, black (000000) border, 1 pixel raised effect, and with a rounded frame.

	XYChart *c = new XYChart(m_oRectWindowGraph.Width(), m_oRectWindowGraph.Height(), SetPlotCtrlBackgroundColor, 0, 1);
	c->setRoundedFrame(m_extBgColor);

	// Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white
	// background. Turn on both horizontal and vertical grid lines with light grey
	// color (0xcccccc). Set clipping mode to clip the data lines to the plot area.
	int iWidth = 0;
	int iHeight = 0;
	iWidth = m_oRectWindowGraph.Width() - SetLegendWidth - m_uiIntervalNum * SetPlotAreaXInterval - SetLegendXInterval;
	iHeight = m_oRectWindowGraph.Height() - SetPlotAreaYInterval - SetPlotAreaBottomInterval;
	c->setPlotArea(m_uiIntervalNum * SetPlotAreaXInterval, SetPlotAreaYInterval, iWidth, iHeight, SetPlotAreaBackgroundColor, -1, -1, SetHGridColor, SetVGridColor);
	//	c->setClipping();

	// Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9
	// pts Arial Bold font. Set the background and border color to Transparent.

	// cxm 2011.10.12不显示标签栏
	//	c->addLegend(rectWindow.Width() - SetLegendWidth, SetLegendYInterval, false, "arialbd.ttf", SetLegnedBoxFontSize)->setBackground(Chart::Transparent);

	// Add a title box to the chart using 15 pts Times Bold Italic font, on a light
	// blue (CCCCFF) background with glass effect. white (0xffffff) on a dark red
	// (0x800000) background, with a 1 pixel 3D border.
	c->addTitle("Hitech Matrix428", "timesbi.ttf", SetTitleBoxFontSize)->setBackground(
		SetTitleBackgroundColor, SetTitleEdgeColor, Chart::glassEffect());

	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		// Swap the x and y axis to become a rotated chart
		c->swapXY();

		// Set the y axis on the top side (right + rotated = top)
		c->setYAxisOnRight(false);

		// Reverse the x axis so it is pointing downwards
		c->xAxis()->setReverse();

	}
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		c->setXAxisOnTop(true);
		c->yAxis()->setReverse();
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 2 - Add data to chart
	///////////////////////////////////////////////////////////////////////////////////////

	// 
	// In this example, we represent the data by lines. You may modify the code below if 
	// you want to use other representations (areas, scatter plot, etc).
	//

	// Add a line layer to the chart
	LineLayer *layer = c->addLineLayer();

	// Set the default line width to 1 pixels
	layer->setLineWidth(SetLineWidth);

	// Set the axes width to 2 pixels
	c->xAxis()->setWidth(SetXAxisWidth);
	c->yAxis()->setWidth(SetYAxisWidth);

	// Now we add the 3 data series to a line layer, using the color red (ff0000), green
	// (00cc00) and blue (0000ff)
	// 载入X轴数据
	layer->setXData(DoubleArray(m_viewPortXStamps, noOfPoints));
	// 载入标签并设置标签和绘线的颜色
	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	{
		char buffer[1024];
		sprintf_s(buffer, sizeof(buffer), "FDU #%d", m_oSocketADCDataRec.m_uiInstrumentNb[i]);

		layer->addDataSet(DoubleArray(m_viewPortYStamps[i], noOfPoints), RedColor, buffer);

		// Chart::Transparent按照原来的背景颜色
		Mark *m = c->yAxis()->addMark(i * m_dbEditLineInterval + 1, Chart::Transparent);
		// 曲线值小于Mark线的部分用原背景色填充，大于Mark线的部分用0x80ff8888颜色填充
		c->addInterLineLayer(layer->getLine(i), m->getLine(), m_oParameterSet.m_iMarkHighColor, m_oParameterSet.m_iMarkLowColor);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 3 - Set up x-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	// Set x-axis date scale to the view port date range. 
	c->xAxis()->setDateScale(viewPortStartDate, viewPortEndDate);

	///////////////////////////////////////////////////////////////////////////////////////
	// Step 4 - Set up y-axis scale
	///////////////////////////////////////////////////////////////////////////////////////

	if (((m_minValue == 0) && (m_maxValue == 0)))
	{
		// y-axis is auto-scaled - save the chosen y-axis scaled to support xy-zoom mode
		c->layout();
		m_minValue = c->yAxis()->getMinValue();
		m_maxValue = c->yAxis()->getMaxValue();
	}
	else
	{
		// xy-zoom mode - compute the actual axis scale in the view port 
		double axisLowerLimit = 0.0;
		double axisUpperLimit = 0.0;
		if (m_oParameterSet.m_uiShowDirection == 1)
		{
			axisLowerLimit = m_minValue + (m_maxValue - m_minValue) * viewer->getViewPortLeft();
			axisUpperLimit = m_minValue + (m_maxValue - m_minValue) * (viewer->getViewPortLeft() + viewer->getViewPortWidth());
		} 
		else if (m_oParameterSet.m_uiShowDirection == 2)
		{
			axisLowerLimit =  m_maxValue - (m_maxValue - m_minValue) * 
				(viewer->getViewPortTop() + viewer->getViewPortHeight());
			axisUpperLimit =  m_maxValue - (m_maxValue - m_minValue) * 
				viewer->getViewPortTop();
		}
		// use the zoomed-in scale
		if (m_oParameterSet.m_uiShowDirection == 1)
		{
			c->yAxis()->setLinearScale(axisLowerLimit, axisUpperLimit);
		} 
		else if (m_oParameterSet.m_uiShowDirection == 2)
		{
			c->yAxis()->setLinearScale(axisUpperLimit, axisLowerLimit);
		}
		// 为true则坐标最大或最小值显示到最近的栅格
		c->yAxis()->setRounding(false, false);
		c->xAxis()->setRounding(false, false);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	// Step 5 - Display the chart
	///////////////////////////////////////////////////////////////////////////////////////

	// Set the chart image to the WinChartViewer
	delete viewer->getChart();
	viewer->setChart(c);
}
//
// Update the image map
//
void C绘图程序Dlg::updateImageMap(CChartViewer *viewer)
{
	if (0 == viewer->getImageMapHandler())
	{
		// no existing image map - creates a new one
		viewer->setImageMap(viewer->getChart()->getHTMLImageMap("clickable", "",
			"title='[{dataSetName}] {x|1}: Volt {value|9}'"));
	}
}
void C绘图程序Dlg::OnCbnSelchangeDuration()
{
	// TODO: 在此添加控件通知处理程序代码
	// Get the selected duration
	CString text;
	m_Duration.GetLBText(m_Duration.GetCurSel(), text);

	// Validate and update the chart
	validateDuration(text);
}

void C绘图程序Dlg::OnCbnKillfocusDuration()
{
	// TODO: 在此添加控件通知处理程序代码
	// Get the duration text
	CString text;
	m_Duration.GetWindowText(text);

	// Validate and update the chart
	validateDuration(text);
}

void C绘图程序Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	// 对话框宽度和高度的增量 
	int iIncrementX = cx - m_iClientWidth;
	int iIncrementY = cy - m_iClientHeight;

	// 最小化时增量为0
	if (nType == SIZE_MINIMIZED)
	{
		iIncrementX = iIncrementY = 0;
	}

	for (int i = 0; i < m_iControlNumber; i++)
	{
		CWnd *pWndCtrl = NULL;

		int iId = m_pControlArray[i].iId;
		int iFlag = m_pControlArray[i].iFlag;
		int iPercent = m_pControlArray[i].iPercent;

		// 无效值
		if ((iPercent < 0) || (iPercent > 100))
			continue;

		// 得到控件指针
		pWndCtrl = GetDlgItem(iId);
		if ((NULL != pWndCtrl) && IsWindow(pWndCtrl->GetSafeHwnd()))
		{
			CRect rectCtrl;
			pWndCtrl->GetWindowRect(rectCtrl);

			ScreenToClient(rectCtrl);

			int iLeft = rectCtrl.left;
			int iTop = rectCtrl.top;
			int iWidth = rectCtrl.Width();
			int iHeight = rectCtrl.Height();

			switch (iFlag)
			{
			case MOVEX: // X方向移动
				iLeft += (iIncrementX * iPercent / 100);
				break;
			case MOVEY: // Y方向移动
				iTop += (iIncrementY * iPercent / 100);
				break;

			case MOVEXY: // X方向和Y方向同时移动
				iLeft += (iIncrementX * iPercent / 100);
				iTop += (iIncrementY * iPercent / 100);
				break;

			case ELASTICX: // X方向改变大小
				iWidth += (iIncrementX * iPercent / 100);
				break;

			case ELASTICY: // Y方向改变大小
				iHeight += (iIncrementY * iPercent / 100);
				break;

			case ELASTICXY: // X方向和Y方向同时改变大小
				iWidth += (iIncrementX * iPercent / 100);
				iHeight += (iIncrementY * iPercent / 100);
				break;

			default:
				;
			}

			// 把控件移动到新位置
			pWndCtrl->MoveWindow(iLeft, iTop, iWidth, iHeight);
		}
	}
	// 把图标移动到对话框右下角
	if (IsWindow(m_wndSizeIcon.GetSafeHwnd()))
		m_wndSizeIcon.MoveWindow(cx - m_bitmap.bmWidth, cy - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// 记录对话框client区域的大小
	if (nType != SIZE_MINIMIZED)
	{
		m_iClientWidth = cx;
		m_iClientHeight = cy;
	}
	// 重绘绘图区
	//
	// Update chart and image map if necessary
	//
	if (NULL != m_ChartViewer.getChart())
	{
		GetDlgItem(IDC_ChartViewer)->GetClientRect(&m_oRectWindowGraph);
		// 重绘绘图区
		m_ChartViewer.updateViewPort(true, false);
	}
	Invalidate(TRUE);//刷新界面
}

void C绘图程序Dlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
	int iWidth = pRect->right - pRect->left;
	int iHeight = pRect->bottom - pRect->top;

	if (iWidth <= m_iMinWidth)
		pRect->right = pRect->left + m_iMinWidth;

	if(iHeight <= m_iMinHeight)
		pRect->bottom = pRect->top + m_iMinHeight;
}
BOOL C绘图程序Dlg::SetControlProperty(PDLGCTLINFO lp, int nElements)
{
	// 设置控件数组信息
	if (NULL == lp)
		return FALSE;

	if (nElements <= 0)
		return FALSE;

	m_pControlArray = lp;
	m_iControlNumber = nElements;
	return TRUE;
}

void C绘图程序Dlg::ShowSizeIcon(BOOL bShow /*=NULL*/)
{
	m_bShowSizeIcon = bShow;
}
void C绘图程序Dlg::OnBnClickedButtonSavechart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csPathName = _T("");
	const wchar_t pszFilter[] = _T("BMP(*.bmp)|*.bmp|GIF(*.gif)|*.gif|JPG(*.jpg)|*.jpg|PNG(*.png)|*.png|JPEG(*.jpeg)|*.jpeg||");
	CFileDialog dlg(FALSE, _T(".bmp"), _T("Matrix428"),OFN_HIDEREADONLY| OFN_OVERWRITEPROMPT,pszFilter, this);

	if ( dlg.DoModal()!=IDOK )
		return;
	csPathName=dlg.GetPathName();
	if (NULL != m_ChartViewer.getChart())
	{
		//因为需要保存的内容包含中文，所以需要如下的转换过程
		int ansiCount = WideCharToMultiByte(CP_ACP, 0, csPathName, -1, NULL, 0, NULL, NULL);
		char * pTempChar = (char*)malloc(ansiCount*sizeof(char));
		memset(pTempChar, 0, ansiCount);
		WideCharToMultiByte(CP_ACP, 0, csPathName, -1, pTempChar, ansiCount, NULL, NULL);
		m_ChartViewer.getChart()->makeChart(pTempChar);
		free(pTempChar);
	}
	// 获取默认打印机信息
	GetAndSetPrinterInfo();
	// 从文件中读取bmp并打印
	LoadBmpFromFileAndPrint(csPathName);
	//  	// 只有bmp格式文件可以读取并打印
	//  	LoadBitmapAndPrint(csPathName);
}

// BOOL C绘图程序Dlg::LoadBitmapAndPrint(CString   csPathName) 
// { 
// 	CDC dc;
// 	CPrintDialog printDlg(FALSE);
// 	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL,csPathName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION); 
// 	if(hBitmap==NULL) 
// 	{ 
// 
// 		TRACE( "Load   Bitmap   Error "); 
// 		return   false; 
// 	} 
//  	BITMAP bitmap;
//  	::GetObject(hBitmap,sizeof(BITMAP),&bitmap);
// 	// 采用默认打印机打印
// 	printDlg.GetDefaults();
// 	DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(printDlg.m_pd.hDevMode);
// 	pDevMode->dmFields = pDevMode->dmFields | DM_PAPERSIZE;
// 	pDevMode->dmPaperSize = DMPAPER_USER;    //将打印纸设置为自定义DMDO_90
// 
// 	pDevMode->dmPaperWidth = bitmap.bmWidth;
// 	pDevMode->dmPaperLength = bitmap.bmHeight;
// 
// 	::GlobalUnlock(printDlg.m_pd.hDevMode);
// 	::DeleteObject(hBitmap);
// 
// 	CPrintInfo Info;
// 	dc.Attach(printDlg.CreatePrinterDC()/*这里很重要,一定要CreatePrinterDC,要不然还是打印机默认的纸型*/); // Attach a printer DC 让HANDLE连接到dc上
// 	int px = dc.GetDeviceCaps(LOGPIXELSX);
// 	int py = dc.GetDeviceCaps(LOGPIXELSY);
// 	dc.m_bPrinting = TRUE;
// 	CString strTitle; // Get the application title ?
// 	strTitle.LoadString(AFX_IDS_APP_TITLE);
// 	DOCINFO di; // Initialise print document details DOCINFO中有相关的打印信息
// 	::ZeroMemory (&di, sizeof (DOCINFO));
// 	di.cbSize = sizeof (DOCINFO);
// 	di.lpszDocName = csPathName;//设置标题
// 
// 	// 打印份数
// 	for (int i=0; i<1; i++)
// 	{
// 		BOOL bPrintingOK = dc.StartDoc(&di); // Begin a new print job 开始打印
// 
// 		// Get the printing extents and store in the m_rectDraw field of a 
// 		// CPrintInfo object  
// 		Info.m_rectDraw.SetRect(0,0,dc.GetDeviceCaps(HORZRES),dc.GetDeviceCaps(VERTRES));//设置范围
// 
// 		Info.SetMaxPage (1);
// 		OnPrint(&dc, &Info,csPathName); // 往DC上画图片,具体实现在下面给出
// 		//OnEndPrinting(&dc, &Info); // 结束打印
// 		if (bPrintingOK)
// 			dc.EndDoc(); // end a print job
// 		else
// 			dc.AbortDoc(); // abort job. 
// 	}
// 	dc.Detach(); // detach the printer DC
// 
// 	return TRUE;
// }
// void C绘图程序Dlg::OnPrint(CDC* pdc,CPrintInfo * lParam,CString strFileName)
// {
// 	CDC* pDC   = pdc;
// 	CPrintInfo* pInfo = (CPrintInfo *)lParam;
// 
// 	CFont DataFont;
// 	DataFont.CreatePointFont(120, _T("宋体"),pDC);
// 
// 	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL,strFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION); 
// 
// 	if(hBitmap==NULL) 
// 	{ 
// 
// 		TRACE( "Load   Bitmap   Error "); 
// 		return; 
// 	} 
// 	BITMAP bitmap;
// 	::GetObject(hBitmap,sizeof(BITMAP),&bitmap);
// 	double dScale=(double)pInfo->m_rectDraw.Width()/bitmap.bmWidth;
// 	//int nScaledWidth=m_cxWidth;
// 	int nScaledHeight=(int)(bitmap.bmHeight*dScale);
// 
// 	HDC dcMem;
// 	dcMem=::CreateCompatibleDC(pDC->m_hDC);
// 	HBITMAP hOldBmp=(HBITMAP)::SelectObject(dcMem,hBitmap);
// 
// 	CRect r = pInfo->m_rectDraw;
// 	//	SizeToPlace(STP_FIX, r, CRect(0,0,bitmap.bmWidth, bitmap.bmHeight));//这个函数没有给出,可以自己写一下,注释掉也行,功能就是调整尺寸的
// 	int nVertCenterPos = pDC->GetDeviceCaps (VERTRES) / 2;
// 	::StretchBlt(pDC->m_hDC, r.left, r.top, r.Width(), r.Height(),
// 		dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
// 	::SelectObject(dcMem,hOldBmp);
// 	::DeleteDC(dcMem);
// 	::DeleteObject(hBitmap);
// }
void C绘图程序Dlg::OnBnClickedButtonOpenadcfile()
{
	// TODO: 在此添加控件通知处理程序代码
	// 打开文件
	const wchar_t pszFilter[] = _T("文本文件(*.text)|*.text|文本文件(*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T(".text"), _T("1.text"), OFN_HIDEREADONLY| OFN_OVERWRITEPROMPT, pszFilter, this);

	if ( dlg.DoModal()!=IDOK )
	{
		return;
	}

	// 载入数据
	m_ADCDataInfo.clear();
	// 读取文件中数据的行数
	m_uiADCFileLineNum = 0;
	if(FALSE == LoadData(dlg.GetPathName(), FALSE))
	{
		return;
	}
	if (FALSE == FraseDataAndDrawGraph())
	{
		return;
	}
	// 绘图
	OnPrepareToDrawGraph();
}

// 从文件中载入数据
BOOL C绘图程序Dlg::LoadData(CString csOpenFilePath, BOOL bLoadLastFile)
{
	if ((_taccess(csOpenFilePath,0)) != -1)
	{
		CFile file;
		if(file.Open(csOpenFilePath, CFile::modeRead) == FALSE)
		{
			AfxMessageBox(_T("打开数据采样文件出错！"));	
			return FALSE;
		}
		else
		{
			CString str = _T("");
			CArchive ar(&file, CArchive::load);
			unsigned int uiADCLastFileLineNum = 0;

			// 参与ADC数据采集的采集站设备数
			m_uiInstrumentADCNum = 0;
			// ADC数据开始的数据点数
			m_uiADCStartNum = 0;
			// ADC数据转换格式
			m_uiADCDataCovNb = 0;
			// ADC数据采样开始时间
			ar.ReadString(str);

			// 初始化变量
			m_DrawPoint_X.clear();
			if (m_dbFduData != NULL)
			{
				for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
				{
					m_dbFduData[i].clear();
				}
				delete[] m_dbFduData;
				m_dbFduData = NULL;
			}
			if (m_dbFduShow != NULL)
			{
				for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
				{
					m_dbFduShow[i].clear();
				}
				delete[] m_dbFduShow;
				m_dbFduShow = NULL;
			}
			// ADC数据采样信息
			ar.ReadString(str);
			_stscanf_s(str, _T("采集站设备总数%d，从第%d个数据开始存储ADC数据，数据转换方式采用方式%d！"), &m_uiInstrumentADCNum, &m_uiADCStartNum, &m_uiADCDataCovNb);
			if (m_uiInstrumentADCNum == 0)
			{
				//				fp_str.close();
				ar.Close();
				file.Close();
				AfxMessageBox(_T("采集站设备总数为0！"));
				return FALSE;
			}
			for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
			{
				m_uiInstrumentNb[i] = 0;
			}
			// 采集站设备标签
			ar.ReadString(str);
			OnPhraseLabels(str);
			while(ar.ReadString(str))
			{
				m_uiADCFileLineNum++;
				if (bLoadLastFile == TRUE)
				{
					uiADCLastFileLineNum++;
				}
				m_ADCDataInfo.push_back(str);
			}
			ar.Close();
			file.Close();
			if (bLoadLastFile == TRUE)
			{
				vector<CString> temp;
				for (unsigned int i=0; i<(m_uiADCFileLineNum - uiADCLastFileLineNum); i++)
				{
					temp.push_back(m_ADCDataInfo[i]);
				}
				m_ADCDataInfo.erase(m_ADCDataInfo.begin(), m_ADCDataInfo.begin() + m_uiADCFileLineNum - uiADCLastFileLineNum);
				for (unsigned int i=0; i<(m_uiADCFileLineNum - uiADCLastFileLineNum); i++)
				{
					m_ADCDataInfo.push_back(temp[i]);
				}
				temp.clear();
			}
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

// 解码一行ADC数据
void C绘图程序Dlg::OnPhraseEachLine(CString str)
{
	int iDirectionPrevious = 0;
	int iDirectionNow = 0;
	int iDirectionEnd = 0;
	double dbTemp = 0.0L;
	CString cstmp = _T("");
	iDirectionEnd = str.GetLength();
	while(iDirectionEnd != iDirectionPrevious)
	{
		iDirectionNow = str.Find(_T(" \t"), iDirectionPrevious);
		cstmp = str.Mid(iDirectionPrevious, iDirectionNow-iDirectionPrevious);
		iDirectionPrevious = iDirectionNow + ADCDataInterval;
		if (cstmp == _T(""))
		{
			break;
		}
		else
		{
			dbTemp = _tstof(cstmp);
			// 将文件中数据换算成电压值
			if (dbTemp < 0x7FFFFF)
			{
				dbTemp = dbTemp/( 0x7FFFFE ) * DrawGraphYAxisUpper;
			}
			else if (dbTemp > 0x800000)
			{
				dbTemp = (0xFFFFFF - dbTemp)/( 0x7FFFFE ) * (DrawGraphYAxisLower);
			}
			m_dbFduData[m_uiADCDataNum%m_uiInstrumentADCNum].push_back(dbTemp);
			m_uiADCDataNum++;
		}
	}
}

void C绘图程序Dlg::OnBnClickedYzoompb()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		m_ChartViewer.setZoomDirection(Chart::DirectionHorizontal); 
		m_ChartViewer.setScrollDirection(Chart::DirectionHorizontal);
	}
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		m_ChartViewer.setZoomDirection(Chart::DirectionVertical); 
		m_ChartViewer.setScrollDirection(Chart::DirectionVertical);
	}
	if (m_dbFduData == NULL)
	{
		return;
	}

	// Update chart to auto-scale axis
	m_ChartViewer.updateViewPort(true, true);
}

void C绘图程序Dlg::OnBnClickedButtonRedraw()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str = _T("");
	// 判断是否有数据可以重绘
	if (m_dbFduData == NULL)
	{
		return;
	}

	// 禁用按键直到绘图重绘操作完成
	GetDlgItem(IDC_BUTTON_REDRAW)->EnableWindow(FALSE);
	// 得到界面设置值
	UpdateData(TRUE);
	unsigned int icurrentDuration = (unsigned int)m_currentDuration;
	GetShowData((icurrentDuration / m_uiEditSampleRate));
	// 重绘绘图区
	m_ChartViewer.updateViewPort(true, true);

	GetDlgItem(IDC_BUTTON_REDRAW)->EnableWindow(TRUE);
}

// 创建ADC数据接收Socket
void C绘图程序Dlg::OnCreateADCRecSocket(void)
{
	BOOL bReturn = FALSE;
	unsigned int uiRcvPort = 0;
	_stscanf_s(m_oParameterSet.m_csRcvPort,_T("%x"), &uiRcvPort);
	// 生成网络端口，接收发送命令应答帧，create函数写入第三个参数IP地址则接收固定IP地址发送的帧，不写则全网接收
	m_oSocketADCDataRec.Close();
	bReturn = m_oSocketADCDataRec.Create(uiRcvPort, SOCK_DGRAM);
	if (bReturn == FALSE)
	{
		AfxMessageBox(_T("ADC数据接收端口创建失败！"));
	}
	else
	{
		int iOptionValue = ADCRecPortBufSize;
		int iOptionLen = sizeof(int);
		bReturn = m_oSocketADCDataRec.SetSockOpt(SO_RCVBUF, (void*)&iOptionValue, iOptionLen, SOL_SOCKET);
		if (bReturn == FALSE)
		{
			AfxMessageBox(_T("ADC数据接收端口接收缓冲区设置失败！"));
		}
	}
}

void C绘图程序Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_oSocketADCDataRec.Close();
	CDialog::OnClose();
}

void C绘图程序Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// ADC命令设置应答监视
	if (nIDEvent == DataRefreshTimerNb)
	{
		if (m_oSocketADCDataRec.m_bPrepareToShow == true)
		{
			OnNetADCGraph();
		}
	}
	else if (nIDEvent == GraphRefreshTimerNb)
	{
		if (m_oSocketADCDataRec.m_bPrepareToShow == true)
		{
			// 得到默认背景颜色
			m_extBgColor = getDefaultBgColor();
			// Y轴范围
			// 如果Y轴坐标固定则只有第一次需要重新画Y轴坐标
			if (m_bStartShow == TRUE)
			{
				m_bStartShow = FALSE;
				// 重新画Y轴坐标
				m_minValue = m_maxValue = 0;
			} 
			else
			{
				// 如果Y轴坐标不固定则每次都重新画Y轴坐标
				if (m_bCheckYAxisFixed == FALSE)
				{
					m_minValue = m_maxValue = 0;
				}
			}
			m_ChartViewer.updateViewPort(true, false);
		}
		// 重绘绘图区
	}
	CDialog::OnTimer(nIDEvent);
}

void C绘图程序Dlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	// 创建Socket
	OnCreateADCRecSocket();

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	m_uiOptType = OptTypeNetwork;
	m_uiRecFrameBeginMaxNb = 0;
	m_uiRecFrameEndMinNb = 0;
	m_uiDrawFrameNum = 0;
	SetTimer(GraphRefreshTimerNb, GraphRefreshTimerSet, NULL);
	SetTimer(DataRefreshTimerNb, DataRefreshTimerSet, NULL);
	m_bStartShow = TRUE;
	UpdateData(TRUE);
	m_oSocketADCDataRec.m_uiSamplingRate = m_uiEditSampleRate;
	m_oSocketADCDataRec.m_bPrepareToShow = false;
	m_oSocketADCDataRec.OnMakeAndSendSetFrame(StartGraphShow);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_PointerPB)->EnableWindow(FALSE);
	GetDlgItem(IDC_ZoomInPB)->EnableWindow(FALSE);
	GetDlgItem(IDC_ZoomOutPB)->EnableWindow(FALSE);
}

void C绘图程序Dlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	KillTimer(GraphRefreshTimerNb);
	KillTimer(DataRefreshTimerNb);
	m_oSocketADCDataRec.OnMakeAndSendSetFrame(StopGraphShow);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_PointerPB)->EnableWindow(TRUE);
	GetDlgItem(IDC_ZoomInPB)->EnableWindow(TRUE);
	GetDlgItem(IDC_ZoomOutPB)->EnableWindow(TRUE);
}

// 绘制网络ADC数据
void C绘图程序Dlg::OnNetADCGraph(void)
{
	m_uiRecFrameBeginMaxNb = m_oSocketADCDataRec.GetRecFrameBeginMaxNb();
	m_uiRecFrameEndMinNb = m_oSocketADCDataRec.GetRecFrameEndMinNb();
	if (m_uiRecFrameEndMinNb == 0)
	{
		return;
	}
	if (m_uiRecFrameEndMinNb < m_uiRecFrameBeginMaxNb)
	{
		return;
	}
	unsigned int uiDrawFrameNumOld = m_uiDrawFrameNum;
	m_uiDrawFrameNum += m_uiRecFrameEndMinNb - m_uiRecFrameBeginMaxNb + 1;

	m_oSocketADCDataRec.GetDrawFrameBeginNum(m_uiRecFrameBeginMaxNb);
	m_currentDuration = ShowLinePointsNumNow * m_oSocketADCDataRec.m_uiSamplingRate;
	//	m_currentDuration = (m_uiRecFrameEndMinNb - m_uiRecFrameBeginMaxNb + m_oSocketADCDataRec.m_uiSamplingRate) * ReceiveDataNum;
	m_minDuration = ShowLinePointsNumMin * m_oSocketADCDataRec.m_uiSamplingRate;
	m_uiInstrumentADCNum = m_oSocketADCDataRec.m_uiInstrumentADCNum;

	for (unsigned int i=0; i<ShowLinePointsNumNow; i++)
	{
		m_oSocketADCDataRec.m_DrawPoint_X[i] = (m_uiRecFrameBeginMaxNb * ReceiveDataNum + i) * m_oSocketADCDataRec.m_uiSamplingRate;
	}
	// 横坐标的最小值为m_DrawPoint_X数组的第一个值
	m_minData = m_oSocketADCDataRec.m_DrawPoint_X[0];
	m_maxData = m_oSocketADCDataRec.m_DrawPoint_X[ShowLinePointsNumNow - 1];
	OnSetXAxisRange(m_maxData, m_minData);

	double dbTemp = 0.0;
	if (m_uiDrawFrameNum > ADCRecFrameShowNum)
	{
		// 从绘图缓冲区尾部开始压入数据
		if (uiDrawFrameNumOld < ADCRecFrameShowNum)
		{
			// 从绘图缓冲区头部开始压入数据
			for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
			{
				for (unsigned int j=0; j<(ADCRecFrameShowNum - uiDrawFrameNumOld) * ReceiveDataNum; j++)
				{
					dbTemp = m_oSocketADCDataRec.m_dbFduData[i][m_oSocketADCDataRec.m_uiDrawFrameBeginNum[i] * ReceiveDataNum + j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
					if (m_bCheckWaveRangeFixed == TRUE)
					{
						if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
						}
						else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
						}
					}
					m_oSocketADCDataRec.m_dbFduShow[i][uiDrawFrameNumOld * ReceiveDataNum + j] = dbTemp;
				}
			}
			// 从绘图缓冲区尾部开始压入数据
			for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
			{
				for (unsigned int j=0; j<(m_uiDrawFrameNum - ADCRecFrameShowNum) * ReceiveDataNum; j++)
				{
					dbTemp = m_oSocketADCDataRec.m_dbFduData[i][(m_oSocketADCDataRec.m_uiDrawFrameBeginNum[i] + ADCRecFrameShowNum - uiDrawFrameNumOld)* ReceiveDataNum + j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
					if (m_bCheckWaveRangeFixed == TRUE)
					{
						if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
						}
						else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
						}
					}
					// 尾部压入缓冲区
					memmove_s(&m_oSocketADCDataRec.m_dbFduShow[i][0], sizeof(double) * ShowLinePointsNumNow,
						&m_oSocketADCDataRec.m_dbFduShow[i][1], sizeof(double) * (ShowLinePointsNumNow - 1));
					m_oSocketADCDataRec.m_dbFduShow[i][ShowLinePointsNumNow - 1] = dbTemp;
				}
			}
		}
		else
		{
			// 从绘图缓冲区尾部开始压入数据
			for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
			{
				for (unsigned int j=0; j<(m_uiDrawFrameNum - uiDrawFrameNumOld) * ReceiveDataNum; j++)
				{
					dbTemp = m_oSocketADCDataRec.m_dbFduData[i][m_oSocketADCDataRec.m_uiDrawFrameBeginNum[i]* ReceiveDataNum + j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
					if (m_bCheckWaveRangeFixed == TRUE)
					{
						if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
						}
						else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
						{
							dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
						}
					}
					// 尾部压入缓冲区
					memmove_s(&m_oSocketADCDataRec.m_dbFduShow[i][0], sizeof(double) * ShowLinePointsNumNow,
						&m_oSocketADCDataRec.m_dbFduShow[i][1], sizeof(double) * (ShowLinePointsNumNow - 1));
					m_oSocketADCDataRec.m_dbFduShow[i][ShowLinePointsNumNow - 1] = dbTemp;
				}
			}
		}
	}
	else
	{
		// 从绘图缓冲区头部开始压入数据
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			for (unsigned int j=0; j<(m_uiDrawFrameNum - uiDrawFrameNumOld) * ReceiveDataNum; j++)
			{
				dbTemp = m_oSocketADCDataRec.m_dbFduData[i][m_oSocketADCDataRec.m_uiDrawFrameBeginNum[i] * ReceiveDataNum + j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
				if (m_bCheckWaveRangeFixed == TRUE)
				{
					if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
					{
						dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
					}
					else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
					{
						dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
					}
				}
				m_oSocketADCDataRec.m_dbFduShow[i][uiDrawFrameNumOld * ReceiveDataNum + j] = dbTemp;
			}
		}
	}

	// 	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	// 	{
	// 		for (unsigned int j=0; j<ShowLinePointsNumNow; j++)
	// 		{
	// 			if (m_uiDrawFrameNum > ADCRecFrameShowNum)
	// 			{
	// 				dbTemp = m_oSocketADCDataRec.m_dbFduData[i][j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
	// 				if (m_bCheckWaveRangeFixed == TRUE)
	// 				{
	// 					if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
	// 					{
	// 						dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
	// 					}
	// 					else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
	// 					{
	// 						dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
	// 					}
	// 				}
	// 				m_oSocketADCDataRec.m_dbFduShow[i][j] = dbTemp;
	// 			}
	// 			else
	// 			{
	// 				if (j >= (ReceiveDataNum * m_uiDrawFrameNum))
	// 				{
	// 					m_oSocketADCDataRec.m_dbFduShow[i][j] = Chart::NoValue;
	// 				}
	// 				else
	// 				{
	// 					dbTemp = m_oSocketADCDataRec.m_dbFduData[i][j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
	// 					if (m_bCheckWaveRangeFixed == TRUE)
	// 					{
	// 						if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
	// 						{
	// 							dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
	// 						}
	// 						else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
	// 						{
	// 							dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
	// 						}
	// 					}
	// 					m_oSocketADCDataRec.m_dbFduShow[i][j] = dbTemp;
	// 				}
	// 			}
	// 		}
	// 	}

	// 缓存数组
	unsigned int uiTemp[ADCFrameNum];
	// 清除已绘制图形的缓冲区
	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	{
		int iSize = m_oSocketADCDataRec.m_uiRecFrameNum[i];
		if (iSize == 0)
		{
			continue;
		}
		unsigned int uiEraseNum = m_oSocketADCDataRec.GetRecFrameBeginToEndNum(i, m_uiRecFrameEndMinNb);
		if (uiEraseNum == 0)
		{
			continue;
		}
		memmove_s(&m_oSocketADCDataRec.m_dbFduData[i][0], ADCRecBufSize * sizeof(double), &m_oSocketADCDataRec.m_dbFduData[i][ReceiveDataNum * uiEraseNum], 
			sizeof(double) * ReceiveDataNum * (ADCRecFrameSaveNum - uiEraseNum));
		m_oSocketADCDataRec.m_uiRecFrameNum[i] -= uiEraseNum;
		memcpy(&uiTemp[0], &m_oSocketADCDataRec.m_uiRecFrameNb[i][0], sizeof(unsigned int) * ADCFrameNum);
		memset(&m_oSocketADCDataRec.m_uiRecFrameNb[i][0], SndFrameBufInit, sizeof(unsigned int) * ADCFrameNum);
		memcpy(&m_oSocketADCDataRec.m_uiRecFrameNb[i][0], &uiTemp[uiEraseNum], sizeof(unsigned int) * (ADCFrameNum - uiEraseNum));
	}
}

// 设置X轴取值范围和标签间隔
void C绘图程序Dlg::OnSetXAxisRange(double dbmaxData, double dbminData)
{
	// 横坐标取值的变化范围
	m_dateRange = dbmaxData - dbminData;

	// 绘图区域左侧间隔个数
	double maxData = 0.0;
	maxData = dbmaxData;
	m_uiIntervalNum = 0;
	while(maxData > 1)
	{
		maxData = maxData / 10;
		m_uiIntervalNum++;
	}
	if (m_uiIntervalNum < SetLegendXIntervalNumMin)
	{
		m_uiIntervalNum = SetLegendXIntervalNumMin;
	}
	// 设置ChartViewer能反映有效且最小的持续时间
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		m_ChartViewer.setZoomInHeightLimit(m_minDuration / m_dateRange);
		m_ChartViewer.setViewPortHeight(m_currentDuration / m_dateRange);
		m_ChartViewer.setViewPortTop(1 - m_ChartViewer.getViewPortHeight());
	} 
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		m_ChartViewer.setZoomInWidthLimit(m_minDuration / m_dateRange);
		m_ChartViewer.setViewPortWidth(m_currentDuration / m_dateRange);
		m_ChartViewer.setViewPortLeft(1 - m_ChartViewer.getViewPortWidth());
	}
}

void C绘图程序Dlg::OnBnClickedCheckYaxisfixed()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bCheckYAxisFixed = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_YAXISFIXED)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
}

// 载入数据文件后做绘图的准备工作
void C绘图程序Dlg::OnPrepareToDrawGraph(void)
{
	//
	// 初始化成员变量
	//
	UpdateData(TRUE);

	// 得到默认背景颜色
	m_extBgColor = getDefaultBgColor();
	// Y轴范围
	m_minValue = m_maxValue = 0;

	// 初始状态每条线显示的点数
	if (m_uiADCDataFduNum > ShowLinePointsNumNow)
	{
		m_currentDuration = ShowLinePointsNumNow * m_uiEditSampleRate;
	} 
	else
	{
		m_currentDuration = m_uiADCDataFduNum * m_uiEditSampleRate;
	}
	if (m_uiADCDataFduNum > ShowLinePointsNumMin)
	{
		m_minDuration = ShowLinePointsNumMin * m_uiEditSampleRate;
	}
	else
	{
		m_minDuration = m_uiADCDataFduNum * m_uiEditSampleRate;
	}
	// 横坐标的最大最小值
	if (m_uiOptType == OptTypeLoadFolder)
	{
		m_minData = 0;
		m_maxData = m_uiEndDrawPointsNumMax - 1;
	}
	else if (m_uiOptType == OptTypeLoadFile)
	{
		m_minData = m_uiADCStartNum;
		m_maxData = m_DrawPoint_X[m_DrawPoint_X.size() - 1];
	}
	OnSetXAxisRange(m_maxData, m_minData);
	// 重绘绘图区
	m_ChartViewer.updateViewPort(true, true);
}

void C绘图程序Dlg::OnBnClickedButtonOpenadcfolder()
{
	// TODO: 在此添加控件通知处理程序代码
	wchar_t szDir[MAX_PATH];
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	CString csSaveFolderPath = _T("");
	bi.hwndOwner = this->m_hWnd; // 指定父窗口，在对话框显示期间，父窗口将被禁用 
	bi.pidlRoot = NULL; // 如果指定NULL，就以“桌面”为根 
	bi.pszDisplayName = szDir; 
	bi.lpszTitle = _T("请选择采样数据存储目录"); // 这一行将显示在对话框的顶端 
	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi);

	if(pidl == NULL) 
	{
		return;
	}
	if(!SHGetPathFromIDList(pidl, szDir))
	{	
		return;
	}
	else
	{
		csSaveFolderPath = szDir;
		m_FileInfo.clear();
		FindFileAndList(csSaveFolderPath);
	}
}

// 查找文件夹下的文件并列出
void C绘图程序Dlg::FindFileAndList(CString csSaveFolderPath)
{
	CListBox* pListBox = (CListBox* )GetDlgItem(IDC_LIST_FILE);
	CFileFind findFile;	// 文件查找对象
	CString strPath = csSaveFolderPath + _T("\\*.text");
	CString strFileName = _T("");
	CString strFilePath = _T("");
	BOOL bWorking = findFile.FindFile(strPath);          //执行文件搜索
	vector<int> fileNb;
	fileNb.clear();
	// 先清空ListBox控件
	int icount = pListBox->GetCount();
	for (int i = icount - 1; i >= 0; i--)
	{
		pListBox->DeleteString(i);
	}
	while(bWorking)
	{
		bWorking = findFile.FindNextFile();						//查找下一个文件
		if(findFile.IsDirectory())										//若为目录，结束本次循环
		{
			continue;
		}
		if (findFile.IsDots())
		{
			continue;
		}
		strFileName=findFile.GetFileName();					//获取文件名称，包括后缀
		int iDirectionLable = 0;
		CString csTemp = _T("");
		iDirectionLable = strFileName.Find(_T(".text"), 0);
		csTemp = strFileName.Mid(0, iDirectionLable);
		fileNb.push_back(_ttoi(csTemp));
	}
	sort(fileNb.begin(), fileNb.end());
	for (unsigned int i=0; i<fileNb.size(); i++)
	{
		strFileName.Format(_T("%d.text"), fileNb[i]);
		pListBox->AddString(strFileName);						// 加入到ListBox控件中
		strFilePath = csSaveFolderPath + _T("\\") + strFileName;
		m_FileInfo.push_back(strFilePath);
	}
	fileNb.clear();
	if (m_FileInfo.size() == 0)
	{
		return;
	}
	if (FALSE == LoadLastADCDataFromFile())
	{
		return;
	}
	LoadADCDataFromFile((m_uiEndDrawPointsNumMax > ShowLinePointsNumNow) ? 
		(m_uiEndDrawPointsNumMax - ShowLinePointsNumNow - 1) : 0, m_uiEndDrawPointsNumMax - 1);
	OnPrepareToDrawGraph();
}

void C绘图程序Dlg::OnLbnDblclkListFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CListBox* pListBox = (CListBox* )GetDlgItem(IDC_LIST_FILE);
	int nIndex = pListBox->GetCurSel();
	CString csOpenFilePath = _T("");
	if (nIndex == CB_ERR)
	{
		return;
	}
	// 	CString strFileName = _T("");
	// 	int n = pListBox->GetTextLen(nIndex);
	// 	pListBox->GetText(nIndex, strFileName.GetBuffer(n));
	// 	strFileName.ReleaseBuffer();

	csOpenFilePath = m_FileInfo[nIndex];
	m_uiOpenFileNb = nIndex;

	m_ADCDataInfo.clear();
	// 读取文件中数据的行数
	m_uiADCFileLineNum = 0;
	// 载入数据
	if(FALSE == LoadData(csOpenFilePath, FALSE))
	{
		return;
	}
	if (FALSE == FraseDataAndDrawGraph())
	{
		return;
	}
	// 绘图
	OnPrepareToDrawGraph();
}


// 从ADC数据信息向量表中解析数据用于绘图
BOOL C绘图程序Dlg::FraseDataToDraw(unsigned int uiStartDrawPointsNum, unsigned int uiEndDrawPointsNum)
{
	CString str = _T("");
	UpdateData(TRUE);
	if (m_uiADCFileLineNum == 0)
	{
		return FALSE;
	}
	m_uiOptType = OptTypeLoadFile;
	if (uiStartDrawPointsNum < m_uiADCStartNum)
	{
		CString csOpenFilePath = _T("");
		if (m_uiOpenFileNb >= 1)
		{
			m_uiOpenFileNb = m_uiOpenFileNb - 1;
			csOpenFilePath = m_FileInfo[m_uiOpenFileNb];
			// 载入数据
			if(FALSE == LoadData(csOpenFilePath, TRUE))
			{
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox(_T("文件不存在！"));
			return FALSE;
		}
	}
	// 初始化变量
	m_DrawPoint_X.clear();
	if (m_dbFduData != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduData[i].clear();
		}
		delete[] m_dbFduData;
		m_dbFduData = NULL;
	}
	if (m_dbFduShow != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduShow[i].clear();
		}
		delete[] m_dbFduShow;
		m_dbFduShow = NULL;
	}

	// 采集站采集到的ADC数据总数
	m_uiADCDataNum = 0;
	// 每个采集站采集到的ADC数据个数
	m_uiADCDataFduNum = 0;
	m_dbFduData = new vector<double>[m_uiInstrumentADCNum];
	m_dbFduShow = new vector<double>[m_uiInstrumentADCNum];

	for (unsigned int i=0; i<(uiEndDrawPointsNum - uiStartDrawPointsNum + 1); i++)
	{
		if (i % m_uiEditSampleRate == 0)
		{
			OnPhraseEachLine(m_ADCDataInfo[i + uiStartDrawPointsNum - m_uiADCStartNum]);
		}
	}
	if ((uiEndDrawPointsNum - uiStartDrawPointsNum) % m_uiEditSampleRate != 0)
	{
		OnPhraseEachLine(m_ADCDataInfo[uiEndDrawPointsNum - m_uiADCStartNum]);
	}

	m_uiADCDataFduNum = m_uiADCDataNum / m_uiInstrumentADCNum;

	for (unsigned int i=0; i<(m_uiADCDataFduNum - 1); i++)
	{
		m_DrawPoint_X.push_back(i * m_uiEditSampleRate + uiStartDrawPointsNum);
	}
	m_DrawPoint_X.push_back(uiEndDrawPointsNum);
	GetShowData(m_uiADCDataFduNum);
	return TRUE;
}

// 解析数据并绘图
BOOL C绘图程序Dlg::FraseDataAndDrawGraph(void)
{
	unsigned int uiStartDrawPointsNum = 0;
	unsigned int uiEndDrawPointsNum = 0;
	if ((m_uiADCFileLineNum + m_uiADCStartNum) > ShowLinePointsNumNow)
	{
		uiStartDrawPointsNum = m_uiADCFileLineNum + m_uiADCStartNum - ShowLinePointsNumNow;
	}
	else
	{
		uiStartDrawPointsNum = m_uiADCStartNum;
	}
	uiEndDrawPointsNum = m_uiADCFileLineNum + m_uiADCStartNum - 1;
	if (FALSE == FraseDataToDraw(uiStartDrawPointsNum, uiEndDrawPointsNum))
	{
		return FALSE;
	}
	return TRUE;
}

// 从文件中载入数据
void C绘图程序Dlg::LoadADCDataFromFile(unsigned int uiStartDrawPointsNum, unsigned int uiEndDrawPointsNum)
{
	unsigned int uiStartFileNb = uiStartDrawPointsNum  / (SaveADCDataPackageNum * ReceiveDataNum);
	ULONGLONG ullPointer = 0;
	CString str = _T("");
	UpdateData(TRUE);

	// 初始化变量
	m_DrawPoint_X.clear();
	if (m_dbFduData != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduData[i].clear();
		}
	}
	else
	{
		m_dbFduData = new vector<double>[m_uiInstrumentADCNum];
	}
	if (m_dbFduShow != NULL)
	{
		for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
		{
			m_dbFduShow[i].clear();
		}
	}
	else
	{
		m_dbFduShow = new vector<double>[m_uiInstrumentADCNum];
	}
	m_uiFindLineCount = 0;
	m_uiADCDataNum = 0;
	m_uiOptType = OptTypeLoadFolder;
	if (m_uiEndDrawPointsNumMax < ShowLinePointsNumNow)
	{
		uiEndDrawPointsNum = m_uiEndDrawPointsNumMax;
	}

	for (unsigned int i=uiStartFileNb; i<m_FileInfo.size(); i++)
	{
		if ((_taccess(m_FileInfo[i], 0)) != -1)
		{
			if(m_file.Open(m_FileInfo[i], CFile::modeRead) == FALSE)
			{
				AfxMessageBox(_T("打开数据采样文件出错！"));	
				break;
			}
			else
			{
				CArchive ar(&m_file, CArchive::load);
				ULONGLONG lActual;
				unsigned int uiLength = 0;
				unsigned int uiRead = 0;
				m_file.SeekToBegin();
				ULONGLONG ullEnd = m_file.SeekToEnd();

				// 光标移动为字符长度+2(\n长度)
				m_file.SeekToBegin();
				// ADC数据采样开始时间
				ar.ReadString(str);
				uiLength += (str.GetLength() + 2) * sizeof(wchar_t);
				lActual = m_file.Seek(uiLength, CFile::begin);
				// ADC数据采样信息
				ar.ReadString(str);
				uiLength  += (str.GetLength() + 2) * sizeof(wchar_t);
				lActual = m_file.Seek(uiLength, CFile::begin);
				// 采集站设备标签
				ar.ReadString(str);
				uiLength  += (str.GetLength() + 2) * sizeof(wchar_t);
				lActual = m_file.Seek(uiLength, CFile::begin);

				if (i == uiStartFileNb)
				{
					ullPointer = uiStartDrawPointsNum % (SaveADCDataPackageNum * ReceiveDataNum);
				}
				// 跳过
				lActual = m_file.Seek(m_uiADCLineBufNum * sizeof(wchar_t)* ullPointer, CFile::current);
				do 
				{
					if (m_uiFindLineCount == (uiEndDrawPointsNum - uiStartDrawPointsNum + 1))
					{
						break;
					}
					else
					{
						memset(m_pLinebufRead, 0, m_uiADCLineBufNum * sizeof(wchar_t));
						// 读1行数据
						uiRead = m_file.Read(m_pLinebufRead, m_uiADCLineBufNum * sizeof(wchar_t));
						str.Format(_T("%s"), m_pLinebufRead);
						OnPhraseEachLine(str);
						if (uiRead == 0)
						{
							break;
						}
						else
						{
							m_uiFindLineCount++;
							// 光标移动为字符长度+4(\r\n长度)
							// 跳过
							lActual = m_file.Seek(m_uiADCLineBufNum * sizeof(wchar_t)* (m_uiEditSampleRate - 1), CFile::current);
						}
					}
				} while (lActual <ullEnd);
				m_file.SeekToEnd();
				ar.Close();
				m_file.Close();
				if ((uiRead != 0) && (m_uiFindLineCount != (uiEndDrawPointsNum - uiStartDrawPointsNum + 1)))
				{
					ullPointer = (lActual - ullEnd) / (m_uiADCLineBufNum * sizeof(wchar_t)); 
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	m_uiADCDataFduNum = m_uiADCDataNum / m_uiInstrumentADCNum;

	for (unsigned int i=0; i<m_uiADCDataFduNum; i++)
	{
		m_DrawPoint_X.push_back(i * m_uiEditSampleRate + uiStartDrawPointsNum);
	}
	GetShowData(m_uiADCDataFduNum);
}

// 载入最后一个ADC数据采样文件
BOOL C绘图程序Dlg::LoadLastADCDataFromFile(void)
{
	unsigned int uiLastFileNb = m_FileInfo.size() - 1;
	if ((_taccess(m_FileInfo[uiLastFileNb], 0)) != -1)
	{
		if(m_file.Open(m_FileInfo[uiLastFileNb], CFile::modeRead) == FALSE)
		{
			AfxMessageBox(_T("打开最后一个数据采样文件出错！"));	
			return FALSE;
		}
		else
		{
			CString str = _T("");
			CArchive ar(&m_file, CArchive::load);
			ULONGLONG lActual = 0;
			ULONGLONG ullEnd = 0;
			unsigned int uiLength = 0;
			unsigned int uiLineNum = 0;
			// 初始化变量
			m_DrawPoint_X.clear();
			if (m_dbFduData != NULL)
			{
				for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
				{
					m_dbFduData[i].clear();
				}
				delete[] m_dbFduData;
				m_dbFduData = NULL;
			}
			if (m_dbFduShow != NULL)
			{
				for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
				{
					m_dbFduShow[i].clear();
				}
				delete[] m_dbFduShow;
				m_dbFduShow = NULL;
			}
			if (m_pLinebufRead != NULL)
			{
				delete[] m_pLinebufRead;
				m_pLinebufRead = NULL;
			}
			// 参与ADC数据采集的采集站设备数
			m_uiInstrumentADCNum = 0;
			// ADC数据开始的数据点数
			m_uiADCStartNum = 0;
			// ADC数据转换格式
			m_uiADCDataCovNb = 0;
			// ADC数据文件一行所占字节数
			m_uiADCLineBufNum = 0;

			m_file.SeekToBegin();
			ullEnd = m_file.SeekToEnd();
			// 光标移动为字符长度+2(\n长度)
			m_file.SeekToBegin();

			// ADC数据采样开始时间
			ar.ReadString(str);
			uiLength += (str.GetLength() + 2) * sizeof(wchar_t);
			lActual = m_file.Seek(uiLength, CFile::begin);
			// ADC数据采样信息
			ar.ReadString(str);
			uiLength  += (str.GetLength() + 2) * sizeof(wchar_t);
			lActual = m_file.Seek(uiLength, CFile::begin);
			_stscanf_s(str, _T("采集站设备总数%d，从第%d个数据开始存储ADC数据，数据转换方式采用方式%d！"), &m_uiInstrumentADCNum, &m_uiADCStartNum, &m_uiADCDataCovNb);
			if (m_uiInstrumentADCNum == 0)
			{
				//				fp_str.close();
				ar.Close();
				m_file.Close();
				AfxMessageBox(_T("采集站设备总数为0！"));
				return FALSE;
			}
			m_dbFduData = new vector<double>[m_uiInstrumentADCNum];
			m_dbFduShow = new vector<double>[m_uiInstrumentADCNum];
			m_uiADCLineBufNum = ADCDataSaveSize * m_uiInstrumentADCNum + 2;
			m_pLinebufRead = new wchar_t[m_uiADCLineBufNum];
			for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
			{
				m_uiInstrumentNb[i] = 0;
			}
			// 采集站设备标签
			ar.ReadString(str);
			OnPhraseLabels(str);
			uiLength  += (str.GetLength() + 2) * sizeof(wchar_t);
			lActual = m_file.Seek(uiLength, CFile::begin);	

			m_file.SeekToEnd();
			ar.Close();
			m_file.Close();
			uiLineNum = (unsigned int)((ullEnd - lActual) / (m_uiADCLineBufNum * sizeof(wchar_t)));
			m_uiEndDrawPointsNumMax = m_uiADCStartNum + uiLineNum;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

// 解析设备标签
void C绘图程序Dlg::OnPhraseLabels(CString str)
{
	int iDirectionPrevious = 0;
	int iDirectionNow = 0;
	int iDirectionEnd = 0;
	CString cstmp = _T("");
	unsigned int uiNb = 0;
	unsigned int uiLocation = 0;
	iDirectionEnd = str.GetLength();
	while(iDirectionEnd != iDirectionPrevious)
	{
		iDirectionNow = str.Find(_T(" \t"), iDirectionPrevious);
		cstmp = str.Mid(iDirectionPrevious, iDirectionNow-iDirectionPrevious);
		iDirectionPrevious = iDirectionNow + ADCDataInterval;
		if (cstmp == _T(" "))
		{
			iDirectionPrevious += ADCDataInterval;
			continue;
		}
		else
		{
			int iDirectionLable = 0;
			int iLength = cstmp.GetLength();
			CString csTemp = _T("");
			iDirectionLable = cstmp.Find(_T("仪器"), 0);
			csTemp = cstmp.Mid(iDirectionLable, iLength);
			_stscanf_s(csTemp, _T("仪器%d"), &uiLocation);
			m_uiInstrumentNb[uiNb] = uiLocation;
			uiNb++;
		}
	}
}

// 得到并设置打印机信息
void C绘图程序Dlg::GetAndSetPrinterInfo(void)
{
	PRINTDLG pd;  //该结构包含打印对话框中的所有信息
	LPDEVMODE  lpDevMode; 
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //获得默认的打印机的信息
	{
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode)
		{   
			lpDevMode->dmPaperSize=DMPAPER_A4;    // 将打印纸设置为A4
			//			lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; // 将打印机设置为横向打印。
			lpDevMode->dmOrientation=DMORIENT_PORTRAIT; // 将打印机设置为纵向打印
			lpDevMode->dmPrintQuality=1200;   //打印分辨率为1200dpi
		}
		GlobalUnlock(pd.hDevMode);
	}
}

// 从文件中读取bmp并打印
void C绘图程序Dlg::LoadBmpFromFileAndPrint(CString csPathName)
{
	CDC memDC;
	CClientDC dc(this);
	memDC.CreateCompatibleDC(&dc);
	CBitmap* bitmap = new CBitmap();
	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage(NULL, csPathName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	if(hBitmap == NULL) 
	{
		TRACE(_T("Load   Bitmap   Error "));
		return; 
	}
	bitmap->Attach(hBitmap);
	BITMAP bm;
	bitmap->GetBitmap(&bm);
	int bmpWidth = bm.bmWidth;
	int bmpHeight = bm.bmHeight;
	CBitmap* pOldBitmap = (CBitmap*)memDC.SelectObject(bitmap);
	if (pOldBitmap == NULL)   //   if   bitmap   is   very   big,   better   check   this   !   
	{
		memDC.DeleteDC();
		delete bitmap;
		return;
	}
	CDC prtDC;
	CPrintInfo printInfo;
	CSize size;
	DOCINFO di;
	CString szPortName = _T("");
	CString szAppName = _T("");
	CString szPrintError = _T("");
	szAppName.LoadString(AFX_IDS_APP_TITLE);
	CSize paper_size;	//printer   paper   size   in   mm   
	int xLogPPI = 0;
	int yLogPPI = 0;
	// 物理长度与屏幕像素间的转换
	if(AfxGetApp()->GetPrinterDeviceDefaults(&printInfo.m_pPD->m_pd))
	{
		HDC hDC = printInfo.m_pPD->m_pd.hDC;
		if(hDC == NULL)  
		{
			hDC = printInfo.m_pPD->CreatePrinterDC();
		}
		if(hDC != NULL)
		{
			prtDC.Attach(hDC);
			// 所有像素数
			paper_size.cx = prtDC.GetDeviceCaps(HORZSIZE);
			paper_size.cy = prtDC.GetDeviceCaps(VERTSIZE);
			// 每英寸点数
			xLogPPI = prtDC.GetDeviceCaps(LOGPIXELSX);
			yLogPPI = prtDC.GetDeviceCaps(LOGPIXELSY);
		}
		else
		{
			AfxMessageBox(_T("Can   not   find   printer.   Please   check   installed/default   printers."));
			return;
		}
	}
	// 当前屏幕像素点数
	int scr_xLogPPI = dc.GetDeviceCaps(LOGPIXELSX);
	int scr_yLogPPI = dc.GetDeviceCaps(LOGPIXELSY);
	int paper_width = (int)((double)paper_size.cx * (double)xLogPPI / 25.4);	//width   of   a   printed   page   in   pixels   
	int paper_height = (int)((double)paper_size.cy * (double)yLogPPI / 25.4);
	// 打印机像素点和当前屏幕像素点的比值
	double ratio_x = (double)xLogPPI / (double)scr_xLogPPI;
	double ratio_y = (double)yLogPPI / (double)scr_yLogPPI;

	CString strPageNumber = _T("");
	// 页码输出区域
	int page_info_left = (int)((double)paper_width * 0.85);
	int page_info_right = paper_width;
	int page_info_top = (int)((double)paper_height * 0.99);
	int page_info_bottom = paper_height;
	CRect page_info_rect = CRect(page_info_left, page_info_top,
		page_info_right, page_info_bottom);
	int printed_pages = 0;
	// 计算打印页数
	int total_pages = (int)((bmpHeight * ratio_y + paper_height - 1) / paper_height);
	//调用打印对话框 
	CPrintDialog   prtDlg(FALSE, PD_PAGENUMS);

	prtDlg.m_pd.nMinPage = 1;
	prtDlg.m_pd.nMaxPage = (WORD)total_pages;
	prtDlg.m_pd.nFromPage = 1;
	prtDlg.m_pd.nToPage = (WORD)total_pages;

	if(prtDlg.DoModal() == IDOK)
	{   
		memset(&di, 0, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = szAppName;
		szPortName = prtDlg.GetPortName();
		di.lpszOutput = szPortName;
		prtDC.m_bPrinting = TRUE;
	}
	else
	{
		return;     //Cancel   button   pressed,   don't   forget   this!
	}
	// 在开始页和结束页之间采用相同的打印设置，直到调用EndDoc为止
	if(prtDC.StartDoc(&di) < 0)   
	{
		AfxMessageBox(_T("Printing   error   occured.   Unable   to   find   printer."));
		prtDC.Detach();
		prtDC.DeleteDC();
		return;
	}
	// 选择逻辑单元转换为设备电源的映射模式
	prtDC.SetMapMode(MM_TEXT);
	for(int i = 0; i < total_pages; i++)
	{
		prtDC.StartPage();
		strPageNumber.Format(_T("Page:%d of %d"), ++printed_pages, total_pages);
		if ((i == (total_pages - 1)) && (total_pages > 1))	// 下一页
		{
			int last_bmpHeight = (int)(bmpHeight - paper_height / ratio_y * i);
			prtDC.StretchBlt(0, 0, (int)(bmpWidth * ratio_x), (int)(last_bmpHeight * ratio_y), &memDC,
				0, (int)(paper_height * i / ratio_y), bmpWidth, last_bmpHeight, SRCCOPY);
		}
		else
		{
			// 显示bmp图片原始尺寸
			// 			prtDC.StretchBlt(0, 0, (int)(bmpWidth * ratio_x), paper_height, &memDC,
			// 				0, (int)(paper_height * i / ratio_y), bmpWidth, (int)(paper_height / ratio_y), SRCCOPY);
			// 显示bmp图片宽度为打印纸宽度
			prtDC.StretchBlt(0, 0, paper_width, paper_height, &memDC,
				0, (int)(paper_height * i / ratio_y), bmpWidth, (int)(paper_height / ratio_y), SRCCOPY);
		}
		prtDC.TextOut(page_info_rect.left, page_info_rect.top, strPageNumber);
		prtDC.EndPage();
	}
	memDC.SelectObject(pOldBitmap);
	delete bitmap;
	memDC.DeleteDC();

	prtDC.EndDoc();
	prtDC.Detach();
	prtDC.DeleteDC();
}

// 载入绘图软件License
bool C绘图程序Dlg::LoadChartDirLicense(void)
{
	return setLicenseCode("R5MNGUVV3UXWFT2CC44B1D7E");
}

void C绘图程序Dlg::OnBnClickedButtonParameterset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_oParameterSet.DoModal();
	// 载入图标到变焦/滚动位置控制按钮
	if (m_oParameterSet.m_uiShowDirection == 1)
	{
		LoadButtonIcon(IDC_XZoomPB, IDI_YZoomPB, 105, 20);
		LoadButtonIcon(IDC_YZoomPB, IDI_XZoomPB, 105, 20);
	}
	else if (m_oParameterSet.m_uiShowDirection == 2)
	{
		LoadButtonIcon(IDC_XZoomPB, IDI_XZoomPB, 105, 20);
		LoadButtonIcon(IDC_YZoomPB, IDI_YZoomPB, 105, 20);
	}
}

void C绘图程序Dlg::OnBnClickedCheckWaverangefixed()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bCheckWaveRangeFixed = (BOOL)::SendMessage(GetDlgItem(IDC_CHECK_WAVERANGEFIXED)->GetSafeHwnd(),   BM_GETCHECK,   0,   0);
}

// 得到绘图所需数据
void C绘图程序Dlg::GetShowData(unsigned int uiDataNum)
{
	double dbTemp = 0.0;
	for (unsigned int i=0; i<m_uiInstrumentADCNum; i++)
	{
		for (unsigned int j=0; j<uiDataNum; j++)
		{
			dbTemp = m_dbFduData[i][j] * m_dbEditLineZoom + i * m_dbEditLineInterval + 1;
			if (m_bCheckWaveRangeFixed == TRUE)
			{
				if (dbTemp > (1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2))
				{
					dbTemp = 1 + i * m_dbEditLineInterval + m_dbEditLineInterval / 2;
				}
				else if (dbTemp < (1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2))
				{
					dbTemp = 1 + i * m_dbEditLineInterval - m_dbEditLineInterval / 2;
				}
			}
			m_dbFduShow[i].push_back(dbTemp);
		}
	}
}
