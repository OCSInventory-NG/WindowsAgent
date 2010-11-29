//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// OCSInventoryState.cpp: implementation of the COCSInventoryState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OCSInventoryState.h"
#include "OcsUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COCSInventoryState::COCSInventoryState()
{
	Clear();
}

COCSInventoryState::~COCSInventoryState()
{
	Clear();
}

void COCSInventoryState::Clear()
{
	m_csHardware.Empty();
	m_csBios.Empty();
	m_csMemories.Empty();
	m_csSlots.Empty();
	m_csRegistry.Empty();
	m_csControllers.Empty();
	m_csMonitors.Empty();
	m_csPorts.Empty();
	m_csStorages.Empty();
	m_csDrives.Empty();
	m_csInputs.Empty();
	m_csModems.Empty();
	m_csNetworks.Empty();
	m_csPrinters.Empty();
	m_csSounds.Empty();
	m_csVideos.Empty();
	m_csSoftwares.Empty();
}

LPCTSTR COCSInventoryState::GetHardware()
{
	return m_csHardware;
}

LPCTSTR COCSInventoryState::GetBios()
{
	return m_csBios;
}

LPCTSTR COCSInventoryState::GetMemories()
{
	return m_csMemories;
}

LPCTSTR COCSInventoryState::GetSlots()
{
	return m_csSlots;
}

LPCTSTR COCSInventoryState::GetRegistry()
{
	return m_csRegistry;
}

LPCTSTR COCSInventoryState::GetControllers()
{
	return m_csControllers;
}

LPCTSTR COCSInventoryState::GetMonitors()
{
	return m_csMonitors;
}

LPCTSTR COCSInventoryState::GetPorts()
{
	return m_csPorts;
}

LPCTSTR COCSInventoryState::GetStorages()
{
	return m_csStorages;
}

LPCTSTR COCSInventoryState::GetDrives()
{
	return m_csDrives;
}

LPCTSTR COCSInventoryState::GetInputs()
{
	return m_csInputs;
}

LPCTSTR COCSInventoryState::GetModems()
{
	return m_csModems;
}

LPCTSTR COCSInventoryState::GetNetworks()
{
	return m_csNetworks;
}

LPCTSTR COCSInventoryState::GetPrinters()
{
	return m_csPrinters;
}

LPCTSTR COCSInventoryState::GetSounds()
{
	return m_csSounds;
}

LPCTSTR COCSInventoryState::GetVideos()
{
	return m_csVideos;
}

LPCTSTR COCSInventoryState::GetSoftwares()
{
	return m_csSoftwares;
}

BOOL COCSInventoryState::ParseFromXML( LPCTSTR lpstrXml)
{
	try
	{
		CMarkup x;
		TiXmlElement *pState, *pNode;

		x.SetDoc( lpstrXml);

		pState = x.FindFirstElem( _T( "LAST_STATE"));

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "HARDWARE"));		
		m_csHardware = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "BIOS"));
		m_csBios = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "MEMORIES"));
		m_csMemories = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "SLOTS"));
		m_csSlots = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "REGISTRY"));
		m_csRegistry = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "CONTROLLERS"));
		m_csControllers = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "MONITORS"));
		m_csMonitors = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "PORTS"));
		m_csPorts = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "STORAGES"));
		m_csStorages = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "DRIVES"));
		m_csDrives = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "INPUTS"));
		m_csInputs = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "MODEMS"));
		m_csModems = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "NETWORKS"));
		m_csNetworks = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "PRINTERS"));
		m_csPrinters = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "SOUNDS"));
		m_csSounds = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "VIDEOS"));
		m_csVideos = x.GetData( pNode);

		x.ResetPos( pState);
		pNode = x.FindFirstElem( _T( "SOFTWARES"));
		m_csSoftwares = x.GetData( pNode);

		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL COCSInventoryState::FormatXML(CMarkup* pX)
{
	pX->AddElem( _T( "LAST_STATE"));
		pX->AddChildElem( _T( "HARDWARE"),m_csHardware);
		pX->AddChildElem( _T( "BIOS"), m_csBios);
		pX->AddChildElem( _T( "MEMORIES"),m_csMemories);
		pX->AddChildElem( _T( "SLOTS"),m_csSlots);
		pX->AddChildElem( _T( "REGISTRY"),m_csRegistry);
		pX->AddChildElem( _T( "CONTROLLERS"),m_csControllers);
		pX->AddChildElem( _T( "MONITORS"),m_csMonitors);
		pX->AddChildElem( _T( "PORTS"),m_csPorts);
		pX->AddChildElem( _T( "STORAGES"),m_csStorages);
		pX->AddChildElem( _T( "DRIVES"),m_csDrives);
		pX->AddChildElem( _T( "INPUTS"),m_csInputs);
		pX->AddChildElem( _T( "MODEMS"),m_csModems);
		pX->AddChildElem( _T( "NETWORKS"),m_csNetworks);
		pX->AddChildElem( _T( "PRINTERS"),m_csPrinters);
		pX->AddChildElem( _T( "SOUNDS"),m_csSounds);
		pX->AddChildElem( _T( "VIDEOS"),m_csVideos);
		pX->AddChildElem( _T( "SOFTWARES"),m_csSoftwares);
	pX->OutOfElem();
	return TRUE;
}

