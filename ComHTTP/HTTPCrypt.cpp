//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AES_BLOCK_SIZE 16

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPCrypt::CHTTPCrypt()
{
	OPENSSL_init_crypto(0, NULL);
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
/*	if (TRUE)
	{
		// PreKey not existing => Generate PreKey for current computer, and store it
		return generatePreKey();
	}
	// Read existing PreKey 
*/	m_csKey = OCS_HTTP_KEY_VALUE;
	return TRUE;
}

BOOL CHTTPCrypt::encrypt( CString &csInput, CString &csOutput)
{
    BYTE pInitVector[AES_BLOCK_SIZE];	// Random generated initialization vector
    EVP_CIPHER_CTX *oEncCtx = EVP_CIPHER_CTX_new();				// Cipher context
    const EVP_CIPHER *oChiper = NULL;	// Cipher object
    LPBYTE pInBuffer = NULL,			// Input buffer for data to encipher
		   pOutBuffer = NULL,			// Output buffer for enciphered data
	       pPaddingBuffer = NULL;		// Output buffer to cipher final block+padding
	int nOutBufferLength,				// Output buffer length for enciphered data	
		nPaddingBufferLength,			// Output buffer length for enciphered final block+padding
		nLength;						// Really used output buffer length
	CString	csBuffer,					// Base 64 encoded encrypted data
			csVector;					// BAse 64 encoded intialisation vector
	CStringA csInputA = GetAnsiFromUnicode( csInput); // Input data in Ansi charset
    
    // Set aes key
	switch (m_csKey.GetLength())
	{
	case 16:
            oChiper = EVP_aes_128_cbc();
			break;
	case 32:
            oChiper = EVP_aes_256_cbc();
			break;
	default:
		 // Wrong key size
         return FALSE;
    }

    // Create initialization vector
	if (RAND_bytes(pInitVector, AES_BLOCK_SIZE) != 1)
		return FALSE;
	// Base64 encode initialization vector
	if (!base64_encode( pInitVector, AES_BLOCK_SIZE, csVector))
		return FALSE;
    // Create encryption context using initialization vector   
    EVP_EncryptInit_ex( oEncCtx, oChiper, NULL, (LPBYTE)LPCSTR( m_csKey), pInitVector);
	// Allocate input buffer to store data to encipher
	pInBuffer = (LPBYTE)malloc(csInputA.GetLength() + 2);
	if (!pInBuffer)
		return FALSE;
	memset( pInBuffer, 0, csInputA.GetLength()+2);
	memcpy( pInBuffer, (LPCSTR) csInputA, csInputA.GetLength());
    // Allocate output buffer for enciphered data, including last block + padding and to be sure, add two blocks.
	nOutBufferLength = csInputA.GetLength()+2*AES_BLOCK_SIZE;
	pOutBuffer = (LPBYTE)malloc(nOutBufferLength);
    if (!pOutBuffer)
	{
		free( pInBuffer);
		return FALSE;
	}
	memset( pOutBuffer, 0, nOutBufferLength);
	// Encrypt most of the data (Input must include the null terminating char)
	nLength = nOutBufferLength;
    if (EVP_EncryptUpdate( oEncCtx, pOutBuffer, &nLength, pInBuffer, csInputA.GetLength()+1) < 0)
	{
		// Encryption fail
		free( pOutBuffer);
		free( pInBuffer);
        return FALSE;
    }
    // Add last block+padding
	pPaddingBuffer = pOutBuffer+nLength;
	nPaddingBufferLength = nLength;
    if (EVP_EncryptFinal_ex( oEncCtx, pPaddingBuffer, &nPaddingBufferLength) < 0)
	{
		// Encryption fail
		free( pOutBuffer);
		free( pInBuffer);
        return FALSE;
    }
    // Base64 encode encrypted data
	if (!base64_encode( pOutBuffer, nLength+nPaddingBufferLength, csBuffer))
	{
		// Encryption fail
		free( pOutBuffer);
		free( pInBuffer);
        return FALSE;
    }
	// Create output string using initialization vector and encrypted data
	csOutput.Format( _T( "%s%s%s"), csVector,  OCS_HTTP_SEPARATOR_VALUE, csBuffer);
	free( pOutBuffer);
	free( pInBuffer);
    EVP_CIPHER_CTX_free(oEncCtx);
	return TRUE;
}

BOOL CHTTPCrypt::decrypt(CString &csInput, CString &csOutput)
{
	EVP_CIPHER_CTX *oEncCtx = EVP_CIPHER_CTX_new();
	const EVP_CIPHER *oChiper = NULL;
	int		nSep;
	UINT	uLength;
	LPBYTE	pOutBuffer = NULL,
			pOutPaddingBuffer = NULL,
			pInitVector = NULL,
			pInBuffer = NULL;
	int nOutBufferLength,
		nOutPaddingLength;
	CString	csBuffer;
    
    // Set aes key
	switch (m_csKey.GetLength())
	{
	case 16:
            oChiper = EVP_aes_128_cbc();
			break;
	case 32:
            oChiper = EVP_aes_256_cbc();
			break;
	default:
		 // Wrong key size
         return FALSE;
    }
	csOutput.Empty();
	// Get initialization vector from input string and base64 decode it
	if ((nSep = csInput.Find( OCS_HTTP_SEPARATOR_VALUE)) <= 0)
		return FALSE;
	csBuffer = csInput.Left( nSep);
	if ((pInitVector = base64_decode( csBuffer, &uLength)) == NULL)
		return FALSE;
	// Create decryption context using initialization vector  
	EVP_DecryptInit_ex( oEncCtx, oChiper, NULL, (LPBYTE) LPCSTR( m_csKey), pInitVector);
	free( pInitVector);
	// Get real encrypted data from input string
	csBuffer = csInput.Mid(nSep + _tcslen(OCS_HTTP_SEPARATOR_VALUE));
	if ((pInBuffer = base64_decode( csBuffer, &uLength)) == NULL)
		return FALSE;
	// Decrypt most of the data
	nOutBufferLength = uLength; 
	pOutBuffer = (LPBYTE)malloc(nOutBufferLength);
    if (!pOutBuffer)
	{
		free( pInBuffer);
		return FALSE;
	}
	memset( pOutBuffer, 0, nOutBufferLength);
	if (EVP_DecryptUpdate( oEncCtx, pOutBuffer, &nOutBufferLength, pInBuffer, nOutBufferLength) < 0)
	{
		// Decryption fail
		free( pInBuffer);
		free( pOutBuffer);
        return FALSE;
    }
	// Decrypt last block+padding:
	pOutPaddingBuffer = pOutBuffer + nOutBufferLength;
	nOutPaddingLength = nOutBufferLength;
	if (EVP_DecryptFinal_ex( oEncCtx, pOutPaddingBuffer, &nOutPaddingLength) < 0)
	{
		// Decryption fail
		free( pInBuffer);
		free( pOutBuffer);
        return FALSE;
    }

	// Copy decrypted data to output
	csOutput = CString((LPCTSTR)pOutBuffer);
	free( pInBuffer);
	free( pOutBuffer);
	EVP_CIPHER_CTX_free(oEncCtx);
	return TRUE;
}
