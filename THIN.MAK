# Microsoft Visual C++ generated build script - Do not modify

PROJ = THIN
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\TEMP\THIN\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"THIN.PDB"
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /AL /O1 /D "NDEBUG" /FR /GA
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE
LIBS_D_WEXE = lafxcwd oldnames libw llibcew commdlg shell 
LIBS_R_WEXE = lafxcw oldnames libw llibcew commdlg shell 
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = THIN.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = STDAFX.SBR \
		THIN.SBR \
		MAINFRM.SBR \
		THINDOC.SBR \
		THINVIEW.SBR \
		CDIB.SBR


THIN_RCDEP = e:\temp\thin\res\thin.ico \
	e:\temp\thin\res\thindoc.ico \
	e:\temp\thin\res\thin.rc2


STDAFX_DEP = e:\temp\thin\stdafx.h


THIN_DEP = e:\temp\thin\stdafx.h \
	e:\temp\thin\thin.h \
	e:\temp\thin\mainfrm.h \
	e:\temp\thin\cdib.h \
	e:\temp\thin\thindoc.h \
	e:\temp\thin\thinview.h


MAINFRM_DEP = e:\temp\thin\stdafx.h \
	e:\temp\thin\thin.h \
	e:\temp\thin\mainfrm.h


THINDOC_DEP = e:\temp\thin\stdafx.h \
	e:\temp\thin\thin.h \
	e:\temp\thin\cdib.h \
	e:\temp\thin\thindoc.h


THINVIEW_DEP = e:\temp\thin\stdafx.h \
	e:\temp\thin\thin.h \
	e:\temp\thin\cdib.h \
	e:\temp\thin\thindoc.h \
	e:\temp\thin\thinview.h


CDIB_DEP = e:\temp\thin\stdafx.h \
	e:\temp\thin\cdib.h


all:	$(PROJ).EXE $(PROJ).BSC

THIN.RES:	THIN.RC $(THIN_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r THIN.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

THIN.OBJ:	THIN.CPP $(THIN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c THIN.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

THINDOC.OBJ:	THINDOC.CPP $(THINDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c THINDOC.CPP

THINVIEW.OBJ:	THINVIEW.CPP $(THINVIEW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c THINVIEW.CPP

CDIB.OBJ:	CDIB.CPP $(CDIB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CDIB.CPP


$(PROJ).EXE::	THIN.RES

$(PROJ).EXE::	STDAFX.OBJ THIN.OBJ MAINFRM.OBJ THINDOC.OBJ THINVIEW.OBJ CDIB.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
THIN.OBJ +
MAINFRM.OBJ +
THINDOC.OBJ +
THINVIEW.OBJ +
CDIB.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
g:\msvc\lib\+
g:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) THIN.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	THIN.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) THIN.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
