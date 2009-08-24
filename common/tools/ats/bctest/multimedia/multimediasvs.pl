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
##rmtree "temp";
mkpath "temp/multimediatest/general/multimedia/t_imagedecoder";
mkpath "temp/multimediatest/general/multimedia/t_imageencoder";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudioconvertutility";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudiooutputstream";
mkpath "temp/multimediatest/general/multimedia/t_mdaaudioplayerutility";
mkpath "temp/multimediatest/general/multimedia/t_audiotoneutility";
mkpath "temp/multimediatest/general/multimedia/t_midiclientutility";
mkpath "temp/multimediatest/general/multimedia/t_videoplayerutility";
mkpath "temp/multimediatest/winscw_udeb/z/resource/plugins";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("multimediatest.xml",	"temp/test.xml");

copy($epoc . "data/z/resource/plugins/ecamtestplugin.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/ecamtestplugin.rsc");
copy($epoc . "release/winscw/udeb/ecamtestplugin.dll",		"temp/multimediatest/winscw_udeb/ecamtestplugin.dll");

copy($epoc . "data/z/resource/plugins/xvidencoderdevice.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/xvidencoderdevice.rsc");
copy($epoc . "data/z/resource/plugins/xviddecoderdevice.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/xviddecoderdevice.rsc");
copy($epoc . "release/winscw/udeb/xvidencoderdevice.dll",		"temp/multimediatest/winscw_udeb/xvidencoderdevice.dll");
copy($epoc . "release/winscw/udeb/xviddecoderdevice.dll",		"temp/multimediatest/winscw_udeb/xviddecoderdevice.dll");

copy($epoc . "release/winscw/udeb/testframeworkserver.exe",	"temp/multimediatest/winscw_udeb/testframeworkserver.exe");
copy($epoc . "release/winscw/udeb/testframeworkclient.dll",	"temp/multimediatest/winscw_udeb/testframeworkclient.dll");

copy($epoc . "data/z/resource/plugins/xvidhwdeviceplugins.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/xvidhwdeviceplugins.rsc");
copy($epoc . "release/winscw/udeb/xvidhwdeviceplugins.dll",		"temp/multimediatest/winscw_udeb/xvidhwdeviceplugins.dll");

copy($epoc . "data/z/resource/plugins/ts_miditstcntrl.rsc",	"temp/multimediatest/winscw_udeb/z/resource/plugins/ts_miditstcntrl.rsc");
copy($epoc . "release/winscw/udeb/ts_miditstcntrl.dll",		"temp/multimediatest/winscw_udeb/ts_miditstcntrl.dll");

copy($epoc . "data/z/multimedia/t_multimedia.tcs",			"temp/multimediatest/general/multimedia/t_multimedia.tcs");
copy($epoc . "data/z/multimedia/t_multimedia.ini",			"temp/multimediatest/general/multimedia/t_multimedia.ini");
copy($epoc . "data/z/multimedia/t_multimedia_location.ini",	"temp/multimediatest/general/multimedia/t_multimedia_location.ini");

copy($epoc . "data/z/multimedia/102070cc.txt",				"temp/multimediatest/general/102070CC.txt");
copy($epoc . "data/z/multimedia/mm-ecm-publicapi.script",	"temp/multimediatest/general/multimedia/mm-ecm-publicapi.script");
copy($epoc . "data/z/multimedia/mm-ecm-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-ecm-publicapi.ini");

