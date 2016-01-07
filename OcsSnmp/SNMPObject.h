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

#define SNMP_INT		0	// SNMP_SYNTAX_INT, SNMP_SYNTAX_INT32	- Indicates a 32-bit signed integer variable =SNMP_SYNTAX_INT32.
#define SNMP_OCTETS		1	// SNMP_SYNTAX_OCTETS - Indicates an octet string variable that is binary or textual data.		
#define SNMP_OPAQUE		2	// SNMP_SYNTAX_OPAQUE - This type provides backward compatibility, and should not be used for new object types. It supports the capability to pass arbitrary Abstract Syntax Notation One (ASN.1) syntax.
#define SNMP_BITS		3	// SNMP_SYNTAX_BITS		
#define SNMP_CNTR64		4	// SNMP_SYNTAX_CNTR64 - Indicates a counter variable that increases until it reaches a maximum value of (2^64) – 1.
#define SNMP_CNTR32		5	// SNMP_SYNTAX_CNTR32 - ndicates a counter variable that increases until it reaches a maximum value of (2^32) – 1.  
#define SNMP_GAUGE32	6	// SNMP_SYNTAX_GAUGE32 - Indicates a gauge variable that is a non-negative integer that can increase or decrease, but never exceed a maximum value.  
#define SNMP_UINT32		7	// SNMP_SYNTAX_UINT32 - Indicates a 32-bit unsigned integer variable.  
#define SNMP_TIMETICKS	8	// SNMP_SYNTAX_TIMETICKS - Indicates a counter variable that measures the time in hundredths of a second, until it reaches a maximum value of (2^32) – 1. It is a non-negative integer that is relative to a specific timer event.
#define SNMP_OID		9	// SNMP_SYNTAX_OID - Indicates an object identifier variable that is an assigned name with a maximum of 128 subidentifiers.
#define SNMP_IPADDR		10	// SNMP_SYNTAX_IPADDR, SNMP_SYNTAX_NSAPADDR - Indicates a 32-bit Internet address variable. 
#define SNMP_NULL		11	// SNMP_SYNTAX_NULL, ... - Indicates a NULL value.  
			

class SNMPRemoteAgent;
class SNMPSession;
class SNMPRequest;
class SNMPManager;

class SNMPObject {
	friend class SNMPRemoteAgent;
	friend class SNMPSession;
	friend class SNMPRequest;
	friend class SNMPManager;
protected:
	DWORD _syntax;
	bool _bInit;
	smiOID _oid;
	bool _binary;

	smiOID * getBinaryOID();
	void setValue(smiVALUE * smiVal);
	void getValue(smiVALUE * smiVal);

	__int64 _int;
	unsigned __int64 _uint;
	std::string _string;
	unsigned char * _octets;
	unsigned int  _octetsLen;

	void setInt(__int64 value) { _int = value; }
	__int64 getInt() const { return _int; }	
	void setUInt(unsigned __int64 value) { _uint = value; }
	unsigned __int64 getUInt() const { return _uint; }	
	void setString(std::string value) { _string = value; }
	std::string getString() const { return _string; }
	unsigned char * getOctets() const { return _octets; }
	unsigned int getOctetsLen() const { return _octetsLen; }
	void setOctets(unsigned char * pValue, unsigned int nValue);

public:
	SNMPObject(std::string oid = "");
	SNMPObject(const SNMPObject & other);
	virtual ~SNMPObject();
	SNMPObject & operator=(const SNMPObject & other);
	
	// OID
	std::string	getOID()const;
	void setOID(std::string oid);		
	int compareOID(SNMPObject * pOther) const;
	int compareOID(std::string oid) const;

	// Syntax
	int getSyntax() const;
	void setSyntax(int syntax);
	std::string getSyntaxAsString() const;
	
	// Value	
	std::string	getValueAsString() const;
	void setValueAsString(std::string);
	__int64 getValueAsInteger() const;
	void setValueAsInteger(__int64 value);	
	char * getValueAsOctets(int * size = NULL) const;
	void setValueAsOctets(char * value, int size);
	int compareValue(SNMPObject * pOther) const;

	// Display
	std::string getDisplayInformation() const;
};


