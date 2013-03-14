//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Cpu.cpp: implementation of the CCpu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cpu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCpu::CCpu()
{
	Clear();
}

CCpu::~CCpu()
{
	Clear();
}

LPCTSTR CCpu::GetSocket()
{
	return m_csSocket;
}

LPCTSTR CCpu::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CCpu::GetName()
{
	return m_csName;
}
			
LPCTSTR CCpu::GetArchitecture()
{
	return m_csArchitecture;
}
		
UINT CCpu::GetNumberOfCores()
{
	return m_uNumberOfCores;
}

UINT CCpu::GetNumberOfLogicalProcessors()
{
	return m_uNumberOfLogicalProcessors;
}

UINT CCpu::GetCurrentClockSpeed()
{
	return m_uCurrentClockSpeed;
}

UINT CCpu::GetMaxClockSpeed()
{
	return m_uMaxClockSpeed;
}

UINT CCpu::GetL2CacheSize()
{
	return m_uL2CacheSize;
}

UINT CCpu::GetAddressWidth()
{
	return m_uAddressWidth;
}

UINT CCpu::GetDataWidth()
{
	return m_uDataWidth;
}

LPCTSTR CCpu::GetVoltage()
{
	return m_csVoltage;
}

LPCTSTR CCpu::GetStatus()
{
	return m_csStatus;
}

void CCpu::SetSocket( LPCTSTR lpstrSocket)
{
	m_csSocket = lpstrSocket;
	StrForSQL( m_csSocket);
}

void CCpu::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CCpu::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CCpu::SetArchitecture(LPCTSTR lpstrArchitecture)
{
	m_csArchitecture = lpstrArchitecture;
	StrForSQL( m_csArchitecture);
}

void CCpu::SetNumberOfCores( UINT uNumberOfCores)
{
	m_uNumberOfCores = uNumberOfCores;
}

void CCpu::SetNumberOfLogicalProcessors( UINT uNumberOfLogicalProcessors)
{
	m_uNumberOfLogicalProcessors = uNumberOfLogicalProcessors;
}
 
void CCpu::SetCurrentSpeed( UINT uCurrentClockSpeed)
{
	m_uCurrentClockSpeed = uCurrentClockSpeed;
}

void CCpu::SetMaxSpeed( UINT uMaxClockSpeed)
{
	m_uMaxClockSpeed = uMaxClockSpeed;
}
	
void CCpu::SetL2CacheSize( UINT uL2CacheSize)
{
	m_uL2CacheSize = uL2CacheSize;
}
	
void CCpu::SetAddressWith( UINT uAddressWidth)
{
	m_uAddressWidth = uAddressWidth;
}

void CCpu::SetDataWidth( UINT uDataWidth)
{
	m_uDataWidth = uDataWidth;
}

void CCpu::SetVoltage( LPCTSTR lpstrVoltage)
{
	m_csVoltage = lpstrVoltage;
	StrForSQL( m_csVoltage);
}

void CCpu::SetStatus( LPCTSTR lpstrSatus)
{
	m_csStatus = lpstrSatus;
	StrForSQL( m_csStatus);
}

void CCpu::Clear()
{
	m_csSocket.Empty();
	m_csManufacturer.Empty();
	m_csName.Empty();
	m_csArchitecture.Empty();
	m_uNumberOfCores = 0;
	m_uNumberOfLogicalProcessors = 0;
	m_uCurrentClockSpeed = 0;
	m_uMaxClockSpeed = 0;
	m_uL2CacheSize = 0;
	m_uAddressWidth = 0;
	m_uDataWidth = 0;
	m_csVoltage.Empty();
	m_csStatus.Empty();
}

int CCpu::operator==(CCpu cObject) const
{
	return ((m_csSocket == cObject.GetSocket()) &&
		(m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csName == cObject.GetName()) &&		
		(m_csArchitecture == cObject.GetArchitecture()) &&
		(m_uNumberOfCores == cObject.GetNumberOfCores()) &&
		(m_uNumberOfLogicalProcessors == cObject.GetNumberOfLogicalProcessors()) &&
		(m_uCurrentClockSpeed == cObject.GetCurrentClockSpeed()) &&
		(m_uMaxClockSpeed == cObject.GetMaxClockSpeed()) &&
		(m_uL2CacheSize == cObject.GetL2CacheSize()) &&
		(m_uAddressWidth == cObject.GetAddressWidth()) &&
		(m_uDataWidth == cObject.GetDataWidth()) &&
		(m_csVoltage == cObject.GetVoltage()) &&
		(m_csStatus == cObject.GetStatus()));
}