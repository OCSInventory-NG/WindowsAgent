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
#include "SNMPException.h"
#include "SNMPObject.h"
#include "SNMPTracer.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"


SNMPAPI_STATUS CALLBACK snmpCallBackFunction( HSNMP_SESSION hSession, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam, LPVOID lpClientData) {
	SNMPSession * session = (SNMPSession*)lpClientData;
	return session->processNotification(wParam, lParam);
}



SNMPSession::SNMPSession() {
	_hSession = SnmpCreateSession (0, 0, snmpCallBackFunction, this);
	if(SNMPAPI_FAILURE == _hSession) {
		SNMP_THROW_ERROR("Could not create WinSNMP session", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	SNMPManager::getManager()->registerSession(this);
}

SNMPSession::~SNMPSession() {
	SNMPManager::getManager()->unregisterSession(this);
	closeSession();
}

void SNMPSession::closeSession() {
	std::map<HSNMP_CONTEXT,SNMPRemoteAgent*>::iterator i;
	while((i=_ragents.begin()) != _ragents.end()) {
		delete (*i).second;
	}
	SnmpClose(_hSession);
}

SNMPAPI_STATUS SNMPSession::processNotification(WPARAM wParam, LPARAM lParam) {
	if(wParam == 0) {
		HSNMP_PDU hPdu;
		HSNMP_CONTEXT hContext;
		HSNMP_ENTITY hDstEntity;
		HSNMP_ENTITY hSrcEntity;
		SNMPAPI_STATUS ret = SnmpRecvMsg(_hSession, &hSrcEntity, &hDstEntity, &hContext, &hPdu);
		SNMPRemoteAgent * pRa = getRemoteAgent(hContext);		
		int requestId = (DWORD)lParam;
		SNMPRequest * pRequest = NULL;
		if(pRa) {
			pRequest = pRa->getRequest(requestId);
		}
		if(ret == SNMPAPI_FAILURE) {
			if(pRequest) {
				pRequest->setResult(SNMP_ERROR_GENERR);
			}
			SnmpFreeVbl(hPdu);
			return SNMPAPI_SUCCESS;
		}	
		smiINT32 lReqId, lType, lErr, lIdx;
		HSNMP_VBL hVbl;
		ret = SnmpGetPduData(hPdu, &lType, &lReqId, &lErr, &lIdx, &hVbl);
		if(ret == SNMPAPI_FAILURE) {
			if(pRequest) {
				pRequest->setResult(SNMP_ERROR_GENERR);
			}
			SnmpFreeVbl(hPdu);
			return SNMPAPI_SUCCESS;
		}
		if(SNMPTRACEAENABLED()) {
			std::stringstream trace;
			trace << "rx " << SNMPManager::pduToStr(hDstEntity, hSrcEntity, hContext, lType, lReqId, lErr, hVbl, false);
			SNMPTRACE(trace.str());
		}
		if(lType != SNMP_PDU_RESPONSE) {
			pRequest = NULL;
		}
		if(pRequest == NULL && lType != SNMP_PDU_TRAP && lType != SNMP_PDU_INFORM) {
			SnmpFreeVbl(hVbl);
			SnmpFreeVbl(hPdu);
			return SNMPAPI_SUCCESS;
		}

		smiOID dRetName;
		smiVALUE dRetValue;
		int count = SnmpCountVbl(hVbl);
		if(count) {
			char szName[MAXOBJIDSTRSIZE+1];
			szName[0] = szName[MAXOBJIDSTRSIZE] = '\0';
			for(int i=0; i<count; i++) {
				if(SnmpGetVb(hVbl, i+1, &dRetName, &dRetValue) == SNMPAPI_FAILURE) {
					SnmpFreeVbl(hVbl);
					SnmpFreePdu(hPdu);
					if(pRequest) {
						pRequest->setResult(SNMP_ERROR_GENERR);
					}
					return SNMPAPI_SUCCESS;
				}
				if(SnmpOidToStr(&dRetName, sizeof(szName), szName) == SNMPAPI_FAILURE) {
					szName[0] = '\0';
				}
				SNMPObject obj;
				obj.setValue(&dRetValue);
				obj.setOID(szName);
				if(pRequest) {
					pRequest->addToResult(obj);
				} else {
					processTrap(obj);
				}
				SnmpFreeDescriptor(SNMP_SYNTAX_OID, (smiLPOPAQUE)&dRetName);
				if(dRetValue.syntax == SNMP_SYNTAX_OID) {
					SnmpFreeDescriptor(dRetValue.syntax, (smiLPOPAQUE)&dRetValue.value.oid);
				} else if(	dRetValue.syntax == SNMP_SYNTAX_OPAQUE ||
							dRetValue.syntax == SNMP_SYNTAX_OCTETS ||
							dRetValue.syntax == SNMP_SYNTAX_BITS ||
							dRetValue.syntax == SNMP_SYNTAX_IPADDR ||
							dRetValue.syntax == SNMP_SYNTAX_NSAPADDR) {
					SnmpFreeDescriptor(dRetValue.syntax, (smiLPOPAQUE)&dRetValue.value.string);
				}
			}
		}
		SnmpFreeVbl(hVbl);
		SnmpFreePdu(hPdu);
		if(pRequest) {
			pRequest->setResult(lErr, false);
		}
		return SNMPAPI_SUCCESS;
	}
	return SNMPAPI_SUCCESS;
}


void SNMPSession::unregisterRemoteAgent(SNMPRemoteAgent * ragent) {
	_ragents.erase(ragent->getContextHandle());
}

void SNMPSession::registerRemoteAgent(SNMPRemoteAgent * ragent) {
	_ragents[ragent->getContextHandle()] = ragent;
}

/*
int SNMPSession::getNextRequestId()
{
	_id++;
	if(!_id) {
		_id++;
	}
	return _id;
}
*/

SNMPRemoteAgent * SNMPSession::getRemoteAgent(HSNMP_CONTEXT handle) {
	std::map<HSNMP_CONTEXT,SNMPRemoteAgent*>::const_iterator pos;
	pos = _ragents.find(handle);
	if(pos == _ragents.end()) {
		return NULL;
	}
	return pos->second;
}
