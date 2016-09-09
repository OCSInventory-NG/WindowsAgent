//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// PrologResponse.h: interface for the CPrologResponse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROLOGRESPONSE_H__F5FE8547_8A08_4473_A3EB_B2D61D7A6BF7__INCLUDED_)
#define AFX_PROLOGRESPONSE_H__F5FE8547_8A08_4473_A3EB_B2D61D7A6BF7__INCLUDED_

#pragma once

#define DEFAULT_PROLOG_FREQ		10

#include "OCSInventory Front Classes.h"
#include "ResponseAbstract.h"
#include "MapStringToStringArray.h"

class OCSINVENTORYFRONT_API CPrologResponse : public CResponseAbstract
{
public:

	/* Constructors, destructors */
	CPrologResponse(CByteArray*);
	virtual ~CPrologResponse();
	
	/* General options */
	LPCTSTR getPrologFreq();
	LPCTSTR getInventoryOnStartVal();
	
	/* Registry */
	BOOL	isRegistryRequired();
	CMapStringToStringArray* getRegistryParameters();

	/* Download */
	BOOL isDownloadRequired();
	CMapStringToStringArray* getDownloadPackages();
	CMapStringToStringArray* getDownloadParameters();

	/* SNMP */
	CMapStringToStringArray* getSnmpParameters();
	BOOL isSnmpRequired();

	/* Inventory */
	BOOL	isInventoryRequired();

	/* Ipdiscover */
	BOOL	isIpdiscoverRequired();
	CString	getIpdiscoverLatency();
	CString	getIpdiscoverLan();

	/* Support Log */
	BOOL getSupportLog( CString &csSupportMessage);

private: // Methods

	/* Generic functions */
	BOOL isActivatedOption(CString);
	CMapStringToStringArray* getOptionAttributes(CString option,...);

private: // Attributes

	/* My CMarkup object: the parsed raw response */
	CMarkup m_xml;
	// Prolog frequency
	CString m_csPrologFreq;
	// Inventory on start up
	CString m_csInventoryOnStartup;
	// Inventory order
	BOOL m_bInventoryRequired;

};

#endif // !defined(AFX_PROLOGRESPONSE_H__F5FE8547_8A08_4473_A3EB_B2D61D7A6BF7__INCLUDED_)
