//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Deviceid.h: interface for the CDeviceid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEID_H__5FCEBB1F_F439_4273_9548_DB243A0B9D50__INCLUDED_)
#define AFX_DEVICEID_H__5FCEBB1F_F439_4273_9548_DB243A0B9D50__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "Log.h"
#include "Singleton.h"
#include "Zip.h"
#include "sysinfo.h"

#define DEVICEID_DAT_FILE			_T( "ocsinventory.dat")

class OCSINVENTORYFRONT_API CDeviceid : public CSingleton<CDeviceid>
{
	friend class CSingleton<CDeviceid>;

public: // Methods

	// Standard destructor
	virtual ~CDeviceid();
	// Returns the deviceid
	CString getDeviceID();
	// Returns the computer name
	CString getComputerName();
	// Write the current to .dat
	BOOL writeDeviceid(); 
	// Returns the oldDeviceid if exists
	CString getOldDeviceID();
	// Check the current deviceid
	void checkDeviceid();

private: // Methods

	// The constructor
	CDeviceid();

private: // Attributes

	// The definitive deviceid for the whole session
	CString m_csDeviceid;
	// The old deviceid if exists
	CString m_csOldDeviceid;
	// The computer's mac addresses
	CString m_csMacConcat;
	// The hostname
	CString m_csHostName;

private: // Methods
	// Retrieve the deviceid-macs from the .dat file
	BOOL loadDeviceid(CString &, CString &);
	// Retrieve the system MAC addresses
	CString getMacs();
	// Generate a deviceid
	CString generate( LPCTSTR );
};

#endif // !defined(AFX_DEVICEID_H__5FCEBB1F_F439_4273_9548_DB243A0B9D50__INCLUDED_)
