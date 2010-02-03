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

set SYSSTART_APPARCTEST_SUMMARY=\epoc32\winscw\c\logs\testexecute\sysstart_apparctest_summary.txt
if exist %SYSSTART_APPARCTEST_SUMMARY% del %SYSSTART_APPARCTEST_SUMMARY%

goto :skipFunctions

:test
	set TEST_CASE=%1
	set TESTCASE_NUMBER=%2
	set EPOCWIND_COPY=\epoc32\winscw\c\logs\testexecute\apparctest_%TEST_CASE%_epocwind.out
	set EPOCWIND_ORIG=%TEMP%\epocwind.out

	call z\apparctest\sysstart_apparc_setup.bat install   %TEST_CASE% %TESTCASE_NUMBER%

	echo testexecute.exe z:\apparctest\apparctest_%TEST_CASE%.script
	testexecute.exe z:\apparctest\apparctest_%TEST_CASE%.script

	call z\apparctest\sysstart_apparc_setup.bat uninstall %TEST_CASE% 

	copy %EPOCWIND_ORIG% %EPOCWIND_COPY% >NUL
	call :updateTestSummary
goto :EOF

:checkLog
	:: checkLog SEARCH_FILE SEARCH_STRING EXPECTED_NUMBER_OF_OCCURENCES
	set SEARCH_FILE=%1
	set SEARCH_STRING=%2
	set TEMP_FILE=sysstart_temp.txt
	set EXPECTED_COUNT=%3
	set LOG_COUNT=0
	type %SEARCH_FILE% | find /c %SEARCH_STRING%>%TEMP_FILE%
	for /f "delims=\" %%i in ('type %TEMP_FILE%') do set LOG_COUNT=%%i
	del %TEMP_FILE%
	if "%EXPECTED_COUNT%" ==  "%LOG_COUNT%" goto :EOF
	if "%EXPECTED_COUNT%" NEQ "%LOG_COUNT%" set TEST_RESULT_FLAG=FAIL
goto :EOF

:updateTestSummary
	set TEST_RESULT_FLAG=PASS
	call :checkLog \epoc32\winscw\c\logs\testexecute\apparctest_%TEST_CASE%.htm "%TEST_CASE% ***Result = PASS" 1
	echo %TEST_CASE% - %TEST_RESULT_FLAG% >> %SYSSTART_APPARCTEST_SUMMARY%
goto :EOF

:skipFunctions 
    
:: SysStart Apparc Function call tests
call :test T_TestStartApp1L		01  
call :test T_TestStartApp2L		02        
call :test T_TestStartApp3L		03     
call :test T_TestStartApp4L		04	   
call :test T_TestStartApp5L		05     
call :test T_TestStartApp6L		06     
call :test T_TestGetAllApps		07     
call :test T_TestInsertDataTypeL	08
call :test T_TestAppForDataTypeL	09 
call :test T_TestDeleteDataTypeL	10 
call :test T_TestServiceDiscovery	11  
call :test T_TestGetAppInfo     	12
call :test T_TestAppCount	    	13
call :test T_TestCreateDoc      	14
call :test T_TestLocalisedCaptionL	29

endlocal
