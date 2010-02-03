#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

EXESOURCEDIR = %EPOCROOT%epoc32\release\winscw\udeb
EXEDESTDIR = %EPOCROOT%epoc32\release\$(PLATFORM)\udeb\z\HelloV2_C
EXEDESTDIR_CONTENTS = $(EXEDESTDIR)\*.*
EMPTYDIR = %EPOCROOT%epoc32\release\$(PLATFORM)\udeb\z\System\data\Testpath\Empty

EXENAME = HelloV2_C.exe
REGNAME = HelloV2_C_reg.rsc
RSRCNAME = HelloV2_C.rsc

EXENAME_Z = HelloV2.exe

SADEXENAME = HelloV2NoIke.exe
SADREGNAME = HelloV2NoIke_reg.rsc

MBMNAME = HelloV2_C_icon.mbm
MBMNAME_DEFAULT_ROOT = default_app_icon.mbm
MBMNAME_C_APP_ROOT = HelloV2_C_icon.mbm
MBMNAME_Z_APP_ROOT = HelloV2_icon.mbm
MBMNAME_Z_APP_NOIKE_ROOT = HelloV2NoIkeIcon.mbm

RSRCAPPSPATH = %EPOCROOT%epoc32\winscw\c\Resource\Apps
RSRCPATH = %EPOCROOT%epoc32\winscw\c\Resource
REGPATH = %EPOCROOT%epoc32\winscw\c\private\10003a3f\Apps

ALTICONSDIR = %EPOCROOT%epoc32\winscw\c\AltIcons
ALTICONSCONTENTS = $(ALTICONSDIR)\*.*
ALTICONSDIR_Z = %EPOCROOT%epoc32\winscw\c\AltIcons\Z
ALTICONSCONTENTS_Z = $(ALTICONSDIR)\Z\*.*

ALTICONS_REPOSITORY = %EPOCROOT%epoc32\release\$(PLATFORM)\udeb\z\IconStore
ALTICONS_REPOSITORY_CONTENTS = $(ALTICONS_REPOSITORY)\*.*
ALTICONS_MBMNAME_C_APP = HelloV2_C_icon.mbm
ALTICONS_SOURCE_MBMNAME_C_APP = ..\tsrc\HelloV2_C\AltIcon\$(ALTICONS_MBMNAME_C_APP)
ALTICONS_MBMNAME_Z_APP = HelloV2_icon.mbm
ALTICONS_SOURCE_MBMNAME_Z_APP = ..\tsrc\HelloV2\AltIcon\$(ALTICONS_MBMNAME_Z_APP)
ALTICONS_SOURCE_MBMNAME_Z_APP_NOIKE = ..\tsrc\HelloV2NoIke\HelloV2NoIkeIcon.mbm
ALTICONS_MBMNAME_Z_APP_NOIKE = HelloV2NoIkeIcon.mbm
ALTICONS_MBMNAME_DEFAULT_ICON = default_app_icon.mbm
ALTICONS_SOURCE_MBMNAME_DEFAULT_ICON = ..\tsrc\HelloV2NoIke\default_app_icon.mbm

SYSDIR = %EPOCROOT%epoc32\winscw\c\sys
BINDIR = $(SYSDIR)\bin
SYSDIR_Z = %EPOCROOT%epoc32\release\winscw\udeb\z\sys
BINDIR_Z = $(SYSDIR_Z)\bin

ROMBUILD_TEMP = %EPOCROOT%epoc32\release\IconsTemp
ROMBUILD_TEMP_CONTENTS = %EPOCROOT%epoc32\release\IconsTemp\*.*

CROOTDIR = %EPOCROOT%epoc32\winscw\c

MAKMAKE FREEZE LIB CLEANLIB RESOURCE RELEASABLES BLD SAVESPACE:

!IF ("$(PLATFORM)" == "WINSCW")

FINAL :
#	Make the C-based-app file repository on Z
	echo Creating $(EXEDESTDIR) for $(PLATFORM)
	-mkdir $(EXEDESTDIR) 2>NUL
	echo .

#	Move the C-based exe from the release directory to the app-file repository on Z
	echo Moving $(EXENAME) for $(PLATFORM) to $(EXEDESTDIR)
	-move $(EXESOURCEDIR)\$(EXENAME) $(EXEDESTDIR)\$(EXENAME)
	echo .

#	Make the folder on C for resource files
	echo Creating $(RSRCAPPSPATH)
	-mkdir $(RSRCPATH)
	-mkdir $(RSRCAPPSPATH)
	echo .

#	Make the folder on C for registration files
	echo Creating $(REGPATH)
	-mkdir $(REGPATH)
	echo .

#	Move the Z-based exe from the release directory to the sys\bin on Z
	echo Moving $(EXENAME_Z) for $(PLATFORM) to $(BINDIR_Z)\$(EXENAME_Z)
	-move $(EXESOURCEDIR)\$(EXENAME_Z) $(BINDIR_Z)\$(EXENAME_Z)
	echo .

	echo Copying the Alt-Icons to repository on Z
	-mkdir $(ALTICONS_REPOSITORY)
	-copy $(ALTICONS_SOURCE_MBMNAME_C_APP) $(ALTICONS_REPOSITORY)\$(ALTICONS_MBMNAME_C_APP)
	-copy $(ALTICONS_SOURCE_MBMNAME_Z_APP) $(ALTICONS_REPOSITORY)\$(ALTICONS_MBMNAME_Z_APP)
	-copy $(ALTICONS_SOURCE_MBMNAME_Z_APP_NOIKE) $(ALTICONS_REPOSITORY)\$(ALTICONS_MBMNAME_Z_APP_NOIKE)
	-copy $(ALTICONS_SOURCE_MBMNAME_DEFAULT_ICON) $(ALTICONS_REPOSITORY)\$(ALTICONS_MBMNAME_DEFAULT_ICON)

