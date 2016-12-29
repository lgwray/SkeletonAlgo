//////////////////////////////////////////
// thin.h 
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"       // main symbols
//////////////////////////////////
// CThinApp:
// See thin.cpp for the implementation of this class
class CThinApp : public CWinApp
{
public:
	CThinApp();
// Overrides
	virtual BOOL InitInstance();
// Implementation
	//{{AFX_MSG(CThinApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////
