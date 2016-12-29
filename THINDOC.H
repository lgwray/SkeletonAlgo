// thindoc.h
class CThinDoc : public CDocument
{   
protected: // create from serialization only
    	CThinDoc();
	    DECLARE_DYNCREATE(CThinDoc)
public:
        CDib*   m_pDib;//was used in thinview.h,so public           
private:BOOL    m_scanline;                    
        enum{whitepoint/*=0*/,blackpoint/*=1*/};
public: int     m_iteration;
        int     m_pass;
public:
	    virtual ~CThinDoc();
	    virtual void Serialize(CArchive& ar);   
#ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
#endif
private:
     long        GetWidth();
     long        GetHeight(); 
     inline BOOL PtInBitmap(long i,long j);
     inline int  GetAt(long i,long j,char * buffer=NULL);
     inline void SetAt(long i,long j,int pixel,
               char * buf=NULL);
     char * CreateTempBmp(long len);
     void CopyToBmp(char * temp,long i,
               long icount,char * des=NULL);
     void DelTempBmp(char * temp);
///////////////////////////////////////////
private:    BOOL CheckThinCond();
public:     BOOL Thin1();
private:    BOOL Match1(long i,long j);
public:     BOOL Thin2();
private:    BOOL Match2(long i,long j);
public:     BOOL Thin3();
private:    BOOL Match3(long i,long j,int pass);            
public:     BOOL Thin4();
private:    BOOL Match4(long i,long j,int pass);
//////////////////////////////////////////
protected:                 
	virtual BOOL OnNewDocument();
    virtual void DeleteContents();
protected:
	//{{AFX_MSG(CThinDoc)
	afx_msg void OnThinScanline();
	afx_msg void OnUpdateThinScanline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////
