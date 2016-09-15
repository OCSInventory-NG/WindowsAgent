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

#include "SNMPException.h"

#define __VER__ "1.0"


SNMPException::SNMPException(	std::string message, 
								SNMPException * prev, 
								const char * file, 
								int line, 
								const char * date, 
								const char * time, 
								const char * ver) { 
	_previous = prev;
	_message = message;
	std::stringstream ss;
	if(file) {
		ss << file;
		ss << "(" << line << ")";
		if(ver) {
			ss << " v" << ver;
		}
		if(date) {
			ss << " " << date;
		}
		if(time) {
			ss << " " << time;
		}
	}
	_source = ss.str();
}

SNMPException::~SNMPException() {
	if(_previous) {
		delete _previous;
	}
}

std::string SNMPException::toStringStack() {	
	std::stringstream ss;
	ss << toString();
	if(_previous) {
		ss << "\n    ";
		ss << _previous->toStringStack();
	}
	return ss.str(); 
}

std::string SNMPException::toString() {
	std::stringstream ss;
	ss << _message << ".";
	if(_source != "") {
		ss << " [" << _source << "]";
	}
	return ss.str(); 
}




SNMPErrorException::SNMPErrorException(int number, int level, std::string message, SNMPException * prev, const char * file, int line, const char * date, const char * time, const char * ver) : SNMPException(message, prev, file, line, date, time, ver) {
	_number = number;
	_level = level;	
}

SNMPErrorException::~SNMPErrorException() {	
}

std::string SNMPErrorException::toString() {
	std::stringstream ss;
	if(SNMP_WARNING == _level) {
		ss << "warning SNMP";
	} else {
		ss << "error SNMP";
	}
	ss << _number << ": " << SNMPException::toString();
	return ss.str(); 
}
