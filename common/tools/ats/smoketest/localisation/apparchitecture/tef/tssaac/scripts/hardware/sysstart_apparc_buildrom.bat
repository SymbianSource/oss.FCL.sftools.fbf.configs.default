@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off
setlocal

:: Initialise local variables
set TEST_CASE=%1
set TESTCASE_NUMBER=%2

set RSC_LOCATION=\epoc32\data\Z\private\10205C44
set TEST_RSC=%RSC_LOCATION%\tssaac_hardware.RSC
set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0_target.RSC

set TEST_ROM_FLAG=false
if /i "%3"=="TEST_ROM_ONLY" set TEST_ROM_FLAG=true

if /i "%TEST_ROM_FLAG%"=="true" set TEST_RSC=%RSC_LOCATION%\appfwk_sysstart_test_eshell_hardware.RSC

call :%TEST_CASE%

set AUTOEXEC=autoexec.bat
set AUTOEXEC_LOC=\epoc32\data\z\apparctest\%AUTOEXEC%

set APPARCTEST_IBY=\epoc32\rom\include\apparctest.iby

set EPOCWIND=%TEMP%\epocwind.out

set SSAAC_RESULT_FILE=\epoc32\data\z\apparctest\T_SSAAC_PanicResultFile.txt
set TESTCASE_CONFIG=\epoc32\data\z\apparctest\TestCaseConfig.txt

set DABS_SCRIPT=\logs\autorom\TESTEXECUTEapparctest_%TEST_CASE%.script.txt

set DABS_FLAG=%4

set HARDWARE_REFERENCE_PLATFORM=%5

goto :skipFunctions

:restore
	cd\
	if exist %APPARCTEST_IBY%.original copy %APPARCTEST_IBY%.original %APPARCTEST_IBY% > NUL
	if exist %APPARCTEST_IBY%.original del %APPARCTEST_IBY%.original >NUL

	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original 

	if exist %TESTCASE_CONFIG% del %TESTCASE_CONFIG% >NUL
	
	if exist %DABS_SCRIPT% del %DABS_SCRIPT% >NUL
	if exist %AUTOEXEC_LOC% del %AUTOEXEC_LOC% >NUL
goto :EOF

:configure
	if not exist %SM0_RSC%.original copy %SM0_RSC% %SM0_RSC%.original > NUL
	copy %TEST_RSC% %SM0_RSC% > NUL

	if not exist %APPARCTEST_IBY%.original copy %APPARCTEST_IBY% %APPARCTEST_IBY%.original > NUL

	echo Test Case %TEST_CASE%=%TESTCASE_NUMBER% > %TESTCASE_CONFIG%
	echo data=DATAZ_\apparctest\TestCaseConfig.txt  \apparctest\TestCaseConfig.txt >> %APPARCTEST_IBY%
	echo data=DATAZ_\apparctest\%AUTOEXEC% \%AUTOEXEC% >> %APPARCTEST_IBY%

	if exist %SSAAC_RESULT_FILE% echo copy z:\apparctest\T_SSAAC_PanicResultFile.txt c:\ >> %AUTOEXEC_LOC%

	if /i "%DABS_FLAG%"=="true" call :configureDabs & goto :EOF

	echo copy z:\apparctest\TestCaseConfig.txt c:\ >>  %AUTOEXEC_LOC%
	echo testexecute z:\apparctest\apparctest_%TEST_CASE%.script >> %AUTOEXEC_LOC%
	echo md e:\logs >> %AUTOEXEC_LOC%
	echo md e:\logs\testexecute >> %AUTOEXEC_LOC%
	echo copy c:\logs\testexecute\apparctest_%TEST_CASE%.htm e:\logs\testexecute\ >> %AUTOEXEC_LOC%
goto :EOF

