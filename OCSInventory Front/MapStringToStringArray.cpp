//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================


#include "stdafx.h"
#include "MapStringToStringArray.h"

CMapStringToStringArray::CMapStringToStringArray(void)
{
	m_MapArray.SetSize( MAPSTRINGTOSTRINGARRAY_INITIAL_SIZE, MAPSTRINGTOSTRINGARRAY_GROW_SIZE);
}

CMapStringToStringArray::~CMapStringToStringArray(void)
{
	RemoveAll();
}

INT_PTR CMapStringToStringArray::GetCount() const
{
	return m_MapArray.GetCount();
}

BOOL CMapStringToStringArray::IsEmpty() const
{
	return m_MapArray.IsEmpty();
}

// Operations
// Clean up
void CMapStringToStringArray::RemoveAll()
{
	CMapStringToString *pMap;

	// Delete all allocated CMapStringToString referenced
	for (INT_PTR nIndex = 0; nIndex < m_MapArray.GetCount(); nIndex++)
	{
		pMap = m_MapArray.GetAt( nIndex);
		if (pMap)
			delete pMap;
		m_MapArray.SetAt( nIndex, NULL);
	}
	m_MapArray.RemoveAll();
}

// Accessing elements
CMapStringToString * CMapStringToStringArray::GetAt(INT_PTR nIndex)
{
	return m_MapArray.GetAt( nIndex);
}

void CMapStringToStringArray::SetAt(INT_PTR nIndex, CMapStringToString * newElement)
{
	m_MapArray.SetAt( nIndex, newElement);
}

// Potentially growing the array
INT_PTR CMapStringToStringArray::Add(CMapStringToString * newElement)
{
	return m_MapArray.Add( newElement);
}

// overloaded operator helpers
CMapStringToString * CMapStringToStringArray::operator[](INT_PTR nIndex)
{
	return m_MapArray[nIndex];
}

