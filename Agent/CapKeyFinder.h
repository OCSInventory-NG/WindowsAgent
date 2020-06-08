//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#pragma once

#include "OCSInventory.h"

class CCapKeyFinder : public CCapacityAbstract
{
public:
	CCapKeyFinder(void);
	~CCapKeyFinder(void);
	// Do the job
	BOOL retrieveKeys();

protected:
	BOOL decodeMicrosoftKey( LPBYTE lpData, UINT uLength, CString csKey);
};
