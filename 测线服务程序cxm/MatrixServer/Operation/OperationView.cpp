// OperationView.cpp : implementation of the COperationView class
//

#include "stdafx.h"
#include "Operation.h"

#include "OperationDoc.h"
#include "OperationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COperationView

IMPLEMENT_DYNCREATE(COperationView, CView)

BEGIN_MESSAGE_MAP(COperationView, CView)
END_MESSAGE_MAP()

// COperationView construction/destruction

COperationView::COperationView()
{
	// TODO: add construction code here

}

COperationView::~COperationView()
{
}

BOOL COperationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// COperationView drawing

void COperationView::OnDraw(CDC* /*pDC*/)
{
	COperationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


// COperationView diagnostics

#ifdef _DEBUG
void COperationView::AssertValid() const
{
	CView::AssertValid();
}

void COperationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COperationDoc* COperationView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COperationDoc)));
	return (COperationDoc*)m_pDocument;
}
#endif //_DEBUG


// COperationView message handlers
