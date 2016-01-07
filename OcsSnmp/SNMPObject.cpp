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

#include "SNMPObject.h"
#include "SNMPException.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"

#pragma warning(disable : 4996)


SNMPObject::SNMPObject(const SNMPObject & other) {
    *this = other;
}

SNMPObject & SNMPObject::operator=(const SNMPObject & other) {
	_syntax = other._syntax;
	_bInit = other._bInit;
	_binary = other._binary;
	std::string oid = other.getOID();
	setOID(oid);
	_int = other._int;
	_uint = other._uint;
	_string = other._string;
	_octetsLen = 0;
	_octets = NULL;
	setOctets(other._octets, other._octetsLen);
	return *this;
}

SNMPObject::SNMPObject(std::string oid) {
	_bInit = false;
	_binary = false;
	_int = 0;
	_uint = 0;
	_string = "";
	_octets = NULL;
	_octetsLen = 0;
	_syntax = SNMP_SYNTAX_NULL;
	setOID(oid);
}

void SNMPObject::setOID(std::string oid) {	
	if(_bInit) {
		SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, (smiLPOPAQUE)&_oid);
		_bInit = false;
	}
	if(oid != "") {
		if(SnmpStrToOid(oid.c_str(), &_oid) == SNMPAPI_FAILURE) {
			SNMP_THROW_ERROR("Could not convert OID from string representation", SnmpGetLastError(NULL), SNMP_ERROR);
		}
		_bInit = true;
	}
}

