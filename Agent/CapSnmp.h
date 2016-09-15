//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2015 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Snmp.h: interface for the Snmp class
//
//////////////////////////////////////////////////////////////////////

#pragma once
#ifndef WIN32
#include <pthread.h>
#define THREAD_CC
#define THREAD_TYPE                    pthread_t
#define THREAD_CREATE(tid, entry, arg) pthread_create(&(tid), NULL, \
                                                      (entry), (arg))
#else
#include <windows.h>
#define THREAD_CC                      __cdecl
#define THREAD_TYPE                    DWORD
#define THREAD_CREATE(tid, entry, arg) do { _beginthread((entry), 0, (arg));\
                                            (tid) = GetCurrentThreadId();   \
									                                          } while (0)
#endif /* WIN32 */

#include "OCSInventory Front.h"
#include "CapacityAbstract.h"
#include "OCSInventory.h"
#include <Afxmt.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

class CCapSnmp : public CCapacityAbstract
{

public: // Methods

	// Standard constructor.and destructor
	CCapSnmp();
	~CCapSnmp();

	BOOL CheckOid(CString Community, CString IP, CString Oid);
	
	void PrinterModule(CString Community, CString IP, CMarkup *Xml);
	void TraysModule(CString Community, CString IP, CMarkup *Xml);
	void CartridgesModule(CString Community, CString IP, CMarkup *Xml);
	CString PhysicalModule(CString Community, CString IP);
	void ScanPhysicalModule(CString Community, CString IP, CMarkup *Xml, CString Name);
	BOOL RetrieveCommunities();
	// Call OcsSnmp
	void CallSnmp(CConnexionAbstract *pConnexion, CConfig *m_pConfig);
	// Call Custom mibs files in snmp directory
	void CallCustomMibs(std::string IpAddr, std::string Community);

};
