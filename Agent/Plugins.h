//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Plugins.h: interface for the CPlugins class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINS_H__AA0B11FA_E9D3_48FB_AB22_3D9950713399__INCLUDED_)
#define AFX_PLUGINS_H__AA0B11FA_E9D3_48FB_AB22_3D9950713399__INCLUDED_

#pragma once

#include "StdAfx.h"
#include "log.h"
#include "Markup.h"
#include "OCSInventory Front.h"

#define PLUGIN_OK 0

class CPrologRequest;
class CPrologResponse;
class CInventoryRequest;
class CInventoryResponse;

typedef int (*HOOK_START)(void);
typedef int (*HOOK_PROLOG_WRITE)(CPrologRequest*);
typedef int (*HOOK_PROLOG_RESP)(CPrologResponse*);
typedef int (*HOOK_INVENTORY)(CInventoryRequest*);
typedef int (*HOOK_END)(CInventoryResponse*);
typedef int (*HOOK_CLEAN)(void);

#define MAX_PLUGINS 64

class CPlugins
{

public:
	CPlugins();
	virtual ~CPlugins();

	// Call plugin Hook when agent loads, typically used to initialize plugin
	void startHook();
	// Call plugin Hook before sending prolog to server, typically used to add/modify information to prolog request
	void prologWriteHook(CPrologRequest*);
	// Call plugin Hook after receiving prolog response from server, typically usd to parse additional data sent by server
	void prologRespHook(CPrologResponse*);
	// Call plugin Hook before sending inventory to server, typically used to add/modify inventory information sent to server
	void inventoryHook(CInventoryRequest*);
	// Call plugin Hook after receiving inventory response from server, typically usd to parse additional data sent by server
	void endHook(CInventoryResponse*);
	// Call plugin Hook when agent exits, typically used to free plugin resources
	void cleanHook();

private:
	CLog *m_pLogger;

	int Load( LPCTSTR lpstrPath = NULL);
	void Unload();

	struct _Plugin {
		CString csName;
		HINSTANCE hDll;
		HOOK_INVENTORY pInventory;
		HOOK_PROLOG_RESP pPrologResp;
		HOOK_PROLOG_WRITE pPrologWrite;
		HOOK_START pStart;
		HOOK_END pEnd;
		HOOK_CLEAN pClean;
	} m_plugin[MAX_PLUGINS];
};

#endif // !defined(AFX_PLUGINS_H__AA0B11FA_E9D3_48FB_AB22_3D9950713399__INCLUDED_)
