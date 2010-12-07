//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPCrypt.cpp: implementation of the CHTTPCrypt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OcsUtils.h"
#include "HTTPCrypt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define AES_BLOCK_SIZE 16

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPCrypt::CHTTPCrypt()
{
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

	retrievePreKey();
}

CHTTPCrypt::~CHTTPCrypt()
{

}

/* Access to crypted container - CString way */
BOOL CHTTPCrypt::getData( LPCTSTR lpstrCrypt, CString & csResult)
{
	CString csValue;

	if ((lpstrCrypt == NULL) || (_tcscmp( lpstrCrypt, _T( "")) == 0))
	{
		csResult.Empty();
		return TRUE;
	}
	csValue = lpstrCrypt;
	return decrypt( csValue, csResult);
}

/* Access to crypted container - Numeric way */
BOOL CHTTPCrypt::getData( LPCTSTR lpstrCrypt, DWORD & dwResult)
{
	CString csValue,
			csResult;
	BOOL	bResult;

	if ((lpstrCrypt == NULL) || (_tcscmp( lpstrCrypt, _T( "")) == 0))
	{
		dwResult = 0;
		return TRUE;
	}
	csValue = lpstrCrypt;
	bResult = decrypt( csValue, csResult);
	dwResult = _ttol( csResult);
	return bResult;
}

/* Access to crypted container - Numeric way */
BOOL CHTTPCrypt::setData( DWORD dwValue, CString &csCrypt)
{
	CString csValue;

	csValue.Format( _T( "%lu"), dwValue);
	return encrypt( csValue, csCrypt);
}

/* Access to crypted container - Numeric way */
BOOL CHTTPCrypt::setData( LPCTSTR lpValue, CString &csCrypt)
{
	CString csValue;

	if ((lpValue == NULL) || (_tcscmp( lpValue, _T( "")) == 0))
	{
		csCrypt.Empty();
		return TRUE;
	}
	csValue = lpValue;
	return encrypt( csValue, csCrypt);
}

/* Generate the AES key */
BOOL CHTTPCrypt::generatePreKey()
{
	// Today, only using fixed key with salt
	m_csKey = OCS_HTTP_KEY_VALUE;
	return TRUE;
}

BOOL CHTTPCrypt::retrievePreKey()
{
	// Detect if pre key not already generated 
	if (TRUE)
	{
		// PreKey not existing => Generate PreKey for current computer, and store it
		return generatePreKey();
	}
	// Read existing PreKey 
	m_csKey = OCS_HTTP_KEY_VALUE;
	return TRUE;
}

BOOL CHTTPCrypt::encrypt( CString &csClear, CString &csCrypted)
{
    BYTE pInitVector[AES_BLOCK_SIZE];
    EVP_CIPHER_CTX oEncCtx;
    const EVP_CIPHER *oChiper = NULL;
    LPBYTE pOutBuffer = NULL;
	int nOutBufferLength,
		nLength;
	CString	csBuffer;
    
    // Set aes key:
	switch (m_csKey.GetLength())
	{
	case 16:
            oChiper = EVP_aes_128_cbc();
			break;
	case 32:
            oChiper = EVP_aes_256_cbc();
			break;
	default:
		 // Wrong key size.
         return FALSE;
    }

    // Create init vector and add him infront of encrypted output data:
    RAND_pseudo_bytes( pInitVector, AES_BLOCK_SIZE);
	// Add init vector base64 encoded to the begining of output
	if (!base64_encode( pInitVector, AES_BLOCK_SIZE, csCrypted))
		return FALSE;
	// Add init vector separator to output
	csCrypted += OCS_HTTP_SEPARATOR_VALUE;

    // Create encryption context.   
    EVP_CIPHER_CTX_init( &oEncCtx);
    EVP_EncryptInit_ex( &oEncCtx, oChiper, 0, (LPBYTE) LPCSTR( GetAnsiFromUnicode( m_csKey)), pInitVector);

    // Encrypt most of the data:
	nOutBufferLength = csClear.GetLength()+2*AES_BLOCK_SIZE;
    if ((pOutBuffer = (LPBYTE) malloc( nOutBufferLength * sizeof( BYTE))) == NULL)
		return FALSE;
	nLength = nOutBufferLength;
    if (EVP_EncryptUpdate( &oEncCtx, pOutBuffer, &nLength,
							(LPBYTE) LPCSTR( GetAnsiFromUnicode( csClear)), csClear.GetLength()) < 0)
	{
		// Encryption fail
		free( pOutBuffer);
        return FALSE;
    }
    // Add encrypted data to output
	if (!base64_encode( pOutBuffer, nOutBufferLength, csBuffer))
	{
		// Encryption fail
		free( pOutBuffer);
        return FALSE;
	}
	csCrypted += csBuffer;
	free( pOutBuffer);
    
    // Add last block+padding:
    nOutBufferLength = 2*AES_BLOCK_SIZE; // To be sure add two blocks.
    if ((pOutBuffer = (LPBYTE) malloc( nOutBufferLength * sizeof( BYTE))) == NULL)
		return FALSE;
	nLength = nOutBufferLength;
    if (EVP_EncryptFinal_ex( &oEncCtx, pOutBuffer, &nLength) < 0)
	{
		// Encryption fail
		free( pOutBuffer);
        return FALSE;
    }
    // Add encrypted data to output
	if (!base64_encode( pOutBuffer, nOutBufferLength, csBuffer))
	{
		// Encryption fail
		free( pOutBuffer);
        return FALSE;
    }
	csCrypted.Append( csBuffer);
	free( pOutBuffer);

    EVP_CIPHER_CTX_cleanup(&oEncCtx);
	return TRUE;
}

