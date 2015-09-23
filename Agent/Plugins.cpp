//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Plugins.cpp: implementation of the CPlugins class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Plugins.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlugins::CPlugins()
{
	m_pLogger = getOcsLogger();
	Load();
}

CPlugins::~CPlugins()
{
	Unload();
}

int CPlugins::Load( LPCTSTR lpstrPath)
{
	CString	csPath;
	int		nPlugin = 0,
			nCount = 0;
	BOOL	bFoundPlugins = FALSE,
			bValidPlugin;

	try
	{
		CFileFind			cFinder;

		// plugin struct array initialization
		for (int i=0; i<MAX_PLUGINS; i++ )
		{
			m_plugin[i].hDll		= NULL;
			m_plugin[i].pInventory	= NULL;
			m_plugin[i].pPrologResp	= NULL;
			m_plugin[i].pPrologWrite= NULL;
			m_plugin[i].pStart		= NULL;
			m_plugin[i].pEnd		= NULL;
			m_plugin[i].pClean		= NULL;
		}
		if ((lpstrPath == NULL) || (_tcslen( lpstrPath) == 0))
			// Use standard install path
			csPath.Format( _T( "%s\\plugins"), getInstallFolder());
		else
			// Use provided path to search for plugins
			csPath = lpstrPath;
		// Search for DLL into path
		m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Searching for Plug-in DLL(s) in folder <%s>"), csPath);
		csPath +=  _T( "\\*.dll");
		bFoundPlugins = cFinder.FindFile( csPath);
		while (bFoundPlugins)
		{
			bValidPlugin = FALSE;
			// One DLL found, try to load it
			bFoundPlugins = cFinder.FindNextFile();
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Trying to validate DLL <%s> as a Plug-in"), cFinder.GetFileName());
			if( (m_plugin[nPlugin].hDll = LoadLibrary( cFinder.GetFilePath())) == NULL )
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => Failed loading Plug-in DLL <%s>, %s"), cFinder.GetFileName(), LookupError( GetLastError()));
				continue;
			}
			// Get name
			m_plugin[nPlugin].csName = cFinder.GetFileTitle();
			// Try to load each API entry
			if( (m_plugin[nPlugin].pEnd = (HOOK_END)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_END_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No End hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if( (m_plugin[nPlugin].pStart = (HOOK_START)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_START_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No Start hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if( (m_plugin[nPlugin].pClean = (HOOK_CLEAN)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_CLEAN_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No Clean hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if( (m_plugin[nPlugin].pInventory = (HOOK_INVENTORY)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_INVENTORY_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No Inventory hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if( (m_plugin[nPlugin].pPrologWrite = (HOOK_PROLOG_WRITE)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_PROLOGWRITE_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No Prolog Read hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if( (m_plugin[nPlugin].pPrologResp = (HOOK_PROLOG_RESP)GetProcAddress( m_plugin[nPlugin].hDll, "OCS_CALL_PROLOGRESP_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => No Prolog response hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			else
				// Hook available, so valid plugin
				bValidPlugin = TRUE;
			if (bValidPlugin)
			{
				// At least one hook available and plugin valid
				m_pLogger->log( LOG_PRIORITY_NOTICE,  _T( "DLL PLUGIN => Plug-in <%s> loaded"), m_plugin[nPlugin].csName);
				// Store and increase plugin number
				nPlugin++;
			}
			else
			{
				// Do not store DLL as a plugin
				m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => DLL <%s> is not a valid Plug-in"), cFinder.GetFileName());
				FreeLibrary( m_plugin[nPlugin].hDll );
			}
			nCount++;
		}
		cFinder.Close();
		m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => %d DLL Plug-in(s) successfully loaded on %d DLL(s) found"), nPlugin, nCount);
		return nPlugin;
	}
	catch (CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "DLL PLUGIN => Error while parsing Plug-in directory <%s>"), LookupError( pEx));
		pEx->Delete();
		return -1;
	}
}

void CPlugins::Unload()
{
	int nPlugin;
	for( nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){
		if( m_plugin[nPlugin].hDll ){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Freeing Plug-in DLL <%s>"), m_plugin[nPlugin].csName);
			FreeLibrary( m_plugin[nPlugin].hDll );
		}
	}
}

void CPlugins::startHook()
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){
		
		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pStart){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching start hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
			try{
				if( (*m_plugin[nPlugin].pStart)() != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:start failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:start successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:start for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No start hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}
}

void CPlugins::endHook(CInventoryResponse *pInventoryResp)
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){

		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pEnd){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching end hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
			try{
				if( (*m_plugin[nPlugin].pEnd)(pInventoryResp) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:end failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:end successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:end for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No end hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}
}

void CPlugins::prologRespHook(CPrologResponse* pPrologResp)
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){

		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pPrologResp){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching prolog_resp hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
			try{
				if( (*m_plugin[nPlugin].pPrologResp)(pPrologResp) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:prolog_resp failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:prolog_resp successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:inventory for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No prolog_resp hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}

}

void CPlugins::prologWriteHook(CPrologRequest *pProlog)
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){

		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pPrologWrite){
			try{
				m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching prolog_write hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
				if( m_plugin[nPlugin].pPrologWrite(pProlog) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:prolog_write failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:prolog_write successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:prolog_write for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No prolog_write hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}
}

void CPlugins::inventoryHook(CInventoryRequest *pInventory)
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){

		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pInventory){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching inventory hook for Plug-in <%s>"), m_plugin[nPlugin].csName);

			try{
				if( (*m_plugin[nPlugin].pInventory)(pInventory) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:inventory failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:inventory successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:inventory for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No inventory hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}
}

void CPlugins::cleanHook()
{
	for( int nPlugin=0; nPlugin<MAX_PLUGINS; nPlugin++ ){

		if(!m_plugin[nPlugin].hDll)
			break;

		if(m_plugin[nPlugin].pClean){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Launching clean hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
			try{
				if( (*m_plugin[nPlugin].pClean)() != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:clean failed for Plug-in <%s>"),m_plugin[nPlugin].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => Hook:clean successful for Plug-in <%s>"),m_plugin[nPlugin].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "DLL PLUGIN => Hook:clean for Plug-in <%s> raised an exception!!"),m_plugin[nPlugin].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "DLL PLUGIN => No clean hook for Plug-in <%s>"), m_plugin[nPlugin].csName);
		}
	}
}
