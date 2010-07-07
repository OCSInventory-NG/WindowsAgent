//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Ipdiscover.h: interface for the CIpdiscover class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPDISCOVER_H__D6015530_B5EE_45F3_B92A_021A5894341A__INCLUDED_)
#define AFX_IPDISCOVER_H__D6015530_B5EE_45F3_B92A_021A5894341A__INCLUDED_

#pragma once

#define IPDISCOVER_MIN_LATENCY 10 //ms
#define IPDISCOVER_DEFAULT_LATENCY 100 //ms
#define NB_IP_MAX				65534
#define DEFAULT_PORT			80
#define MAX_THREADS				300
#define MAX_FASTIP_THREADS		800

#ifndef WIN32
#include <pthread.h>
#define THREAD_CC
#define THREAD_TYPE                    pthread_t
#define THREAD_CREATE(tid, entry, arg) pthread_create(&(tid), NULL, \
                                                      (entry), (arg))
#else
#include <windows.h>
#define THREAD_CC                      __cdecl
#define THREAD_TYPE                    DWORD
#define THREAD_CREATE(tid, entry, arg) do { _beginthread((entry), 0, (arg));\
                                            (tid) = GetCurrentThreadId();   \
                                       } while (0)
#endif /* WIN32 */

#include "CapacityAbstract.h"
#include <Afxmt.h>

class CCapIpdiscover : public CCapacityAbstract
{

public: // Methods

	// Standard constructor.and destructor
	CCapIpdiscover();
	~CCapIpdiscover();

	// overloaded CCapacity method to set ipdiscover parameters
	void setPrologResp(CPrologResponse* pPrologResp);

	// Launch the scan
	BOOL scanLocalNetwork();
	// set the lan to scan (can be used manually)
	void setLocalNetwork( LPCTSTR);
	// set the latency (can be used manually)
	void setLatency( LPCTSTR);
	// Speed up detection by decreasing the latency significantly
	void setFastIp();
	// Do not wait for the threads termination
	void setNoWait();
	// Get current IP address worker thread must scan
	ULONG CCapIpdiscover::GetIpToScan();
	// Return the network number
	ULONG GetNetworkNumber();
	// Indicates that a new host was found
	void AddHostFound( LPCTSTR lpstrIP, LPCTSTR lpstrMac, LPCTSTR lpstrHostName);
	// Report that there was an error for host
	void ReportHostFailed( LPCTSTR lpstrMessage);
	// Lock and unlock access to the object, to grant worker threads exclusive access when updating results
	BOOL LockCriticalSection();
	BOOL UnlockCriticalSection();

	// Function SendARP of IPHLPAPI.DLL (available on Windows 98 or higher and NT4 SP4 or higher)
	DWORD (WINAPI *lpfn_SendARP)(IPAddr DestIP, IPAddr SrcIP, PULONG pMacAddr, PULONG PhyAddrLen);
	
	// Functions from Winsock2
	u_long (WINAPI *lpfn_htonl)(u_long hostlong);
	u_long (WINAPI *lpfn_ntohl)(u_long netlong);
	unsigned long (WINAPI *lpfn_inet_addr)(const char* cp);
	char* FAR (WINAPI *lpfn_inet_ntoa)(struct in_addr in);

private: // Attributes
	ULONG m_ulIpToScan;		// IP to scan
	ULONG m_ulNetNumber;	// Network scanned
	CString m_csLan;
	ULONG m_ulHostFound;	// Number of hosts found
	ULONG m_ulIpDisc_lat;	// Latency between each thread 
	CString m_csLatency;
	BOOL m_bFastIp;			// Do not wait Latency
	CCriticalSection m_cs;	// To grant exclusive access
	DWORD m_dwMaxWaitTime;
};


#endif // !defined(AFX_IPDISCOVER_H__D6015530_B5EE_45F3_B92A_021A5894341A__INCLUDED_)
