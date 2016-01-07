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
#include <sstream>

#include "SNMPTracer.h"

#define SNMP_THROW_BASE(e) { SNMPException * psnmpe = e; SNMPTRACE(psnmpe->toString()); throw psnmpe; }

#define SNMP_THROW_EXCEPTION(message) SNMP_THROW_BASE(new SNMPException(message, NULL, __FILE__, __LINE__, __DATE__, __TIME__, __VER__))
#define SNMP_RETHROW_EXCEPTION(message, exception) SNMP_THROW_BASE(new SNMPException(, exception, __FILE__, __LINE__, __DATE__, __TIME__, __VER__))

#define SNMP_THROW_ERROR(message, number, level) SNMP_THROW_BASE(new SNMPErrorException(number, level, message, NULL, __FILE__, __LINE__, __DATE__, __TIME__, __VER__))
#define SNMP_RETHROW_ERROR(message, exception, number, level) SNMP_THROW_BASE(new SNMPErrorException(number, level, message, exception, __FILE__, __LINE__, __DATE__, __TIME__, __VER__))


class SNMPException {
protected:
	std::string _message;
	std::string _source;
	SNMPException * _previous;
public:
	SNMPException(	std::string message = "", 
					SNMPException * prev = NULL, 
					const char * file = NULL, 
					int line = 0, 
					const char * date = NULL,
					const char * time = NULL, 
					const char * ver = NULL);
	virtual ~SNMPException();
	virtual std::string toString();
	virtual std::string toStringStack();
};


#define SNMP_ERROR		0
#define SNMP_WARNING	1


class SNMPErrorException : public SNMPException {
protected:
	int _level;
	int _number;
public:
	SNMPErrorException(int number = SNMPAPI_OTHER_ERROR, int level = SNMP_WARNING, std::string message = "", SNMPException * prev = NULL, const char * file = NULL, int line = 0, const char * date = NULL, const char * time = NULL, const char * ver = NULL);	
	virtual ~SNMPErrorException();
	virtual std::string toString();
};



