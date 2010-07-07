//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SoundDevice.h: interface for the CSoundDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDDEVICE_H__ACFCECE1_8559_4161_8305_B8E69BF21C25__INCLUDED_)
#define AFX_SOUNDDEVICE_H__ACFCECE1_8559_4161_8305_B8E69BF21C25__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CSoundDevice  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSoundDevice();
	virtual ~CSoundDevice();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetName();
	LPCTSTR GetManufacturer();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetDescription( LPCTSTR lpstrDescription);
	void SetName( LPCTSTR lpstrName);
	void SetManufacturer( LPCTSTR lpstrManufacturer);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CSoundDevice cObject) const;

protected: // Attributes
	CString m_csManufacturer;	// Sound Device Manufacturer
	CString m_csName;			// Sound Device name
	CString m_csDescription;	// Sound Device description
};
#endif // !defined(AFX_SOUNDDEVICE_H__ACFCECE1_8559_4161_8305_B8E69BF21C25__INCLUDED_)

