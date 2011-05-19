//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// BlackList.cpp: implementation of the CBlackList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlackList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBlackList::CBlackList(void)
{
	m_csBlackList.Empty();
}

CBlackList::~CBlackList(void)
{
	m_csBlackList.Empty();
}

void CBlackList::Add( LPCTSTR lpstrID)
{
	m_csBlackList.AppendFormat( _T( "/%s/"), lpstrID);
}

BOOL CBlackList::IsListed( LPCTSTR lpstrID)
{
	CString csID;

	csID.Format( _T( "/%s/"), lpstrID);
	return (m_csBlackList.Find( csID) != -1);
}
