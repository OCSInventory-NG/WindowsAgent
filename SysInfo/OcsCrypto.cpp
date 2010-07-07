//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// COcsCrypto.cpp: implementation of the OcsCrypto class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OcsCrypto.h"
#include <openssl/sha.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COcsCrypto::COcsCrypto()
{
	m_pHash = NULL;
}

COcsCrypto::~COcsCrypto()
{
	if (m_pHash != NULL)
		free( m_pHash);
}

BOOL COcsCrypto::HashInit()
{
	if (m_pHash != NULL)
		free( m_pHash);
	if ((m_pHash = (SHA_CTX *) malloc( sizeof( SHA_CTX))) == NULL)
		return FALSE;
	SHA1_Init( (SHA_CTX *)m_pHash);
	return TRUE;
}

BOOL COcsCrypto::HashUpdate(LPCVOID pData, ULONG ulLength)
{
	if ((m_pHash == NULL) || (pData == NULL))
		return FALSE;
	SHA1_Update( (SHA_CTX *)m_pHash, pData, ulLength);
	return TRUE;
}

LPCTSTR COcsCrypto::HashFinal()
{
	BYTE	digest[SHA_DIGEST_LENGTH];
	UINT	uLength = SHA_DIGEST_LENGTH;
	CString csTemp;
	static CString csHash;

	if (m_pHash == NULL)
		return NULL;
	SHA1_Final( digest, (SHA_CTX *)m_pHash);
	free( m_pHash);
	m_pHash = NULL;
	// Store hash in hex
	csHash.Empty();
	for (UINT i=0; i<uLength; i++)
	{
		csTemp.Format( _T( "%.02X"), digest[i]);
		csHash += csTemp;
	}
	return csHash;
}
