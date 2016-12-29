//cdib.h
class CDib:public CObject
{DECLARE_SERIAL(CDib)
public:
        char * m_lpBuf;//DIB data buffer
        DWORD      m_dwLength;//total buffer length,including file header
        int        m_nBits;//number of color bits per pixel
        DWORD      m_LineBytes;
        //pointers for internal use
        LPBITMAPFILEHEADER m_lpBMFH;
        LPBITMAPINFOHEADER m_lpBMIH;
        LPBITMAPINFO       m_lpBMI;
        LPSTR              m_lpData;
public:
        CDib();
        ~CDib();
        virtual void Serialize(CArchive&ar);
        CBitmap* MakeBitmap(CDC* pDC,CSize& bmSize);
        BOOL     Display(CDC* pDC,CPoint origin);
        BOOL     Stretch(CDC* pDC,CPoint origin,CSize size);
private:
        int      GetColorBits();
        DWORD    GetLength();
        void     SetMonoColors(DWORD dwForeground,DWORD dwBackground);
        BOOL     GetMonoColors(DWORD& dwForeground,DWORD& dwBackground);
        BOOL     AllocateMemory(BOOL bRealloc=FALSE);
};                        