//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemSlot.h: interface for the CSystemSlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMSLOT_H__C9C1C41A_5FCD_412A_AD56_6BAC3EA69E30__INCLUDED_)
#define AFX_SYSTEMSLOT_H__C9C1C41A_5FCD_412A_AD56_6BAC3EA69E30__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class SYSINFO_API CSystemSlot  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSystemSlot();
	virtual ~CSystemSlot();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetName();
	LPCTSTR GetDescription();
	LPCTSTR GetSlotDesignation();
	LPCTSTR GetUsage();
	LPCTSTR GetStatus();
	BOOL IsShared();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetName( LPCTSTR lpstrName);
	void SetDescription( LPCTSTR lpstrDescription);
	void SetType( DWORD dwType);
	void SetBusWidth( DWORD dwWidth);
	void SetSlotDesignation( LPCTSTR lpstrDesignation);
	void SetUsage( LPCTSTR lpstrUsage);
	void SetUsage( DWORD dwUsage);
	void SetStatus( LPCTSTR lpstrStatus);
	void SetShared( BOOL bShared);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CSystemSlot cObject) const;

protected: // Attributes
	CString m_csName;		// Slot name
	CString m_csDescription;// Slot description
	CString m_csDesignation;// Slot designation (ISA1, PCI1, PCI2, AGP1...)
	CString m_csUsage;		// Usage (available, in use...)
	CString m_csStatus;		// Status (OK, failure...)
	BOOL	m_bShared;		// Shared with another slot (example ISA and PCI)
};
#endif // !defined(AFX_SYSTEMSLOT_H__C9C1C41A_5FCD_412A_AD56_6BAC3EA69E30__INCLUDED_)

