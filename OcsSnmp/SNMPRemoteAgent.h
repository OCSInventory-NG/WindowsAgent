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
#include <vector>
#include <map>
#include "SNMPObject.h"

class SNMPSession;
class SNMPRemoteAgent;
class SNMPRequest;
class SNMPRequestSet;
class SNMPTrap;

class SNMPRemoteAgent {
	friend class SNMPSession;
	friend class SNMPRequest;
	friend class SNMPRequestSet;
	friend class SNMPTrap;
protected:
	std::string _name;
	std::string _community;
	UINT _port;
	bool _bInit;
	bool _mySession;
	SNMPSession * _pSession;
	HSNMP_ENTITY _hEntity;
	HSNMP_CONTEXT _hContext;

	HSNMP_ENTITY getCommunityHandle() { return _hEntity; }
	HSNMP_CONTEXT getContextHandle() { return _hContext; }
	SNMPSession * getSession() { return _pSession; }

	std::map<int,SNMPRequest*> _requestMap;
	void addToRequestMap(int id, SNMPRequest* pRequest) { _requestMap[id] = pRequest; }
	void removeFromRequestMap(int id) { 
		_requestMap.erase(id); 
	}
	SNMPRequest* getRequest(int id);
    
	void sendRequest(SNMPRequest * pRequest);
	void cancelRequest(int id);

public:	
	SNMPRemoteAgent(std::string name, std::string community, unsigned int port = 0, SNMPSession * pSession = NULL);
	~SNMPRemoteAgent();

	std::string	getName() { return _name; }
	std::string	getCommunity() { return _community; }

	virtual std::string getDisplayInformation() const;
};


