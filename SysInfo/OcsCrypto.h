//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsCrypto.h: interface for the COcsCrypto class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COCSCRYPTO_H__213BCCE2_C103_4127_A208_BEF1F271A958__INCLUDED_)
#define AFX_COCSCRYPTO_H__213BCCE2_C103_4127_A208_BEF1F271A958__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

class COcsCrypto  
{
public:
	COcsCrypto();
	virtual ~COcsCrypto();

	// Initialize hash algorithm, actually SHA1
	// Must be called before any Hash operation
	BOOL HashInit();
	// Compute Hash on NULL terminated string
	// Can be called as many times as you want
	BOOL HashUpdate( LPCTSTR pData);
	// Compute Hash on pData which has length of ulLength
	// Can be called as many times as you want
	BOOL HashUpdate( LPCVOID pData, ULONG ulLength);
	// Finalize Hash and return in csHash computed hash in hex format
	// Return NULL if error
	LPCTSTR HashFinal();

protected:
	LPVOID m_pHash;		// Pointer du hash algorithm context
};

#endif // !defined(AFX_COCSCRYPTO_H__213BCCE2_C103_4127_A208_BEF1F271A958__INCLUDED_)
