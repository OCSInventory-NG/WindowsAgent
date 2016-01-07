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
#include "SNMPRemoteAgent.h"


class SNMPRemoteAgentSet {
protected :
	std::vector<SNMPRemoteAgent*> _agents;
public:	
	SNMPRemoteAgentSet();	
	virtual ~SNMPRemoteAgentSet();

	void addAgent(std::string name, std::string community, unsigned int port = 0, SNMPSession * pSession = NULL);
	int getAgentCount()			{ return (int)_agents.size(); }
	SNMPRemoteAgent * getAgent(int i)	{ return _agents[i]; }
	void clear();

	std::string getDisplayInformation() const;
};
