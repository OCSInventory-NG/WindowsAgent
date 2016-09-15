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

#include "SNMPTrap.h"
#include "SNMPRemoteAgent.h"
#include "SNMPSession.h"
#include "SNMPObject.h"
#include "SNMPException.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"


std::string SNMPTrap::getDisplayInformation() const
{
	std::stringstream ss;
	ss << "Trap: [" << _pattern << "]";
	return ss.str();
}

SNMPTrap * SNMPTrap::clone()
{
	SNMPTrap * myclone = new SNMPTrap(_pattern, _pRemoteAgent);
	return myclone;
}

SNMPTrap::SNMPTrap(std::string pattern, SNMPRemoteAgent * pRemoteAgent) {
	_registered = false;
	_pRemoteAgent = pRemoteAgent;
	_pattern = pattern;
	if(_pattern != "") {
		if(SnmpStrToOid(_pattern.c_str(), &_oidPattern) == SNMPAPI_FAILURE) {
			SNMP_THROW_ERROR("Could not convert OID from string representation", SnmpGetLastError(NULL), SNMP_ERROR);
		}
	}
}

SNMPTrap::~SNMPTrap()
{
	disable();
	if(_pattern != "") {
		SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, (smiLPOPAQUE)&_oidPattern);
	}
}

void SNMPTrap::disable()
{
	_registered = false;
	if(!_pRemoteAgent || !_registered) {
		return;
	}
	SNMPSession * session = _pRemoteAgent->getSession();
	smiLPCOID notification = NULL;
	if(_pattern != "") {
		notification = &_oidPattern;
	}
	if(SNMPTRACEAENABLED()) {
		std::stringstream trace;
		trace << "trap off < " << _pRemoteAgent->getName() << " " << _pRemoteAgent->getCommunity() << " " << _pattern;
		SNMPTRACE(trace.str());
	}
	if(SNMPAPI_SUCCESS != SnmpRegister(	session->getHandle(),
										NULL,
										_pRemoteAgent->getCommunityHandle(),
										_pRemoteAgent->getContextHandle(),
										notification,
										SNMPAPI_OFF)) {
		SNMP_THROW_ERROR("Could not unregister trap", SnmpGetLastError(session->getHandle()), SNMP_ERROR);
	}
}

void SNMPTrap::enable(SNMPRemoteAgent * pRemoteAgent) {
	if(pRemoteAgent != _pRemoteAgent) {
		disable();
		if(pRemoteAgent != NULL) {
			_pRemoteAgent = pRemoteAgent;
		}
	}
	if(!_pRemoteAgent || _registered) {
		return;
	}
	SNMPSession * session = _pRemoteAgent->getSession();
	smiLPCOID notification = NULL;
	if(_pattern != "") {
		notification = &_oidPattern;
	}
	if(SNMPTRACEAENABLED()) {
		std::stringstream trace;
		trace << "trap on < " << _pRemoteAgent->getName() << " " << _pRemoteAgent->getCommunity() << " " << _pattern;
		SNMPTRACE(trace.str());
	}
	if(SNMPAPI_SUCCESS != SnmpRegister(	session->getHandle(),
										NULL,
										_pRemoteAgent->getCommunityHandle(),
										_pRemoteAgent->getContextHandle(),
										notification,
										SNMPAPI_ON)) {
		SNMP_THROW_ERROR("Could not register trap", SnmpGetLastError(session->getHandle()), SNMP_ERROR);
	}
}

