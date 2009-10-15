#!/usr/bin/perl
# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
# Maciej Seroka <maciejs@symbian.org>
#
# Description:
# Script to build ATS test drop for Secure Socket Layer BC tests


use strict;
use File::Copy;
use File::Path;

unlink "ssltest.zip";
rmtree "temp";
mkpath "temp/ssltest/general/testframework";
mkpath "temp/ssltest/winscw_udeb";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("ssl.xml",	"temp/test.xml");

copy($epoc . "winscw/c/testframework/crypto_test/testframework.ini",	"temp/ssltest/general/testframework/testframework.ini") or die "failed : $!";
copy($epoc . "winscw/c/testframework/crypto_test.cfg",	"temp/ssltest/general/testframework/crypto_test.cfg") or die "failed : $!";
copy($epoc . "winscw/c/testframework/evptests.txt",		"temp/ssltest/general/testframework/evptests.txt") or die "failed : $!";
copy($epoc . "winscw/c/tpkcs7d.txt",					"temp/ssltest/general/testframework/tpkcs7d.txt") or die "failed : $!";
copy($epoc . "winscw/c/tpkcs7.txt",						"temp/ssltest/general/testframework/tpkcs7.txt") or die "failed : $!";
copy($epoc . "winscw/c/treq2.txt",						"temp/ssltest/general/testframework/treq2.txt") or die "failed : $!";
copy($epoc . "winscw/c/treq.txt",						"temp/ssltest/general/testframework/treq.txt") or die "failed : $!";
copy($epoc . "winscw/c/trsa.txt",						"temp/ssltest/general/testframework/trsa.txt") or die "failed : $!";
copy($epoc . "winscw/c/tsid.txt",						"temp/ssltest/general/testframework/tsid.txt") or die "failed : $!";
copy($epoc . "winscw/c/tx5091.txt",						"temp/ssltest/general/testframework/tx5091.txt") or die "failed : $!";
copy($epoc . "winscw/c/tx5092.txt",						"temp/ssltest/general/testframework/tx5092.txt") or die "failed : $!";
copy($epoc . "winscw/c/tx509.txt",						"temp/ssltest/general/testframework/tx509.txt") or die "failed : $!";
copy($epoc . "winscw/c/tcrl.txt",						"temp/ssltest/general/testframework/tcrl.txt") or die "failed : $!";
copy($epoc . "winscw/c/testenc.txt",					"temp/ssltest/general/testframework/testenc.txt") or die "failed : $!";
copy($epoc . "winscw/c/testgen.txt",					"temp/ssltest/general/testframework/testgen.txt") or die "failed : $!";
copy($epoc . "winscw/c/testss.txt",						"temp/ssltest/general/testframework/testss.txt") or die "failed : $!";
copy($epoc . "winscw/c/test_verify.txt",				"temp/ssltest/general/testframework/test_verify.txt") or die "failed : $!";
copy($epoc . "winscw/c/CAss.cnf",						"temp/ssltest/general/testframework/CAss.cnf") or die "failed : $!";
copy($epoc . "winscw/c/openssl.cnf",					"temp/ssltest/general/testframework/openssl.cnf") or die "failed : $!";
copy($epoc . "winscw/c/P1ss.cnf",						"temp/ssltest/general/testframework/P1ss.cnf") or die "failed : $!";
copy($epoc . "winscw/c/P2ss.cnf",						"temp/ssltest/general/testframework/P2ss.cnf") or die "failed : $!";
copy($epoc . "winscw/c/test.cnf",						"temp/ssltest/general/testframework/test.cnf") or die "failed : $!";
copy($epoc . "winscw/c/Uss.cnf",						"temp/ssltest/general/testframework/Uss.cnf") or die "failed : $!";
copy($epoc . "winscw/c/argena.pem",						"temp/ssltest/general/testframework/argena.pem") or die "failed : $!";
copy($epoc . "winscw/c/argeng.pem",						"temp/ssltest/general/testframework/argeng.pem") or die "failed : $!";
copy($epoc . "winscw/c/eng1.pem",						"temp/ssltest/general/testframework/eng1.pem") or die "failed : $!";
copy($epoc . "winscw/c/eng2.pem",						"temp/ssltest/general/testframework/eng2.pem") or die "failed : $!";
copy($epoc . "winscw/c/eng3.pem",						"temp/ssltest/general/testframework/eng3.pem") or die "failed : $!";
copy($epoc . "winscw/c/eng4.pem",						"temp/ssltest/general/testframework/eng4.pem") or die "failed : $!";
copy($epoc . "winscw/c/eng5.pem",						"temp/ssltest/general/testframework/eng5.pem") or die "failed : $!";
copy($epoc . "winscw/c/pkcs7-1.pem",					"temp/ssltest/general/testframework/pkcs7-1.pem") or die "failed : $!";
copy($epoc . "winscw/c/RegTP-5R.pem",					"temp/ssltest/general/testframework/RegTP-5R.pem") or die "failed : $!";
copy($epoc . "winscw/c/RegTP-6R.pem",					"temp/ssltest/general/testframework/RegTP-6R.pem") or die "failed : $!";
copy($epoc . "winscw/c/testcrl.pem",					"temp/ssltest/general/testframework/testcrl.pem") or die "failed : $!";
copy($epoc . "winscw/c/testp7.pem",						"temp/ssltest/general/testframework/testp7.pem") or die "failed : $!";
copy($epoc . "winscw/c/testreq.pem",					"temp/ssltest/general/testframework/testreq.pem") or die "failed : $!";
copy($epoc . "winscw/c/testreq2.pem",					"temp/ssltest/general/testframework/testreq2.pem") or die "failed : $!";
copy($epoc . "winscw/c/testrsa.pem",					"temp/ssltest/general/testframework/testrsa.pem") or die "failed : $!";
copy($epoc . "winscw/c/testsid.pem",					"temp/ssltest/general/testframework/testsid.pem") or die "failed : $!";
copy($epoc . "winscw/c/testx509.pem",					"temp/ssltest/general/testframework/testx509.pem") or die "failed : $!";
copy($epoc . "winscw/c/thawteCb.pem",					"temp/ssltest/general/testframework/thawteCb.pem") or die "failed : $!";
copy($epoc . "winscw/c/thawteCp.pem",					"temp/ssltest/general/testframework/thawteCp.pem") or die "failed : $!";
copy($epoc . "winscw/c/v3-cert1.pem",					"temp/ssltest/general/testframework/v3-cert1.pem") or die "failed : $!";
copy($epoc . "winscw/c/v3-cert2.pem",					"temp/ssltest/general/testframework/v3-cert2.pem") or die "failed : $!";
copy($epoc . "winscw/c/vsign1.pem",						"temp/ssltest/general/testframework/vsign1.pem") or die "failed : $!";
copy($epoc . "winscw/c/vsign3.pem",						"temp/ssltest/general/testframework/vsign3.pem") or die "failed : $!";
copy($epoc . "winscw/c/vsignss.pem",					"temp/ssltest/general/testframework/vsignss.pem") or die "failed : $!";
copy($epoc . "winscw/c/wellsfgo.pem",					"temp/ssltest/general/testframework/wellsfgo.pem") or die "failed : $!";
copy($epoc . "winscw/c/certCA.ss",						"temp/ssltest/general/testframework/certCA.ss") or die "failed : $!";
copy($epoc . "winscw/c/certU.ss",						"temp/ssltest/general/testframework/certU.ss") or die "failed : $!";
copy($epoc . "winscw/c/keyCA.ss",						"temp/ssltest/general/testframework/keyCA.ss") or die "failed : $!";
copy($epoc . "winscw/c/keyU.ss",						"temp/ssltest/general/testframework/keyU.ss") or die "failed : $!";
copy($epoc . "winscw/c/reqCA.ss",						"temp/ssltest/general/testframework/reqCA.ss") or die "failed : $!";
copy($epoc . "winscw/c/reqU.ss",						"temp/ssltest/general/testframework/reqU.ss") or die "failed : $!";
copy($epoc . "winscw/c/reqCA.ss2",						"temp/ssltest/general/testframework/reqCA.ss2") or die "failed : $!";
copy($epoc . "winscw/c/certP1.ss",						"temp/ssltest/general/testframework/certP1.ss") or die "failed : $!";
copy($epoc . "winscw/c/certP2.ss",						"temp/ssltest/general/testframework/certP2.ss") or die "failed : $!";
copy($epoc . "winscw/c/intP1.ss",						"temp/ssltest/general/testframework/intP1.ss") or die "failed : $!";
copy($epoc . "winscw/c/intP2.ss",						"temp/ssltest/general/testframework/intP2.ss") or die "failed : $!";
copy($epoc . "winscw/c/keyP1.ss",						"temp/ssltest/general/testframework/keyP1.ss") or die "failed : $!";
copy($epoc . "winscw/c/keyP2.ss",						"temp/ssltest/general/testframework/keyP2.ss") or die "failed : $!";
copy($epoc . "winscw/c/reqP1.ss",						"temp/ssltest/general/testframework/reqP1.ss") or die "failed : $!";
copy($epoc . "winscw/c/reqP2.ss",						"temp/ssltest/general/testframework/reqP2.ss") or die "failed : $!";
copy($epoc . "winscw/c/tmp_intP2.ss",					"temp/ssltest/general/testframework/tmp_intP2.ss") or die "failed : $!";
copy($epoc . "winscw/c/Makefile",						"temp/ssltest/general/testframework/Makefile") or die "failed : $!";
copy($epoc . "winscw/c/ca_cert.der",					"temp/ssltest/general/testframework/ca_cert.der") or die "failed : $!";
copy($epoc . "winscw/c/ca_cert.pem",					"temp/ssltest/general/testframework/ca_cert.pem") or die "failed : $!";
copy($epoc . "winscw/c/server.pem",						"temp/ssltest/general/testframework/server.pem") or die "failed : $!";
copy($epoc . "winscw/c/server2.pem",					"temp/ssltest/general/testframework/server2.pem") or die "failed : $!";
copy($epoc . "release/winscw/udeb/crypto_test.dll",		"temp/ssltest/winscw_udeb/crypto_test.dll") or die "failed : $!";
copy($epoc . "release/winscw/udeb/openssl.exe",			"temp/ssltest/winscw_udeb/openssl.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ssltest.exe",			"temp/ssltest/winscw_udeb/ssltest.exe") or die "failed : $!";

system("7z a -tzip ssltest.zip ./temp/*");
