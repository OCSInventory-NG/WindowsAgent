//
//  SNMP - A C++ Wrapper for the WinSnmp library
//  Copyright (C) 2004  Rogério Paulo, rpaulo@bigfoot.com
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#pragma once 

#include "Winsnmp.h"
#include <string>
#include <map>
#include "SNMPRemoteAgent.h"
#include "SNMPRequest.h"
#include "SNMPTrap.h"

class SNMPSession {

friend class SNMPRemoteAgent;
friend class SNMPManager;
friend class SNMPRequest;
friend class SNMPTrap;
friend SNMPAPI_STATUS CALLBACK snmpCallBackFunction( HSNMP_SESSION hSession, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam, LPVOID lpClientData);

protected:
	HSNMP_SESSION _hSession;
	SNMPSession(HSNMP_SESSION hSession);
	void closeSession();

	std::map<HSNMP_CONTEXT,SNMPRemoteAgent*> _ragents;
	SNMPRemoteAgent * getRemoteAgent(HSNMP_CONTEXT handle);
	void unregisterRemoteAgent(SNMPRemoteAgent * ragent);
	void registerRemoteAgent(SNMPRemoteAgent * ragent); 
	SNMPAPI_STATUS processNotification(WPARAM wParam, LPARAM lParam);
	
	const HSNMP_SESSION getHandle() const { return _hSession; }
public:
	SNMPSession();
	~SNMPSession();
	
	virtual void processTrap(const SNMPObject & object) {}
};


