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

class SNMPTrap {
	friend class SNMPRemoteAgent;
	friend class SNMPSession;
protected :
	std::string _pattern;
	smiOID _oidPattern;
	SNMPRemoteAgent * _pRemoteAgent;
	bool _registered;

public:	
	SNMPTrap(std::string pattern, SNMPRemoteAgent * pRemoteAgent);
	virtual ~SNMPTrap();

public:
	void enable(SNMPRemoteAgent * pRemoteAgent = NULL);
	void disable();

	SNMPTrap * clone();

	std::string getDisplayInformation() const;
};


