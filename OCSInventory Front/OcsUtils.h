//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OcsUtils.h : main header file for the OcsUtils
//
#ifndef _OCS_UTILS_H_INCLUDED_
#define  _OCS_UTILS_H_INCLUDED_

#pragma once

#include "stdafx.h"
#include "OCSInventory Front Classes.h"

// Transform UNICODE string to ANSI string
CStringA OCSINVENTORYFRONT_API GetAnsiFromUnicode(LPCTSTR a_wstrString);

// Transform ANSI string to UNICODE string
CStringW OCSINVENTORYFRONT_API GetUnicodeFromAnsi(LPCSTR a_strString);

// Transform Unicode source text string to UTF8
CStringA OCSINVENTORYFRONT_API GetUTF8FromUnicode( LPCTSTR a_wstrString);

// Transform UTF-8 source text string to Unicode
CStringW OCSINVENTORYFRONT_API GetUnicodeFromUTF8( LPCSTR a_strString);

// Ensure if a string if UTF8 encoded
BOOL OCSINVENTORYFRONT_API isValidUTF8( LPCSTR a_strString);

// Create directory (and every missing parent directories if needed)
BOOL OCSINVENTORYFRONT_API directoryCreate( LPCTSTR lpstrDir);

// Delete directory (and every file or sub directory)
BOOL OCSINVENTORYFRONT_API directoryDelete( LPCTSTR lpstrDir);

// Check if file or directory exists
BOOL OCSINVENTORYFRONT_API fileExists( LPCTSTR lpstrFile);

// Get Partition Free Space in bytes
__int64 OCSINVENTORYFRONT_API GetDiskFree( LPCTSTR lpstrDrive);

// Load file content into CByteArray
CByteArray OCSINVENTORYFRONT_API * LoadFileToByteArray( LPCTSTR lpstrFilename);

// Write CByteArray content into file
BOOL OCSINVENTORYFRONT_API WriteByteArrayToFile( CByteArray* pByte, LPCTSTR lpstrFilename);

// Load file into a text buffer (using file specified encoding)
BOOL OCSINVENTORYFRONT_API LoadFileToText( CString &csBuffer, LPCTSTR lpstrFilename);

// Write text buffer into file using default OS charset
BOOL OCSINVENTORYFRONT_API WriteTextToFile( LPCTSTR lpstrText, LPCTSTR lpstrFilename);

// Write text buffer into UTF-8 encoded file
BOOL OCSINVENTORYFRONT_API WriteTextToUTF8File( LPCTSTR lpstrText, LPCTSTR lpstrFilename);

// Write content into file
BOOL OCSINVENTORYFRONT_API WriteVoidToFile( LPCVOID lpVoid, UINT uLength, LPCTSTR lpstrFilename);

// Compute digest on a file, using algorithm lpstralgo, and return result encode in base64 or hexa
BOOL OCSINVENTORYFRONT_API fileDigest( LPCTSTR lpstrFile, CString &csDigest, LPCTSTR lpstrAlgo = _T( "sha1"), BOOL bBase64 = TRUE);

// Check wether a string is hex encoded or not
BOOL OCSINVENTORYFRONT_API is_hex( LPCTSTR lpstrString);

// Encode pBuffer (uLength bytes long) into hexadecimal, and put result into CString
BOOL OCSINVENTORYFRONT_API hex_encode( LPBYTE pBuffer, UINT uLength, CString &csHex);

// Decode CString hexadecimal encoded, and put result into a BYTE array of uLength elements
// You MUST free this pointer when no more needed
LPBYTE OCSINVENTORYFRONT_API hex_decode( CString &csHex, UINT *uLength);

// Check wether a string is base64 encoded or not
BOOL OCSINVENTORYFRONT_API is_base64( CString myString);

// Encode pBuffer (uLength bytes long) into base64, and put result into CString
BOOL OCSINVENTORYFRONT_API base64_encode( LPBYTE pBuffer, UINT uLength, CString &csBase64);

// Decode null terminated string, and put result into a BYTE array of uLength elements
// You MUST free this pointer when no more needed
LPBYTE OCSINVENTORYFRONT_API base64_decode( LPCTSTR lpstrBase64, UINT *uLength);

// Check wether a string is printable or not
BOOL OCSINVENTORYFRONT_API is_printable( CString myString);

#endif // _OCS_UTILS_H_INCLUDED_