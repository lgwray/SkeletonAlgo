// thinview.cpp 
#include "stdafx.h"
#include "thin.h"
#include "resource.h"
#include "cdib.h"
#include "thindoc.h"
#include "thinview.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////
// CThinView
IMPLEMENT_DYNCREATE(CThinView, CScrollView)
BEGIN_MESSAGE_MAP(CThinView, CScrollView)
	//{{AFX_MSG_MAP(CThinView)
	ON_WM_CREATE()
	ON_COMMAND(ID_THIN_START, OnThinStart)
    ON_UPDATE_COMMAND_UI(ID_THIN_START, OnUpdateThinStart)
	ON_COMMAND(ID_VIEW_FIT, OnViewFit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FIT, OnUpdateViewFit)
	ON_COMMAND(ID_THIN_ALGO4, OnThinAlgo4)
	ON_UPDATE_COMMAND_UI(ID_THIN_ALGO4, OnUpdateThinAlgo4)
	ON_WM_TIMER()
	ON_COMMAND(ID_THIN_ALGO1, OnThinAlgo1)
	ON_UPDATE_COMMAND_UI(ID_THIN_ALGO1, OnUpdateThinAlgo1)
	ON_COMMAND(ID_THIN_ALGO2, OnThinAlgo2)
	ON_UPDATE_COMMAND_UI(ID_THIN_ALGO2, OnUpdateThinAlgo2)
	ON_COMMAND(ID_THIN_ALGO3, OnThinAlgo3)
	ON_UPDATE_COMMAND_UI(ID_THIN_ALGO3, OnUpdateThinAlgo3)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CThinView construction/destruction
CThinView::CThinView():m_totalSize(480,640)//800,1050
{
 m_StretchToFit=FALSE;
 m_thinalgo=1;
}
CThinView::~CThinView()
{}
/////////////////////////////////////////////////////////////////////////////
// CThinView drawing
int CThinView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(m_StretchToFit){
	    SetScaleToFitSize(m_totalSize);}
	else{
	  CSize pageSize=CSize(m_totalSize.cx/4,
	                       m_totalSize.cy/4);
	  CSize lineSize=CSize(m_totalSize.cx/100,
	                       m_totalSize.cy/100);                     
	  SetScrollSizes(MM_TEXT,m_totalSize,pageSize,lineSize);}
	return 0;
}
///////////////////////////////////////////
void CThinView::OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint)
{
 if(!m_StretchToFit){
    ScrollToPosition(CPoint(0,0));}
 Invalidate(FALSE);
}
void CThinView::OnDraw(CDC* pDC)
{
 CThinDoc* pDoc = GetDocument();
 if(!m_StretchToFit)
    pDoc->m_pDib->Display(pDC,CPoint(0,0));
 else 
    pDoc->m_pDib->Stretch(pDC,CPoint(0,0),m_totalSize);
}
////////////////////////////////////////////////////
// CThinView printing
BOOL CThinView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
void CThinView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
void CThinView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
/////////////////////////////////////////////////////////////////////////////
// CThinView diagnostics
#ifdef _DEBUG
void CThinView::AssertValid() const
{
	CScrollView::AssertValid();
}
void CThinView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
CThinDoc* CThinView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CThinDoc)));
	return (CThinDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CThinView message handlers
void CThinView::OnViewFit()
{
 m_StretchToFit=!m_StretchToFit;
 Invalidate();
}
void CThinView::OnUpdateViewFit(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_StretchToFit);}
///////////////////////////
void CThinView::OnThinStart()
{
 CThinDoc* pDoc=(CThinDoc*)GetDocument();
 m_min=0;m_sec=0;
 BeginWaitCursor();
 SetTimer(1,1000,NULL);
 switch(m_thinalgo){
 case 1:pDoc->Thin1();break;
 case 2:pDoc->Thin2();break;
 case 3:pDoc->Thin3();break;
 case 4:pDoc->Thin4();break;
 default:break;}
 KillTimer(1);
 EndWaitCursor();
}

void CThinView::OnUpdateThinStart(CCmdUI* pCmdUI)
{
 CThinDoc* pDoc=(CThinDoc*)GetDocument();
 if(pDoc->m_pDib->m_lpBuf)pCmdUI->Enable(1);
 else pCmdUI->Enable(0);	
}

void CThinView::OnTimer(UINT nIDEvent)
{
 BeginWaitCursor();
 CThinDoc* pDoc=(CThinDoc*)GetDocument();
 char text[20];int iter,pas;
 CStatusBar* pStatus=(CStatusBar*)
  AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 if(pStatus){    
   wsprintf(text,"Time:%2dmin%2dsec",
       m_min,m_sec);
   pStatus->SetPaneText(2,text);
   if(!((iter=pDoc->m_iteration)<0)){        
    if(!((pas=pDoc->m_pass)<0))
      wsprintf(text,"Iteration:%2d,Pass:%d",iter,pas);
    else wsprintf(text,"Iteration:%2d",iter);  
    pStatus->SetPaneText(3,text);}}	
 m_sec++;
 if(m_sec==60){m_sec=0;m_min++;}  
}

//////////////////////////////////////////////////
void CThinView::OnThinAlgo1()
{m_thinalgo=1;}
void CThinView::OnUpdateThinAlgo1(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_thinalgo==1);}
void CThinView::OnThinAlgo2()
{m_thinalgo=2;}
void CThinView::OnUpdateThinAlgo2(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_thinalgo==2);}
void CThinView::OnThinAlgo3()
{m_thinalgo=3;}
void CThinView::OnUpdateThinAlgo3(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_thinalgo==3);}
void CThinView::OnThinAlgo4()
{m_thinalgo=4;}
void CThinView::OnUpdateThinAlgo4(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_thinalgo==4);}

