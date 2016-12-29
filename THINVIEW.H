// thinview.h 
class CThinView : public CScrollView
{
protected: // create from serialization only
	CThinView();
	DECLARE_DYNCREATE(CThinView)
public:
	CThinDoc* GetDocument();
private:CSize   m_totalSize;
private:
        BOOL    m_StretchToFit;
        int     m_thinalgo;
        int     m_min;
        int     m_sec;
public:
	virtual ~CThinView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
  virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);
protected:
   	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
// Generated message map functions
protected:
	//{{AFX_MSG(CThinView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnThinStart();
    afx_msg void OnUpdateThinStart(CCmdUI* pCmdUI);
	afx_msg void OnViewFit();
	afx_msg void OnUpdateViewFit(CCmdUI* pCmdUI);
	afx_msg void OnThinAlgo1();
	afx_msg void OnUpdateThinAlgo1(CCmdUI* pCmdUI);
	afx_msg void OnThinAlgo2();
	afx_msg void OnUpdateThinAlgo2(CCmdUI* pCmdUI);
	afx_msg void OnThinAlgo3();
	afx_msg void OnUpdateThinAlgo3(CCmdUI* pCmdUI);
	afx_msg void OnThinAlgo4();
	afx_msg void OnUpdateThinAlgo4(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG  // debug version in fanview.cpp
inline CThinDoc* CThinView::GetDocument()
   { return (CThinDoc*)m_pDocument; }
#endif
//////////////////////////////////////////////
