//////////////////////////
// thindoc.cpp 
#include "stdafx.h"
#include "resource.h"
#include "thin.h"
#include "cdib.h"
#include "thindoc.h"
#include <windowsx.h>//for GlobalAllocPtr()
//#include <memory.h>  //for memcpy()
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////
// CThinDoc
IMPLEMENT_DYNCREATE(CThinDoc, CDocument)
BEGIN_MESSAGE_MAP(CThinDoc, CDocument)
	//{{AFX_MSG_MAP(CThinDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_THIN_SCANLINE, OnThinScanline)
	ON_UPDATE_COMMAND_UI(ID_THIN_SCANLINE, OnUpdateThinScanline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
// CThinDoc construction/destruction
CThinDoc::CThinDoc()
{m_pDib=new CDib;
 m_scanline=FALSE;
 m_iteration=-1;
 m_pass=-1;
}
CThinDoc::~CThinDoc()
{delete m_pDib;}

BOOL CThinDoc::OnNewDocument()
{ 	if (!CDocument::OnNewDocument())
		return FALSE;
 	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}
void CThinDoc::DeleteContents()
{delete m_pDib;
 m_pDib=new CDib;
}
// CThinDoc serialization 
void CThinDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{pCmdUI->Enable(IsModified());}

void CThinDoc::Serialize(CArchive& ar)
{
 m_pDib->Serialize(ar);
 if(!ar.IsStoring()){
   CStatusBar* pStatus=(CStatusBar*)
   AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 
   if(pStatus){    
      char text[20];
      wsprintf(text,"%ld x %ld",GetWidth(),GetHeight());
      pStatus->SetPaneText(1,text);   
      if(m_pDib->m_lpBMIH->biCompression)
          wsprintf(text,"Compressed");
      else wsprintf(text,"Non_compressed"); 
      pStatus->SetPaneText(0,text);
      wsprintf(text," ");
      pStatus->SetPaneText(2,text);
      pStatus->SetPaneText(3,text);}
      }
}
// CThinDoc diagnostics
#ifdef _DEBUG
void CThinDoc::AssertValid() const
{	CDocument::AssertValid();}
void CThinDoc::Dump(CDumpContext& dc) const
{CDocument::Dump(dc);}
#endif //_DEBUG
///////////////////////////////////////////////
long CThinDoc::GetWidth()
{return m_pDib->m_lpBMIH->biWidth;}
long CThinDoc::GetHeight()
{return m_pDib->m_lpBMIH->biHeight;}
////////////////////////
BOOL CThinDoc::PtInBitmap(long i,long j)
{
 if(i<0||i>=m_pDib->m_lpBMIH->biHeight)
   return FALSE;
 if(j<0||j>=m_pDib->m_lpBMIH->biWidth)
   return FALSE;
 return TRUE;
}
int  CThinDoc::GetAt(long i,long j,char * buffer)
{
 if(!PtInBitmap(i,j)) return CThinDoc::whitepoint;
 if(buffer==NULL)buffer=(char *)m_pDib->m_lpData;
 int order_in_byte=(int)j%8;
 DWORD byte_offset=i*(m_pDib->m_LineBytes)+j/8;
 char ch=*(buffer+byte_offset);
 //but in .bmp,0: black color
 //            1: white color    
 if((ch>>(7-order_in_byte))%2) return CThinDoc::whitepoint;
 return CThinDoc::blackpoint;
}//return 1:black,0:white,because it's convenient in match.

void CThinDoc::SetAt(long i,long j,int pixel,char *buf)
{
 if(!PtInBitmap(i,j))return;
 if(buf==NULL)buf=(char *)m_pDib->m_lpData;
 int order_in_byte=(int)j%8;
 DWORD byte_offset=i*(m_pDib->m_LineBytes)+j/8;
 char * pch=buf+byte_offset;
 *pch=(*pch)&(~(1<<(7-order_in_byte)));
 if(pixel==CThinDoc::whitepoint)//Set value=1
  *pch=(*pch)|(1<<(7-order_in_byte));
}

char * CThinDoc::CreateTempBmp(long len) 
{return (char *)GlobalAllocPtr(GHND,len);}

void CThinDoc::CopyToBmp(char *temp,long i,
              long icount,char *des)
{
 if(des==NULL)des=(char *)m_pDib->m_lpData;
  des+=(i*m_pDib->m_LineBytes);
  for(long k=0;k<long(icount*m_pDib->m_LineBytes);
   *des=*temp,des++,temp++,k++);       
}       
void CThinDoc::DelTempBmp(char *temp)
{if(temp) GlobalFreePtr(temp);}
//////////////////////////////// /////////////////
BOOL CThinDoc::CheckThinCond()
{
 if(m_pDib->m_nBits!=1||m_pDib->m_lpBMIH->biCompression){
  AfxMessageBox("Cannot thin the image, which may\
 be colored or compressed.");
    return FALSE;}
 return TRUE;
}   
//////////////////////////////// /////////////////
BOOL CThinDoc::Match1(long i,long j)
{
 //if(GetAt(i,j)!=CThinDoc::blackpoint) return FALSE;
 /*    0   1   2
       3 (i,j) 4  5
       6   7   8
           9
 */
 int p[10];
 p[0]=GetAt(i-1,j-1);p[1]=GetAt(i-1,j);
 p[2]=GetAt(i-1,j+1);p[3]=GetAt(i,j-1);
 p[4]=GetAt(i,j+1);p[5]=GetAt(i,j+2);
 p[6]=GetAt(i+1,j-1);p[7]=GetAt(i+1,j);
 p[8]=GetAt(i+1,j+1);p[9]=GetAt(i+2,j);
//whitepoint:0 blackpoint:1 
 if(p[0]+p[1]+p[3]+p[6]+p[7]==5&&p[4]==0&&
     p[2]+p[8]<2) return TRUE;
 if(p[0]+p[1]+p[2]+p[3]+p[4]==5&&p[7]==0&&
     p[6]+p[8]<2) return TRUE;     
 if(p[1]+p[2]+p[4]+p[5]+p[7]+p[8]==6&&p[3]==0&&
     p[0]+p[6]<2) return TRUE;    
 if(p[3]+p[4]+p[6]+p[7]+p[8]+p[9]==6&&p[1]==0&&
     p[0]+p[2]<2) return TRUE;     
     
 if(p[1]+p[2]+p[4]==0&&p[3]+p[7]==2)
                  return TRUE;     
 if(p[1]+p[2]+p[4]==3&&p[3]+p[6]+p[7]==0)
                  return TRUE;          
 if(p[0]+p[2]+p[3]+p[6]+p[7]+p[8]==0&&p[1]+p[4]==2)
                  return TRUE;          
     
 if(p[4]+p[7]+p[8]==0&&p[1]+p[3]==2)
                  return TRUE;     
 if(p[4]+p[7]+p[8]==3&&p[0]+p[1]+p[3]==0)
                  return TRUE;          
 if(p[0]+p[1]+p[2]+p[3]+p[6]+p[8]==0&&p[4]+p[7]==2)
                  return TRUE;
                  
 if(p[6]+p[7]+p[8]==3&&p[0]+p[1]+p[2]+p[3]+p[4]==0)
                  return TRUE;  
 if(p[0]+p[3]+p[6]==3&&p[1]+p[2]+p[4]+p[7]+p[8]==0)
                  return TRUE;  
 if(p[0]+p[1]+p[2]==3&&p[3]+p[4]+p[6]+p[7]+p[8]==0)
                  return TRUE; 
 if(p[2]+p[4]+p[8]==3&&p[0]+p[1]+p[3]+p[6]+p[7]==0)
                  return TRUE;                                                                                       
 return FALSE;}

BOOL CThinDoc::Match2(long i,long j)
{
//if(GetAt(i,j)!=CThinDoc::blackpoint) return FALSE;
 /*    0   1   2
       3 (i,j) 4  5
       6   7   8
           9
 */
 int p[10];
 p[0]=GetAt(i-1,j-1);p[1]=GetAt(i-1,j);
 p[2]=GetAt(i-1,j+1);p[3]=GetAt(i,j-1);
 p[4]=GetAt(i,j+1);p[5]=GetAt(i,j+2);
 p[6]=GetAt(i+1,j-1);p[7]=GetAt(i+1,j);
 p[8]=GetAt(i+1,j+1);p[9]=GetAt(i+2,j);
//whitepoint:0 blackpoint:1 
//AT FIRST REGAIN TEMPLATE:
 if(p[3]+p[5]==0&&p[4]==1)
    return FALSE;
 if(p[1]+p[9]==0&&p[7]==1)
    return FALSE;   
//NOISE TEMPLATE: 
 if(p[0]+p[1]+p[2]+p[3]+p[4]==0&&p[7]==1)
    return TRUE;
 if(p[0]+p[1]+p[3]+p[6]+p[7]==0&&p[4]==1)
    return TRUE;
 if(p[3]+p[4]+p[6]+p[7]+p[8]==0&&p[1]==1)
    return TRUE;
 if(p[1]+p[2]+p[4]+p[7]+p[8]==0&&p[3]==1)
    return TRUE;       
 if(p[0]+p[1]+p[2]+p[3]+p[4]+p[7]+p[8]==0&&p[6]==1)
    return TRUE;
 if(p[0]+p[1]+p[2]+p[3]+p[4]+p[6]+p[7]==0&&p[8]==1)
    return TRUE;
 if(p[0]+p[1]+p[3]+p[4]+p[6]+p[7]+p[8]==0&&p[2]==1)
    return TRUE;
 if(p[1]+p[2]+p[3]+p[4]+p[6]+p[7]+p[8]==0&&p[0]==1)
    return TRUE;   
//THIN TEMPLATE:
 if(p[0]+p[1]+p[2]==0&&p[3]+p[4]+p[7]==3)
    return TRUE;
 if(p[0]+p[3]+p[6]==0&&p[1]+p[4]+p[7]==3)
    return TRUE;
 if(p[6]+p[7]+p[8]==0&&p[1]+p[3]+p[4]==3)
    return TRUE;
 if(p[2]+p[4]+p[8]==0&&p[1]+p[3]+p[7]==3)
    return TRUE;       
 if(p[1]+p[2]+p[4]==0&&p[3]+p[7]==2)
    return TRUE;
 if(p[0]+p[1]+p[3]==0&&p[4]+p[7]==2)
    return TRUE;
 if(p[3]+p[6]+p[7]==0&&p[1]+p[4]==2)
    return TRUE;
 if(p[4]+p[7]+p[8]==0&&p[1]+p[3]==2)
    return TRUE;
return FALSE;
}           
BOOL CThinDoc::Match3(long i,long j,int pass)
{
 /*     7    0     1
        6   (i,j)  2
        5    4     3
 */       
 int p[8],Ap=0,Bp=0;
 p[0]=GetAt(i-1,j);p[1]=GetAt(i-1,j+1);
 p[2]=GetAt(i,j+1);p[3]=GetAt(i+1,j+1);
 p[4]=GetAt(i+1,j);p[5]=GetAt(i+1,j-1);
 p[6]=GetAt(i,j-1);p[7]=GetAt(i-1,j-1);
 for(int k=0;k<8;k++){
  Bp+=p[k];
  if(k>0&&p[k-1]==0&&p[k]==1)Ap++;}
 if(!(Bp>=2&&Bp<=6&&Ap==1))return FALSE;
 switch(pass){
  case 1:if(p[0]*p[2]*p[4]==0&&p[2]*p[4]*p[6]==0)
             return TRUE;break;
  case 2:if(p[0]*p[2]*p[6]==0&&p[0]*p[4]*p[6]==0)           
             return TRUE;break;}
 return FALSE;            
}    
BOOL CThinDoc::Match4(long i,long j,int pass)
{
 //if(GetAt(i,j)!=CThinDoc::blackpoint) return FALSE;
 /*    0   1   2
       3 (i,j) 4  
       5   6   7
 */
 int p[8];
 p[0]=GetAt(i-1,j-1);p[1]=GetAt(i-1,j);
 p[2]=GetAt(i-1,j+1);p[3]=GetAt(i,j-1);
 p[4]=GetAt(i,j+1);p[5]=GetAt(i+1,j-1);
 p[6]=GetAt(i+1,j);p[7]=GetAt(i+1,j+1);
//whitepoint:0 blackpoint:1 
 switch(pass){
  case 1:
     if(p[0]+p[1]+p[3]==0&&p[4]+p[6]==2)  //D1
                        return TRUE;
     if(p[1]+p[2]+p[4]==0&&p[3]+p[6]==2)  //D2
                        return TRUE;
     if(p[1]==0&&p[3]+p[4]+p[6]==3)       //E1
                        return TRUE;
     if(p[0]+p[1]+p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[2]==1)
                        return FALSE;//TRUE;      //G2
     if(p[0]+p[1]+p[3]+p[5]+p[6]==0&&p[4]==1)
                        return FALSE;//TRUE;      //G3
     if(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]==0&&p[7]==1)
                        return FALSE;//TRUE;      //G4
     if(p[0]+p[1]+p[2]+p[3]+p[4]==0&&p[6]==1)
                        return FALSE;//TRUE;      //G5 
     break;
  case 2:
     if(p[1]+p[2]+p[4]==0&&p[3]+p[6]==2)  //D2
                        return TRUE;
     if(p[4]+p[6]+p[7]==0&&p[1]+p[3]==2)  //D3
                        return TRUE;
     if(p[4]==0&&p[1]+p[3]+p[6]==3)       //E2
                        return TRUE;
     if(p[0]+p[1]+p[2]+p[3]+p[4]+p[5]+p[6]==0&&p[7]==1)
                        return FALSE;//TRUE;      //G4
     if(p[0]+p[1]+p[2]+p[3]+p[4]==0&&p[6]==1)
                        return FALSE;//TRUE;      //G5
     if(p[0]+p[1]+p[2]+p[3]+p[4]+p[6]+p[7]==0&&p[5]==1)
                        return FALSE;//TRUE;      //G6
     if(p[1]+p[2]+p[4]+p[6]+p[7]==0&&p[3]==1)
                        return FALSE;//TRUE;      //G7 
     break;
  case 3:
     if(p[4]+p[6]+p[7]==0&&p[1]+p[3]==2)  //D3
                        return TRUE;
     if(p[3]+p[5]+p[6]==0&&p[1]+p[4]==2)  //D4
                        return TRUE;
     if(p[6]==0&&p[1]+p[3]+p[4]==3)       //E3
                        return TRUE;
     if(p[0]+p[1]+p[2]+p[3]+p[4]+p[6]+p[7]==0&&p[5]==1)
                        return FALSE;//TRUE;      //G6
     if(p[1]+p[2]+p[4]+p[6]+p[7]==0&&p[3]==1)
                        return FALSE;//TRUE;      //G7
     if(p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[0]==1)
                        return FALSE;//TRUE;      //G8
     if(p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[1]==1)
                        return FALSE;//TRUE;      //G1 
     break;
  case 4:
     if(p[3]+p[5]+p[6]==0&&p[1]+p[4]==2)  //D4
                        return TRUE;
     if(p[0]+p[1]+p[3]==0&&p[4]+p[6]==2)  //D1
                        return TRUE;
     if(p[3]==0&&p[1]+p[4]+p[6]==3)       //E4
                        return TRUE;
     if(p[1]+p[2]+p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[0]==1)
                        return FALSE;//TRUE;      //G8
     if(p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[1]==1)
                        return FALSE;//TRUE;      //G1
     if(p[0]+p[1]+p[3]+p[4]+p[5]+p[6]+p[7]==0&&p[2]==1)
                        return FALSE;//TRUE;      //G2
     if(p[0]+p[1]+p[3]+p[5]+p[6]==0&&p[4]==1)
                        return FALSE;//TRUE;      //G3 
     break;
  default:break;}                                                                                                                                                                                    
 return FALSE;
}
////////////////////////////////////////////////////
BOOL CThinDoc::Thin1()
{
 long i,j;BOOL changed; 
 POSITION pos;MSG Message;
 char * tempbmp=CreateTempBmp(2*m_pDib->m_LineBytes);
 if(!tempbmp)               return FALSE;
 if(CheckThinCond()==FALSE) return FALSE;            	
 
 CStatusBar* pStatus=(CStatusBar*)
   AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 if(pStatus){char text[20];    
   wsprintf(text,"Parallel");
   pStatus->SetPaneText(0,text);}
 m_pass=-1;     
 m_iteration=0;   
 do{
    changed=FALSE;
    for(i=0;i<GetHeight();i++){
       if(i>=2){
          CopyToBmp(tempbmp,i-2,1);
          CopyToBmp(tempbmp+m_pDib->m_LineBytes,0,
                             1,tempbmp);
          if(m_scanline){
           pos=GetFirstViewPosition();
           while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}}
       for(j=0;j<GetWidth();j++){
          long x=((i==0)?0:1);
          if(GetAt(i,j)==CThinDoc::whitepoint)
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
          else  if(Match1(i,j)){
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
             changed=TRUE;}
          else   SetAt(x,j,CThinDoc::blackpoint,tempbmp);
          if(::PeekMessage(&Message,NULL,0,0,PM_REMOVE)){
           ::TranslateMessage(&Message);
           ::DispatchMessage(&Message);}}
                        }
   CopyToBmp(tempbmp,GetHeight()-2,2);
   m_iteration++;
   if(changed){
     SetModifiedFlag();
     pos=GetFirstViewPosition();
     while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}
  }while(changed);
  DelTempBmp(tempbmp);
  return TRUE;
}
BOOL CThinDoc::Thin2()
{
 long i,j;BOOL changed;POSITION pos;MSG Message;
 char * tempbmp=CreateTempBmp(2*m_pDib->m_LineBytes);
 if(!tempbmp)               return FALSE;
 if(CheckThinCond()==FALSE) return FALSE;
             	            	
 CStatusBar* pStatus=(CStatusBar*)
   AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 if(pStatus){char text[20];    
   wsprintf(text,"Parallel");
   pStatus->SetPaneText(0,text);}   
 m_pass=-1;  
 m_iteration=0;   
 do{
    changed=FALSE;
    for(i=0;i<GetHeight();i++){
       if(i>=2){
          CopyToBmp(tempbmp,i-2,1);
          CopyToBmp(tempbmp+m_pDib->m_LineBytes,0,
                 1,tempbmp);
          if(m_scanline){
           pos=GetFirstViewPosition();
           while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}}
       for(j=0;j<GetWidth();j++){
          long x=((i==0)?0:1);
          if(GetAt(i,j)==CThinDoc::whitepoint)
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
          else  if(Match2(i,j)){
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
             changed=TRUE;}
          else   SetAt(x,j,CThinDoc::blackpoint,tempbmp);
          if(::PeekMessage(&Message,NULL,0,0,PM_REMOVE)){
           ::TranslateMessage(&Message);
           ::DispatchMessage(&Message);}}
                        }
   CopyToBmp(tempbmp,GetHeight()-2,2);
   m_iteration++;
   if(changed){
     SetModifiedFlag();
     pos=GetFirstViewPosition();
     while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}
  }while(changed);
  DelTempBmp(tempbmp);
  return TRUE;
}
    
