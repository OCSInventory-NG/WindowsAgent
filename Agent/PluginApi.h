//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#if !defined( _PLUGIN_API_H_INCLUDED_)
#define _PLUGIN_API_H_INCLUDED_

#pragma once

#include "markup.h"
#include "PrologResponse.h"
#include "InventoryResponse.h"
#include "PrologRequest.h"
#include "InventoryRequest.h"

#ifdef _OCS_AGENT
// We are in agent code, we import plugins functions
#define OCSINVENTORY_API_EXPORTED _declspec(dllimport)
#else
// We are in plugin, we export functions
#define OCSINVENTORY_API_EXPORTED _declspec(dllexport)
#endif

// Plugin hook return code if bo error
// Other return code are specified by plugin, and means error
#define PLUGIN_OK 0


#ifdef __cplusplus
extern "C" {
#endif

// Hook called when agent loads, typically used to initialize plugin
typedef int (*HOOK_START)(void);
OCSINVENTORY_API_EXPORTED int OCS_CALL_START_EXPORTED();
// Hook called before sending prolog to server, typically used to add/modify information to prolog request
typedef int (*HOOK_PROLOG_WRITE)(CPrologRequest*);
OCSINVENTORY_API_EXPORTED int OCS_CALL_PROLOGWRITE_EXPORTED(CPrologRequest*);
// Hook called after receiving prolog response from server, typically usd to parse additional data sent by server
typedef int (*HOOK_PROLOG_RESP)(CPrologResponse*);
OCSINVENTORY_API_EXPORTED int OCS_CALL_PROLOGRESP_EXPORTED(CPrologResponse*);
// Hook called before sending inventory to server, typically used to add/modify inventory information sent to server
typedef int (*HOOK_INVENTORY)(CInventoryRequest*);
OCSINVENTORY_API_EXPORTED int OCS_CALL_INVENTORY_EXPORTED(CInventoryRequest*);
// Hook called after receiving inventory response from server, typically usd to parse additional data sent by server
typedef int (*HOOK_END)(CInventoryResponse*);
OCSINVENTORY_API_EXPORTED int OCS_CALL_END_EXPORTED(CInventoryResponse*);
// Hook called when agent exits, typically used to free plugin resources
typedef int (*HOOK_CLEAN)(void);
OCSINVENTORY_API_EXPORTED int OCS_CALL_CLEAN_EXPORTED();

#ifdef __cplusplus
};
#endif

#endif // _PLUGIN_API_H_INCLUDED_