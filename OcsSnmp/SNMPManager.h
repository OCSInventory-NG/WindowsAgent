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
#include "SNMPSession.h"
#include <map>


class SNMPManager {

friend class SNMPSession;
friend class SNMPRequest;
friend class SNMPRemoteAgent;

protected:
	static SNMPManager _snmpManager;
	
	static std::string pduTypeToStr(int type);
	static std::string pduErrorToStr(int error);	
	static std::string pduToStr(	HSNMP_ENTITY hDstEntity,
									HSNMP_ENTITY hSrcEntity,
									HSNMP_CONTEXT hContext,
									smiINT32 pduType,
									smiINT32 pduId,
									smiINT32 pduError,
									HSNMP_VBL hVbl,
									bool leftToRight = true);

	bool _init;
	smiUINT32 _nMajorVersion;
	smiUINT32 _nMinorVersion;
	smiUINT32 _nLevel;
	
	SNMPManager();
	~SNMPManager();
	void cleanUp();
	
	std::map<HSNMP_SESSION,SNMPSession*> _sessions;
	void unregisterSession(SNMPSession * session);
	void registerSession(SNMPSession * session); 
	SNMPSession * getSession(HSNMP_SESSION handle);

public:
	static SNMPManager * getManager() { return &_snmpManager; }

	void startup();
	std::string getDisplayInformation() const;
};