#	Create 'Empty' folder for T_Serv2 tests
	echo Creating $(EMPTYDIR) for $(PLATFORM)
	-mkdir $(EMPTYDIR) 2>NUL
	echo .


CLEAN :
#	Removing files from drive Z, the C-app-file repository
    echo Deleting $(EXEDESTDIR_CONTENTS) for $(PLATFORM)
    -del /q $(EXEDESTDIR_CONTENTS) 2>NUL
	echo .

#	Remove the drive Z, C-app-file repository
	echo Removing $(EXEDESTDIR) if empty for $(PLATFORM)
    -rmdir $(EXEDESTDIR) 2>NUL
	echo .

#	Remove the Z-based exe from the bin directory
    echo Deleting $(BINDIR_Z)\$(EXENAME_Z) for $(PLATFORM)
    -del $(BINDIR_Z)\$(EXENAME_Z) 2>NUL
	echo .

#	Remove the C-based exe from the bin directory
    echo Deleting $(BINDIR)\$(EXENAME) for $(PLATFORM)
    -del $(BINDIR)\$(EXENAME) 2>NUL
	echo .

	echo Removing $(BINDIR) if empty for $(PLATFORM)
    -rmdir $(BINDIR) 2>NUL
	echo .

	echo Removing $(SYSDIR) if empty for $(PLATFORM)
    -rmdir $(SYSDIR) 2>NUL
	echo .

#	Remove the alt-icon files  for Z-based apps
	echo Deleting $(ALTICONSCONTENTS_Z) for $(PLATFORM)
	-del /q $(ALTICONSCONTENTS_Z)
	echo .

	echo Removing $(ALTICONSDIR_Z) directory for $(PLATFORM)
	-rmdir $(ALTICONSDIR_Z)
	echo .

#	Remove the alt-icon files  for C-based apps
	echo Deleting $(ALTICONSCONTENTS) for $(PLATFORM)
	-del /q $(ALTICONSCONTENTS)
	echo .

	echo Removing $(ALTICONSDIR) directory for $(PLATFORM)
	-rmdir $(ALTICONSDIR)
	echo .

	echo Removing the alt-icons repository
	-del /q $(ALTICONS_REPOSITORY_CONTENTS)
	-rmdir $(ALTICONS_REPOSITORY)
	echo .

#	Uninstall the C-based app
	echo Deleting registration file $(REGPATH)\$(REGNAME) for $(PLATFORM)
	-del $(REGPATH)\$(REGNAME)
	echo .

	echo Removing $(REGPATH) if empty for $(PLATFORM)
	-rmdir $(REGPATH) 
	echo .

	echo Removing $(RSRCAPPSPATH)\$(RSRCNAME) for $(PLATFORM)
	-del $(RSRCAPPSPATH)\$(RSRCNAME)
	echo .

	echo Deleting $(RSRCAPPSPATH)\$(MBMNAME) for $(PLATFORM)
	-del $(RSRCAPPSPATH)\$(MBMNAME)
	echo .

	echo Removing $(RSRCAPPSPATH) if empty for $(PLATFORM)
	-rmdir $(RSRCAPPSPATH)
	echo .

	echo Removing $(RSRCPATH) if empty for $(PLATFORM)
	-rmdir $(RSRCPATH)
	echo .

	echo Deleting MBM files in the root of emulated drive-C
	echo .
	echo $(CROOTDIR)\$(MBMNAME_DEFAULT_ROOT)
	-del $(CROOTDIR)\$(MBMNAME_DEFAULT_ROOT)
	echo $(CROOTDIR)\$(MBMNAME_DEFAULT_ROOT)
	-del $(CROOTDIR)\$(MBMNAME_C_APP_ROOT)
	echo $(CROOTDIR)\$(MBMNAME_DEFAULT_ROOT)
	-del $(CROOTDIR)\$(MBMNAME_Z_APP_ROOT)
	echo $(CROOTDIR)\$(MBMNAME_Z_APP_NOIKE_ROOT)
	-del $(CROOTDIR)\$(MBMNAME_Z_APP_NOIKE_ROOT)
	echo .

#	Remove the "Empty" dir from the drive Z
	echo Removing $(EMPTYDIR) if empty for $(PLATFORM)
    -rmdir $(EMPTYDIR) 2>NUL
	echo .
!ELSE
FINAL :
	echo Copying AltIcons to temp directory
	-mkdir $(ROMBUILD_TEMP) 2>NUL
	-copy $(ALTICONS_SOURCE_MBMNAME_C_APP)	$(ROMBUILD_TEMP)
	-copy $(ALTICONS_SOURCE_MBMNAME_Z_APP) $(ROMBUILD_TEMP)
	-copy $(ALTICONS_SOURCE_MBMNAME_Z_APP_NOIKE) $(ROMBUILD_TEMP)
	-copy $(ALTICONS_SOURCE_MBMNAME_DEFAULT_ICON) $(ROMBUILD_TEMP)	
CLEAN :
	-del /q $(ROMBUILD_TEMP_CONTENTS) 2>NUL
	-rmdir $(ROMBUILD_TEMP) 2>NUL

!ENDIF