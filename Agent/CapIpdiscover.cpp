//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Ipdiscover.cpp: implementation of the CIpdiscover class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapIpdiscover.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


UINT WINAPI IpDiscoverWorkerThread( LPVOID pParam )
{
	// Local
	CCapIpdiscover* pIpd = (CCapIpdiscover*)pParam;
	ULONG ulCurrentIp = pIpd->GetNetworkNumber() + pIpd->GetIpToScan();
	HRESULT hr;
	ULONG ulLen=6;
	ULONG pulMac[2];
	in_addr ipa;
	CString csSent, csMac, csName;
	hostent *hName = NULL;

	memset (pulMac, 0xff, sizeof (pulMac));
	// ARP sent to ulCurrentIp
	hr = pIpd->lpfn_SendARP( pIpd->lpfn_ntohl( ulCurrentIp), 0, pulMac, &ulLen);

	if( ulLen > 0 && hr == NO_ERROR && ulLen==6)
	{
		size_t i, j;
		char * szMac = new char[ulLen*3];
		PBYTE pbHexMac = (PBYTE) pulMac;
		
		// Convert the binary MAC address into human-readable
		for (i = 0, j = 0; i < ulLen - 1; ++i)
		{
			j += sprintf (szMac + j, "%02X:", pbHexMac[i]);
		}			

		sprintf (szMac + j, "%02X", pbHexMac[i]);

		ipa.S_un.S_addr = pIpd->lpfn_htonl(ulCurrentIp);
		csSent = pIpd->lpfn_inet_ntoa(ipa);		

		struct sockaddr_in saIn;
		char szHostName[256];
		char szServInfo[256];
		
		//u_short port;
		//port = 27015;
		// Set up sockaddr_in structure which is passed
		// to the getnameinfo function
		saIn.sin_family = AF_INET;
		saIn.sin_addr.s_addr = inet_addr( GetAnsiFromUnicode( csSent) );
		//saGNI.sin_port = htons(port);

		// Call getnameinfo (either NetBios or DNS request)
		if( getnameinfo((SOCKADDR *)&saIn, sizeof(sockaddr), szHostName, 256, szServInfo, 256, NI_NUMERICSERV) != 0)
		{
			CString csMessage;

			csMessage.Format( _T( "IPDISCOVER => getnameinfo() for host <%s> failed because error #%ld."), csSent, WSAGetLastError());
			pIpd->LockCriticalSection();
			pIpd->ReportHostFailed( csMessage);
			pIpd->UnlockCriticalSection();		
		}

		csName = szHostName;
		csMac = szMac;
		pIpd->LockCriticalSection();
		pIpd->AddHostFound( csSent, csMac, csName);
		pIpd->UnlockCriticalSection();		
		delete [] szMac;
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapIpdiscover::CCapIpdiscover()
{
	m_ulIpToScan	= 0;
	m_ulNetNumber	= 0;
	m_ulHostFound	= 0;
	m_ulIpDisc_lat	= IPDISCOVER_DEFAULT_LATENCY;;
	m_bFastIp		= FALSE;
	m_dwMaxWaitTime = 3600;
}

CCapIpdiscover::~CCapIpdiscover()
{
}

// Overload the capacityAbstract to set the ipdiscover parameters
void CCapIpdiscover::setPrologResp(CPrologResponse* pPrologResp)
{
	CCapacityAbstract::setPrologResp(pPrologResp);
	if(pPrologResp->isIpdiscoverRequired())
	{
		m_csLan = pPrologResp->getIpdiscoverLan();
		m_csLatency = pPrologResp->getIpdiscoverLatency();
		m_ulIpDisc_lat = _ttol( m_csLatency);
		m_ulNetNumber = _ttol( m_csLan);
	}
}

// set the lan to scan (can be used manually)
void CCapIpdiscover::setLocalNetwork( LPCTSTR lpstrLan)
{
	m_csLan = lpstrLan;
	m_ulNetNumber = _ttol( m_csLan);
}

// set the latency manually
void CCapIpdiscover::setLatency( LPCTSTR lpstrLatency)
{
	m_csLatency = lpstrLatency;
	m_ulIpDisc_lat = _ttol( m_csLatency);
}


// Launch the scan
BOOL CCapIpdiscover::scanLocalNetwork()
{
	ULONG    ulCurrentIp=1;
	CObArray handlesThread;
	DWORD	 dwErr;
	CMarkup* pXml;
	DOUBLE	waitedMaxThreads = 0;
	ULONG   ulIpMask, ulMaxIp;	
	CString csSent, csIncrIp;
	CString csMask=m_pInventory->m_NetworkList.GetMaskByNetNumber(m_csLan);	
	
	// Update Didier LIROULET: To allow running under Win 95 or NT4 without iphlpapi.dll
	// Load the IPHLPAPI 32 bit and winsock 2 DLL
	HINSTANCE	hDllIpHlpApi, hDllWS2;
	hDllIpHlpApi = LoadLibrary( _T( "iphlpapi.dll"));
	hDllWS2 = LoadLibrary( _T( "ws2_32.dll"));


	if ((hDllIpHlpApi == NULL) || (hDllWS2 == NULL))
	{
		// Tell the user that we could not find a usable WinSock DLL.                                  
		m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "IPDISCOVER => Failed to load <iphlpapi.dll> or <ws2_32.dll> !"));
	}
	else
	{
		// Load the SendARP, htonl, ntol, inet_addr and inet_ntoa functions
		if (((*(FARPROC*)&lpfn_SendARP = GetProcAddress( hDllIpHlpApi, "SendARP")) == NULL) ||
			((*(FARPROC*)&lpfn_htonl = GetProcAddress( hDllWS2, "htonl")) == NULL) ||
			((*(FARPROC*)&lpfn_ntohl = GetProcAddress( hDllWS2, "ntohl")) == NULL) ||
			((*(FARPROC*)&lpfn_inet_addr = GetProcAddress( hDllWS2, "inet_addr")) == NULL) ||
			((*(FARPROC*)&lpfn_inet_ntoa = GetProcAddress( hDllWS2, "inet_ntoa")) == NULL))
		{
			// Tell the user that we could not find a usable SetupAPI DLL or wsock32 DLL.                                  
			m_pLogger->log( LOG_PRIORITY_ERROR, _T( "IPDISCOVER => Failed to load <SendARP> function from iphlpapi.dll or <htonl/ntol/inet_addr/inet_ntoa> functions from ws2_32.dll !"));
		}
		else
		{					

			if(csMask.GetLength() == 0) {
				m_pLogger->log( LOG_PRIORITY_ERROR, _T("IPDISCOVER => No mask is available for the given network number: %s"), csMask );
			}
			else {
				// Retrieve the inventory xml pointer and add elements
				pXml=m_pInventory->getXmlPointerContent();
				pXml->AddElem( _T( "IPDISCOVER"));

				m_pLogger->log( LOG_PRIORITY_DEBUG, _T("IPDISCOVER => Scanning to detect IPv4 enabled hosts for network <%s> with <%i> ms between each request"),m_csLan.GetBuffer(0),m_ulIpDisc_lat);
				ulIpMask = lpfn_htonl( lpfn_inet_addr( GetAnsiFromUnicode( csMask)));

				m_ulNetNumber = lpfn_ntohl( lpfn_inet_addr( GetAnsiFromUnicode( m_csLan)));
				ulMaxIp = ~ulIpMask;		
									
				// Launching worker threads
				do {
					// Cleaning dead threads
					int thdCount=0;
					while( thdCount < handlesThread.GetSize() )
					{
						GetExitCodeThread( ((CWinThread*)handlesThread.GetAt(thdCount))->m_hThread, &dwErr );
						if( dwErr != STILL_ACTIVE )
						{																			
							delete (CWinThread*)handlesThread.GetAt(thdCount);
							handlesThread.RemoveAt(thdCount);
							thdCount--;
						}
						thdCount++;
					}

					// Too many threads for too long (1 hour)
					if( waitedMaxThreads > (m_dwMaxWaitTime * 1000) ) {
						m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "IPDISCOVER => Waited %u ms with max threads, skipping ipdiscover"), m_dwMaxWaitTime);
						return FALSE;
					}

					// Too many threads ?
					if( handlesThread.GetSize()> (m_bFastIp?MAX_FASTIP_THREADS:MAX_THREADS)-4)
					{
						Sleep(1);
						waitedMaxThreads += 1;
						continue;
					}

					// Create new worker thread
					m_ulIpToScan = ulCurrentIp;
					CWinThread* pT;								
					pT = AfxBeginThread((AFX_THREADPROC)IpDiscoverWorkerThread, (LPVOID)this, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
					// Add it to thread array
					handlesThread.Add((CObject*)pT);
					// Disable autodelete of thread pointer, even after thread ended
					pT->m_bAutoDelete = FALSE;
					// Start thread
					pT->ResumeThread();
					ulCurrentIp++;
					// Wait latency before launching a new thread, to avoid overloading network
					if( !m_bFastIp )
						Sleep( m_ulIpDisc_lat );
				}
				while( ulCurrentIp < ulMaxIp && ulCurrentIp <= NB_IP_MAX );
						
				UINT uTimeWaited = 0;
				UINT uTimeToWait = 60;
				UINT uMaxTimeToWait = 300000;
				BOOL allThreadsDead ;

				// Waiting for threads to die
				do  {
					DWORD dwErr;
					allThreadsDead = TRUE;
											
					int thdCount=0;
					while( thdCount < handlesThread.GetSize() )
					{								
						GetExitCodeThread( ((CWinThread*)handlesThread.GetAt(thdCount))->m_hThread, &dwErr );
						if( dwErr == STILL_ACTIVE )
						{
							//A thread still alive after %u msecs, waiting %u msecs more\n",  timeWaited, timeToWait);
							allThreadsDead = FALSE;
							break;
						}
						else
						{																		
							// A thread died after %u msecs\n",  timeWaited);
							delete (CWinThread*)handlesThread.GetAt(thdCount);
							handlesThread.RemoveAt(thdCount);
							thdCount--;
						}
						thdCount++;
					}
					Sleep( uTimeToWait);
					uTimeWaited += uTimeToWait;
				}
				while(!allThreadsDead && (uTimeWaited < uMaxTimeToWait));

				// Cleaning living threads
				if( uTimeWaited >= uMaxTimeToWait)
				{
					for( int thdCount=0; thdCount < handlesThread.GetSize(); thdCount++)
					{
						((CWinThread*)handlesThread.GetAt(thdCount))->SuspendThread();
						delete (CWinThread*)handlesThread.GetAt(thdCount);
						handlesThread.RemoveAt(thdCount);
						m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "IPDISCOVER => Thread still alive after %u msecs, thread killed"), uTimeWaited);
					}
				}
				else
					m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "IPDISCOVER => Waited %u ms for the threads to die"), uTimeWaited);
				
				m_pLogger->log( LOG_PRIORITY_DEBUG,  _T("IPDISCOVER => NETWORK scan finished, %u hosts found, %u scanned"), m_ulHostFound, ulCurrentIp);
				pXml->OutOfElem();
			}
			FreeLibrary( hDllIpHlpApi);
			FreeLibrary( hDllWS2);
		}
	}
	return TRUE;
}

