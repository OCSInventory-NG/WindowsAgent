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
#include "SNMPRequest.h"
#include "SNMPRemoteAgent.h"
#include "SNMPSession.h"
#include "SNMPObject.h"
#include "SNMPException.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"


std::string SNMPRequest::getDisplayInformation() const
{
	std::stringstream ss;
	ss << SNMPManager::pduTypeToStr(_pduType) << " Request: ";		
	switch(_state) {
		case SNMP_RAREQ_STATE_NONE: ss << "NONE"; break;
		case SNMP_RAREQ_STATE_ONGOING: ss << "ONGOING"; break;
		case SNMP_RAREQ_STATE_TIMEDOUT: ss << "TIMEDOUT"; break;
		case SNMP_RAREQ_STATE_SUCCEEDED: ss << "SUCCEEDED"; break;
		case SNMP_RAREQ_STATE_ERROR: ss << "ERROR"; break;
		case SNMP_RAREQ_STATE_CANCELLED: ss << "CANCELLED"; break;
		default: ss << "?"; break;
	}
	ss << "(" << _error << ") [";
	int i;
	for(i=0; i<(int)_request.size(); i++) {
		ss << _request[i]->getOID();
		if(i < (int)_request.size()-1) {
			ss << ", ";
		}
	}
	ss << "] = [";
	for(i=0; i<(int)_result.size(); i++) {
		ss <<_result[i]->getDisplayInformation();
		if(i < (int)_result.size()-1) {
			ss << ", ";
		}
	}
	ss << "]";

	return ss.str();
}

SNMPRequest * SNMPRequest::clone()
{
	SNMPRequest * myclone = new SNMPRequest(_pduType, NULL, 0, _pRemoteAgent);
	for(int i=0; i<(int)_request.size(); i++) {
		myclone->_request.insert(myclone->_request.end(), new SNMPObject(*_request[i]));
	}
	return myclone;
}

SNMPRequest::SNMPRequest(int pduType, SNMPObject * objects, int count, SNMPRemoteAgent * pRemoteAgent) {
	_pduType = pduType;
	_state = SNMP_RAREQ_STATE_NONE;
	_error = SNMP_ERROR_NOERROR;
	_requestId = 0;
	_hWait = ::CreateEvent (NULL, TRUE, FALSE, NULL);
	if(_hWait == NULL) {
		_error = SNMP_ERROR_GENERR;
		SNMP_THROW_ERROR("Failed to create event", _error, SNMP_ERROR);
	}
	_pRemoteAgent = pRemoteAgent;
	for(int i=0; i<count; i++) {
		_request.insert(_request.end(), new SNMPObject(objects[i]));
	}
}

SNMPRequest::~SNMPRequest()
{
	if(!isTerminated()) {
		cancel();
	}
	if(_hWait) {
		::CloseHandle(_hWait);
	}
	while((int)_request.size()) {
		delete _request[0];
		_request.erase(_request.begin());
	}
	while((int)_result.size()) {
		delete _result[0];
		_result.erase(_result.begin());
	}
}


std::string SNMPRequest::getErrorAsString()	{ 
	return SNMPManager::pduErrorToStr(_error); 
}

