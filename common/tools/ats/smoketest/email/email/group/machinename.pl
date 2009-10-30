# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

use Win32;

my $computer;
$computer=Win32::NodeName();
$machineName = lc($computer);

$fileName = "\\epoc32\\winscw\\c\\msgtest\\MachineName.txt";
open(FILE, "> " . $fileName);

# Write settings to output files
print FILE "$machineName";
close(FILE);
