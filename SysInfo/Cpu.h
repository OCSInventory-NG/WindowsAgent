//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Cpu.h: interface for the CCpu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPU_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_)
#define AFX_CPU_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CCpu  
{
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
	CCpu();
	virtual ~CCpu();

	void Clear();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

	LPCTSTR GetSocket();			
	LPCTSTR GetManufacturer();		
	LPCTSTR GetName();			
	LPCTSTR GetArchitecture();		
	UINT GetNumberOfCores();
	UINT GetNumberOfLogicalProcessors(); 
	UINT GetCurrentClockSpeed(); // MHz
	UINT GetMaxClockSpeed();	 // MHz
	UINT GetL2CacheSize();		 // KB
	UINT GetAddressWidth();	
	UINT GetDataWidth();		
	LPCTSTR GetVoltage();		
	LPCTSTR GetStatus();		

	//////////////////////////////////
	// Set attributes values
	//////////////////////////////////

	void SetSocket( LPCTSTR lpstrSocket);				
	void SetManufacturer( LPCTSTR lpstrManufacturer);		
	void SetName( LPCTSTR lpstrName);				
	void SetArchitecture( LPCTSTR lpstrArchitecture);		
	void SetNumberOfCores( UINT uNumberOfCores);			
	void SetNumberOfLogicalProcessors( UINT uNumberOfLogicalProcessors); 
	void SetCurrentSpeed( UINT uCurrentClockSpeed);
	void SetMaxSpeed( UINT uMaxClockSpeed);	
	void SetL2CacheSize( UINT uL2CacheSize);	
	void SetAddressWith( UINT uAddressWidth);	
	void SetDataWidth( UINT uDataWidth);		
	void SetVoltage( LPCTSTR lpstrVoltage);		
	void SetStatus( LPCTSTR lpstrSatus);				

	//////////////////////////////////
	// Comparison operators
	//////////////////////////////////

	int operator==( const CCpu cObject) const;

protected: // Attributes
	CString m_csSocket;				// Socket
	CString m_csManufacturer;		// CPU type
	CString m_csName;				// CPU name
	CString m_csArchitecture;		// CPU Architecture (x86, x64, Itanium...)
	UINT m_uNumberOfCores;			// Number of cores for the current instance of the processor
	UINT m_uNumberOfLogicalProcessors; // Number of logical processors for the current instance of the processor. 
									   // If hyperthreading is enabled in the BIOS for the processor, then 
									   // NumberOfCores is less than NumberOfLogicalProcessors
	UINT m_uCurrentClockSpeed;		// Current speed of the processor, in MHz (may be lower than max speed, 
	                                // especially on netbook)
	UINT m_uMaxClockSpeed;			// Maximum speed of the processor, in MHz
	UINT m_uL2CacheSize;			// Size of the Level 2 processor cache
	UINT m_uAddressWidth;			// Processor address width, in bits
	UINT m_uDataWidth;				// Processor data width, in bits
	CString m_csVoltage;			// Voltage capabilities of the processor (5, 3.3 or 2.9 volts)
	CString m_csStatus;				// CPU status (Enabled, disabled...)
};
#endif // !defined(AFX_CPU_H__13BBD121_3DC4_4C39_8023_2409A47F8704__INCLUDED_)

