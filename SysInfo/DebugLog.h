//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DebugLog.h: interface for the CDebugLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGLOG_H__0CB97901_C4D7_11D5_B2E3_0040055338AF__INCLUDED_)
#define AFX_DEBUGLOG_H__0CB97901_C4D7_11D5_B2E3_0040055338AF__INCLUDED_

#pragma once

#include "SysInfoClasses.h"

void OpenLog( LPCTSTR lpstrFolder = NULL);
void AddLog( LPCTSTR lpstrLog,...);
void CloseLog();

#endif // !defined(AFX_DEBUGLOG_H__0CB97901_C4D7_11D5_B2E3_0040055338AF__INCLUDED_)
