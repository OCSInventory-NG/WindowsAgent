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


We hope it will works for you !

OCS Inventory Team