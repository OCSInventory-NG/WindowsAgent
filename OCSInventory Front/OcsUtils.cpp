//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsUtils.cpp: implementation of the OcsUtils.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OcsUtils.h"
#include <shlwapi.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>


CStringA OCSINVENTORYFRONT_API GetAnsiFromUnicode(LPCTSTR a_wstrString)
{
	USES_CONVERSION;
/*	static char	szBuffer[1024*1024+1]; // 1MB buffer to handle string 

	if (_tcslen( a_wstrString) > 1024*1024)
		AfxThrowMemoryException();
	strcpy_s( szBuffer, 1024*1024, CT2CA(a_wstrString));
	return szBuffer;
*/
	static CStringA csAnsi;

	csAnsi = CT2CA(a_wstrString);
	return csAnsi;
}

CStringW OCSINVENTORYFRONT_API GetUnicodeFromAnsi(LPCSTR a_strString)
{
	USES_CONVERSION;
/*	static TCHAR szBuffer[1024*1024]; // 1MB buffer to handle string 

	if (strlen( a_strString) > 1024*1024)
		AfxThrowMemoryException();
	_tcscpy_s( szBuffer, 1024*1024, CA2CT( a_strString));
	return szBuffer;
*/
	static CStringW csWide;

	csWide = A2CW(a_strString);
	return csWide;

}

CStringA OCSINVENTORYFRONT_API GetUTF8FromUnicode( LPCTSTR a_wstrString)
{
	// http://weblogs.asp.net/kennykerr/archive/2008/07/24/visual-c-in-short-converting-between-unicode-and-utf-8.aspx

	USES_CONVERSION;
	static CStringA utf8;
	
	utf8 = CT2CA( a_wstrString, CP_UTF8);
	return utf8;
}

CStringW OCSINVENTORYFRONT_API GetUnicodeFromUTF8( LPCSTR a_strString)
{
	// http://weblogs.asp.net/kennykerr/archive/2008/07/24/visual-c-in-short-converting-between-unicode-and-utf-8.aspx

	USES_CONVERSION;
	static CStringW unicode;
	
	unicode = CA2T( a_strString, CP_UTF8);
	return unicode;
}

BOOL OCSINVENTORYFRONT_API directoryCreate( LPCTSTR lpstrDir)
{
	// Create it if not exists
	if (fileExists( lpstrDir))
		return TRUE;
	switch (SHCreateDirectoryEx( NULL, lpstrDir, NULL))
	{
	case ERROR_SUCCESS: // Directory create successful
	case ERROR_FILE_EXISTS: // Directory already exists
	case ERROR_ALREADY_EXISTS:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL OCSINVENTORYFRONT_API directoryDelete( LPCTSTR lpstrDir)
{
	CFileFind cFinder;
	CString csPath;
	BOOL	bWorking;

	try
	{
		csPath.Format( _T( "%s\\*.*"), lpstrDir);
		bWorking = cFinder.FindFile( csPath);
		while (bWorking)
		{
			bWorking = cFinder.FindNextFile();
			if (cFinder.IsDots())
				continue;
			if (cFinder.IsDirectory())
				directoryDelete( cFinder.GetFilePath());
			else 
				DeleteFile( cFinder.GetFilePath());
		}
		cFinder.Close();
		return RemoveDirectory( lpstrDir);
	}
	catch (CException *pEx)
	{
		cFinder.Close();
		pEx->Delete();
		return FALSE;
	}
}

BOOL OCSINVENTORYFRONT_API fileExists( LPCTSTR lpstrFile)
{
	return PathFileExists( lpstrFile); 
}


__int64 OCSINVENTORYFRONT_API GetDiskFree( LPCTSTR lpstrDrive)
{
	DWORD	dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;

	unsigned __int64 i64FreeBytesToCaller,
                     i64TotalBytes,
                     i64FreeBytes;
    BOOL	fResult;

	typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,
		PULARGE_INTEGER, PULARGE_INTEGER);

    P_GDFSE pGetDiskFreeSpaceEx = NULL;

//         Use GetDiskFreeSpaceEx if available; otherwise, use
//         GetDiskFreeSpace.
//
//         Note: Since GetDiskFreeSpaceEx is not in Windows 95 Retail, we
//         dynamically link to it and only call it if it is present.  We 
//         don't need to call LoadLibrary on KERNEL32.DLL because it is 
//         already loaded into every Win32 process's address space.

#ifndef _UNICODE
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExA");
#else
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
                               GetModuleHandle( _T( "KERNEL32.DLL")),
                                                "GetDiskFreeSpaceExW");
#endif
    if (pGetDiskFreeSpaceEx)
    {
		fResult = pGetDiskFreeSpaceEx (lpstrDrive,
							(PULARGE_INTEGER)&i64FreeBytesToCaller,
                            (PULARGE_INTEGER)&i64TotalBytes,
                            (PULARGE_INTEGER)&i64FreeBytes);
    }
	else
	{
		// Cannot load GetDiskFreeSpaceEx => use GetDiskFreeSpace
        fResult = GetDiskFreeSpace (lpstrDrive, 
                             &dwSectPerClust,
                             &dwBytesPerSect, 
                             &dwFreeClusters,
                             &dwTotalClusters);

        if (fResult)
        {
            // force 64-bit math
			i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust *
                              dwBytesPerSect;
            i64FreeBytes = (__int64)dwFreeClusters * dwSectPerClust *
                              dwBytesPerSect;
        }
	}
	
	if (!fResult)
		// Error
		return -1;

    return (__int64) (i64FreeBytes);
}

