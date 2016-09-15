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

#include <sstream>
#include <iostream>

#define __VER__ "1.0"

#pragma warning(disable : 4996)

SNMPManager SNMPManager::_snmpManager;

SNMPManager::SNMPManager() {
	_init = false;
	_nMajorVersion = 0;
	_nMinorVersion = 0;
	_nLevel = 0;
}

SNMPManager::~SNMPManager() {
	cleanUp();
}

void SNMPManager::startup() {	
	if(!_init) {
		smiUINT32 nTranslateMode;
		smiUINT32 nRetransmitMode;
		if(SnmpStartup(&_nMajorVersion, &_nMinorVersion, &_nLevel, &nTranslateMode, &nRetransmitMode) != SNMPAPI_SUCCESS) {			
			SNMP_THROW_ERROR("Could not start WinSNMP API", SnmpGetLastError(NULL), SNMP_ERROR);
		}
		SnmpSetTranslateMode(SNMPAPI_UNTRANSLATED_V1);
		SnmpSetRetransmitMode(SNMPAPI_OFF);
		_init = true;
	}
}


std::string SNMPManager::getDisplayInformation() const
{
	std::stringstream ss;
	ss << "WinSNMP API v" << _nMajorVersion << "." << _nMinorVersion << " level " << _nLevel;
	smiVENDORINFO vi;
	if(SnmpGetVendorInfo(&vi) == SNMPAPI_SUCCESS) {
		ss << " - " << vi.vendorName << " (" << vi.vendorEnterprise << ") " << vi.vendorContact;
		ss << " - API " << vi.vendorVersionId << " " << vi.vendorVersionDate;
	} else {
		ss << " - Vendor and API information not available";
	}	

	smiUINT32 nRetransmitMode;
	ss << " - (r=";
	if(SnmpGetRetransmitMode(&nRetransmitMode) == SNMPAPI_SUCCESS) {		
		switch(nRetransmitMode) {
			case SNMPAPI_ON:
				ss << "ON";
				break;
			case SNMPAPI_OFF:
				ss << "OFF";
				break;
			default:
				ss << "?";
				break;
		}		
	} else {
		ss << "n/a";
	}
	smiUINT32 nTranslateMode;
	ss << "; t=";
	if(SnmpGetTranslateMode(&nTranslateMode) == SNMPAPI_SUCCESS) {
		switch(nRetransmitMode) {
			case SNMPAPI_TRANSLATED:
				ss << "T";
				break;
			case SNMPAPI_UNTRANSLATED_V2 :
				ss << "UV2";
				break;
			case SNMPAPI_UNTRANSLATED_V1 :
				ss << "UV1";
				break;
			default:
				ss << "?";
				break;
		}
	} else {
		ss << "n/a";
	}
	ss << ")";
	return ss.str();
}

void SNMPManager::cleanUp() {
	if(_init) {
		_init = false;
		std::map<HSNMP_SESSION,SNMPSession*>::iterator i;
		while((i=_sessions.begin()) != _sessions.end()) {
			delete (*i).second;
		}
		SnmpCleanup();
	}
}

void SNMPManager::unregisterSession(SNMPSession * session) 
{	
	_sessions.erase(session->getHandle());
}

void SNMPManager::registerSession(SNMPSession * session) 
{	
	_sessions[session->getHandle()] = session;
}

SNMPSession * SNMPManager::getSession(HSNMP_SESSION handle) 
{	
	std::map<HSNMP_SESSION,SNMPSession*>::const_iterator pos;
	pos = _sessions.find(handle);
	if(pos == _sessions.end()) {
		return NULL;
	}
	return pos->second;
}


