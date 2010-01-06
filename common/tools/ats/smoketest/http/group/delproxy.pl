#!/usr/bin/perl

# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#   Maciej Seroka, maciej@symbian.org
#
# Description:
#   This is a tool for removing proxy settings from ethernetced.xml.

use Tie::File;

my $filename;
if ($ARGV[0]) {
  $filename = $ARGV[0];
 }
else { die 'Missing parameter "filename"'; }

my $LINE;
print $filename;
tie @LINE, 'Tie::File', $filename or die 'file not found';
my $n = 0;
my $proxy_begin = 0;
my $proxy_end = 0;

for (@LINE) {
  if ($LINE[$n] =~ /<ProxyTable>/) {
     $proxy_begin = $n;
   }
  if ($LINE[$n] =~ /<\/ProxyTable>/) {
     $proxy_end = $n;
     last;
   }
  $n += 1;
 }
 
if ($proxy_end > 0) {
  splice @LINE, $proxy_begin, ($proxy_end - $proxy_begin + 1);
 }

untie @LINE;