CByteArray OCSINVENTORYFRONT_API * LoadFileToByteArray( LPCTSTR lpstrFilename)
{
	CByteArray* res = NULL;
	try
	{		
		BYTE	lu;
		CFile	myFile;

		res = new CByteArray();
		if( myFile.Open( lpstrFilename,CFile::modeRead|CFile::shareDenyNone))
		{
			while (myFile.Read(&lu,1))
				res->Add( lu );
			myFile.Close();
		}
		else 
		{
			delete res;
			res = NULL;
		}
	}
	catch (CException* pEx)
	{
		pEx->Delete();
		if (res != NULL)
			delete res;
		res = NULL;
	}
	return res;
}

BOOL OCSINVENTORYFRONT_API WriteByteArrayToFile( CByteArray* pByte, LPCTSTR lpstrFilename)
{
	if (pByte==NULL)
		return FALSE;
	
	try
	{
		CFile myFile;
		if (!myFile.Open( lpstrFilename, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
		{
			return FALSE;
		}
		myFile.Write( pByte->GetData(), pByte->GetSize());
		myFile.Close();
	}
	catch (CException* pEx)
	{
		pEx->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL OCSINVENTORYFRONT_API LoadFileToText( CString &csBuffer, LPCTSTR lpstrFilename)
{
	CStdioFile	cFile;
	CString		csLine;

	try
	{
		// Open the file for reading, using file specified encoding
		if (!cFile.Open( lpstrFilename, CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
			return FALSE;
		while (cFile.ReadString( csLine))
			csBuffer += csLine;
		cFile.Close();
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		cFile.Abort();
		return FALSE;
	}
	return TRUE;
}

BOOL OCSINVENTORYFRONT_API WriteTextToFile( LPCTSTR lpstrText, LPCTSTR lpstrFilename)
{
	CStdioFile	cFile;

	try
	{
		// Open the file for writing using OS default encoding
		if (!cFile.Open( lpstrFilename, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite))
			return FALSE;
		cFile.WriteString( lpstrText);
		cFile.Close();
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		cFile.Abort();
		return FALSE;
	}
	return TRUE;
}

BOOL OCSINVENTORYFRONT_API WriteTextToUTF8File( LPCTSTR lpstrText, LPCTSTR lpstrFilename)
{
	FILE *pFile = NULL;

	try
	{
		// Open the file for writing with UTF-8 encoding
		if (_tfopen_s( &pFile, lpstrFilename, _T( "wt,ccs=UTF-8")) != 0)
			return FALSE;
		CStdioFile	cFile( pFile);
		cFile.WriteString( lpstrText);
		cFile.Close();
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		if (pFile != NULL)
			fclose( pFile);
		return FALSE;
	}
	return TRUE;
}

BOOL OCSINVENTORYFRONT_API WriteVoidToFile( LPCVOID lpVoid, UINT uLength, LPCTSTR lpstrFilename)
{
	CFile	cFile;

	try
	{
		if (!cFile.Open( lpstrFilename, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
			return FALSE;
		cFile.Write( lpVoid, uLength);
		cFile.Close();
	}
	catch( CException *pEx)
	{
		// Exception=> free exception, but continue
		pEx->Delete();
		cFile.Abort();
		return FALSE;
	}
	return TRUE;
}

BOOL OCSINVENTORYFRONT_API fileDigest( LPCTSTR lpstrFile, CString &csDigest, LPCTSTR lpstrAlgo, BOOL bBase64)
{
	CFile	fRead;
	BYTE	pBuffer[1024],
			pDigest[EVP_MAX_MD_SIZE];

	ASSERT( lpstrFile);
	ASSERT( lpstrAlgo);

	// Compute digest on file
	if( ! fRead.Open( lpstrFile, CFile::modeRead|CFile::shareDenyNone))
		return FALSE;

	// Initialize checksum computing
	const EVP_MD *evpMD;
	EVP_MD_CTX ctx;
	UINT uLength;

	OpenSSL_add_all_digests();
	// Set the algorythm
	if (!(evpMD = EVP_get_digestbyname( GetAnsiFromUnicode( lpstrAlgo))))
		// Unsupported digest
		return FALSE;
	// Computing the checksum
	EVP_DigestInit (&ctx, evpMD);

	int nByteRead;
	do
	{
		nByteRead = fRead.Read( pBuffer, 1024);
		EVP_DigestUpdate (&ctx, pBuffer, nByteRead);
	}
	while(nByteRead>0);

	EVP_DigestFinal( &ctx, pDigest, &uLength);
	// Close file
	fRead.Close();
	// Now, encode result
	if (bBase64)
		// Digest is base64 encoded
		return base64_encode( pDigest, uLength, csDigest);
	// Digest is hexadecimal encoded
	return hex_encode( pDigest, uLength, csDigest);
}

BOOL OCSINVENTORYFRONT_API hex_encode( LPBYTE pBuffer, UINT uLength, CString &csHex)
{
	ASSERT( pBuffer);

	csHex.Empty();
	for (UINT uIndex = 0; uIndex<uLength; uIndex++)
	{
		csHex.AppendFormat( _T( "%02x"), pBuffer[uIndex]);
	}
	return TRUE;
}

/* A helper function for base64 encoding */
BOOL OCSINVENTORYFRONT_API base64_encode( LPBYTE pBuffer, UINT uLength, CString &csBase64)
{
  LPBYTE	pRet = NULL;
  UINT		uLengthB64;

	ASSERT( pBuffer);

	try
	{
		csBase64.Empty();
		// the b64data to data ratio is 3 to 4.
		// integer divide by 3 then multiply by 4, add one for NULL terminator.
		uLengthB64 = (((uLength+2) / 3) * 4) + 1;
		if ((pRet = (unsigned char *) malloc( uLengthB64)) == NULL)
			return FALSE;
		if (EVP_EncodeBlock( pRet, pBuffer, uLength) < 0)
		{
			free( pRet);
			return FALSE;
		}
		pRet[uLengthB64 - 1] = 0;
		for (UINT uIndex = 0; uIndex<uLengthB64; uIndex++)
		{
			csBase64.AppendFormat( _T( "%c"), pRet[uIndex]);
		}
		free( pRet);
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		if (pRet != NULL)
			free( pRet);
		return FALSE;
	}
}

LPBYTE OCSINVENTORYFRONT_API base64_decode( LPCTSTR lpstrBase64, UINT *uLength)
{
	LPBYTE	pRet = NULL;
	UINT	uOutLength;

	ASSERT( lpstrBase64);

	try
	{
		/* The exact amount is 3 * inlen / 4, minus 1 if the input ends
		 with "=" and minus another 1 if the input ends with "==".
		 Dividing before multiplying avoids the possibility of overflow.
		 */
		uOutLength = _tcslen( lpstrBase64);
		if (uOutLength < 2)
			// Buffer to small, not base 64 encoded
			return NULL;
		if ((lpstrBase64[uOutLength-1] == '=') &&
			(lpstrBase64[uOutLength-2] == '='))
			// end with "=="
			uOutLength = 3 * (_tcslen( lpstrBase64) / 4) - 2;
		else
		{
			if (lpstrBase64[uOutLength-1] != '=')
				// Not base64 encoded
				return NULL;
			// end with "="
			uOutLength = 3 * (_tcslen( lpstrBase64) / 4) - 1;
		}
		if ((pRet = (LPBYTE) malloc( uOutLength)) == NULL)
			return NULL;
		memset( pRet, 0, uOutLength);
		if (EVP_DecodeBlock( pRet, (LPBYTE) LPCSTR( GetAnsiFromUnicode( lpstrBase64)), _tcslen( lpstrBase64)) < 0)
		{
			free( pRet);
			return NULL;
		}
		*uLength = uOutLength;
		return pRet;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		if (pRet != NULL)
			free( pRet);
		return NULL;
	}
}
