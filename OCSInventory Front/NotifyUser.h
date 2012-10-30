//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// NotifyUser.h : main header file for the CNotifyUser
//
#ifndef _OCS_NOTIFY_USER_H_INCLUDED_
#define  _OCS_NOTIFY_USER_H_INCLUDED_

#pragma once

#include "stdafx.h"
#include "OCSInventory Front Classes.h"

#define OCS_NO_WTS_SESSION_ID		0xFFFFFFFF
#define OCS_NOTIFY_USER_COMMAND		_T( "OcsNotifyUser.exe")

// Notification exit codes
#define OCS_NOTIFY_APP_OK						0
#define OCS_NOTIFY_APP_CANCEL					1
#define OCS_NOTIFY_APP_DELAY					2
#define OCS_NOTIFY_APP_GENERIC_ERROR			3
#define OCS_NOTIFY_APP_ALREADY_RUNNING_ERROR	4

#define USER_ANSWER_TIME_OUT					120 // 120 minutes

class OCSINVENTORYFRONT_API CNotifyUser
{
public:
	CNotifyUser(void);
	~CNotifyUser(void);

	// Ask a question to user
	// Return must be 0 = OK, 1 = Cancel, otherwise error
	UINT AskQuestion( LPCTSTR lpstrText);

	// Display an information to userusing MessageBox
	// Return must be 0 = OK, otherwise error
	UINT ShowInformation( LPCTSTR lpstrText);

	// Display pre-installation status message
	// Return must be 0 = OK, 1 = Cancel, 2 = Delay, otherwise error
	UINT ShowPreInstall( LPCTSTR lpstrText, BOOL bCancelAllowed = FALSE, BOOL bDelayAllowed = FALSE, UINT uTimeOut = 0);

	// Display post-installation status message
	// Return must be 0 = OK, 2 = Delay, otherwise error
	UINT ShowPostInstall( LPCTSTR lpstrText, BOOL bRebootRequired = FALSE, BOOL bDelayAllowed = FALSE, UINT uTimeOut = 0);

	// Ask TAG to user and store result into csTag
	// Return must be 0 = OK, otherwise error
	UINT AskTag( LPCTSTR lpstrText, CString &csTag);

private:
	BOOL getActiveConsoleSessionID();
	UINT display( CString &csCommand);
	UINT displayAsDefaultUser( CString &csCommand); 
	UINT displayAsConnectedUser( CString &csCommand);

	HANDLE m_hUserToken;		// Active console session user token or NULL
	BOOL  m_bIsVistaOrHigher;	// Is Vista or higher
	UINT  m_uResult;
	DWORD m_dwTimeOut;
};

#endif // _OCS_NOTIFY_USER_H_INCLUDED_
