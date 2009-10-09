#!/usr/bin/perl

use strict;
use File::Copy;
use File::Path;

unlink "graphicssvs.zip";
rmtree "temp";
mkpath "temp/graphicssvs/graphics/general";
mkpath "temp/graphicssvs/graphics/winscw/general";
mkpath "temp/graphicssvs/winscw_udeb";
mkpath "temp/graphicssvs/graphics/t_fbservapi/general";

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("graphicssvs.xml",                                                          			"temp/test.xml");

copy $epoc . "release/winscw/udeb/t_fbservapi.exe",                                            "temp/graphicssvs/winscw_udeb/t_fbservapi.exe" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_graphics.ini",                                               "temp/graphicssvs/graphics/general/t_graphics.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/winscw/t_graphics.ini",                                        "temp/graphicssvs/graphics/winscw/general/t_graphics.ini" or die "failed : $!";

copy $epoc . "winscw/c/graphics/graphics-fbserv-publicapi.tcs",                                 "temp/graphicssvs/graphics/general/graphics-fbserv-publicapi.tcs" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-publicapi.script",                  		   "temp/graphicssvs/graphics/general/graphics-fbserv-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-bitmaputil-publicapi.script",                  "temp/graphicssvs/graphics/general/graphics-fbserv-bitmaputil-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbsbitmap-publicapi.script",                   "temp/graphicssvs/graphics/general/graphics-fbserv-fbsbitmap-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbscolor256bitmaputil-publicapi.script",       "temp/graphicssvs/graphics/general/graphics-fbserv-fbscolor256bitmaputil-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbsfont-publicapi.script",                     "temp/graphicssvs/graphics/general/graphics-fbserv-fbsfont-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbssession-publicapi.script",                  "temp/graphicssvs/graphics/general/graphics-fbserv-fbssession-publicapi.script" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbstypefacestore-publicapi.script",            "temp/graphicssvs/graphics/general/graphics-fbserv-fbstypefacestore-publicapi.script" or die "failed : $!";

copy $epoc . "winscw/c/graphics/graphics-fbserv-bitmaputil-publicapi.ini",                     "temp/graphicssvs/graphics/general/graphics-fbserv-bitmaputil-publicapi.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbsbitmap-publicapi.ini",                      "temp/graphicssvs/graphics/general/graphics-fbserv-fbsbitmap-publicapi.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbscolor256bitmaputil-publicapi.ini",          "temp/graphicssvs/graphics/general/graphics-fbserv-fbscolor256bitmaputil-publicapi.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbsfont-publicapi.ini",                        "temp/graphicssvs/graphics/general/graphics-fbserv-fbsfont-publicapi.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbssession-publicapi.ini",                     "temp/graphicssvs/graphics/general/graphics-fbserv-fbssession-publicapi.ini" or die "failed : $!";
copy $epoc . "winscw/c/graphics/graphics-fbserv-fbstypefacestore-publicapi.ini",            	"temp/graphicssvs/graphics/general/graphics-fbserv-fbstypefacestore-publicapi.ini" or die "failed : $!";

copy $epoc . "winscw/c/graphics/t_fbservapi/empty_file.txt",                                   "temp/graphicssvs/graphics/t_fbservapi/general/empty_file.txt" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/scaniline_10bytes.txt",                            "temp/graphicssvs/graphics/t_fbservapi/general/scaniline_10bytes.txt" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/testfontfb.gdr",                                   "temp/graphicssvs/graphics/t_fbservapi/general/testfontfb.gdr" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/test_bitmap.mbm",                                  "temp/graphicssvs/graphics/t_fbservapi/general/test_bitmap.mbm" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/test_bitmap_24bit_400x300.mbm",                    "temp/graphicssvs/graphics/t_fbservapi/general/test_bitmap_24bit_400x300.mbm" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/test_bitmap_32x8.mbm",                             "temp/graphicssvs/graphics/t_fbservapi/general/test_bitmap_32x8.mbm" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/test_bitmap_offset8.mbm",                          "temp/graphicssvs/graphics/t_fbservapi/general/test_bitmap_offset8.mbm" or die "failed : $!";
copy $epoc . "winscw/c/graphics/t_fbservapi/test_bitmap_rom.rbm",                              "temp/graphicssvs/graphics/t_fbservapi/general/test_bitmap_rom.rbm" or die "failed : $!";

system("7z a -tzip graphicssvs.zip ./temp/*");
