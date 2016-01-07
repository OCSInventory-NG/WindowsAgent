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

//
// This is a test program set for the code wrapper.
//
// Comment/uncomment these macros to produce different applications
// Note that only one of them can be uncommented:
//#define _SNMP_SCAN_
#define _SNMP_GET_
//#define _SNMP_SET_
//#define _SNMP_WALK_
//#define _SNMP_TRAP_
// 

#include "Winsnmp.h"
#include "SNMPManager.h"
#include "SNMPRemoteAgent.h"
#include "SNMPRemoteAgentSet.h"
#include "SNMPRequest.h"
#include "SNMPRequestSet.h"
#include "SNMPObject.h"
#include "SNMPException.h"
#include "SNMPTracer.h"

#include <iostream>
#include <fstream>
using namespace std;

#define __VER__ "1.0"

#define IS_OPTION(c)   (((c) == '-') || ((c) == '/'))

int main(int argc, char* argv[]) {
	try {
		SNMPManager::getManager()->startup();

#ifdef _SNMP_WALK_
		std::string oid = "1.3.6.1.6.3.15.4.1.2.1.0";
		std::string host = "172.18.25.254";
		std::string community = "factorfx";
		bool compare_equal = false;
		bool compare_smaller = false;
		int tmo = 1000;
		unsigned int port = 0;	
		++argv;
		--argc;
		int argn = 0;
		while(argc>0) {
			if(IS_OPTION(**argv)) {
				switch(*(*argv+1)) {
				case 'h':case '?':
					cout << "snmpwalk [-h?] [-T] [-c] [-t<n>] [-p:<n>] host [community [oid]]\n";
					cout << "Options:\n";
					cout << "  h, ? - show this help screen\n";
					cout << "  s - stop walk if next OID is smaller than current OID\n";
					cout << "  e - stop walk if OID is repeated.\n";
					cout << "  t:<n> - set timeout to <n> ms\n";
					cout << "  p:<n> - set remote agent port to <n>, ex: 123\n";
					cout << "  T - enable trace\n";
					cout << "Arguments:\n";
					cout << "  host - address or name of agent's host\n";
					cout << "  community - community name to scan for, ex: public\n";
					cout << "  oid - oid name to walk from, ex: 1.3.6.1.2.1.1.1.0\n";
					return 0;
					break;
				case 'p':
					if( (*(*argv+2)) == ':') { port = atoi(*argv+3); } break;
				case 'e':
					if( (*(*argv+2)) == '\0') { compare_equal = true; } break;
				case 's':
					if( (*(*argv+2)) == '\0') { compare_smaller = true; } break;
				case 't':
					if( (*(*argv+2)) == ':') { tmo = atoi(*argv+3); } break;
				case 'T':
					if( (*(*argv+2)) == '\0') { SNMPTracer::getTracer()->enable(true); } break;
				}
			} else {
				switch(argn) {
					case 0: host = *argv; break;
					case 1: community = *argv; break;
					case 2:	oid = *argv; break;
				}
				argn++;
			}
			++argv; --argc;
		}

		SNMPRemoteAgent ragent(host, community, port);
		
		int oidcount = 1;
		int response = 1;
		SNMPObject * pRet = NULL;
		string old_result = "";

		bool continueWalk = true;
		do {
			SNMPObject sysObj(oid);
			SNMPRemoteAgentGetNextRequest getReq(&sysObj);
			getReq.execute(&ragent);
			getReq.wait(tmo);
			if(getReq.getResultCount()) {
				for(int i=0; i<getReq.getResultCount(); i++) {	
					oid = getReq.getResult(i)->getOID();
					if(compare_smaller) {
						continueWalk &= sysObj.compareOID(oid) >= 0;
					}
					if(compare_equal) {
						continueWalk &= sysObj.compareOID(oid) != 0;
					}
					if(continueWalk) {
						std::cout << response << ": " << getReq.getResult(i)->getDisplayInformation() << "\n";
						
						if (old_result == getReq.getResult(i)->getDisplayInformation()){
							std::cout << "-------->" << "\n";
							std::cout << old_result << "\n";
							std::cout << getReq.getResult(i)->getDisplayInformation() << "\n";
							std::cout << "Identique => FIN" << "\n";
							std::cout << "-------->" << "\n";
							continueWalk = false;
							break;
						}

						old_result = getReq.getResult(i)->getDisplayInformation();
						response++;
					} else {
						break;
					}
				}						
			} else {
				continueWalk = false;
			}
		} while(continueWalk);
		std::cout << "\n";
		return 0;
#endif

#ifdef _SNMP_SET_
		std::string host = "172.18.15.227";
		std::string community = "public";
		int tmo = 10000;
		unsigned int port = 0;	
		++argv;
		--argc;
		std::string oid = "1.3.6.1.2.1.1.1.0";
		std::string value = "0";
		int type = SNMP_INT;
		int argn = 0;
		while(argc>0) {
			if(IS_OPTION(**argv)) {
				switch(*(*argv+1)) {
				case 'h':case '?':
					cout << "snmpset [-h?] [-T] [-t<n>] [-p:<n>] host [community [oid [value [syntax]]]\n";
					cout << "Options:\n";
					cout << "  h, ? - show this help screen\n";
					cout << "  t:<n> - set timeout to <n> ms\n";
					cout << "  p:<n> - set remote agent port to <n>, ex: 123\n";
					cout << "  T - enable trace\n";
					cout << "Arguments:\n";
					cout << "  host - address or name of agent's host\n";
					cout << "  community - community name to scan for, ex: public \n";
					cout << "  oid - oid name to set, ex: 1.3.6.1.2.1.1.1.0 \n";
					cout << "  value - value to set\n";
					cout << "  syntax - value syntax\n";
					return 0;
					break;
				case 'p':
					if( (*(*argv+2)) == ':') { port = atoi(*argv+3); } break;
				case 't':
					if( (*(*argv+2)) == ':') { tmo = atoi(*argv+3);	} break;
				case 'T':
					if( (*(*argv+2)) == '\0') { SNMPTracer::getTracer()->enable(true); } break;
				}
			} else {
				switch(argn) {
					case 0: host = *argv; break;
					case 1: community = *argv; break;
					case 2: oid = *argv; break;
					case 3: value = *argv; break;
					case 4: type = atoi(*argv); break;
				}
				argn++;
			}
			++argv; --argc;
		}

		SNMPRemoteAgent ragent(host, community, port);
		if(type > SNMP_NULL || type < SNMP_INT) {
			type = SNMP_INT;
		}
		SNMPObject obj(oid);
		obj.setSyntax(type);
		obj.setValueAsString(value);
		SNMPRemoteAgentSetRequest setReq(&obj);
		setReq.execute(&ragent);
		setReq.wait(tmo);
		if(!setReq.succeeded()) {
			if(!setReq.timedOut()) {
				std::cout << setReq.getErrorAsString();
			} else {
				std::cout << "timeout";
			}
		} else {	
			for(int i=0; i<setReq.getResultCount(); i++) {	
				std::cout << setReq.getResult(i)->getDisplayInformation() << "\n";
			}
		}
		std::cout << "\n";
		return 0;
#endif

#ifdef _SNMP_GET_
		std::string host = "";
		std::string community = "";
		int tmo = 10000;
		unsigned int port = 161;	
		++argv;
		--argc;
		int oidcount = 0;
		int oidi = 0;
		std::string oidarray[512];
		int argn = 0;
		while(argc>0) {
			if(IS_OPTION(**argv)) {
				switch(*(*argv+1)) {
				case 'h':case '?':
					cout << "OcsSnmp.exe [-h?] [-T] [-t<n>] [-p:<n>] host [community [oid1 [oid2 [... [oidn]]]]\n";
					cout << "Options:\n";
					cout << "  h, ? - show this help screen\n";
					cout << "  t:<n> - set timeout to <n> ms\n";
					cout << "  p:<n> - set remote agent port to <n>, ex: 123\n";
					cout << "  T - enable trace\n";
					cout << "Arguments:\n";
					cout << "  host - address or name of agent's host\n";
					cout << "  community - community name to scan for, ex: public \n";
					cout << "  oid1..n - oid names to scan for, ex: 1.3.6.1.2.1.1.1.0 \n";
					return 0;
					break;
				case 'p':
					if( (*(*argv+2)) == ':') { port = atoi(*argv+3); } break;
				case 't':
					if( (*(*argv+2)) == ':') { tmo = atoi(*argv+3);	} break;
				case 'T':
					if( (*(*argv+2)) == '\0') { SNMPTracer::getTracer()->enable(true); } break;
				}
			} else {
				switch(argn) {
					case 0: host = *argv; break;
					case 1: community = *argv; break;
					default:
						if(oidcount < 512 ) {
							oidarray[oidi] = *argv;
							oidi++;
							oidcount++;
						}
						break;
				}
				argn++;
			}
			++argv; --argc;
		}

		SNMPRemoteAgent ragent(host, community, port);
		
		if(oidcount <= 0) {
			oidcount = 1;
			oidarray[0] = "1.3.6.1.2.1.1.1.0";
		}
		
		SNMPObject * oids = new SNMPObject[oidcount];
		for(int i=0; i<oidcount; i++) {			
			oids[i].setOID(oidarray[i]);
		}
		SNMPRemoteAgentGetRequest getReq(oids, oidcount);
		getReq.execute(&ragent);
		getReq.wait(tmo);
		delete [] oids;		
		if(!getReq.succeeded()) {
			if(!getReq.timedOut()) {
				std::cout << getReq.getErrorAsString();
				ofstream myfile("snmp.txt");
				if (myfile.is_open())
				{
					myfile << getReq.getErrorAsString() << "\n";
					myfile.close();
				}

			} else {
				std::cout << "timeout";
				ofstream myfile("snmp.txt");
				if (myfile.is_open())
				{
					myfile << "Timeout \n";
					myfile.close();
				}
			}
		} else {

			ofstream myfile("snmp.txt");
			for(int i=0; i<getReq.getResultCount(); i++) {	
				std::cout << getReq.getResult(i)->getDisplayInformation() << "\n";
				if (myfile.is_open())
				{
					myfile << getReq.getResult(i)->getDisplayInformation() << "\n";
				}
				
			}
			myfile.close();
			
		}
		std::cout << "\n";
		return 0;
#endif

#ifdef _SNMP_SCAN_	
		unsigned long net = inet_addr("172.18.32.100");
		unsigned long mask = inet_addr("255.255.255.0");
		std::string community = "factorfx";
		std::string oid = "1.3.6.1.2.1.1.1.0";
		int tmo = 500;
		unsigned int port = 0;
		bool longOutput = false;
		++argv;
		--argc;
		int argn = 0;
		while(argc>0) {
			if(IS_OPTION(**argv)) {
				switch(*(*argv+1)) {
				case 'h':case '?':
					cout << "snmpscan [-h?] [-T] [-t<n>] [-p:<n>] [-a] net mask [community [oid] ]\n";
					cout << "Options:\n";
					cout << "  h, ? - show this help screen\n";
					cout << "  t:<n> - set timeout to <n> ms\n";
					cout << "  p:<n> - set remote agent port to <n>, ex: 123\n";
					cout << "  a - print all attempts\n";
					cout << "  T - enable trace\n";
					cout << "Arguments:\n";
					cout << "  net - address of network to scan (x.y.z.w), ex: 1.2.128.4\n";
					cout << "  mask - network mask (x.y.z.w), ex: 255,255,0,0\n";
					cout << "  community - community name to scan for, ex: public \n";
					cout << "  oid - oid name to scan for, ex: 1.3.6.1.2.1.1.1.0 \n";
					return 0;
					break;
				case 'p':
					if( (*(*argv+2)) == ':') { port = atoi(*argv+3); } break;
				case 't':
					if( (*(*argv+2)) == ':') { tmo = atoi(*argv+3);	} break;
				case 'a':
					if( (*(*argv+2)) == '\0') { longOutput = true; } break;
				case 'T':
					if( (*(*argv+2)) == '\0') { SNMPTracer::getTracer()->enable(true); } break;
				}
			} else {
				switch(argn) {
					case 0: net = inet_addr(*argv);	break;
					case 1:	mask = inet_addr(*argv); break;
					case 2:	community = *argv; break;
					case 3: oid = *argv; break;
				}
				argn++;
			}
			++argv; --argc;
		}

		std::string shost;
		unsigned long host;
		unsigned long subhost = ntohl(net & (~mask));
		unsigned long maxsubhost = ntohl(0xFFFFFFFF & (~mask));

		unsigned long scanCount = 0;
		unsigned long foundCount = 0;
		DWORD tnow = ::GetTickCount();
		int maxInSet = 64;
		
		SNMPRemoteAgentSet agentSet;
		SNMPRequestSet requestSet;
		while(subhost <= maxsubhost) {
			while(subhost <= maxsubhost && requestSet.getRequestCount() < maxInSet) {
				if(subhost == 0) {
					subhost++;
				}
				host = (net & mask) | htonl(subhost);
				struct in_addr addr;
				addr.S_un.S_addr = host;
				shost = std::string(inet_ntoa(addr));
				agentSet.addAgent(shost, community, port);
				SNMPObject obj;
				if(oid == "") {
					obj.setOID("0.0");
					SNMPRemoteAgentGetNextRequest getNextReq(&obj, 1, agentSet.getAgent(agentSet.getAgentCount()-1));
					requestSet.addRequest(&getNextReq);
				} else {		
					obj.setOID(oid);
					SNMPRemoteAgentGetRequest getReq(&obj, 1, agentSet.getAgent(agentSet.getAgentCount()-1));
					requestSet.addRequest(&getReq);
				}
				subhost++;
				scanCount++;
			}
			if(requestSet.getRequestCount()) {
				for(int i=0; i<requestSet.getRequestCount(); i++) {
					requestSet.getRequest(i)->execute();
				}
				requestSet.wait(tmo);

				for(int i=0; i<requestSet.getRequestCount(); i++) {
					if(requestSet.getRequest(i)->getResultCount()) {
						foundCount++;
						std::cout << agentSet.getAgent(i)->getName() << ": " << requestSet.getRequest(i)->getResult(0)->getValueAsString() << "\n";
					} else {							
						if(longOutput) {
							std::cout << agentSet.getAgent(i)->getName() << ": " << "n/a\n";
						}
					}
				}
				requestSet.clear();
				agentSet.clear();
			}
		}
	
		tnow = ::GetTickCount() - tnow;
			std::cout << "\nFound " << foundCount << "/" << scanCount << " in " << ((double)tnow)/((double)1000) << " seconds.\n";
		std::cout << "\n";
		return 0;
#endif

#ifdef _SNMP_TRAP_	
		std::string host = "172.18.200.90";
		std::string community = "public";
		++argv;
		--argc;
		std::string oid = "";
		std::string oidarray[512];
		int argn = 0;
		while(argc>0) {
			if(IS_OPTION(**argv)) {
				switch(*(*argv+1)) {
				case 'h':case '?':
					cout << "snmptrap [-h?] [-T] host [community [oid]]\n";
					cout << "Options:\n";
					cout << "  h, ? - show this help screen\n";
					cout << "  T - enable trace\n";
					cout << "Arguments:\n";
					cout << "  host - address or name of agent's host\n";
					cout << "  community - community name to scan for, ex: public \n";
					cout << "  oid - trap filter \n";
					return 0;
					break;
				case 'T':
					if( (*(*argv+2)) == '\0') { SNMPTracer::getTracer()->enable(true); } break;
				}
			} else {
				switch(argn) {
					case 0: host = *argv; break;
					case 1: community = *argv; break;
					case 2: oid = *argv; break;
				}
				argn++;
			}
			++argv; --argc;
		}

		class MySession : public SNMPSession {
		public: 
			virtual void processTrap(const SNMPObject & obj) {
				std::cout << obj.getDisplayInformation() << "\n";
			}
		};

		MySession session;
		SNMPRemoteAgent ragent(host, community, 0, &session);
		SNMPTrap trap(oid, &ragent);
		trap.enable();
		
		while(true) {
			::Sleep(1000);
		}

		trap.disable();
		std::cout << "\n";
		return 0;		
#endif

	} catch (SNMPException * pe) {
		std::cout << "\n\n";
		std::cout << pe->toStringStack();
		std::cout << "\n";
		delete pe;
	}
	return 0;
}