//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// SystemController.h: interface for the CSystemController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEMCONTROLLER_H__C2B544BB_1F51_4D7F_8978_17AF12FF645B__INCLUDED_)
#define AFX_SYSTEMCONTROLLER_H__C2B544BB_1F51_4D7F_8978_17AF12FF645B__INCLUDED_

#pragma once

#define SYSTEM_CONTROLER_FLOPPY		_T( "Floppy Controller")
#define SYSTEM_CONTROLER_IDE		_T( "IDE Controller")
#define SYSTEM_CONTROLER_SCSI		_T( "SCSI Controller")
#define SYSTEM_CONTROLER_INFRARED	_T( "Infrared Controller")
#define SYSTEM_CONTROLER_USB		_T( "USB Controller")
#define SYSTEM_CONTROLER_IEEE1394	_T( "IEEE1394 Controller")
#define SYSTEM_CONTROLER_PCMCIA		_T( "PCMCIA Controller")
#define SYSTEM_CONTROLER_SATA		_T( "Serial ATA Controller")

#include "SysInfoClasses.h"

class SYSINFO_API CSystemController  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CSystemController();
	virtual ~CSystemController();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetDescription();
	LPCTSTR GetCaption();
	LPCTSTR GetHardwareVersion();
	LPCTSTR GetName();
	LPCTSTR GetType();
	LPCTSTR GetManufacturer();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetDescription( LPCTSTR lpstrDescription);
	void SetCaption( LPCTSTR lpstrCaption);
	void SetHardwareVersion( LPCTSTR lpstrVersion);
	void SetName( LPCTSTR lpstrName);
	void SetType( LPCTSTR lpstrType);
	void SetManufacturer( LPCTSTR lpstrManufacturer);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CSystemController cObject) const;

protected: // Attributes
	CString m_csType;			// Controller type (Floppy, IDE, SCSI, PCMCIA...)
	CString m_csManufacturer;	// Manufacturer
	CString m_csName;			// Name
	CString m_csHardwareVersion;// Hardware or firmware version
	CString m_csCaption;		// Caption
	CString m_csDescription;	// Description
};
#endif // !defined(AFX_SYSTEMCONTROLLER_H__C2B544BB_1F51_4D7F_8978_17AF12FF645B__INCLUDED_)

