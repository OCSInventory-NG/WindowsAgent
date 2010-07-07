//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPCrypt.h: interface for the CHTTPCrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCRYPT_H__BC0469FF_3E9D_446B_8D7A_CBE161DA8D6A__INCLUDED_)
#define AFX_HTTPCRYPT_H__BC0469FF_3E9D_446B_8D7A_CBE161DA8D6A__INCLUDED_

#pragma once

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

// 32 Bytes = 256 bits key
#define OCS_HTTP_KEY_VALUE			_T( "v1{!\b-'6C8E56eI'&OA/m7!^&l:e23E#")
#define OCS_HTTP_SEPARATOR_VALUE	_T( "|||")

class CHTTPCrypt
{
public: // Methods
	// Standard constructor and destructor
	CHTTPCrypt();
	virtual ~CHTTPCrypt();

	// Decrypt value and put it to DWORD parameter
	BOOL getData( LPCTSTR, DWORD&);
	// Decrypt value and put it to CString parameter
	BOOL getData( LPCTSTR, CString&);
	// Encrypt data (numeric or string) 
	BOOL setData( LPCTSTR, CString &);
	BOOL setData( DWORD, CString &);	

private: // Methods
	// Encrypt and decrypt
	BOOL decrypt(CString &, CString &);
	BOOL encrypt(CString &, CString &);
	// Generate a key on a new setup
	BOOL generatePreKey();
	// Retrieve the key in order to decrypt
	BOOL retrievePreKey();

private: //Attributes
	CString	m_csKey;
};

#endif // !defined(AFX_HTTPCRYPT_H__BC0469FF_3E9D_446B_8D7A_CBE161DA8D6A__INCLUDED_)
