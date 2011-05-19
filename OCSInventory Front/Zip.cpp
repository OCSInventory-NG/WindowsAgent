//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Zip.cpp: implementation of the CZip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Zip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZip::CZip()
{

}

CZip::~CZip()
{

}

/**
 *	Returns a decompressed String of the "data" byte array
 */
CStringA CZip::inflate(CByteArray* data) {

	if( data == NULL )
		return CStringA("");
	
	CByteArray* inflated = new CByteArray();
	Flate myZip;

	if( ! myZip.Uncompress(*inflated, *data)) {
		delete inflated;
		inflated = NULL;
	}

	if(inflated == NULL)
		return CStringA("");

/*  Bug Windows 2003 - Copy BYTE TO BYTE to create CString result
	CString result = inflated->GetData();

	if( result.GetLength() >= inflated->GetSize() )
		result.SetAt(inflated->GetSize(),0);
*/

	CStringA result("");
	for (int i=0;i<inflated->GetSize();i++){
 		result += inflated->GetAt(i);
 	}
	
	delete inflated;

	return result;
}

/**
 *	Returns a deflate compressed byte array of the "data" string
 */
CByteArray* CZip::deflate(CStringA data)
{	
	CByteArray *pDeflated = new CByteArray();
	CByteArray src;
	Flate myZip;
	
	for(int i=0;i<data.GetLength();i++) {
		src.Add( data.GetAt(i)) ;
	}

	myZip.Compress(*pDeflated, src);
	return pDeflated;
}

CByteArray* CZip::deflate( LPBYTE pByte, UINT nLength)
{	
	CByteArray *pDeflated = new CByteArray();
	CByteArray src;
	Flate myZip;
	
	for(UINT i=0;i<nLength;i++) {
		src.Add( pByte[i]) ;
	}

	myZip.Compress(*pDeflated, src);
	return pDeflated;
}