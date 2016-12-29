/////////////////////////////
///cdib.cpp
#include "stdafx.h"
#include "resource.h"
#include "cdib.h"
#include <windowsx.h>//for GlobalAllocPtr
IMPLEMENT_SERIAL(CDib,CObject,0)
///////////////////////////////
CDib::CDib()
{
 m_dwLength=0L;
 m_nBits   =0;
 m_lpBuf   =NULL;
 m_LineBytes=0L;
}
/////////////////////////////////////
CDib::~CDib()
{ if(m_lpBuf)GlobalFreePtr(m_lpBuf);}               
/////////////////////////////////////
void CDib::Serialize(CArchive&ar)
{
 CObject::Serialize(ar);
 if(ar.IsStoring()){
    ar.Flush();
    ar.GetFile()->WriteHuge(m_lpBuf,m_dwLength);}
 else{
    ASSERT(m_dwLength==0L);//DIB must be empty
    ar.Flush();
    m_dwLength=(ar.GetFile())->GetLength();
    if(m_dwLength>0L){
         if(!AllocateMemory()){return;}
         ar.GetFile()->ReadHuge(m_lpBuf,m_dwLength);
         m_lpData=(LPSTR)m_lpBMFH+m_lpBMFH->bfOffBits;
         m_nBits=m_lpBMIH->biBitCount;
         m_LineBytes=((DWORD)m_lpBMIH->biWidth*m_nBits)/32;
         if(((DWORD)m_lpBMIH->biWidth*m_nBits)%32)m_LineBytes++;
         m_LineBytes*=4;}
     }
}
/////////////////////////////////////////
CBitmap* CDib::MakeBitmap(CDC*pDC,CSize& bmSize)
{//replaces the DC's existing bitmap with a new one from DIB
 //returns the old one
 BITMAP bm;
 DWORD dwFore,dwBack;
 //checks to see whether DIB buffer is properly loaded
 if(m_dwLength==0L){
   bmSize.cx=bmSize.cy=0;
   return NULL;}
 //this code conditions the bitmap for mono or color
 int nPlanes=pDC->GetDeviceCaps(PLANES);
 int  nBitsPixel=pDC->GetDeviceCaps(BITSPIXEL);
 CBitmap* pConfigBitmap=new CBitmap;
 char bits[100];
 if(m_lpBMIH->biBitCount==1){
   pConfigBitmap->CreateBitmap(1,1,1,1,bits);}
 else{pConfigBitmap->CreateBitmap(1,1,nPlanes,nBitsPixel,bits);}
 CBitmap* pOriginalBitmap=
    (CBitmap*)pDC->SelectObject(pConfigBitmap);
//CreateDIBitmap "switches bits" for mono bitmaps,
//depending on colors,so we'll fool it
 if(GetMonoColors(dwFore,dwBack)){
   SetMonoColors(0L,0xFFFFFFL);}
   
 #ifdef _WIN32
   HBITMAP hBitmap=::CreateDIBitmap(pDC->GetSafeHdc(),m_lpBMIH,
            CBM_INIT,(CONST BYTE*)(m_lpBuf+m_lpBMFH->bfOffBits),
            m_lpBMI,DIB_RGB_COLORS);
 #else
   HBITMAP hBitmap=::CreateDIBitmap(pDC->GetSafeHdc(),m_lpBMIH,
            CBM_INIT,(LPSTR)(m_lpBuf+m_lpBMFH->bfOffBits),
            m_lpBMI,DIB_RGB_COLORS);           
 #endif
 
 if(hBitmap==NULL){
    TRACE("Null bitmap\n");
    delete pDC->SelectObject(pOriginalBitmap);//del. config bitmap
    return NULL;}//untested error logic        
 SetMonoColors(dwFore,dwBack);
 //Can't use CBitmap::FromHandle here because we need to
 //delete the object later
 CBitmap* pBitmap=new CBitmap;
 pBitmap->Attach(hBitmap);
 pBitmap->GetObject(sizeof(bm),&bm);
 bmSize.cx=bm.bmWidth;
 bmSize.cy=bm.bmHeight;
 delete pDC->SelectObject(pBitmap);//delete configuration bitmap
 return pOriginalBitmap;
}
////////////////////////////////////////////
BOOL CDib::Display(CDC*pDC,CPoint origin)
{//direct to device --bypass the GDI bitmap
 if(!m_lpBuf){return FALSE;}//nothing to display
 if(!::SetDIBitsToDevice(pDC->GetSafeHdc(),origin.x,origin.y,
   (WORD)m_lpBMIH->biWidth,(WORD)m_lpBMIH->biHeight,0,0,0,
   (WORD)m_lpBMIH->biHeight,m_lpData,m_lpBMI,
   DIB_RGB_COLORS)){
     return FALSE;}
 return TRUE;
}
///////////////////////////////////////////
BOOL CDib::Stretch(CDC* pDC,CPoint origin,CSize size)
{//direct to device--bypass the GDI bitmap
 if(!m_lpBuf){ return FALSE;}//nothing to display
 if(!::StretchDIBits(pDC->GetSafeHdc(),origin.x,origin.y,
    size.cx,size.cy,0,0,(WORD)m_lpBMIH->biWidth,
    (WORD)m_lpBMIH->biHeight,m_lpData,m_lpBMI,
    DIB_RGB_COLORS,SRCCOPY)){
   return FALSE;}
 return TRUE;
}
///////////////////////////////////////////
void CDib::SetMonoColors(DWORD dwForeground,DWORD dwBackground)
{
 if(m_nBits!=1){return;}
 unsigned long far* pPalette=(unsigned long far*)
     ((LPSTR)m_lpBMIH+sizeof(BITMAPINFOHEADER));
 *pPalette=dwForeground;
 *(++pPalette)=dwBackground;
 return;
}
////////////////////////////////////////////
BOOL CDib::GetMonoColors(DWORD& dwForeground,DWORD& dwBackground)
{
 if(m_nBits!=1){return FALSE;}
 unsigned long far* pPalette=(unsigned long far*)
     ((LPSTR)m_lpBMIH+sizeof(BITMAPINFOHEADER));
 dwForeground=*pPalette;
 dwBackground=*(++pPalette);
 return TRUE;
}
////////////////////////////////////////////
BOOL CDib::AllocateMemory(BOOL bRealloc)
//bRealloc default=FALSE
{
 if(bRealloc){
   m_lpBuf=(char *)GlobalReAllocPtr(m_lpBuf,
                                   m_dwLength,GHND);}
 else{m_lpBuf=(char *)GlobalAllocPtr(GHND,m_dwLength);}
 
 if(!m_lpBuf){
    AfxMessageBox("unable to allocate DIB memory");
    m_dwLength=0L;
    m_nBits=0;
    return FALSE;}
 m_lpBMFH=(LPBITMAPFILEHEADER)m_lpBuf;
 m_lpBMIH=(LPBITMAPINFOHEADER)(m_lpBuf+sizeof(BITMAPFILEHEADER));
 m_lpBMI=(LPBITMAPINFO)m_lpBMIH;
 return TRUE;
}
             