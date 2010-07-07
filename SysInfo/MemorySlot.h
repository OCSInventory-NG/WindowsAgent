//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// MemorySlot.h: interface for the CMemorySlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYSLOT_H__B07E0FAA_CE82_4915_B2B4_5E622AA26582__INCLUDED_)
#define AFX_MEMORYSLOT_H__B07E0FAA_CE82_4915_B2B4_5E622AA26582__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CMemorySlot  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CMemorySlot();
	virtual ~CMemorySlot();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetCaption();
	LPCTSTR GetDescription();
	// Get memory max capacity in KB
	LPCTSTR GetCapacity();
	// Get memory usage (system, video, flash...)
	LPCTSTR GetUsage();
	// Get memory speed in MHz
	LPCTSTR GetSpeed();
	// Get memory type (EDO, SDRAM, DDR...)
	LPCTSTR GetType();
	// Get slot number in the memory block
	UINT	GetSlotNumber();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetCaption( LPCTSTR lpstrCaption);
	void SetDescription( LPCTSTR lpstrDescription);
	void SetCapacity( LPCTSTR lpstrCapacity);
	void SetUsage( LPCTSTR lpstrUsage);
	void SetUsage( DWORD dwUsage);
	void SetSpeed( LPCTSTR lpstrSpeed);
	void SetType( LPCTSTR lpstrType);
	void SetType( DWORD dwType);
	void SetTypeECC( DWORD dwECC);
	void SetSlotNumber( UINT uNumber);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CMemorySlot cObject) const;

protected: // Attributes
	CString m_csCaption;	// Caption
	CString m_csDescription;// Description
	CString m_csCapacity;	// Max capacity in KB
	CString m_csType;		// Memory type (EDO, SDRAM, DDR...)
	CString m_csSpeed;		// Memory access speed in MHz
	CString	m_csUsage;		// Memory usage (system, video, flash...)
	UINT	m_uSlotNumber;	// Memory slot number
};
#endif // !defined(AFX_MEMORYSLOT_H__B07E0FAA_CE82_4915_B2B4_5E622AA26582__INCLUDED_)

