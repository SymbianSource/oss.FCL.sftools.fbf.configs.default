@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Symbian Foundation License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
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
set COMMAND_CHOICE=%1
set RSC_LOCATION=Z\private\10205C44
set TEST_CASE=%2
set TESTCASE_NUMBER=%3
set TEST_RSC=%RSC_LOCATION%\tssaac_emulator.RSC
set TEST_EXE_RSC=%RSC_LOCATION%\tssaac_tapp_emulator.RSC
set SM0_RSC=%RSC_LOCATION%\SSCForStartupMode0.RSC

set TESTCASE_CONFIG=z\apparctest\TestCaseConfig.txt
set EPOCWIND=%TEMP%\epocwind.out
set TESTNONPANIC_RESULT_FILE=\epoc32\winscw\c\SysStartApparc_TestNonPanic.txt

goto :%COMMAND_CHOICE%

:install
	if not exist "%TEST_RSC%" echo SysStart Test: ERROR - test resource file does not exist & goto :EOF
	if not exist "%TEST_EXE_RSC%" echo SysStart Test: ERROR - test resource file does not exist & goto :EOF

	if not exist %SM0_RSC%.original copy %SM0_RSC% %SM0_RSC%.original > NUL
	
	if /i "%TEST_CASE%" NEQ "T_TestLocalisedCaptionL" copy %TEST_RSC% %SM0_RSC% > NUL
	if /i "%TEST_CASE%" == "T_TestLocalisedCaptionL" copy %TEST_EXE_RSC% %SM0_RSC% > NUL
	
	if exist %TESTCASE_CONFIG% del %TESTCASE_CONFIG%
	if exist %TESTNONPANIC_RESULT_FILE% del %TESTNONPANIC_RESULT_FILE%

	echo Test Case %TEST_CASE%=%TESTCASE_NUMBER% > %TESTCASE_CONFIG%

goto :EOF

:uninstall
	call :restore	
	:: TestStartApp1L and TestLocalisedCaptionL are the only non-paniccing test cases
	if /i "%TEST_CASE%" == "T_TestStartApp1L" goto :bypass
	if /i "%TEST_CASE%" == "T_TestLocalisedCaptionL" goto :bypass
	call :test
	:bypass
	if exist %EPOCROOT%epoc32\winscw\c\T_SSAAC_PanicResultFile.txt del %EPOCROOT%epoc32\winscw\c\T_SSAAC_PanicResultFile.txt
goto :EOF

:execTimeOut
	set TIMEOUT=300
	\ExecTimeOut.exe "testexecute.exe z:\apparctest\apparctest_%TEST_CASE%.script" %TIMEOUT%
goto :EOF

:testExecute
	testexecute.exe z:\apparctest\apparctest_%TEST_CASE%.script
goto :EOF

:test
	call z\apparctest\sysstart_apparc_checkEpocWind.bat %TEST_CASE%

	if exist %TEMP%\epocwind.out copy %TEMP%\epocwind.out %TEMP%\epocwind.out.temp > NUL

	:: If using SYMBIAN overnight DABS test scripts, then utilise ExecTimeOut to ensure
	:: execution does not exceed expected execution time.
	if exist \ExecTimeOut.exe call :execTimeOut

	:: If simply executing manually, then call testexecute directly.
	if not exist \ExecTimeOut.exe call :testExecute

	if exist %TEMP%\epocwind.out.temp copy %TEMP%\epocwind.out.temp %TEMP%\epocwind.out > NUL
	if exist %TEMP%\epocwind.out.temp del %TEMP%\epocwind.out.temp
goto :EOF

:restore
	if exist %SM0_RSC%.original copy %SM0_RSC%.original %SM0_RSC% > NUL
	if exist %SM0_RSC%.original del %SM0_RSC%.original
goto :EOF

endlocal

