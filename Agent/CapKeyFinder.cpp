//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "stdafx.h"
#include "CapacityAbstract.h"
#include "CapKeyFinder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCapKeyFinder::CCapKeyFinder(void)
{
}

CCapKeyFinder::~CCapKeyFinder(void)
{
}

BOOL CCapKeyFinder::decodeMicrosoftKey( LPBYTE lpData, UINT uLength, CString csKey)
{
	char KeyChars[24] = {'B','C','D','F','G','H','J','K','M','P','Q','R','T','V','W','X','Y','2','3','4','6','7','8','9'};
	char szResult[30];
	// Decoding variables
	BYTE BinaryKey[15]; // Part of raw data contains the binary Product Key
	char DecodedKey[25]; // Decoded Product Key is placed here
	UINT A, i, j;

	if (uLength < 67) // 0x34 + 15
		return FALSE;
	// We need 15 bytes from $35 in BinaryKey
	memcpy( BinaryKey, &lpData[0x34], sizeof( BinaryKey));
	for( i=24; i>=0; i-- )
	{
		A = 0;
		// decoding the current symbol
		for( j=14; j>=0; j-- )
		{
			A = ((A<<8) + BinaryKey[j]);
			BinaryKey[j] = int(A / 24);
			A = A % 24;
		}
		DecodedKey[i] = KeyChars[A];
	}

	
	j=0;
	for( i=0; i<25; i++ )
	{
		szResult[j] = DecodedKey[i];
		j++;
		if (((i + 1) % 5 == 0) && (i < 24))
		{
			szResult[j] = '-';
			j++;
		}
	}
	szResult[29]='\0';
	csKey = szResult;
	return TRUE;
}

/***
 *
 * Retrieve the keys from registry
 *
 ***/
BOOL CCapKeyFinder::retrieveKeys()
{
	INT_PTR nIndex = 0;
	CMapStringToStringArray *pQueryArray = NULL;
	CMapStringToString		*pQuery = NULL;

	try
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "KEYFINDER => Searching for software Product Key"));
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "KEYFINDER => Unknown error while searching for Product Key %ld"), nIndex+1);
		return FALSE;
	}
}