copy($epoc . "data/z/multimedia/mm-icl-decde-publicapi.script",	"temp/multimediatest/general/multimedia/mm-icl-decde-publicapi.script");
copy($epoc . "data/z/multimedia/mm-icl-decde-publicapi.ini",	"temp/multimediatest/general/multimedia/mm-icl-decde-publicapi.ini");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.apm",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.apm");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.bmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/4bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.bmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.bmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.bmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.bmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.clp",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.clp");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/2bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/2bit.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/4bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/6bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/6bit.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.gif",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/commented.gif",	"temp/multimediatest/general/multimedia/t_imagedecoder/commented.gif");
copy($epoc . "data/z/multimedia/t_imagedecoder/4bit.ico",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.ico");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.ico",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.ico");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.jpg",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.jpg");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.jpg",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.jpg");
copy($epoc . "data/z/multimedia/t_imagedecoder/thumbnail.jpg",	"temp/multimediatest/general/multimedia/t_imagedecoder/thumbnail.jpg");
copy($epoc . "data/z/multimedia/t_imagedecoder/commented.jpg",	"temp/multimediatest/general/multimedia/t_imagedecoder/commented.jpg");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec1bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec1bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec2bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec2bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec4bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec4bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec8bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec8bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec16bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec16bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/dec24bit.mbm",	"temp/multimediatest/general/multimedia/t_imagedecoder/dec24bit.mbm");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.mng",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.mng");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.ota",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.ota");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.png");
copy($epoc . "data/z/multimedia/t_imagedecoder/4bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.png");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.png");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.png",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.png");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bitg3.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bitg3.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bitg4.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bitg4.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/4bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/4bit.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/8bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/8bit.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.tif",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.tif");
copy($epoc . "data/z/multimedia/t_imagedecoder/1bit.wbmp",		"temp/multimediatest/general/multimedia/t_imagedecoder/1bit.wbmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/24bit.wmf",		"temp/multimediatest/general/multimedia/t_imagedecoder/24bit.wmf");
copy($epoc . "data/z/multimedia/t_imagedecoder/corrupted.bmp",	"temp/multimediatest/general/multimedia/t_imagedecoder/corrupted.bmp");
copy($epoc . "data/z/multimedia/t_imagedecoder/partial.bmp",	"temp/multimediatest/general/multimedia/t_imagedecoder/partial.bmp");

copy($epoc . "data/z/multimedia/mm-icl-encde-publicapi.script",	"temp/multimediatest/general/multimedia/mm-icl-encde-publicapi.script");
copy($epoc . "data/z/multimedia/mm-icl-encde-publicapi.ini",	"temp/multimediatest/general/multimedia/mm-icl-encde-publicapi.ini");
copy($epoc . "data/z/multimedia/t_imageencoder/enc1bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc1bit.mbm");
copy($epoc . "data/z/multimedia/t_imageencoder/enc2bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc2bit.mbm");
copy($epoc . "data/z/multimedia/t_imageencoder/enc4bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc4bit.mbm");
copy($epoc . "data/z/multimedia/t_imageencoder/enc8bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc8bit.mbm");
copy($epoc . "data/z/multimedia/t_imageencoder/enc16bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc16bit.mbm");
copy($epoc . "data/z/multimedia/t_imageencoder/enc24bit.mbm",	"temp/multimediatest/general/multimedia/t_imageencoder/enc24bit.mbm");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-cnvrt-publicapi.script",					"temp/multimediatest/general/multimedia/mm-mmf-aclnt-cnvrt-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-cnvrt-publicapi.ini",						"temp/multimediatest/general/multimedia/mm-mmf-aclnt-cnvrt-publicapi.ini");
copy($epoc . "data/z/multimedia/t_mdaaudioconvertutility/note.wav",						"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/note.wav");
copy($epoc . "data/z/multimedia/t_mdaaudioconvertutility/note.raw",						"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/note.raw");
copy($epoc . "data/z/multimedia/t_mdaaudioconvertutility/corrupted.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/corrupted.wav");
copy($epoc . "release/winscw/udeb/z/multimedia/t_mdaaudioconvertutility/explode.wav",	"temp/multimediatest/general/multimedia/t_mdaaudioconvertutility/explode.wav");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-inpt-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-aclnt-inpt-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-inpt-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-aclnt-inpt-publicapi.ini");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-outpt-publicapi.script",						"temp/multimediatest/general/multimedia/mm-mmf-aclnt-outpt-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-outpt-publicapi.ini",							"temp/multimediatest/general/multimedia/mm-mmf-aclnt-outpt-publicapi.ini");
copy($epoc . "data/z/multimedia/t_mdaaudiooutputstream/mm-mmf-aclnt-outpt-publicapi.raw",	"temp/multimediatest/general/multimedia/t_mdaaudiooutputstream/mm-mmf-aclnt-outpt-publicapi.raw");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-plyr-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-aclnt-plyr-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-plyr-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-aclnt-plyr-publicapi.ini");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/test.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test.wav");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/test.bad",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test.bad");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/test_err.wav",			"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/test_err.wav");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/empty.wav",				"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/empty.wav");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/answeringmachine.wav",	"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/answeringmachine.wav");
copy($epoc . "data/z/multimedia/t_mdaaudioplayerutility/explode.au",			"temp/multimediatest/general/multimedia/t_mdaaudioplayerutility/explode.au");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-rcrdr-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-aclnt-rcrdr-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-rcrdr-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-aclnt-rcrdr-publicapi.ini");
copy($epoc . "data/z/multimedia/t_mdaaudiorecorderutility/pcm16stereo8khz.wav",	"temp/multimediatest/general/multimedia/t_mdaaudiorecorderutility/pcm16stereo8khz.wav");
copy($epoc . "data/z/multimedia/t_mdaaudiorecorderutility/corrupted.wav",		"temp/multimediatest/general/multimedia/t_mdaaudiorecorderutility/corrupted.wav");

