//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ServerConfig.h: interface for the CServerConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_)
#define AFX_SERVERCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_

#pragma once

class CServerConfig;

#include "OCSInventory Front Classes.h"

class OCSINVENTORYFRONT_API CServerConfig
{
public: // Methods
	// The constructor
	CServerConfig();
	// Standard destructor
	virtual ~CServerConfig();

	// Reset default values
	virtual void Clear() = NULL;

	// Allow parsing agent command line to grab command line params for
	// Communication provider
	virtual BOOL parseCommandLine( LPCTSTR lpstrCommand) = NULL;
};

#endif // !defined(AFX_SERVERCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_)
