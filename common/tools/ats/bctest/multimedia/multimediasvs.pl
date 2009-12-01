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
# Louis Henry Nayegon <louisn@symbian.org>
#
# Description:
# Script to build ATS test drop multimedia svs tests



use strict;
use File::Copy;
use File::Path;

unlink "multimediatest.zip";
rmtree "temp";

mkpath "temp/multimediatest/general/multimedia/t_imagedecoder";
mkpath "temp/multimediatest/general/multimedia/t_imageencoder";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudioconvertutility";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudiooutputstream";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudioplayerutility";
mkpath "temp/multimediatest/general/multimedia/t_audiotoneutility";
mkpath "temp/multimediatest/general/multimedia/t_midiclientutility";
mkpath "temp/multimediatest/general/multimedia/t_videoplayerutility";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudiorecorderutility";
mkpath "temp/multimediatest/winscw_udeb/z/resource/plugins";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("multimediatest.xml",	"temp/test.xml");

copy($epoc . "data/z/resource/plugins/ecamtestplugin.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/ecamtestplugin.rsc") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ecamtestplugin.dll",		"temp/multimediatest/winscw_udeb/ecamtestplugin.dll") or die "failed : $!";

copy($epoc . "release/winscw/udeb/testframeworkserver.exe",	"temp/multimediatest/winscw_udeb/testframeworkserver.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/testframeworkclient.dll",	"temp/multimediatest/winscw_udeb/testframeworkclient.dll") or die "failed : $!";

copy($epoc . "data/z/resource/plugins/ts_miditstcntrl.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/ts_miditstcntrl.rsc") or die "failed : $!";
copy($epoc . "release/winscw/udeb/ts_miditstcntrl.dll",		"temp/multimediatest/winscw_udeb/ts_miditstcntrl.dll") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/t_multimedia.tcs",			"temp/multimediatest/general/multimedia/t_multimedia.tcs") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_multimedia.ini",			"temp/multimediatest/general/multimedia/t_multimedia.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_multimedia_location.ini",	"temp/multimediatest/general/multimedia/t_multimedia_location.ini") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/102070cc.txt",				"temp/multimediatest/general/102070CC.txt") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-ecm-publicapi.script",	"temp/multimediatest/general/multimedia/mm-ecm-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-ecm-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-ecm-publicapi.ini") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-icl-decde-publicapi.script",	"temp/multimediatest/general/multimedia/mm-icl-decde-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-icl-decde-publicapi.ini",	"temp/multimediatest/general/multimedia/mm-icl-decde-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.apm",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.apm") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.bmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/4bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.bmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.bmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.bmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.clp",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.clp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/2bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/2bit.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/4bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/6bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/6bit.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/commented.gif",	"temp/multimediatest/general/multimedia/t_imagedecoder/commented.gif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/4bit.ico",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.ico") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.ico",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.ico") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.jpg",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.jpg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.jpg",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.jpg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/thumbnail.jpg",	"temp/multimediatest/general/multimedia/t_imagedecoder/thumbnail.jpg") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/commented.jpg",	"temp/multimediatest/general/multimedia/t_imagedecoder/commented.jpg") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec1bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec1bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec2bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec2bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec4bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec4bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec8bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec8bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec16bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec16bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imagedecoder/dec24bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec24bit.mbm") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.mng",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.mng") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.ota",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.ota") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.png") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/4bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.png") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.png") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.png") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bitg3.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bitg3.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bitg4.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bitg4.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/4bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/8bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.tif") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/1bit.wbmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.wbmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/24bit.wmf",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.wmf") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/corrupted.bmp",	"temp/multimediatest/general/multimedia/t_imagedecoder/corrupted.bmp") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_imagedecoder/partial.bmp",	"temp/multimediatest/general/multimedia/t_imagedecoder/partial.bmp") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-icl-encde-publicapi.script",	"temp/multimediatest/general/multimedia/mm-icl-encde-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-icl-encde-publicapi.ini",	"temp/multimediatest/general/multimedia/mm-icl-encde-publicapi.ini") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc1bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc1bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc2bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc2bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc4bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc4bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc8bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc8bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc16bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc16bit.mbm") or die "failed : $!";
copy($epoc . "data/z/multimedia/t_imageencoder/enc24bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc24bit.mbm") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-cnvrt-publicapi.script",					"temp/multimediatest/general/multimedia/mm-mmf-aclnt-cnvrt-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-cnvrt-publicapi.ini",						"temp/multimediatest/general/multimedia/mm-mmf-aclnt-cnvrt-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioconvertutility/note.wav",						"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/note.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioconvertutility/note.raw",						"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/note.raw") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioconvertutility/corrupted.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/corrupted.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioconvertutility/explode.wav",	"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/explode.wav") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-inpt-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-aclnt-inpt-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-inpt-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-aclnt-inpt-publicapi.ini") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-outpt-publicapi.script",						"temp/multimediatest/general/multimedia/mm-mmf-aclnt-outpt-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-outpt-publicapi.ini",							"temp/multimediatest/general/multimedia/mm-mmf-aclnt-outpt-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudiooutputstream/mm-mmf-aclnt-outpt-publicapi.raw",	"temp/multimediatest/general/multimedia/t_mdaaudiooutputstream/mm-mmf-aclnt-outpt-publicapi.raw") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-plyr-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-aclnt-plyr-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-plyr-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-aclnt-plyr-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/test.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/test.bad",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test.bad") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/test_err.wav",			"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test_err.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/empty.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/empty.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/answeringmachine.wav",	"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/answeringmachine.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioplayerutility/explode.au",			"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/explode.au") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-rcrdr-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-aclnt-rcrdr-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-rcrdr-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-aclnt-rcrdr-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudiorecorderutility/pcm16stereo8khz.wav",	"temp/multimediatest/general/multimedia/t_mdaaudiorecorderutility/pcm16stereo8khz.wav") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudiorecorderutility/corrupted.wav",		"temp/multimediatest/general/multimedia/t_mdaaudiorecorderutility/corrupted.wav") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-tone-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-aclnt-tone-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-aclnt-tone-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-aclnt-tone-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_audiotoneutility/sequence1.sqn",		"temp/multimediatest/general/multimedia/t_audiotoneutility/sequence1.sqn") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_audiotoneutility/sequence2.sqn",		"temp/multimediatest/general/multimedia/t_audiotoneutility/sequence2.sqn") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_audiotoneutility/badsequence.sqn",	"temp/multimediatest/general/multimedia/t_audiotoneutility/badsequence.sqn") or die "failed : $!";

copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-midi-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-midi-publicapi.script") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/mm-mmf-midi-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-midi-publicapi.ini") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_midiclientutility/midi.mid",			"temp/multimediatest/general/multimedia/t_midiclientutility/midi.mid") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_midiclientutility/textmidi.mid",		"temp/multimediatest/general/multimedia/t_midiclientutility/textmidi.mid") or die "failed : $!";
copy($epoc . "release/winscw/udeb/z/multimedia/t_midiclientutility/midiclienttest.gm",	"temp/multimediatest/general/multimedia/t_midiclientutility/sample.gm") or die "failed : $!";

copy($epoc . "release/winscw/udeb/t_camera.exe",					"temp/multimediatest/winscw_udeb/t_camera.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_imagedecoder.exe",				"temp/multimediatest/winscw_udeb/t_imagedecoder.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_imageencoder.exe",				"temp/multimediatest/winscw_udeb/t_imageencoder.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudioconvertutility.exe",	"temp/multimediatest/winscw_udeb/t_mdaaudioconvertutility.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudioinputstream.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudioinputstream.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudiooutputstream.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudiooutputstream.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudioplayerutility.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudioplayerutility.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudiorecorderutility.exe",	"temp/multimediatest/winscw_udeb/t_mdaaudiorecorderutility.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_mdaaudiotoneutility.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudiotoneutility.exe") or die "failed : $!";
copy($epoc . "release/winscw/udeb/t_midiclientutility.exe",			"temp/multimediatest/winscw_udeb/t_midiclientutility.exe") or die "failed : $!";

system("7z a -tzip multimediatest.zip ./temp/*");
