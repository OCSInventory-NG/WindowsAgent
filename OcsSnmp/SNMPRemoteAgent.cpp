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

#include "SNMPManager.h"
#include "SNMPRemoteAgent.h"
#include "SNMPSession.h"
#include "SNMPObject.h"
#include "SNMPException.h"
#include "SNMPRequest.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"


SNMPRemoteAgent::SNMPRemoteAgent(std::string name, std::string community, unsigned int port, SNMPSession * pSession) {
	_name = name;
	_port = port;
	_community = community;
	_bInit = false;
	_mySession = false;
	_pSession = pSession;
	_hEntity = SNMPAPI_FAILURE;
	_hContext = SNMPAPI_FAILURE;
	if(_pSession == NULL) {
		try {
			_pSession = new SNMPSession();
		} catch(SNMPException * pe) {
			SNMP_RETHROW_ERROR("Could not create WinSNMP session", pe, SNMPAPI_OTHER_ERROR, SNMP_ERROR);
		}
		_mySession = true;
	}
	HSNMP_SESSION hSession = _pSession->getHandle();
	if(hSession == SNMPAPI_FAILURE) {
		if(_mySession && _pSession != NULL) {
			delete _pSession;
		}
		_pSession = NULL;		
		SNMP_THROW_ERROR("Invalid session", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	_hEntity = SnmpStrToEntity(hSession, _name.c_str());
	if (_hEntity == SNMPAPI_FAILURE) {
		LPHOSTENT lpHostent = gethostbyname(_name.c_str());
		IN_ADDR host;
		if(!lpHostent) {
			if(_mySession && _pSession != NULL) {
				delete _pSession;
			}
			_pSession = NULL;
			SNMP_THROW_ERROR("Could not get host by name", SNMPAPI_OTHER_ERROR, SNMP_ERROR);
		}
		memmove (&host, lpHostent->h_addr, sizeof(IN_ADDR));
		_hEntity = SnmpStrToEntity(hSession, inet_ntoa(host));
		if (_hEntity == SNMPAPI_FAILURE) {
			if(_mySession && _pSession != NULL) {
				delete _pSession;
			}
			_pSession = NULL;
			SNMP_THROW_ERROR("Could not convert remote agent address to entity", SnmpGetLastError(hSession), SNMP_ERROR);
		}
	}
	if(port != 0) {
		if(SnmpSetPort(_hEntity, port) != SNMPAPI_SUCCESS) {
			SnmpFreeEntity(_hEntity);
			_hEntity = SNMPAPI_FAILURE;
			if(_mySession && _pSession != NULL) {
				delete _pSession;
			}
			_pSession = NULL;
			SNMP_THROW_ERROR("Could not set remote agent port", SnmpGetLastError(hSession), SNMP_ERROR);
		}
	}
	smiOCTETS dCtx;
	dCtx.len = (smiUINT32)strlen(community.c_str());
	dCtx.ptr = (smiLPBYTE)community.c_str();
	_hContext = SnmpStrToContext(hSession, &dCtx);
	if (_hContext == SNMPAPI_FAILURE)  {
		SnmpFreeEntity(_hEntity);
		_hEntity = SNMPAPI_FAILURE;
		if(_mySession && _pSession != NULL) {
			delete _pSession;
		}
		_pSession = NULL;
		SNMP_THROW_ERROR("Could not convert community name to context", SnmpGetLastError(hSession), SNMP_ERROR);
	}
	_pSession->registerRemoteAgent(this);
	_bInit = true;
}


std::string SNMPRemoteAgent::getDisplayInformation() const
{
	std::stringstream ss;
	if(_port != 0) {
		ss << "Remote Agent " << _name << ":" << _port << " " << _community;	
	} else {
		ss << "Remote Agent " << _name << " " << _community;	
	}
	return ss.str();
}

SNMPRemoteAgent::~SNMPRemoteAgent() {
	if(_bInit) {
		_pSession->unregisterRemoteAgent(this);
		SnmpFreeContext(_hContext);
		SnmpFreeEntity(_hEntity);
		_bInit = false;
	}
	if(_mySession && _pSession != NULL) {
		delete _pSession;
	}
	std::map<int,SNMPRequest*>::iterator i;
	while((i=_requestMap.begin()) != _requestMap.end()) {
		cancelRequest((*i).first);
	}
}

SNMPRequest* SNMPRemoteAgent::getRequest(int id) {
	std::map<int,SNMPRequest*>::const_iterator pos;
	pos = _requestMap.find(id);
	if(pos == _requestMap.end()) {
		return NULL;
	}
	return pos->second;
}

void SNMPRemoteAgent::sendRequest(SNMPRequest * pRequest) {
	if(!_bInit) {
		pRequest->setResult(SNMP_ERROR_GENERR);
		SNMP_THROW_ERROR("Not initialized", SNMPAPI_OTHER_ERROR, SNMP_ERROR);
	}
	HSNMP_PDU hPDU = pRequest->_hPdu;
	int id = pRequest->_requestId;
	addToRequestMap(id, pRequest);
	if(SNMPTRACEAENABLED()) {
		std::stringstream trace;
		smiINT32 lReqId, lType, lErr, lIdx;
		HSNMP_VBL hVbl;
		SnmpGetPduData(hPDU, &lType, &lReqId, &lErr, &lIdx, &hVbl);
		trace << "tx " << SNMPManager::pduToStr(_hEntity, 0, _hContext, lType, lReqId, lErr, hVbl, true);
		SnmpFreeVbl(hVbl);
		SNMPTRACE(trace.str());
	}
	if(SnmpSendMsg(_pSession->getHandle(), 0, _hEntity, _hContext, hPDU) == SNMPAPI_FAILURE) {
		pRequest->setResult(SNMP_ERROR_GENERR);
		//SNMP_THROW_ERROR("Could not send PDU", SnmpGetLastError(_pSession->getHandle()), SNMP_ERROR);
	}
}

void SNMPRemoteAgent::cancelRequest(int id) {
	SNMPRequest* pRequest = getRequest(id);
	if(pRequest) {
		SnmpCancelMsg(_pSession->getHandle(), id);
		removeFromRequestMap(id);
	}
}
