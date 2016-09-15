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
#include "SNMPManager.h"


class SNMPSession;
class SNMPRemoteAgent;
class SNMPRequestSet;

#define SNMP_RAREQ_STATE_NONE		0
#define SNMP_RAREQ_STATE_ONGOING	1
#define SNMP_RAREQ_STATE_TIMEDOUT	3
#define SNMP_RAREQ_STATE_SUCCEEDED	4
#define SNMP_RAREQ_STATE_ERROR		5
#define SNMP_RAREQ_STATE_CANCELLED	6

class SNMPRequest {
	friend class SNMPRemoteAgent;
	friend class SNMPSession;
	friend class SNMPRequestSet;
protected :
	std::vector<SNMPObject*> _result;	
	std::vector<SNMPObject*> _request;
	HSNMP_PDU _hPdu;
	HSNMP_VBL _hVbl;
	HANDLE _hWait;
	int _requestId;
	int _error;
	int _state;
	int _pduType;
	SNMPRemoteAgent * _pRemoteAgent;
protected :	
	SNMPRequest(int pduType, SNMPObject * objects, int count, SNMPRemoteAgent * pRemoteAgent);	
	void addToResult(SNMPObject & object);
	void setResult(int error = SNMP_ERROR_NOERROR, bool traceIt = true);
	int getRequestCount()			{ return (int)_request.size(); }
	SNMPObject * getRequest(int i)	{ return _request[i]; }

public:	
	virtual ~SNMPRequest();

public:
	void execute(SNMPRemoteAgent * pRemoteAgent = NULL);
	void cancel();
	void wait(DWORD timeout = INFINITE);

	bool isTerminated()		{ return _state != SNMP_RAREQ_STATE_ONGOING; }
	bool timedOut()			{ return _state == SNMP_RAREQ_STATE_TIMEDOUT; }
	bool succeeded()		{ return _state == SNMP_RAREQ_STATE_SUCCEEDED; }

	int getError()					{ return _error; }
	std::string getErrorAsString();

	int getResultCount()			{ return (int)_result.size(); }
	SNMPObject * getResult(int i)	{ return _result[i]; }

	std::string getDisplayInformation() const;

	SNMPRequest * clone();
};

class SNMPRemoteAgentGetRequest : public SNMPRequest {
public:
	SNMPRemoteAgentGetRequest(SNMPObject * objects, int count=1, SNMPRemoteAgent * pRemoteAgent = NULL) : SNMPRequest(SNMP_PDU_GET, objects, count, pRemoteAgent) {}
	virtual ~SNMPRemoteAgentGetRequest() {}
};

class SNMPRemoteAgentGetNextRequest : public SNMPRequest {
public:
	SNMPRemoteAgentGetNextRequest(SNMPObject * objects, int count=1, SNMPRemoteAgent * pRemoteAgent = NULL) : SNMPRequest(SNMP_PDU_GETNEXT, objects, count, pRemoteAgent) {}
	virtual ~SNMPRemoteAgentGetNextRequest() {}
};

class SNMPRemoteAgentSetRequest : public SNMPRequest {
public:
	SNMPRemoteAgentSetRequest(SNMPObject * objects, int count=1, SNMPRemoteAgent * pRemoteAgent = NULL) : SNMPRequest(SNMP_PDU_SET, objects, count, pRemoteAgent) {}
	virtual ~SNMPRemoteAgentSetRequest() {}
};


