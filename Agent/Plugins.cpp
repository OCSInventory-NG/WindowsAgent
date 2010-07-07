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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlugins::CPlugins()
{
	int nPlugins;

	m_bAlreadyLoaded=FALSE;
	m_pLogger = getOcsLogger();
	
	if(!m_bAlreadyLoaded)
	{
		if ((nPlugins = Load()) >= 0)
		{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => %d Plug-in(s) succesfully loaded"), nPlugins);
			m_bAlreadyLoaded = TRUE;
		}
		else
		{
			m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Failed to load Plug-in(s)"));
		}
	}
}

CPlugins::~CPlugins()
{
	int pluginNumber;
	for( pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){
		if( m_plugin[pluginNumber].hDll ){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Freeing Plug-in DLL <%s>"), m_plugin[pluginNumber].csName);
			FreeLibrary( m_plugin[pluginNumber].hDll );
		}
	}
}

int CPlugins::Load( LPCTSTR lpstrPath)
{
	CString				csPath;
	int					pluginNumber = 0;
	BOOL				bFoundPlugins = FALSE;

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
		m_pLogger->log( LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Searching for Plug-in DLL(s) in folder <%s>"), csPath);
		csPath +=  _T( "\\*.dll");
		bFoundPlugins = cFinder.FindFile( csPath);
		while (bFoundPlugins)
		{
			// One DLL found, try to load it
			bFoundPlugins = cFinder.FindNextFile();
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Found Plug-in DLL <%s>"), cFinder.GetFileName());
			if( (m_plugin[pluginNumber].hDll = LoadLibrary( cFinder.GetFilePath())) == NULL )
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => Failed loading Plug-in DLL <%s>, %s"), cFinder.GetFileName(), LookupError( GetLastError()));
				continue;
			}
			// Get name
			m_plugin[pluginNumber].csName = cFinder.GetFileTitle();
			// Try to load each API entry
			if( (m_plugin[pluginNumber].pEnd = (HOOK_END)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_END_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No End hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}

			if( (m_plugin[pluginNumber].pStart = (HOOK_START)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_START_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No Start hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}

			if( (m_plugin[pluginNumber].pClean = (HOOK_CLEAN)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_CLEAN_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No Clean hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}

			if( (m_plugin[pluginNumber].pInventory = (HOOK_INVENTORY)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_INVENTORY_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No Inventory hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}

			if( (m_plugin[pluginNumber].pPrologWrite = (HOOK_PROLOG_WRITE)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_PROLOGWRITE_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No Prolog Read hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}

			if( (m_plugin[pluginNumber].pPrologResp = (HOOK_PROLOG_RESP)GetProcAddress( m_plugin[pluginNumber].hDll, "OCS_CALL_PROLOGRESP_EXPORTED")) == NULL)
			{
				m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => No Prolog response hook for Plug-in <%s>, %s"), cFinder.GetFileTitle(), LookupError( GetLastError()));
			}
			// Increase plugin number
			pluginNumber++;
		}
		cFinder.Close();
		return pluginNumber;
	}
	catch (CException *pEx)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING,  _T( "PLUGIN => Error while parsing Plug-in directory <%s>"), LookupError( pEx));
		pEx->Delete();
		return -1;
	}
}



void CPlugins::startHook()
{
	for( int pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){
		
		if(!m_plugin[pluginNumber].hDll)
			break;

		if(m_plugin[pluginNumber].pStart){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Launching start hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
			try{
				if( (*m_plugin[pluginNumber].pStart)() != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:start failed for Plug-in <%s>"),m_plugin[pluginNumber].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Hook:start successful for Plug-in <%s>"),m_plugin[pluginNumber].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:start for Plug-in <%s> raised an exception!!"),m_plugin[pluginNumber].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => No start hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
		}
	}
}

void CPlugins::endHook()
{
	for( int pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){

		if(!m_plugin[pluginNumber].hDll)
			break;

		if(m_plugin[pluginNumber].pEnd){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Launching end hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
			try{
				if( (*m_plugin[pluginNumber].pEnd)() != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:end failed for Plug-in <%s>"),m_plugin[pluginNumber].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Hook:end successful for Plug-in <%s>"),m_plugin[pluginNumber].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:end for Plug-in <%s> raised an exception!!"),m_plugin[pluginNumber].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => No end hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
		}
	}

}

void CPlugins::prologRespHook(CPrologResponse* pPrologResp)
{
	for( int pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){

		if(!m_plugin[pluginNumber].hDll)
			break;

		if(m_plugin[pluginNumber].pPrologResp){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Launching prolog_resp hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
			try{
				if( (*m_plugin[pluginNumber].pPrologResp)(pPrologResp) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:prolog_resp failed for Plug-in <%s>"),m_plugin[pluginNumber].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Hook:prolog_resp successful for Plug-in <%s>"),m_plugin[pluginNumber].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:inventory for Plug-in <%s> raised an exception!!"),m_plugin[pluginNumber].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => No prolog_resp hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
		}
	}

}

void CPlugins::prologWriteHook(CPrologRequest *pProlog)
{
	for( int pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){

		if(!m_plugin[pluginNumber].hDll)
			break;

		if(m_plugin[pluginNumber].pPrologWrite){
			try{
				m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Launching prolog_write hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
				if( m_plugin[pluginNumber].pPrologWrite(pProlog) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:prolog_write failed for Plug-in <%s>"),m_plugin[pluginNumber].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Hook:prolog_write successful for Plug-in <%s>"),m_plugin[pluginNumber].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:prolog_write for Plug-in <%s> raised an exception!!"),m_plugin[pluginNumber].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => No prolog_write hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
		}
	}

}

void CPlugins::inventoryHook(CInventoryRequest *pInventory)
{
	for( int pluginNumber=0; pluginNumber<MAX_PLUGINS; pluginNumber++ ){

		if(!m_plugin[pluginNumber].hDll)
			break;

		if(m_plugin[pluginNumber].pInventory){
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Launching inventory hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);

			try{
				if( (*m_plugin[pluginNumber].pInventory)(pInventory) != PLUGIN_OK ){
					m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:inventory failed for Plug-in <%s>"),m_plugin[pluginNumber].csName);
				}
				else{
					m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => Hook:inventory successful for Plug-in <%s>"),m_plugin[pluginNumber].csName);		
				}
			}
			catch(...){
				m_pLogger->log(LOG_PRIORITY_ERROR,  _T( "PLUGIN => Hook:inventory for Plug-in <%s> raised an exception!!"),m_plugin[pluginNumber].csName);
			}
		}
		else{
			m_pLogger->log(LOG_PRIORITY_DEBUG,  _T( "PLUGIN => No inventory hook for Plug-in <%s>"), m_plugin[pluginNumber].csName);
		}
	}

}
