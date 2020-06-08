//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapRegistry.h: interface for the CCapRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__8AC92BC2_61C6_4D17_AD76_B4EF462378AD__INCLUDED_)
#define AFX_REGISTRY_H__8AC92BC2_61C6_4D17_AD76_B4EF462378AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CapacityAbstract.h"

class CCapRegistry : public CCapacityAbstract
{
public: // Methods

	// Standard constructor and destructor
	CCapRegistry();
	virtual ~CCapRegistry();
	// Do the job
	BOOL retrieveKeys();
};

#endif // !defined(AFX_REGISTRY_H__8AC92BC2_61C6_4D17_AD76_B4EF462378AD__INCLUDED_)
