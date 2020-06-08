//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Zip.h: interface for the CZip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIP_H__6285B091_15B6_49B9_AAAD_DBE203883E53__INCLUDED_)
#define AFX_ZIP_H__6285B091_15B6_49B9_AAAD_DBE203883E53__INCLUDED_

#pragma once

#include "Ocsinventory Front Classes.h"
#include "Flate.h"

class OCSINVENTORYFRONT_API CZip  
{
public: // Methods

	// Standard constructor and destructor
	CZip();
	virtual ~CZip();

	// The two simple tools provided
	static CStringA inflate(CByteArray*);
	static CByteArray* deflate(CStringA);
	static CByteArray* deflate( LPBYTE pByte, UINT nLength);
};

#endif // !defined(AFX_ZIP_H__6285B091_15B6_49B9_AAAD_DBE203883E53__INCLUDED_)
