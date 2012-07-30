//==============================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence Version 2.0
// http://www.gnu.org/ or Licence.txt
//==============================================================================


REQUIREMENTS
============

- NSIS 2.46 or higher (http://nsis.sourceforge.net))
- Services plugin (http://nsis.sourceforge.net/Services_plug-in)
- KillProcDLL plug-in (http://nsis.sourceforge.net/KillProcDLL_plug-in)
- SetACL 2.0.3 or higher (http://setacl.sourceforge.net/)
- NSIS Simple Firewall Plugin 1.19 or higher (http://nsis.sourceforge.net/NSIS_Simple_Firewall_Plugin)

Put SetACL.exe into OCS-NG_Windows_Agent_Setup.nsi script directory, and 
compile OCS-NG_Windows_Agent_Setup.nsi (Script assume that agent's binary
are on directory "../release", and that MS VC++ 2008 is installed into
default folder "C:\Program Files\Microsoft Visual Studio 9.0\VC").

It's quite difficult to build libcurl for target Windows 2000 using MS Visual
C++ 2008 or higher. It needs to modify code and Microsoft Windows SDK 6.X,
shipped with Visual Studio 2008, does not mention Windows 2000 as a valid build
target, only Windows XP and higher.  

So, for Windows 2000 target only, we prefer using a precompiled binary distribution
from Günter Knauf, built using MingW32 compiler, and available at location
http://www.gknw.net/mirror/curl/win32/curl-7.21.3-ssl-sspi-zlib-static-bin-w32.zip
Just download this package and unzip it into nsis_agent_setup folder.

MS11-025 critical fix introduces incompatibilty problem of MS CRT/MFC 9.0 with
Windows 2000 (http://blogs.msdn.com/b/vcblog/archive/2011/04/26/10158277.aspx).
So for Windows 2000 only, w've choosen to install original MS CRT/MFC 
9.00.21022.8 provided with Visual C++ 2008 without Service Pack or hot fixes.
You must put these files into "vc2008_redist_9.00.21022.8_for_Windows_2000"
sub directory before building NSIS installer.


INSTALLER COMMAND LINE OPTIONS
==============================

- /S to run the installer in silent mode, no interraction with user
 
- /NOSPLASH disable spash screen when installer starts

- /UPGRADE to deploy new agent through OCS deployement feature.
  Because you upgrade agent using agent itself to run the upgrade, it is needed
  to notify the installer (which terminates agent's processes) to specify the
  result in order to send it back to the server next time agent will run

- /NO_SERVICE to not register OCS Inventory NG Agent into Windows Service
  Manager. Agent can onlybe launched manually or through a script/GPO.

- /NO_SYSTRAY to not create a shortcut into "All Users" startup folder to start
  systray applet when user log in.

- /NOW to launch inventory just after setup

Also, you can pass to the installer the following agent's command line
parameters (see agent's options for more detail):
- /SERVER=
- /USER=
- /PWD=
- /SSL=
- /CA=
- /PROXY_TYPE=
- /PROXY=
- /PROXY_PORT=
- /PROXY_USER=
- /PROXY_PWD=
- /DEBUG=
- /NOTAG
- /TAG=
- /NOSOFTWARE


We hope it will works for you !

OCS Inventory Team