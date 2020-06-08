//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#pragma once

#include "OCSInventory Front Classes.h"

class OCSINVENTORYFRONT_API CMapStringToStringArray
{
public:
	CMapStringToStringArray(void);
	~CMapStringToStringArray(void);

	// Operations
	INT_PTR GetCount() const;
	BOOL IsEmpty() const;

	// Clean up
	void RemoveAll();

	// Accessing elements
	CMapStringToString * GetAt(INT_PTR nIndex);
	void SetAt(INT_PTR nIndex, CMapStringToString * newElement);

	// Potentially growing the array
	INT_PTR Add(CMapStringToString * newElement);

	// overloaded operator helpers
	CMapStringToString * operator[](INT_PTR nIndex);

protected:
	CArray< CMapStringToString *, CMapStringToString *> m_MapArray;
};
