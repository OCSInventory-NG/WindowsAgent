//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// StoragePeripheral.cpp: implementation of the CStoragePeripheral class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StoragePeripheral.h"
#include <VersionHelpers.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStoragePeripheral::CStoragePeripheral()
{
	Clear();
}

CStoragePeripheral::~CStoragePeripheral()
{
	Clear();
}

LPCTSTR CStoragePeripheral::GetManufacturer()
{
	return m_csManufacturer;
}

LPCTSTR CStoragePeripheral::GetType()
{
	return m_csType;
}

LPCTSTR CStoragePeripheral::GetModel()
{
	return m_csModel;
}

LPCTSTR CStoragePeripheral::GetDescription()
{
	return m_csDescription;
}

LPCTSTR CStoragePeripheral::GetName()
{
	return m_csName;
}

unsigned __int64 CStoragePeripheral::GetSize()
{
	return m_u64Size;
}

LPCTSTR CStoragePeripheral::GetSizeString()
{
	static CString csBuffer;

	if (m_u64Size > 0)
		//csBuffer.Format( _T( "%I64u MB"), m_u64Size);
		csBuffer.Format( _T( "%I64u"), m_u64Size);
	else
		csBuffer.Empty();
	return csBuffer;
}

LPCTSTR CStoragePeripheral::GetSN()
{
	return m_csSN;
}

LPCTSTR CStoragePeripheral::GetFirmware()
{
	return m_csFirmware;
}

void CStoragePeripheral::SetManufacturer(LPCTSTR lpstrManufacturer)
{
	m_csManufacturer = lpstrManufacturer;
	StrForSQL( m_csManufacturer);
}

void CStoragePeripheral::SetType(LPCTSTR lpstrType)
{
	m_csType = lpstrType;
	StrForSQL( m_csType);
}

void CStoragePeripheral::SetName(LPCTSTR lpstrName)
{
	m_csName = lpstrName;
	StrForSQL( m_csName);
}

void CStoragePeripheral::SetModel(LPCTSTR lpstrModel)
{
	m_csModel = lpstrModel;
	StrForSQL( m_csModel);
}

BOOL CStoragePeripheral::IsValidModel( LPCTSTR lpstrModel)
{
	CString csModel;

	if (lpstrModel == NULL)
		csModel = m_csModel;
	else
		csModel = lpstrModel;
	return (csModel.GetLength() > SYSINFO_MIN_DEFAULT_LENGTH);
}

void CStoragePeripheral::SetDescription(LPCTSTR lpstrDescription)
{
	m_csDescription = lpstrDescription;
	StrForSQL( m_csDescription);
}

void CStoragePeripheral::SetSize( unsigned __int64 u64Size)
{
	m_u64Size = u64Size;
}

