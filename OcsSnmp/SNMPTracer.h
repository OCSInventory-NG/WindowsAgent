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

#define SNMPTRACE(a)		SNMPTracer::getTracer()->trace(a)
#define SNMPTRACEAENABLED()	SNMPTracer::getTracer()->isEnabled()

class SNMPTracer {
protected:	
	static SNMPTracer _snmpTracer;
	
    HANDLE _hConsole;
	HANDLE _hConsoleInput;
	bool _active;

	SNMPTracer();
	~SNMPTracer();
	
public:
	static SNMPTracer * getTracer() { return &_snmpTracer; }

	void enable(bool on) { _active = on; }
	bool isEnabled() const { return _active; }
	void trace(const std::string & message);
};


