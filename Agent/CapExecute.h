//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapExecute.h: interface for the CCapExecute class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CAPEXECUTE_H_INCLUDED_
#define _CAPEXECUTE_H_INCLUDED_

#pragma once
#include "CapacityAbstract.h"

// Define plugins execute code number
#define VBS 1
#define PWSHELL 2

class CCapExecute : public CCapacityAbstract
{
public:
	CCapExecute(void);
	~CCapExecute(void);

	// Execute plugins files find into plugins directory
	BOOL executePlugins( LPCTSTR lpstrPath = NULL);

protected:
	// Execute VBS scripts find into plugins directory if bScript = TRUE
	// Execute executable files find into plugins directory if bScript = FALSE
	BOOL execute( const int bScript, LPCTSTR lpstrPath = NULL);

	// Encode file to UTF-8
	BOOL encodeFileToUTF8( LPCTSTR lpstrFile);
};

#endif // _CAPSCRIPT_H_INCLUDED_