std::string SNMPManager::pduToStr(	HSNMP_ENTITY hDstEntity,
									HSNMP_ENTITY hSrcEntity,
									HSNMP_CONTEXT hContext,
									smiINT32 pduType,
									smiINT32 pduId,
									smiINT32 pduError,
									HSNMP_VBL hVbl,
									bool leftToRight) {
	std::stringstream ss;
	// direction
	char dstbuf[256];
	dstbuf[0] = '\0';
	if(hDstEntity) {
		if(SnmpEntityToStr(hDstEntity, 255, dstbuf) == SNMPAPI_FAILURE) {
			dstbuf[0] = '\0';		   
		}
	}
	char srcbuf[256];	
	srcbuf[0] = '\0';
	if(hSrcEntity) {
		if(SnmpEntityToStr(hSrcEntity, 255, srcbuf) == SNMPAPI_FAILURE) {			
			srcbuf[0] = '\0';
		}
	}
	if(leftToRight) {
		ss << srcbuf;
		if(srcbuf[0] != '\0') {
			ss << " ";
		}
		ss << ">";
		if(dstbuf[0] != '\0') {
			ss << " ";
		}
		ss << dstbuf;
		ss << " ";
	} else {
		ss << dstbuf;
		if(dstbuf[0] != '\0') {
			ss << " ";
		}
		ss << "<";
		if(srcbuf[0] != '\0') {
			ss << " ";
		}
		ss << srcbuf;
		ss << " ";
	}
	// head
	ss << SNMPManager::pduTypeToStr(pduType) << "(" << pduId << ", " << SNMPManager::pduErrorToStr(pduError) << ", ";
	smiOCTETS ctxString;
	if(SnmpContextToStr(hContext, &ctxString) == SNMPAPI_SUCCESS) {
		strncpy(srcbuf, (char*)ctxString.ptr, ctxString.len > 255 ? 255 : ctxString.len);
		srcbuf[ctxString.len > 255 ? 255 : ctxString.len] ='\0';
		ss << srcbuf << ", ";
		SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, &ctxString);
	} else {
		ss << "?, ";
	}
	// objects
	smiOID dRetName;
	smiVALUE dRetValue;
	int count = SnmpCountVbl(hVbl);		
	if(count) {
		char szName[MAXOBJIDSTRSIZE+1];
		szName[0] = szName[MAXOBJIDSTRSIZE] = '\0';
		for(int i=0; i<count; i++) {
			if(SnmpGetVb(hVbl, i+1, &dRetName, &dRetValue) == SNMPAPI_FAILURE) {
				break;
			}
			if(SnmpOidToStr(&dRetName, sizeof(szName), szName) == SNMPAPI_FAILURE) {
				szName[0] = '?';
				szName[1] = '\0';
			}
			if(pduType == SNMP_PDU_GETBULK || pduType == SNMP_PDU_GET || pduType == SNMP_PDU_GETNEXT) {
				ss << szName;
			} else {
				SNMPObject obj;
				obj.setValue(&dRetValue);
				obj.setOID(szName);
				ss << obj.getDisplayInformation();				
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
			if(i < count-1) {
				ss << ", ";
			}
		}
	}
	ss << ")";
	return ss.str();
}

std::string SNMPManager::pduErrorToStr(int error) {
	switch(error) {
	/* Error Codes Common to SNMPv1 and SNMPv2 */
	case SNMP_ERROR_NOERROR: return "No error";
	case SNMP_ERROR_TOOBIG: return "Too big";
	case SNMP_ERROR_NOSUCHNAME: return "No such name";
	case SNMP_ERROR_BADVALUE: return "Bad value";
	case SNMP_ERROR_READONLY: return "Read only";
	case SNMP_ERROR_GENERR: return "Generic error";
	/* Error Codes Added for SNMPv2 */
	case SNMP_ERROR_NOACCESS: return "No access";
	case SNMP_ERROR_WRONGTYPE: return "Wrong type";
	case SNMP_ERROR_WRONGLENGTH: return "Wrong length";
	case SNMP_ERROR_WRONGENCODING: return "Wrong coding";
	case SNMP_ERROR_WRONGVALUE: return "Wrong value";
	case SNMP_ERROR_NOCREATION: return "No creation";
	case SNMP_ERROR_INCONSISTENTVALUE: return "Inconsistent value";
	case SNMP_ERROR_RESOURCEUNAVAILABLE: return "Resource unavailable";
	case SNMP_ERROR_COMMITFAILED: return "Commit failed";
	case SNMP_ERROR_UNDOFAILED: return "Undo failed";
	case SNMP_ERROR_AUTHORIZATIONERROR: return "Authorization error";
	case SNMP_ERROR_NOTWRITABLE: return "Not writable";
	case SNMP_ERROR_INCONSISTENTNAME: return "Inconsistent name";
	}
	return "?";
}

std::string SNMPManager::pduTypeToStr(int type) { 
	switch(type) {
	case SNMP_PDU_GET: return "GET";
	case SNMP_PDU_GETNEXT: return "GETNEXT";
	case SNMP_PDU_RESPONSE: return "RESPONSE";
	case SNMP_PDU_SET: return "SET";
	case SNMP_PDU_V1TRAP: return "V1TRAP";
	case SNMP_PDU_GETBULK: return "GETBULK";
	case SNMP_PDU_INFORM: return "INFORM";
	case SNMP_PDU_TRAP: return "TRAP";
	}
	return "?";
}