:configureDabs 
	echo testexecute z:\apparctest\apparctest_%TEST_CASE%.script >> %DABS_SCRIPT% 
	echo thindump -nop c:\logs\testexecute\apparctest_%TEST_CASE%.htm >> %DABS_SCRIPT% 

	echo #include ^<thindump.iby^> >> %APPARCTEST_IBY%
	echo data=\logs\autorom\TESTEXECUTEapparctest_%TEST_CASE%.script.txt   \test\TESTEXECUTEapparctest_%TEST_CASE%.script.txt >> %APPARCTEST_IBY%
	echo data=\logs\autorom\dummytest.txt   \test\dummytest.txt >> %APPARCTEST_IBY%

	echo md c:\logs >> %AUTOEXEC_LOC%
	echo md c:\logs\testresults >> %AUTOEXEC_LOC%
	echo md c:\system >> %AUTOEXEC_LOC%
	echo md c:\system\data >> %AUTOEXEC_LOC%
	echo z: >> %AUTOEXEC_LOC%
	echo cd test >> %AUTOEXEC_LOC%
	echo RUNTESTS TESTEXECUTEapparctest_%TEST_CASE%.script.txt -t 13 >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="false" echo format k: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo c: >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo attrib -r >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo del c:\*.txt >> %AUTOEXEC_LOC%
	if "%TEST_ROM_FLAG%"=="true" echo z: >> %AUTOEXEC_LOC%
	echo RUNTESTS dummytest.txt -p >> %AUTOEXEC_LOC%
goto :EOF

:zipRom
	if exist rombld.img call zip sys$rom.zip rombld.img > NUL
	if exist rombld.symbol call zip symbol.zip rombld.symbol > NUL
	if exist rombld.img echo ROM built: \rom\%TEST_CASE%\sys$rom.zip
	if not exist sys$rom.zip echo ERROR: ROM failed to build.
goto :EOF

:addSsaacResultFile
	echo data=DATAZ_\apparctest\T_SSAAC_PanicResultFile.txt  \apparctest\T_SSAAC_PanicResultFile.txt >> %APPARCTEST_IBY%
goto :EOF

:buildTestRom
	cd\
	if exist \rom\%TEST_CASE%\testrom rd /s/q \rom\%TEST_CASE%\testrom
	md \rom\%TEST_CASE%\testrom
	cd \rom\%TEST_CASE%\testrom
 
	echo Building Test ROM for SysStart Apparc test case: %TEST_CASE%
	if exist %SSAAC_RESULT_FILE% call :addSsaacResultFile

	call buildrom -D_FULL_DEBUG -D_LFFSC %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby appfwk_test.iby -orombld.img> NUL
	call :zipRom

	if exist %SSAAC_RESULT_FILE% del %SSAAC_RESULT_FILE% >NUL

	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%\testrom
goto :EOF

:build
	if not exist %TEST_RSC% echo SysStart Apparc Test: ERROR - test resource file does not exist & goto :EOF

	call :restore
	call :configure

	if "%TEST_ROM_FLAG%"=="false" call :buildRom
	if "%TEST_ROM_FLAG%"=="true"  call :buildTestRom

	call :restore
goto :EOF

:buildRom
	set LFFSC_FLAG=
	if /i "%TEST_FLAG%"=="checkEpocWind" set LFFSC_FLAG=-D_LFFSC

	if exist \rom\%TEST_CASE% rd /s/q \rom\%TEST_CASE%

	cd\
	md \rom\%TEST_CASE%
	cd \rom\%TEST_CASE%
	if /i "%TEST_FLAG%"=="checkEpocWind" md \rom\%TEST_CASE%\TEST_ROM_REQUIRED

	echo Building ROM for SysStart Apparc test case: %TEST_CASE%
	call buildrom -D_FULL_DEBUG %LFFSC_FLAG% %HARDWARE_REFERENCE_PLATFORM% techview.oby platsec.oby appfwk_test.iby -orombld.img> rombuild.txt
	call :zipRom
	copy %AUTOEXEC_LOC% \rom\%TEST_CASE%
	echo %HARDWARE_REFERENCE_PLATFORM% UIF-Techview Apparc - %TEST_CASE% > rom_desc.txt
goto :EOF

:T_TestStartApp1L
	set TEST_FLAG=
goto :EOF

:T_TestLocalisedCaptionL
	set TEST_FLAG=
	set TEST_RSC=%RSC_LOCATION%\tssaac_tapp_hardware.RSC
goto :EOF

:T_TestStartApp2L		
:T_TestStartApp3L		
:T_TestStartApp4L		
:T_TestStartApp5L		
:T_TestStartApp6L		
:T_TestGetAllApps		
:T_TestInsertDataTypeL	
:T_TestAppForDataTypeL	
:T_TestDeleteDataTypeL	
:T_TestServiceDiscovery	
:T_TestGetAppInfo     	
:T_TestAppCount	    	
:T_TestCreateDoc
	set TEST_FLAG=checkEpocWind
goto :EOF
      
:skipFunctions

call :build

endlocal