void SNMPRequest::execute(SNMPRemoteAgent * pRemoteAgent) {
	if(!isTerminated()) {
		cancel();
	}	
	while((int)_result.size()) {
		delete _result[0];
		_result.erase(_result.begin());
	}
	_state = SNMP_RAREQ_STATE_ONGOING;
	_error = SNMP_ERROR_NOERROR;
	if(pRemoteAgent) {
		_pRemoteAgent = pRemoteAgent;
	}

	SNMPSession * session = _pRemoteAgent->getSession();
	//_requestId = session->getNextRequestId();
	smiVALUE dValue;
	int i;
	int count = getRequestCount();
	dValue.syntax = SNMP_SYNTAX_NULL;
	dValue.value.uNumber = 0;	
	_hVbl = SnmpCreateVbl(session->getHandle(), NULL, NULL);
	if(_hVbl == SNMPAPI_FAILURE) {
		_requestId = 0;
		_state = SNMP_RAREQ_STATE_ERROR;
		_error = SnmpGetLastError(session->getHandle());
		SNMP_THROW_ERROR("Could not create variable bindings list", _error, SNMP_ERROR);
	}
	for(i=0; i<count; i++) {
		SNMPObject *pObj = getRequest(i);
		smiOID * pOid = pObj->getBinaryOID();
		if(_pduType == SNMP_PDU_SET) {
			pObj->getValue(&dValue);
		}		
		if(pOid==NULL || SnmpSetVb(_hVbl, 0, pOid, &dValue) == SNMPAPI_FAILURE) {
			SnmpFreeVbl(_hVbl);
			_requestId = 0;
			_state = SNMP_RAREQ_STATE_ERROR;
			_error = SNMP_ERROR_GENERR;
			SNMP_THROW_ERROR("Could not add object to the variable bindings list", SnmpGetLastError(session->getHandle()), SNMP_ERROR);
		}
		if(_pduType == SNMP_PDU_SET) {
			if(dValue.syntax == SNMP_SYNTAX_OID) {
				SnmpFreeDescriptor(dValue.syntax, (smiLPOPAQUE)&dValue.value.oid);
			} else if(	dValue.syntax == SNMP_SYNTAX_OPAQUE ||
						dValue.syntax == SNMP_SYNTAX_OCTETS ||
						dValue.syntax == SNMP_SYNTAX_BITS ||
						dValue.syntax == SNMP_SYNTAX_IPADDR ||
						dValue.syntax == SNMP_SYNTAX_NSAPADDR) {
				free(dValue.value.string.ptr);
			}
		}
	}
	_hPdu = SnmpCreatePdu(session->getHandle(), _pduType, 0, 0, 0, _hVbl);
	if(_hPdu == SNMPAPI_FAILURE) {
		SnmpFreeVbl(_hVbl);
		_requestId = 0;
		_state = SNMP_RAREQ_STATE_ERROR;
		_error = SNMP_ERROR_GENERR;
		SNMP_THROW_ERROR("Could not create PDU", SnmpGetLastError(session->getHandle()), SNMP_ERROR);
	}
	smiINT32 id;
	if(SnmpGetPduData(_hPdu, NULL, &id, NULL, NULL, NULL) == SNMPAPI_FAILURE) {
		SnmpFreeVbl(_hVbl);
		_requestId = 0;
		_state = SNMP_RAREQ_STATE_ERROR;
		_error = SNMP_ERROR_GENERR;
		SNMP_THROW_ERROR("Could not query PDU request id", SnmpGetLastError(session->getHandle()), SNMP_ERROR);
	}
	_requestId = id;
	_pRemoteAgent->sendRequest(this);
}

void SNMPRequest::cancel() {	
	if(isTerminated()) {
		return;
	}
	_pRemoteAgent->cancelRequest(_requestId);
	if(SNMPTRACEAENABLED()) {
		std::stringstream trace;
		trace << "rx < cancelled(" << _requestId << ")";
		SNMPTRACE(trace.str());
	}
	_requestId = 0;
	SnmpFreePdu(_hPdu);
	SnmpFreeVbl(_hVbl);
	_error = SNMP_ERROR_NOERROR;
	_state = SNMP_RAREQ_STATE_CANCELLED;
	::ResetEvent(_hWait);
}

void SNMPRequest::addToResult(SNMPObject & object) {
	_result.insert(_result.end(), new SNMPObject(object));
}

void SNMPRequest::setResult(int error, bool traceIt) {
	if(isTerminated()) {
		return;
	}
	SnmpFreePdu(_hPdu);
	SnmpFreeVbl(_hVbl);
	_error = error;
	if(_error != SNMP_ERROR_NOERROR) {
		_state = SNMP_RAREQ_STATE_ERROR;
		if(traceIt && SNMPTRACEAENABLED()) {
			std::stringstream trace;
			trace << "rx < error(" << _requestId << ", " << _error << ")";
			SNMPTRACE(trace.str());
		}
	} else {
		_state = SNMP_RAREQ_STATE_SUCCEEDED;
	}
	::SetEvent(_hWait);
}

void SNMPRequest::wait(DWORD timeout) {
	DWORD waitRet = ::WaitForSingleObject(_hWait, timeout);
	::ResetEvent(_hWait);
	// _state, _error and result set by WinSNMP!
	_pRemoteAgent->cancelRequest(_requestId);	
	SnmpFreePdu(_hPdu);
	SnmpFreeVbl(_hVbl);
	if(waitRet == WAIT_FAILED) {
		_state = SNMP_RAREQ_STATE_ERROR;
		_error = SNMP_ERROR_GENERR;
		_requestId = 0;
		SNMP_THROW_ERROR("Wait failed", _error, SNMP_ERROR);
	}
	if(waitRet == WAIT_TIMEOUT || _state == SNMP_RAREQ_STATE_ONGOING) {	
		_state = SNMP_RAREQ_STATE_TIMEDOUT;
		_error = SNMP_ERROR_NOERROR;
		if(SNMPTRACEAENABLED()) {
			std::stringstream trace;
			trace << "rx < timeout(" << _requestId << ", " << timeout << ")";
			SNMPTRACE(trace.str());
		}
	}
	_requestId = 0;
}


