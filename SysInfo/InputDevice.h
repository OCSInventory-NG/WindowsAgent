//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// InputDevice.h: interface for the CInputDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTDEVICE_H__5BCE7A79_F8EC_4281_BF76_58D394AE4E08__INCLUDED_)
#define AFX_INPUTDEVICE_H__5BCE7A79_F8EC_4281_BF76_58D394AE4E08__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


#define INPUT_DEVICE_KEYBOARD	_T( "Keyboard")
#define INPUT_DEVICE_POINTING	_T( "Pointing")
#define INPUT_DEVICE_SMARTCARD	_T( "Smartcard Reader")

class SYSINFO_API CInputDevice  
{
public: // Methods
	//////////////////////////////////
	// Standard constructor/destructor
	//////////////////////////////////
	CInputDevice();
	virtual ~CInputDevice();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetType();
	LPCTSTR GetManufacturer();
	LPCTSTR GetCaption();
	LPCTSTR GetDescription();
	LPCTSTR GetPointingType();
	LPCTSTR GetPointingInterface();

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetType( LPCTSTR lpstrType);
	void SetManufacturer( LPCTSTR lpstrManufacturer);
	void SetDescription( LPCTSTR lpstrDescription);
	void SetCaption( LPCTSTR lpstrCaption);
	void SetPointingType( LPCTSTR lpstrPointingType);
	void SetPointingType( DWORD dwPointingType);
	void SetPointingInterface( LPCTSTR lpstrInterface);
	void SetPointingInterface( DWORD dwInterface);

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CInputDevice cObject) const;

protected: // Attributes
	CString m_csType;			// Input type (keyboard, pointing)
	CString m_csManufacturer;	// Input Device manufacturer
	CString m_csCaption;		// Caption
	CString m_csDescription;	// Description
	CString m_csPointingType;	// Pointing device type (trackball, trackpoint,touchpad,mouse...)
	CString m_csInterface;		// Pointing device interface (Serial, PS/2, Infrared, USB...)
};
#endif // !defined(AFX_INPUTDEVICE_H__5BCE7A79_F8EC_4281_BF76_58D394AE4E08__INCLUDED_)