// Speed up detection by decreasing the latency significantly
void CCapIpdiscover::setFastIp()
{
	m_ulIpDisc_lat = IPDISCOVER_MIN_LATENCY;
	m_bFastIp = TRUE;
}

// Do not wait for the threads termination
void CCapIpdiscover::setNoWait()
{
	m_dwMaxWaitTime = 60;
}

ULONG CCapIpdiscover::GetIpToScan()
{
	return m_ulIpToScan;
}

ULONG CCapIpdiscover::GetNetworkNumber()
{
	return m_ulNetNumber;
}

BOOL CCapIpdiscover::LockCriticalSection()
{
	return m_cs.Lock();
}

BOOL CCapIpdiscover::UnlockCriticalSection()
{
	return m_cs.Unlock();
}

void CCapIpdiscover::AddHostFound( LPCTSTR lpstrIP, LPCTSTR lpstrMac, LPCTSTR lpstrHostName)
{
	m_ulHostFound++;
	CMarkup *pXml = m_pInventory->getXmlPointerContent();
	pXml->FindFirstElem( _T( "IPDISCOVER"));
	pXml->AddElem( _T( "H"));
		pXml->AddChildElem( _T( "I"),lpstrIP);
		pXml->AddChildElem( _T( "M"),lpstrMac);
		pXml->AddChildElem( _T( "N"),lpstrHostName);
	pXml->OutOfElem();
	m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "IPDISCOVER => Computer found @IPv4:%s @MAC:%s NAME:%s"), lpstrIP, lpstrMac, lpstrHostName);
	
	std::ofstream snmplist;
	snmplist.open("snmplist.txt", std::ios_base::app);
	std::string IP = CT2A(lpstrIP);
	std::string MAC = CT2A(lpstrMac);
	snmplist << IP << " " << MAC << "\n";
	snmplist.close();
	
}

void CCapIpdiscover::ReportHostFailed( LPCTSTR lpstrMessage)
{
	m_pLogger->log( LOG_PRIORITY_ERROR, lpstrMessage);	
}
