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

#include "SNMPRemoteAgentSet.h"

#include <sstream>
#include <iostream>

#define __VER__ "1.0"


SNMPRemoteAgentSet::SNMPRemoteAgentSet() {
}

SNMPRemoteAgentSet::~SNMPRemoteAgentSet() {
	clear();
}

void SNMPRemoteAgentSet::clear() {
	while((int)_agents.size()) {
		delete _agents[0];
		_agents.erase(_agents.begin());
	}
}

void SNMPRemoteAgentSet::addAgent(std::string name, std::string community, unsigned int port, SNMPSession * pSession)
{
	_agents.insert(_agents.end(), new SNMPRemoteAgent(name, community, port, pSession));
}

std::string SNMPRemoteAgentSet::getDisplayInformation() const
{
	std::stringstream ss;
	ss << "Remote Agent Set: [";
	for(int i=0; i<(int)_agents.size(); i++) {
		ss << _agents[i]->getDisplayInformation();
		if(i < (int)_agents.size()-1) {
			ss << ", ";
		}
	}
	ss << "]";
	return ss.str();
}
