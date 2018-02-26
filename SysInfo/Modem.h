//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Modem.h: interface for the CModem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEM_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_)
#define AFX_MODEM_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CModem  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CModem();
	virtual ~CModem();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetModel();
	LPCTSTR GetName();
	LPCTSTR GetType();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetDescription( LPCTSTR lpstrDescription);
	void SetModel( LPCTSTR lpstrModel);
	void SetName( LPCTSTR lpstrName);
	void SetType( LPCTSTR lpstrType);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CModem cObject) const;

protected: // Attributes
	CString m_csType;		// Modem type (external, internal...)
	CString m_csName;		// Modem name
	CString m_csModel;		// Model
	CString m_csDescription;// Description
};
#endif // !defined(AFX_MODEM_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_)