copy($epoc . "data/z/multimedia/mm-mmf-aclnt-tone-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-aclnt-tone-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-aclnt-tone-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-aclnt-tone-publicapi.ini");
copy($epoc . "data/z/multimedia/t_audiotoneutility/sequence1.sqn",		"temp/multimediatest/general/multimedia/t_audiotoneutility/sequence1.sqn");
copy($epoc . "data/z/multimedia/t_audiotoneutility/sequence2.sqn",		"temp/multimediatest/general/multimedia/t_audiotoneutility/sequence2.sqn");
copy($epoc . "data/z/multimedia/t_audiotoneutility/badsequence.sqn",	"temp/multimediatest/general/multimedia/t_audiotoneutility/badsequence.sqn");

copy($epoc . "data/z/multimedia/mm-mmf-midi-publicapi.script",			"temp/multimediatest/general/multimedia/mm-mmf-midi-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-midi-publicapi.ini",				"temp/multimediatest/general/multimedia/mm-mmf-midi-publicapi.ini");
copy($epoc . "data/z/multimedia/t_midiclientutility/midi.mid",			"temp/multimediatest/general/multimedia/t_midiclientutility/midi.mid");
copy($epoc . "data/z/multimedia/t_midiclientutility/textmidi.mid",		"temp/multimediatest/general/multimedia/t_midiclientutility/textmidi.mid");
copy($epoc . "data/z/multimedia/t_midiclientutility/midiclienttest.gm",	"temp/multimediatest/general/multimedia/t_midiclientutility/midiclienttest.gm");

copy($epoc . "data/z/multimedia/mm-mmf-vclnt-plyr-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-vclnt-plyr-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-vclnt-plyr-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-vclnt-plyr-publicapi.ini");
copy($epoc . "data/z/multimedia/t_videoplayerutility/alter.mp4",		"temp/multimediatest/general/multimedia/t_videoplayerutility/alter.mp4");
copy($epoc . "data/z/multimedia/t_videoplayerutility/xvid_clock.avi",	"temp/multimediatest/general/multimedia/t_videoplayerutility/xvid_clock.avi");
copy($epoc . "data/z/multimedia/t_videoplayerutility/corrupted.avi",	"temp/multimediatest/general/multimedia/t_videoplayerutility/corrupted.avi");

copy($epoc . "data/z/multimedia/mm-mmf-vclnt-rcrdr-publicapi.script",	"temp/multimediatest/general/multimedia/mm-mmf-vclnt-rcrdr-publicapi.script");
copy($epoc . "data/z/multimedia/mm-mmf-vclnt-rcrdr-publicapi.ini",		"temp/multimediatest/general/multimedia/mm-mmf-vclnt-rcrdr-publicapi.ini");

copy($epoc . "release/winscw/udeb/t_camera.exe",					"temp/multimediatest/winscw_udeb/t_camera.exe");
copy($epoc . "release/winscw/udeb/t_imagedecoder.exe",				"temp/multimediatest/winscw_udeb/t_imagedecoder.exe");
copy($epoc . "release/winscw/udeb/t_imageencoder.exe",				"temp/multimediatest/winscw_udeb/t_imageencoder.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudioconvertutility.exe",	"temp/multimediatest/winscw_udeb/t_mdaaudioconvertutility.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudioinputstream.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudioinputstream.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudiooutputstream.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudiooutputstream.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudioplayerutility.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudioplayerutility.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudiorecorderutility.exe",	"temp/multimediatest/winscw_udeb/t_mdaaudiorecorderutility.exe");
copy($epoc . "release/winscw/udeb/t_mdaaudiotoneutility.exe",		"temp/multimediatest/winscw_udeb/t_mdaaudiotoneutility.exe");
copy($epoc . "release/winscw/udeb/t_midiclientutility.exe",			"temp/multimediatest/winscw_udeb/t_midiclientutility.exe");
copy($epoc . "release/winscw/udeb/t_videoplayerutility.exe",		"temp/multimediatest/winscw_udeb/t_videoplayerutility.exe");
copy($epoc . "release/winscw/udeb/t_videorecorderutility.exe",		"temp/multimediatest/winscw_udeb/t_videorecorderutility.exe");

system("7z a -tzip multimediatest.zip ./temp/*");
