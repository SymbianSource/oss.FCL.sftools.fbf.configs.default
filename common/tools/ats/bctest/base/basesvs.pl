#!/usr/bin/perl

use strict;
use File::Copy;
use File::Path;

unlink "basesvs.zip";
rmtree "temp";
mkpath "temp/basesvs/base/general";
mkpath "temp/basesvs/winscw_udeb";
mkpath "temp/basesvs/base/t_sfsrv/general";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("basesvs.xml",                                                          			"temp/test.xml");

copy $epoc . "release/winscw/udeb/t_prompt.exe",                                       	"temp/basesvs/winscw_udeb/t_prompt.exe" or die "failed : $!";
copy $epoc . "release/winscw/udeb/t_sfsrv.exe",                                        	"temp/basesvs/winscw_udeb/t_sfsrv.exe" or die "failed : $!";
copy $epoc . "release/winscw/udeb/t_testfsy1.fsy",                                     	"temp/basesvs/winscw_udeb/t_testfsy1.fsy" or die "failed : $!";
copy $epoc . "release/winscw/udeb/t_testfxt.fxt",                                      	"temp/basesvs/winscw_udeb/t_testfxt.fxt" or die "failed : $!";

copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-any.script",    "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-any.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-files-publicapi-any.script",     "temp/basesvs/base/general/pbase-f32-rfs-files-publicapi-any.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-misc-publicapi-any.script",      "temp/basesvs/base/general/pbase-f32-rfs-misc-publicapi-any.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-mounts-publicapi-any.script",    "temp/basesvs/base/general/pbase-f32-rfs-mounts-publicapi-any.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-publicapi-os.script",            "temp/basesvs/base/general/pbase-f32-rfs-publicapi-os.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-ram.script",    "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-ram.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-files-publicapi-ram.script",     "temp/basesvs/base/general/pbase-f32-rfs-files-publicapi-ram.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-misc-publicapi-ram.script",      "temp/basesvs/base/general/pbase-f32-rfs-misc-publicapi-ram.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-mounts-publicapi-ram.script",    "temp/basesvs/base/general/pbase-f32-rfs-mounts-publicapi-ram.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-publicapi-rom.script",           "temp/basesvs/base/general/pbase-f32-rfs-publicapi-rom.script" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-rem.script",    "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-rem.script" or die "failed : $!";

copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-any.ini",       "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-any.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-files-publicapi-any.ini",        "temp/basesvs/base/general/pbase-f32-rfs-files-publicapi-any.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-misc-publicapi-any.ini",         "temp/basesvs/base/general/pbase-f32-rfs-misc-publicapi-any.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-mounts-publicapi-any.ini",       "temp/basesvs/base/general/pbase-f32-rfs-mounts-publicapi-any.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-publicapi-os.ini",               "temp/basesvs/base/general/pbase-f32-rfs-publicapi-os.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-ram.ini",       "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-ram.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-files-publicapi-ram.ini",        "temp/basesvs/base/general/pbase-f32-rfs-files-publicapi-ram.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-misc-publicapi-ram.ini",         "temp/basesvs/base/general/pbase-f32-rfs-misc-publicapi-ram.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-mounts-publicapi-ram.ini",       "temp/basesvs/base/general/pbase-f32-rfs-mounts-publicapi-ram.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-publicapi-rom.ini",              "temp/basesvs/base/general/pbase-f32-rfs-publicapi-rom.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/pbase-f32-rfs-drives-publicapi-rem.ini",       "temp/basesvs/base/general/pbase-f32-rfs-drives-publicapi-rem.ini" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/winscw/base_f32_env.ini",                      "temp/basesvs/base/general/base_f32_env.ini" or die "failed : $!";

copy $epoc . "release/winscw/udeb/z/base/t_sfsrv/test.txt",                            	"temp/basesvs/base/t_sfsrv/general/test.txt" or die "failed : $!";
copy $epoc . "release/winscw/udeb/z/base/t_sfsrv/usbload.zip",                         	"temp/basesvs/base/t_sfsrv/general/usbload.zip" or die "failed : $!";

system("7z a -tzip basesvs.zip ./temp/*");
