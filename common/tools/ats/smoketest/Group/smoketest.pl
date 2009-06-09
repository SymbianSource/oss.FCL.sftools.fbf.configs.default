#!/usr/bin/perl

use strict;
use File::Copy;
use File::Path;

unlink "smoketest.zip";
rmtree "temp";
mkpath "temp/smoketest/general";
mkpath "temp/smoketest/winscw_udeb";
mkpath "temp/smoketest/sms/general";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("smoketest.xml",												"temp/test.xml");
copy($epoc . "data/z/smoketest/smoketest_agenda.ini",				"temp/smoketest/general/smoketest_agenda.ini");
copy($epoc . "data/z/smoketest/smoketest_agenda.script",			"temp/smoketest/general/smoketest_agenda.script");
copy($epoc . "data/z/smoketest/smoketest_apploader.ini",			"temp/smoketest/general/smoketest_apploader.ini");
copy($epoc . "data/z/smoketest/smoketest_apploader.script",			"temp/smoketest/general/smoketest_apploader.script");
copy($epoc . "data/z/smoketest/smoketest_contacts.ini",				"temp/smoketest/general/smoketest_contacts.ini");
copy($epoc . "data/z/smoketest/smoketest_contacts.script",			"temp/smoketest/general/smoketest_contacts.script");
copy($epoc . "data/z/smoketest/smoketest_createsmsaccount.script",	"temp/smoketest/general/smoketest_createsmsaccount.script");
copy($epoc . "data/z/smoketest/smoketest_mess.ini",					"temp/smoketest/general/smoketest_mess.ini");
copy($epoc . "data/z/smoketest/smoketest_mess.script",				"temp/smoketest/general/smoketest_mess.script");
copy($epoc . "data/z/smoketest/smoketest_phone.script",				"temp/smoketest/general/smoketest_phone.script");
copy($epoc . "data/z/smoketest/smoketest_syncmlapp.script",			"temp/smoketest/general/smoketest_syncmlapp.script");
copy($epoc . "data/z/smoketest/smoketest_system.script",			"temp/smoketest/general/smoketest_system.script");
copy($epoc . "data/z/smoketest/smoketest_timew.ini",				"temp/smoketest/general/smoketest_timew.ini");
copy($epoc . "data/z/smoketest/smoketest_timew.script",				"temp/smoketest/general/smoketest_timew.script");
copy($epoc . "data/z/smoketest/sms/message1.txt",					"temp/smoketest/sms/general/message1.txt");
copy($epoc . "release/winscw/udeb/SmokeTestSecureFSclient.dll",		"temp/smoketest/winscw_udeb/SmokeTestSecureFSclient.dll");
copy($epoc . "release/winscw/udeb/SmokeTestSecureFSserver.exe",		"temp/smoketest/winscw_udeb/SmokeTestSecureFSserver.exe");
copy($epoc . "release/winscw/udeb/Smoketest_Agenda_Server.exe",		"temp/smoketest/winscw_udeb/Smoketest_Agenda_Server.exe");
copy($epoc . "release/winscw/udeb/smoketest_apploader.exe",			"temp/smoketest/winscw_udeb/smoketest_apploader.exe");
copy($epoc . "release/winscw/udeb/Smoketest_Contacts_Server.exe",	"temp/smoketest/winscw_udeb/Smoketest_Contacts_Server.exe");
copy($epoc . "release/winscw/udeb/Smoketest_Mess_Server.exe",		"temp/smoketest/winscw_udeb/Smoketest_Mess_Server.exe");
copy($epoc . "release/winscw/udeb/Smoketest_System.exe",			"temp/smoketest/winscw_udeb/Smoketest_System.exe");
copy($epoc . "release/winscw/udeb/Smoketest_Timew_Server.exe",		"temp/smoketest/winscw_udeb/Smoketest_Timew_Server.exe");
copy($epoc . "release/winscw/udeb/SmokeTest_Utils.dll",				"temp/smoketest/winscw_udeb/SmokeTest_Utils.dll");

system("7z a -tzip smoketest.zip ./temp/*");
