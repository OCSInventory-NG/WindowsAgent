//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// RequestAbstract.h: interface for the CRequestAbstract class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTABSTRACT_H__2161ADD9_34EF_4EEF_81DB_99C7F63A30BE__INCLUDED_)
#define AFX_REQUESTABSTRACT_H__2161ADD9_34EF_4EEF_81DB_99C7F63A30BE__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "Markup.h"
#include "Log.h"
#include "Deviceid.h"
#include "Zip.h"

#ifdef _MBCS
#define XML_HEADERS				_T( "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<!DOCTYPE REQUEST>\r\n")
#else
// UTF-8 / Unicode
#define XML_HEADERS				_T( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<!DOCTYPE REQUEST>\r\n")
#endif

class OCSINVENTORYFRONT_API CRequestAbstract  
{
public: // Methods

	//Standards constructor and destructor
	CRequestAbstract();
	virtual ~CRequestAbstract() = 0;

	// Return the raw message in CString format
	CString getMessage() { return m_cmXml.GetDoc(); }
	DWORD getMessageLength() { return (m_cmXml.GetDoc().GetLength()*sizeof(TCHAR)); }

	// Return the compressed sent message 
	CByteArray* getRawMessage() { return m_pRawMessage; }
	DWORD getRawMessageLength();  

	// Final operations before a request can be sent
	virtual BOOL final();

	// Set the query name, and optional type
	BOOL setQuery( LPCTSTR lpstrQuery, LPCTSTR lpstrType = NULL);
	// Return a pointer to our global Markup
	CMarkup* getXmlPointer();
	// Return a pointer to our request
	CMarkup* getXmlPointerRequest();
	// Return a pointer to our request content
	CMarkup* getXmlPointerContent();
	// Add XML tags to the request element
	BOOL xmlAddRequestElem( LPCTSTR lpstrTag, LPCTSTR lpstrValue);
	// Add XML tags to the request content element
	BOOL xmlAddContentElem( LPCTSTR lpstrTag, LPCTSTR lpstrValue);

	// Do some action when request successfully executed
	virtual void setSuccess() = NULL;

protected: // Methods
	// Replace any binary data by 'x'
	void cleanXml();

protected: // Attributes
	// Compressed message (feed only when "final" is called)
	CByteArray *m_pRawMessage;
	// The content in CMarkup format
	CMarkup m_cmXml;
	// Pointer to Deviceid class
	CDeviceid *m_pDeviceid;
	// Error handling
	DWORD m_dwErrorStatus;
	CString	m_csErrorString;
	// The request was finalized
	BOOL m_bFinal;
	// A pointer to logging machine
	CLog *m_pLogger;

};

#endif // !defined(AFX_REQUESTABSTRACT_H__2161ADD9_34EF_4EEF_81DB_99C7F63A30BE__INCLUDED_)
