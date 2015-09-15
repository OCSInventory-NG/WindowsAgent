//==============================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence Version 2.0
// http://www.gnu.org/ or Licence.txt
//==============================================================================

OcsLogon is a tool which allow to install/upgrade or run OCS Inventory NG Agent
for Windows through a logon or GPO script.

It will ensure that Agent is installed, and install/upgrade it if needed by
downloading installation package for OCS Inventory NG Server.

By default, OcsLogon try to download and install standard Agent setup file 
"ocsagent.exe", but it can also use "ocspackage.exe", file created by OCS
Inventory NG Packager, which can embbed certificate file and can be executed
under a fixed administrator account. This is useful when user logon script
is used in Samba domain, to allow installation process by non administrator
users.       

If Agent is already up to date and service is not used, it will also start Agent
to begin inventory process.

 
REQUIREMENTS
============

- NSIS 2.46 or higher (http://nsis.sourceforge.net))
- Inetc plugin (http://nsis.sourceforge.net/Inetc_plug-in), at least 2011/04/28
  release


OCSLOGON COMMAND LINE OPTIONS
==============================

- /PACKAGER download "ocspackage.exe", created with OCS Inventory NG Packager
  Tool. 

- /GPO tell OcsLogon that setup file to download is in the same folder as 
  ocslogon.exe. So, OcsLogon does not try to download setup file from server, 
  but try to use the one copied in the GPO folder.
  
  - /DEPLOY=a.b.c.d ensure there is at least the specified version installed. 
  OcsLogon install/upgrade Agent only if installed version is lower than the 
  specified one "a.b.c.d".

- /FORCE force reinstalling agent, even if the installed version is newer or 
  equal to the specified one. Use it to downgrade version for example.

- /URL=http[s]://your_server:port/path_to_file_to_download.exe

- /TIMEOUT=x (milliseconds) allow to modify default download timeout of 
  60 seconds
  
- /UNINSTALL uninstall agent, instead of installing it.

You must also provide standard agent setup command line parameters (see agent's 
installer options for more detail), and at least:

- /SERVER=http[s]://you_cs_server[:port]/ocsinventory, URL to connect to
  OCS Inventory NG Server.
   
- /NO_SERVICE to not register OCS Inventory NG Agent into Windows Service
  Manager. OcsLogon will then launch Agent each time. Otherwise, it will never
  run Agent, assuming Agent is launched by Windows Service.

We hope it will works for you !

OCS Inventory Team
