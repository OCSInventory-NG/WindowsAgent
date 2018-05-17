// FileVersion.h: interface for the CFileVersion class.
//
// This code comes from www.codeguru.com
// by Manuel Laflamme (mailto:mlaflamm@total.net)
//////////////////////////////////////////////////////////////////////

#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#pragma once

#include "SysInfoClasses.h"


class SYSINFO_API CFileVersion
{ 
public: // Methods
	//////////////////////////////////
	// Standard contructor/destructor
	//////////////////////////////////
    CFileVersion();

    BOOL    Open(LPCTSTR lpszModuleName);
    void    Close();

	//////////////////////////////////
	// Get attributes values
	//////////////////////////////////

    CString QueryValue(LPCTSTR lpszValueName, DWORD dwLangCharset = 0);
    CString GetFileDescription()  {return QueryValue(_T( "FileDescription")); };
    CString GetFileVersion()      {return QueryValue(_T( "FileVersion"));     };
    CString GetInternalName()     {return QueryValue(_T( "InternalName"));    };
    CString GetCompanyName()      {return QueryValue(_T( "CompanyName"));     }; 
    CString GetLegalCopyright()   {return QueryValue(_T( "LegalCopyright"));  };
    CString GetOriginalFilename() {return QueryValue(_T( "OriginalFilename"));};
    CString GetProductName()      {return QueryValue(_T( "ProductName"));     };
    CString GetProductVersion()   {return QueryValue(_T( "ProductVersion"));  };
	CString GetProductLanguage();

    BOOL    GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
    CString GetFixedFileVersion();
    CString GetFixedProductVersion();

// Attributes
protected:
    LPBYTE  m_lpVersionData; 
    DWORD   m_dwLangCharset; 

// Implementation
public:
    ~CFileVersion(); 
}; 

#endif  // __FILEVERSION_H_