BOOL CThinDoc::Thin3()
{
 long i,j;BOOL changed,ch;int pass; 
 POSITION pos;
 MSG Message;
 char * tempbmp=CreateTempBmp(2*m_pDib->m_LineBytes);
 if(!tempbmp)                 return FALSE;
 if(CheckThinCond()==FALSE)   return FALSE;
 
 CStatusBar* pStatus=(CStatusBar*)
   AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 if(pStatus){    
   char text[20];
   wsprintf(text,"Parallel,2 passes");
   pStatus->SetPaneText(0,text);} 
   
 m_iteration=0; 
 do{
    changed=FALSE;
    m_pass=0;
   for(pass=1;pass<=2;pass++,m_pass++){
    ch=FALSE;
    for(i=0;i<GetHeight();i++){
       if(i>=2){
          CopyToBmp(tempbmp,i-2,1);
          CopyToBmp(tempbmp+m_pDib->m_LineBytes,0,
                       1,tempbmp);
          if(m_scanline){
            pos=GetFirstViewPosition();
            while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}}            
       for(j=0;j<GetWidth();j++){
          long x=((i==0)?0:1);
          if(GetAt(i,j)==CThinDoc::whitepoint)
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
          else  if(Match3(i,j,pass)){
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
             ch=TRUE;}
          else   SetAt(x,j,CThinDoc::blackpoint,tempbmp);
          if(::PeekMessage(&Message,NULL,0,0,PM_REMOVE)){
             ::TranslateMessage(&Message);
             ::DispatchMessage(&Message);}}
          }
   CopyToBmp(tempbmp,GetHeight()-2,2);
   if(ch){
     changed=TRUE;
     SetModifiedFlag();
     pos=GetFirstViewPosition();
     while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}
    }
    m_iteration++;   
 }while(changed);      
  DelTempBmp(tempbmp);
  return TRUE;
}   
BOOL CThinDoc::Thin4()
{
 long i,j;int pass; 
 BOOL changed,ch;
 POSITION pos;MSG Message;
 char * tempbmp=CreateTempBmp(2*m_pDib->m_LineBytes);
 if(!tempbmp)               return FALSE;
 if(CheckThinCond()==FALSE) return FALSE;            	
 
 CStatusBar* pStatus=(CStatusBar*)
   AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
 if(pStatus){    
   char text[20];
   wsprintf(text,"Parallel,4 passes");
   pStatus->SetPaneText(0,text);}    
   
 m_iteration=0; 
 do{
    changed=FALSE;
    m_pass=0;
   for(pass=1;pass<=4;pass++,m_pass++){
    ch=FALSE;
    for(i=0;i<GetHeight();i++){
       if(i>=2){
          CopyToBmp(tempbmp,i-2,1);
          CopyToBmp(tempbmp+m_pDib->m_LineBytes,0,
                       1,tempbmp);
          if(m_scanline){
            pos=GetFirstViewPosition();
            while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}}            
       for(j=0;j<GetWidth();j++){
          long x=((i==0)?0:1);
          if(GetAt(i,j)==CThinDoc::whitepoint)
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
          else  if(Match4(i,j,pass)){
             SetAt(x,j,CThinDoc::whitepoint,tempbmp);
             ch=TRUE;}
          else   SetAt(x,j,CThinDoc::blackpoint,tempbmp);
          if(::PeekMessage(&Message,NULL,0,0,PM_REMOVE)){
             ::TranslateMessage(&Message);
             ::DispatchMessage(&Message);}}
          }
   CopyToBmp(tempbmp,GetHeight()-2,2);
   if(ch){
     changed=TRUE;
     SetModifiedFlag();
     pos=GetFirstViewPosition();
     while(pos!=NULL)GetNextView(pos)->Invalidate(FALSE);}
    }
    m_iteration++;   
 }while(changed);     
  DelTempBmp(tempbmp);
  return TRUE;
}
//////////////////////////
// CThinDoc commands
void CThinDoc::OnThinScanline()
{m_scanline=!m_scanline;}
void CThinDoc::OnUpdateThinScanline(CCmdUI* pCmdUI)
{pCmdUI->SetCheck(m_scanline);}
////////////////////////