int SNMPObject::compareOID(SNMPObject * pOther) const
{
	if(!_bInit || !pOther->_bInit) {
		SNMP_THROW_ERROR("SNMP object OID(s) not initialized", SNMPAPI_OTHER_ERROR, SNMP_ERROR);
	}
	smiINT res = 0;
	if(SnmpOidCompare(&_oid, &(pOther->_oid), 0, &res) != SNMPAPI_SUCCESS) {
		SNMP_THROW_ERROR("Could not compare OIDs", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	return (int)res;
}

int SNMPObject::compareOID(std::string oid) const
{
	if(!_bInit) {
		SNMP_THROW_ERROR("SNMP object OID not initialized", SNMPAPI_OTHER_ERROR, SNMP_ERROR);
		return 0;
	}
	smiOID otherOid;
	if(SnmpStrToOid(oid.c_str(), &otherOid) == SNMPAPI_FAILURE) {
		SNMP_THROW_ERROR("Could not convert OID from string representation", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	smiINT res = 0;
	if(SnmpOidCompare(&_oid, &otherOid, 0, &res) != SNMPAPI_SUCCESS) {
		SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, (smiLPOPAQUE)&otherOid);
		SNMP_THROW_ERROR("Could not compare OIDs", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, (smiLPOPAQUE)&otherOid);
	return (int)res;
}

SNMPObject::~SNMPObject() {
	if(_octets) {
		free(_octets);
	}
	if(_bInit) {
		SnmpFreeDescriptor(SNMP_SYNTAX_OCTETS, (smiLPOPAQUE)&_oid);
	}
}

void SNMPObject::setOctets(unsigned char * pValue, unsigned int nValue) {
	if(_octets) {
		free(_octets);
	}
	if(pValue == NULL || nValue == 0) {
		_octets = NULL;
		_octetsLen = 0;
	} else {
		_octets = (unsigned char*)malloc(sizeof(unsigned char)*nValue);
		_octetsLen = nValue;
		memcpy(_octets, pValue, _octetsLen);
	}
}

#define DAY       ((unsigned __int64)(24L*60L*60L))
#define HOUR      ((unsigned __int64)(60L*60L))
#define MINUTE    ((unsigned __int64)(60L))
#define HUNDRED   ((unsigned __int64)(100L))
std::string SNMPObject::getValueAsString() const {
	std::stringstream ss;
	switch(_syntax) {		
		case SNMP_SYNTAX_INT:
			ss << getInt();
			break;		
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			if(_binary) {
				if(getOctets() && getOctetsLen()) {
					unsigned int i;
					char buf[25];
					for(i=0; i<getOctetsLen(); i++) {
						sprintf(buf, "%2.2X", getOctets()[i]);
						ss << buf;
						if(i+1 < getOctetsLen()) {
							ss << "-";
						}
					}					
				} else {
					ss << "empty";
				}
			} else {
				ss << "\"" << getString() << "\"";
			}
			break;		
		case SNMP_SYNTAX_CNTR64:
			ss << getUInt();
			break;		
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_UINT32:
			ss << getUInt();
			break;			
		case SNMP_SYNTAX_TIMETICKS:
			{
				unsigned __int64 tt = getUInt();
				unsigned __int64 lDays = 0;
				unsigned __int64 lHours = 0;
				unsigned __int64 lMinutes = 0;
				unsigned __int64 lSeconds = 0;
				unsigned __int64 lHundreths = 0;
				if(tt)  {
					lHundreths = tt % HUNDRED;
					lSeconds = tt / HUNDRED;
					if (lSeconds !=0) {
						lDays = lSeconds / DAY;
						lSeconds %= DAY;
						if (lSeconds != 0) {
							lHours = lSeconds / HOUR;
							lSeconds %= HOUR;
							if (lSeconds != 0) {
								lMinutes = lSeconds / MINUTE;
								lSeconds %= MINUTE;
							}
						}
					}
				}
				ss << lDays << "," << lHours << ":" << lMinutes << ":" << lSeconds << ".";
				if(lHundreths == 0) {
					ss << "000";
				} else if (lHundreths < 10) {
					ss << "0" << lHundreths << "0";
				} else {
					ss << lHundreths << "0";
				}				 
			}
			break;
		case SNMP_SYNTAX_OID:
			ss << getString();
			break;			
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			ss << getString();	
			break;		
		case SNMP_SYNTAX_NULL:
			ss << "null";
			break;
		case SNMP_SYNTAX_NOSUCHOBJECT:
			ss << "no such object";
			break;
		case SNMP_SYNTAX_NOSUCHINSTANCE:
			ss << "no such instance";
			break;
		case SNMP_SYNTAX_ENDOFMIBVIEW:
			ss << "end of mib";
			break;
		default:
			ss << "not printable (" << _syntax << ")";
			break;
	}	

	return ss.str();
}

void SNMPObject::setValueAsString(std::string value) {	
	switch(_syntax) {		
		case SNMP_SYNTAX_INT:
			setInt(_atoi64(value.c_str()));
			break;		
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			_binary = false;
			setString(value);
			break;
		case SNMP_SYNTAX_CNTR64:
			setUInt((unsigned __int64)_atoi64(value.c_str()));
			break;		
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_UINT32:
			setInt(_atoi64(value.c_str()));
			break;			
		case SNMP_SYNTAX_TIMETICKS:
			{
				unsigned __int64 fields[5];
				fields[0] = fields[1] = fields[2] = fields[3] = fields[4] = 0;
				int lDays = 0;
				int lHours = 1;
				int lMinutes = 2;
				int lSeconds = 3;
				int lHundreths = 4;
				int i;
				std::string aux;
				int ifield = 0;
				for (i=0; i<(int)_string.size(); i++) {
					if(_string[i] == '.' || _string[i] == ',' || _string[i] == ':') {
						if(ifield < 5) {
							fields[ifield] = atoi(aux.c_str());
						}
						aux = "";
						ifield++;
					} else {
						aux += _string[i];
					}
				}
				if(ifield < 5) {
					fields[ifield] = atoi(aux.c_str());
				}
				unsigned __int64 tt = 
						fields[lHundreths] + 
						(fields[lSeconds] +
						(fields[lDays] * DAY) +
						(fields[lHours] * HOUR) +
						(fields[lMinutes] * MINUTE)* HUNDRED);
				setUInt(tt);
							 
			}
			break;
		case SNMP_SYNTAX_OID:
			_binary = false;
			setString(value);
			break;			
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			_binary = false;
			setString(value);
			break;		
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
		default:
			break;
	}	
}


void SNMPObject::setValueAsOctets(char * value, int size)
{
	switch(_syntax) {	
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			_binary = true;
			setOctets((unsigned char*)value, size);
		break;
	}
}

char * SNMPObject::getValueAsOctets(int * size) const {
	char * pChar = NULL;
	int sz = 0;
	switch(_syntax) {	
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			if(_binary) {
				pChar = (char*)(getOctets());
				sz = getOctetsLen();
			} else {
				pChar = (char*)_string.c_str();
				sz = (int)_string.size();
			}
			break;
	}
	if(size) {
		*size = sz;
	}
	return pChar;
}

void SNMPObject::setValueAsInteger(__int64 value) {
	switch(_syntax) {		
		case SNMP_SYNTAX_INT:
			setInt(value);
			break;
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:	
			{
				std::stringstream ss;
				_binary = false;
				ss << value;
				setString(ss.str());
			}
			break;		
		case SNMP_SYNTAX_CNTR64:
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_UINT32:
		case SNMP_SYNTAX_TIMETICKS:
			setUInt((unsigned __int64)value);
			break;
		case SNMP_SYNTAX_OID:
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
			break;
	}	
}

__int64 SNMPObject::getValueAsInteger() const {
	switch(_syntax) {		
		case SNMP_SYNTAX_INT:
			return (__int64)getInt();
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:	
			if(_binary) {
				int sz = 0;
				char * pChar = getValueAsOctets(&sz);				
				if(pChar && sz) {
					switch(sz) {
						case 1: return (__int64)(char)*((char*)pChar);
						case 2:
						case 3: return (__int64)(short)ntohs(*((short*)pChar));
						default: return (__int64)(long)ntohl(*((long*)pChar));
					}						
				}
			} else {
				return (__int64)_atoi64(_string.c_str());
			}
			break;		
		case SNMP_SYNTAX_CNTR64:
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_UINT32:
		case SNMP_SYNTAX_TIMETICKS:
			return (__int64)getUInt();
		case SNMP_SYNTAX_OID:
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
			break;
	}	
	return 0;
}

int SNMPObject::compareValue(SNMPObject * pOther) const
{
	switch(_syntax) {		
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
		case SNMP_SYNTAX_OID:
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			return getValueAsString().compare(pOther->getValueAsString());
		case SNMP_SYNTAX_INT:
			if(getInt() < pOther->getInt()) {
				return -1;
			} else if(getInt() == pOther->getInt()) {
				return 0;
			} else  {
				return 1;
			}		
		case SNMP_SYNTAX_CNTR64:
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_UINT32:
		case SNMP_SYNTAX_TIMETICKS:
			if(getUInt() < pOther->getUInt()) {
				return -1;
			} else if(getUInt() == pOther->getUInt()) {
				return 0;
			} else  {
				return 1;
			}
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
			return 0;
	}
	return 0;
}



std::string SNMPObject::getDisplayInformation() const {
	std::stringstream ss;
	//ss << getOID() << " = " << getValueAsString() << " [" << getSyntaxAsString() + "]";
	ss << getValueAsString();
	return ss.str();
}

smiOID * SNMPObject::getBinaryOID() {
	if(!_bInit) {
		return NULL;
	}
	return &_oid;
}

std::string	SNMPObject::getOID() const {
	if(!_bInit) {
		return "";
	}
	char szName[MAXOBJIDSTRSIZE+1];
	szName[0] = szName[MAXOBJIDSTRSIZE] = '\0';
	if(SnmpOidToStr (&_oid, sizeof(szName), szName) == SNMPAPI_FAILURE) {
		SNMP_THROW_ERROR("Failed to convert OID to string", SnmpGetLastError(NULL), SNMP_ERROR);
	}
	return std::string(szName);
}

void SNMPObject::setSyntax(int syntax) {
	switch(syntax) {
		case SNMP_INT:
			_syntax = SNMP_SYNTAX_INT; break;
		case SNMP_OCTETS:
			_syntax = SNMP_SYNTAX_OCTETS; break;
		case SNMP_OPAQUE:
			_syntax = SNMP_SYNTAX_OPAQUE; break;
		case SNMP_BITS:
			_syntax = SNMP_SYNTAX_BITS; break;		
		case SNMP_CNTR64:
			_syntax = SNMP_SYNTAX_CNTR64; break;		
		case SNMP_CNTR32:
			_syntax = SNMP_SYNTAX_CNTR32; break;
		case SNMP_GAUGE32:
			_syntax = SNMP_SYNTAX_GAUGE32; break;
		case SNMP_TIMETICKS:
			_syntax = SNMP_SYNTAX_TIMETICKS; break;
		case SNMP_UINT32:
			_syntax = SNMP_SYNTAX_UINT32; break;
		case SNMP_OID:
			_syntax = SNMP_SYNTAX_OID; break;
		case SNMP_IPADDR:
			_syntax = SNMP_SYNTAX_IPADDR; break;
		default:
			_syntax = SNMP_NULL; break;
	}		
}

int SNMPObject::getSyntax() const {
	switch(_syntax) {		
		case SNMP_SYNTAX_INT:
			return SNMP_INT;
		case SNMP_SYNTAX_OCTETS:
			return SNMP_OCTETS;
		case SNMP_SYNTAX_OPAQUE:
			return SNMP_OPAQUE;
		case SNMP_SYNTAX_BITS:
			return SNMP_BITS;		
		case SNMP_SYNTAX_CNTR64:
			return SNMP_CNTR64;		
		case SNMP_SYNTAX_CNTR32:
			return SNMP_CNTR32;
		case SNMP_SYNTAX_GAUGE32:
			return SNMP_GAUGE32;
		case SNMP_SYNTAX_TIMETICKS:
			return SNMP_TIMETICKS;
		case SNMP_SYNTAX_UINT32:
			return SNMP_UINT32;
		case SNMP_SYNTAX_OID:
			return SNMP_OID;
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			return SNMP_IPADDR;			
	}	
	return SNMP_NULL;
}

std::string SNMPObject::getSyntaxAsString() const {
	static char * _syntaxStrMap[12] = {
		"Integer",
		"Octets",
		"Opaque",
		"Bits",
		"Counter64",
		"Counter32",
		"Gauge32",
		"UnsignedInteger32",
		"TimeTicks",
		"ObjectIdentifier",
		"IPAddr",
		"Null" };
	return _syntaxStrMap[getSyntax()];
}


void SNMPObject::setValue(smiVALUE * smiVal)
{
	_syntax = (DWORD)smiVal->syntax;
	switch(smiVal->syntax) {
		
		case SNMP_SYNTAX_INT:
			setInt((__int64)smiVal->value.sNumber);
			break;
		
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			{
				unsigned int i;
				unsigned char cB;
				bool binary = false;
				for (i=0; i<smiVal->value.string.len; i++) {
					cB = (unsigned char)smiVal->value.string.ptr[i];
					if((cB < 0x20 && (cB != 0x0A && cB != 0x0D)) || cB > 0x7E) {
						binary = true;
						break;
					}
				}
				_binary = binary;
				if(binary) {
					setOctets(smiVal->value.string.ptr, smiVal->value.string.len);
				} else {
					std::stringstream ss;
					for (i=0; i<smiVal->value.string.len; i++) {
						ss << (unsigned char)smiVal->value.string.ptr[i];
					}
					setString(ss.str());
                }
			}
			break;
		
		case SNMP_SYNTAX_CNTR64:
			setUInt( (((unsigned __int64)(smiVal->value.hNumber.hipart)) << 32) | ((unsigned __int64)(smiVal->value.hNumber.lopart)) );
			break;
		
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_TIMETICKS:
		case SNMP_SYNTAX_UINT32:
			setUInt((unsigned __int64)smiVal->value.uNumber);
			break;

		case SNMP_SYNTAX_OID:
			{
				char szName[MAXOBJIDSTRSIZE+1];
				szName[0] = szName[MAXOBJIDSTRSIZE] = '\0';
				if(SnmpOidToStr (&smiVal->value.oid, sizeof(szName), szName) == SNMPAPI_FAILURE) {
					szName[0] = '\0';
				}
				setString(szName);
			}
			break;
			
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			{
				unsigned int i;
				std::stringstream ss;
				for (i=0; i<smiVal->value.string.len; i++) {
					ss << ((unsigned int)smiVal->value.string.ptr[i]);
					if(i+1 < smiVal->value.string.len) {
						ss << ".";
					}
				}
				setString(ss.str());
			}			
			break;
		
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
		default:
			break;
	}	
}


void SNMPObject::getValue(smiVALUE * smiVal)
{
	smiVal->syntax = (smiUINT32)_syntax;
	switch(smiVal->syntax) {
		
		case SNMP_SYNTAX_INT:
			smiVal->value.sNumber = (smiINT)getInt();
			break;
		
		case SNMP_SYNTAX_OCTETS:
		case SNMP_SYNTAX_OPAQUE:
		case SNMP_SYNTAX_BITS:
			{				
				int sz = 0;
				char * pChar = getValueAsOctets(&sz);
				smiVal->value.string.len = sz;
				if(sz) {
					smiVal->value.string.ptr = (smiLPBYTE)malloc(sz);
					memcpy(smiVal->value.string.ptr, pChar, sz);
				} else {
					smiVal->value.string.ptr = NULL;
				}
			}
			break;
		
		case SNMP_SYNTAX_CNTR64:
			smiVal->value.hNumber.hipart = (smiUINT32)((getUInt() >> 32) & 0x00000000FFFFFFFF);
			smiVal->value.hNumber.lopart = (smiUINT32)(getUInt() & 0x00000000FFFFFFFF);
			break;
		
		case SNMP_SYNTAX_CNTR32:
		case SNMP_SYNTAX_GAUGE32:
		case SNMP_SYNTAX_TIMETICKS:
		case SNMP_SYNTAX_UINT32:
			smiVal->value.uNumber = (smiUINT32)getUInt();
			break;

		case SNMP_SYNTAX_OID:
			SnmpStrToOid(_string.c_str(), &smiVal->value.oid);
			break;
			
		case SNMP_SYNTAX_NSAPADDR:
		case SNMP_SYNTAX_IPADDR:
			{
				int i;
				int sz = 0;
				for (i=0; i<(int)_string.size(); i++) {
					if(_string[i] == '.') {
						sz++;
					}
				}
				smiVal->value.string.ptr = (smiLPBYTE)malloc(sz);
				smiVal->value.string.len = sz;
				std::string aux;
				int istore = 0;
				for (i=0; i<(int)_string.size(); i++) {
					if(_string[i] == '.') {
						if(istore < sz) {
							smiVal->value.string.ptr[istore] = (smiBYTE)atoi(aux.c_str());
						}
						aux = "";
						istore++;
					} else {
						aux += _string[i];
					}
				}
				if(istore < sz) {
					smiVal->value.string.ptr[istore] = (smiBYTE)atoi(aux.c_str());
				}
			}			
			break;
		
		case SNMP_SYNTAX_NULL:
		case SNMP_SYNTAX_NOSUCHOBJECT:
		case SNMP_SYNTAX_NOSUCHINSTANCE:
		case SNMP_SYNTAX_ENDOFMIBVIEW:
		default:
			smiVal->value.uNumber = 0;
			break;
	}	
}
