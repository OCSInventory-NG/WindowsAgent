//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// BlackList.h: interface for the CBlackList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BLACKLIST_H_INCLUDED_
#define _BLACKLIST_H_INCLUDED_

#pragma once

class CBlackList
{
public:
	CBlackList(void);
	~CBlackList(void);

	// Add an ID to the blacklist
	void Add( LPCTSTR lpstrID);
	// Find if an ID is blacklisted
	BOOL IsListed( LPCTSTR lpstrID);

protected:
	CString m_csBlackList;
};

#endif // _BLACKLIST_H_INCLUDED_