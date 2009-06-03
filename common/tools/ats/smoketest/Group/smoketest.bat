del smoketest.zip
rd /S /Q temp
md temp
md temp\smoketest
md temp\smoketest\general
md temp\smoketest\winscw_udeb
md temp\smoketest\sms
md temp\smoketest\sms\general

copy /Y smoketest.xml temp\test.xml
copy %EPOCROOT%epoc32\data\z\smoketest									temp\smoketest\general
copy %EPOCROOT%epoc32\data\z\smoketest\sms								temp\smoketest\sms\general
copy %EPOCROOT%epoc32\release\winscw\udeb\Smoketest_Agenda_Server.exe	temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\smoketest_apploader.exe		temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\Smoketest_Contacts_Server.exe	temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\Smoketest_Mess_Server.exe		temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\Smoketest_System.exe			temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\Smoketest_Timew_Server.exe	temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\SmokeTest_Utils.dll			temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\SmokeTestSecureFSclient.dll	temp\smoketest\winscw_udeb
copy %EPOCROOT%epoc32\release\winscw\udeb\SmokeTestSecureFSserver.exe	temp\smoketest\winscw_udeb

7z a -tzip smoketest.zip .\temp\*