void COCSInventoryState::SetHardware( LPCTSTR lpstrValue)
{
	m_csHardware = lpstrValue;
}

void COCSInventoryState::SetBios( LPCTSTR lpstrValue)
{
	m_csBios = lpstrValue;
}

void COCSInventoryState::SetMemories( LPCTSTR lpstrValue)
{
	m_csMemories = lpstrValue;
}

void COCSInventoryState::SetSlots( LPCTSTR lpstrValue)
{
	m_csSlots = lpstrValue;
}

void COCSInventoryState::SetRegistry( LPCTSTR lpstrValue)
{
	m_csRegistry = lpstrValue;
}

void COCSInventoryState::SetControllers( LPCTSTR lpstrValue)
{
	m_csControllers = lpstrValue;
}

void COCSInventoryState::SetMonitors( LPCTSTR lpstrValue)
{
	m_csMonitors = lpstrValue;
}

void COCSInventoryState::SetPorts( LPCTSTR lpstrValue)
{
	m_csPorts = lpstrValue;
}

void COCSInventoryState::SetStorages( LPCTSTR lpstrValue)
{
	m_csStorages = lpstrValue;
}

void COCSInventoryState::SetDrives( LPCTSTR lpstrValue)
{
	m_csDrives = lpstrValue;
}

void COCSInventoryState::SetInputs( LPCTSTR lpstrValue)
{
	m_csInputs = lpstrValue;
}

void COCSInventoryState::SetModems( LPCTSTR lpstrValue)
{
	m_csModems = lpstrValue;
}

void COCSInventoryState::SetNetworks( LPCTSTR lpstrValue)
{
	m_csNetworks = lpstrValue;
}

void COCSInventoryState::SetPrinters( LPCTSTR lpstrValue)
{
	m_csPrinters = lpstrValue;
}

void COCSInventoryState::SetSounds( LPCTSTR lpstrValue)
{
	m_csSounds = lpstrValue;
}

void COCSInventoryState::SetVideos( LPCTSTR lpstrValue)
{
	m_csVideos = lpstrValue;
}

void COCSInventoryState::SetSoftwares( LPCTSTR lpstrValue)
{
	m_csSoftwares = lpstrValue;
}

BOOL COCSInventoryState::ReadFromFile(LPCTSTR lpstrFilename)
{
	CString csBuffer;

	// Fill Device hardware properties from string
	if (!LoadFileToText( csBuffer, lpstrFilename))
		return FALSE;
	if (!ParseFromXML( csBuffer))
		return FALSE;
	return TRUE;
}

BOOL COCSInventoryState::WriteToFile(LPCTSTR lpstrFilename)
{
	CMarkup	myXml;

	FormatXML( &myXml);
	if (!WriteTextToFile( myXml.GetDoc(), _tcslen( myXml.GetDoc()), lpstrFilename))
		return FALSE;
	return TRUE;
}