void CStoragePeripheral::SetSN( LPCTSTR lpstrSN)
{
	CString	csSubString;
	TCHAR	*pStopChar,
			cPermute;
	char	myCar;

	m_csSN.Empty();

	if (IsWindows8OrGreater())
	{
		if (is_hex(lpstrSN))
		{
			// Each serial number character is coded in ASCII hexadecimal value using 2 bytes, so we have to decode it
			for (size_t i = 0; (i<STORAGE_MAX_LENGTH_SERIAL) && ((i * 2)<_tcslen(lpstrSN)); i++)
			{
				csSubString.Empty();
				csSubString.Format(_T("%.02s"), lpstrSN + 2 * i);
				myCar = (char)_tcstol(csSubString, &pStopChar, 16);
				m_csSN.AppendFormat(_T("%c"), myCar);
			}
			// Permute low order and high order characters if serial length is multiple of 2
			for (int i = 0; (i<m_csSN.GetLength()) && (m_csSN.GetLength() % 2 == 0); i += 2)
			{
				cPermute = m_csSN.GetAt(i);
				m_csSN.SetAt(i, m_csSN.GetAt(i + 1));
				m_csSN.SetAt(i + 1, cPermute);
			}
			StrForSQL(m_csSN);
			if (is_printable(m_csSN))
				// Hex decode successful
				return;
		}
		// Not hex encoded (or hex decode failed), ensure printable
		if (is_printable(lpstrSN))
			// Serial length seems good, assume well formatted
			m_csSN = lpstrSN;
		else
		{
			// Not printable, hex encode it
			m_csSN.Empty();
			for (UINT uIndex = 0; uIndex<_tcslen(lpstrSN); uIndex++)
			{
				m_csSN.AppendFormat(_T("%02x"), lpstrSN[uIndex]);
			}
		}
		StrForSQL(m_csSN);
	}
	if(!IsWindows8OrGreater()) 
	{
		if ((lpstrSN != NULL) && (_tcslen(lpstrSN) > STORAGE_MAX_LENGTH_SERIAL))
		{
			// Each serial number character is coded in ASCII hexadecimal value using 2 bytes, so we have to decode it
			for (size_t i = 0; (i<STORAGE_MAX_LENGTH_SERIAL) && ((i * 2)<_tcslen(lpstrSN)); i++)
			{
				csSubString.Empty();
				csSubString.Format(_T("%.02s"), lpstrSN + 2 * i);
				myCar = (char)_tcstol(csSubString, &pStopChar, 16);
				m_csSN.AppendFormat(_T("%c"), myCar);
			}
			// Permute low order and high order characters if serial length is multiple of 2
			for (int i = 0; (i<m_csSN.GetLength()) && (m_csSN.GetLength() % 2 == 0); i += 2)
			{
				cPermute = m_csSN.GetAt(i);
				m_csSN.SetAt(i, m_csSN.GetAt(i + 1));
				m_csSN.SetAt(i + 1, cPermute);
			}
		}
		else
			// Serial length seems good, assume well formatted
			m_csSN = lpstrSN;
		StrForSQL(m_csSN);
	}
}

BOOL CStoragePeripheral::IsValidSN( LPCTSTR lpstrSN)
{
	CString csSN;

	if (lpstrSN == NULL)
		csSN = m_csSN;
	else
		csSN = lpstrSN;
	return (csSN.GetLength() > SYSINFO_MIN_SN_LENGTH);
}

void CStoragePeripheral::SetFirmware( LPCTSTR lpstrRev)
{
	m_csFirmware = lpstrRev;
	StrForSQL( m_csFirmware);
}

BOOL CStoragePeripheral::IsValidFirmware( LPCTSTR lpstrRev)
{
	CString csRev;

	if (lpstrRev == NULL)
		csRev = m_csFirmware;
	else
		csRev = lpstrRev;
	return (csRev.GetLength() > SYSINFO_MIN_DEFAULT_LENGTH);
}

void CStoragePeripheral::Clear()
{
	m_csType.Empty();			
	m_csManufacturer.Empty();	
	m_csName.Empty();			
	m_csModel.Empty();			
	m_csDescription.Empty();	
	m_csSN.Empty();
	m_csFirmware.Empty();
	m_u64Size = 0;			
}

int CStoragePeripheral::operator==(CStoragePeripheral cObject) const
{
	return ((m_csType == cObject.GetType()) &&
		(m_csManufacturer == cObject.GetManufacturer()) &&
		(m_csName == cObject.GetName()) &&
		(m_csModel == cObject.GetModel()) &&
		(m_csDescription == cObject.GetDescription()) &&
		(m_u64Size == cObject.GetSize()) &&
		(m_csSN == cObject.GetSN()) &&
		(m_csFirmware == cObject.GetFirmware()));
}

BOOL CStoragePeripheral::is_hex( LPCTSTR lpstrString)
{
	DWORD_PTR dw;
	TCHAR ch; // !!!

	return (1 == _stscanf(lpstrString, TEXT("&#37;x%c"), &dw, &ch));
}

BOOL CStoragePeripheral::is_printable( CString myString)
{
	BOOL  bPrintable = TRUE;
	static CStringA csAnsi;
	CT2W			pszA(myString);

	// Convert to Ansi, and avoid producing "(null)" string we converting
	if (!myString.IsEmpty() && (myString.GetLength() > 0))
		csAnsi = pszA;
	else
		csAnsi.Empty();

	for (int i = 0; i < csAnsi.GetLength(); i++)
	{
		if (!isprint(csAnsi.GetAt(i)))
			bPrintable = FALSE;
	}
	return bPrintable;
}
