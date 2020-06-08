//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrinterList.h: interface for the CPrinterList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTERLIST_H__EB59F014_89CE_11D5_B284_0040055338AF__INCLUDED_)
#define AFX_PRINTERLIST_H__EB59F014_89CE_11D5_B284_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CPrinterList : public CList<CPrinter, CPrinter&>  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CPrinterList();
	virtual ~CPrinterList();
	// Get hash code of data, to determine if changed since last inventory
	LPCTSTR GetHash();
};
#endif // !defined(AFX_PRINTERLIST_H__EB59F014_89CE_11D5_B284_0040055338AF__INCLUDED_)

