// Draw3DGraph_Test4.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDraw3DGraph_Test4App:
// �йش����ʵ�֣������ Draw3DGraph_Test4.cpp
//

class CDraw3DGraph_Test4App : public CWinApp
{
public:
	CDraw3DGraph_Test4App();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDraw3DGraph_Test4App theApp;