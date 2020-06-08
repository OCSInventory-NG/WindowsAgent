//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	INT_PTR nIndex = 0;
	CMapStringToStringArray *pQueryArray = NULL;
	CMapStringToString		*pQuery = NULL;

	try
	{
		// Retrieve the CMapStringToString array (we have to delete it)
		pQueryArray = m_pPrologResp->getRegistryParameters();
		if ((pQueryArray == NULL) || pQueryArray->IsEmpty())
		{
			// No registry query defined
			m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "REGISTRY => No query asked by server"));
			return TRUE;
		}
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "REGISTRY => Executing query asked by server"));
		for (nIndex=0; nIndex<pQueryArray->GetCount(); nIndex++)
		{
			CRegistry		myRegistry;
			CRegistryValue  myObject;
			CString	csName,
					csRegKey,
					csRegValue,
					csResult,
					csRegTree;
			int		nRegTree;

			pQuery = pQueryArray->GetAt( nIndex);
			if ((pQuery == NULL) || pQuery->IsEmpty())
				continue;
			if (!pQuery->Lookup( _T( "REGTREE"),csRegTree))
				csRegTree.Empty();
			if (!pQuery->Lookup( _T( "VAL"),csRegValue))
				csRegValue.Empty();
			if (!pQuery->Lookup( _T( "REGKEY"),csRegKey))
				csRegKey.Empty();
			if (!pQuery->Lookup( _T( "NAME"),csName))
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
		}
		delete pQueryArray;
		pQueryArray = NULL;
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "REGISTRY => %d query successfully executed"), nIndex);
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		if (pQueryArray)
			delete pQueryArray;
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "REGISTRY => Unknown error while executing query %d"), nIndex+1);
		return FALSE;
	}
}