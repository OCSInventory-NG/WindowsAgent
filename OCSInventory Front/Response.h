//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Response.h: interface for the CResponse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESPONSE_H__CAEA5EC4_E324_4886_B91E_AD159045AE39__INCLUDED_)
#define AFX_RESPONSE_H__CAEA5EC4_E324_4886_B91E_AD159045AE39__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "ResponseAbstract.h"

class OCSINVENTORYFRONT_API CResponse  : public CResponseAbstract
{

public: // Methods
	
	// Standard constructor and destructor
	CResponse( CByteArray* );
	// Empty response object
	CResponse(){};
	virtual ~CResponse();
};

#endif // !defined(AFX_RESPONSE_H__CAEA5EC4_E324_4886_B91E_AD159045AE39__INCLUDED_)
