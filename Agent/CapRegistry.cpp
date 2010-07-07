//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// CapRegistry.cpp: implementation of the CCapRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapRegistry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CCapRegistry::CCapRegistry()
{		
}

CCapRegistry::~CCapRegistry()
{
}


/***
 *
 * Retrieve the keys from registry
 *
 ***/
BOOL CCapRegistry::retrieveKeys()
{
	// Retrieve the CMapStringToString array (we have to delete it, or it will be done in destructor)
	DWORD dwParameterNumber = 0;
	m_pParameters = m_pPrologResp->getRegistryParameters();

	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "REGISTRY => Starting scanning"), dwParameterNumber);

	while (!m_pParameters[dwParameterNumber].IsEmpty())
	{
		CRegistry		myRegistry;
		CRegistryValue  myObject;
		CString	csName,
				csRegKey,
				csRegValue,
				csResult,
				csRegTree;
		int		nRegTree;

		if (!m_pParameters[dwParameterNumber].Lookup( _T( "REGTREE"),csRegTree))
			csRegTree.Empty();
		if (!m_pParameters[dwParameterNumber].Lookup( _T( "VAL"),csRegValue))
			csRegValue.Empty();
		if (!m_pParameters[dwParameterNumber].Lookup( _T( "REGKEY"),csRegKey))
			csRegKey.Empty();
		if (!m_pParameters[dwParameterNumber].Lookup( _T( "NAME"),csName))
			csName.Empty();
		
		nRegTree = _ttoi( csRegTree );

		if (csRegValue == REGISTRY_ALL_VALUES_OF_KEY)
		{
			// Get all values of a key
			myRegistry.GetRegistryMultipleValues( csName, nRegTree, csRegKey, &(m_pInventory->m_RegistryList));
		}
		else
		{
			// Get single value
			if (myRegistry.GetRegistryValue( nRegTree, csRegKey, csRegValue, csResult))
			{
				// Add result to list
				myObject.Set( csName, csResult);
				m_pInventory->m_RegistryList.AddTail( myObject);								
			}
		}
		dwParameterNumber++;							
	}
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "REGISTRY => scan finished (%d query executed)"), dwParameterNumber);
	return TRUE;
}