BOOL CHTTPCrypt::decrypt(CString &csCrypted, CString &csClear)
{
	EVP_CIPHER_CTX oEncCtx;
	const EVP_CIPHER *oChiper = NULL;
	int		nSep;
	UINT	uLength;
	LPBYTE	pOutBuffer,
			pInBuffer;
	int nOutBufferLength;
	CString	csBuffer;
    
    // Set aes key:
	switch (m_csKey.GetLength())
	{
	case 16:
            oChiper = EVP_aes_128_cbc();
			break;
	case 32:
            oChiper = EVP_aes_256_cbc();
			break;
	default:
		 // Wrong key size.
         return FALSE;
    }

	csClear.Empty();

	// Get init vector:
	if ((nSep = csCrypted.Find( OCS_HTTP_SEPARATOR_VALUE)) <= 0)
		return FALSE;
	csBuffer = csCrypted.Left( nSep);
	if ((pInBuffer = base64_decode( csBuffer, &uLength)) == NULL)
		return FALSE;

	// Create decryption context.   
	EVP_CIPHER_CTX_init( &oEncCtx);
	EVP_DecryptInit_ex( &oEncCtx, oChiper, 0, (LPBYTE) LPCSTR( GetAnsiFromUnicode( m_csKey)), pInBuffer);
	free( pInBuffer);

	// Subtract the InitVec.
	csBuffer = csCrypted.Right( nSep + _tcslen( OCS_HTTP_SEPARATOR_VALUE));
	if ((pInBuffer = base64_decode( csBuffer, &uLength)) == NULL)
		return FALSE;
	// Decrypt most of the data:
	nOutBufferLength = uLength; 
    if ((pOutBuffer = (LPBYTE) malloc( nOutBufferLength * sizeof( BYTE))) == NULL)
	{
		free( pInBuffer);
		return FALSE;
	}
	if (EVP_DecryptUpdate( &oEncCtx, pOutBuffer, &nOutBufferLength, 
			pInBuffer, nOutBufferLength) < 0)
	{
		// Decryption fail
		free( pInBuffer);
		free( pOutBuffer);
        return FALSE;
    }
	// Add decrypted data to output:
	for (int i=0; i<nOutBufferLength; i++)
		csClear.AppendFormat( _T( "%c"), pOutBuffer[i]);
	free( pOutBuffer);

	//Add last block+padding:
	nOutBufferLength = 2*AES_BLOCK_SIZE; // To be sure add two blocks.
	if ((pOutBuffer = (LPBYTE) malloc( nOutBufferLength * sizeof( BYTE))) == NULL)
	{
		free( pInBuffer);
		return FALSE;
	}
	if (EVP_DecryptFinal_ex( &oEncCtx, pOutBuffer, &nOutBufferLength) < 0)
	{
		// Decryption fail
		free( pInBuffer);
		free( pOutBuffer);
        return FALSE;
    }

	// Add decrypted data to output:
	for (int i=0; i<nOutBufferLength; i++)
		csClear.AppendFormat( _T( "%c"), pOutBuffer[i]);
	free( pInBuffer);
	free( pOutBuffer);

	EVP_CIPHER_CTX_cleanup(&oEncCtx);
	return TRUE;
}
