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

class CInventoryRequest;
class CPrologRequest;
class CPrologResponse;

typedef int (*HOOK_START)(void);
typedef int (*HOOK_END)(void);
typedef int (*HOOK_CLEAN)(void);
typedef int (*HOOK_PROLOG_WRITE)(CPrologRequest*);
typedef int (*HOOK_PROLOG_RESP)(CPrologResponse*);
typedef int (*HOOK_INVENTORY)(CInventoryRequest*);

#define MAX_PLUGINS 64

class CPlugins
{

public:
	void inventoryHook(CInventoryRequest*);
	void prologWriteHook(CPrologRequest*);
	void prologRespHook(CPrologResponse*);
	void endHook();
	void startHook();

	CPlugins();
	virtual ~CPlugins();

private:
	CLog *m_pLogger;
	BOOL m_bAlreadyLoaded;
	int Load( LPCTSTR lpstrPath = NULL);
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
