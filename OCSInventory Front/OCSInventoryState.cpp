//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	m_csVirtualMachines.Empty();
	m_csCPUs.Empty();
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

LPCTSTR COCSInventoryState::GetVirtualMachines()
{
	return m_csVirtualMachines;
}

LPCTSTR COCSInventoryState::GetCPUs()
{
	return m_csCPUs;
}

BOOL COCSInventoryState::ParseFromXML( CMarkup* pXml, LPCTSTR lpstrSection)
{
	try
	{
		TiXmlElement *pState, *pNode;

		pXml->ResetPos();
		pState = pXml->FindFirstElem( lpstrSection);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "HARDWARE"));		
		m_csHardware = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "BIOS"));
		m_csBios = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "MEMORIES"));
		m_csMemories = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "SLOTS"));
		m_csSlots = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "REGISTRY"));
		m_csRegistry = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "CONTROLLERS"));
		m_csControllers = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "MONITORS"));
		m_csMonitors = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "PORTS"));
		m_csPorts = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "STORAGES"));
		m_csStorages = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "DRIVES"));
		m_csDrives = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "INPUTS"));
		m_csInputs = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "MODEMS"));
		m_csModems = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "NETWORKS"));
		m_csNetworks = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "PRINTERS"));
		m_csPrinters = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "SOUNDS"));
		m_csSounds = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "VIDEOS"));
		m_csVideos = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "SOFTWARES"));
		m_csSoftwares = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "VIRTUALMACHINES"));
		m_csVirtualMachines = pXml->GetData( pNode);

		pXml->ResetPos( pState);
		pNode = pXml->FindFirstElem( _T( "CPUS"));
		m_csCPUs = pXml->GetData( pNode);

		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL COCSInventoryState::FormatXML(CMarkup* pXml, LPCTSTR lpstrSection)
{
	pXml->AddElem( lpstrSection);
		pXml->AddChildElem( _T( "HARDWARE"),m_csHardware);
		pXml->AddChildElem( _T( "BIOS"), m_csBios);
		pXml->AddChildElem( _T( "MEMORIES"),m_csMemories);
		pXml->AddChildElem( _T( "SLOTS"),m_csSlots);
		pXml->AddChildElem( _T( "REGISTRY"),m_csRegistry);
		pXml->AddChildElem( _T( "CONTROLLERS"),m_csControllers);
		pXml->AddChildElem( _T( "MONITORS"),m_csMonitors);
		pXml->AddChildElem( _T( "PORTS"),m_csPorts);
		pXml->AddChildElem( _T( "STORAGES"),m_csStorages);
		pXml->AddChildElem( _T( "DRIVES"),m_csDrives);
		pXml->AddChildElem( _T( "INPUTS"),m_csInputs);
		pXml->AddChildElem( _T( "MODEMS"),m_csModems);
		pXml->AddChildElem( _T( "NETWORKS"),m_csNetworks);
		pXml->AddChildElem( _T( "PRINTERS"),m_csPrinters);
		pXml->AddChildElem( _T( "SOUNDS"),m_csSounds);
		pXml->AddChildElem( _T( "VIDEOS"),m_csVideos);
		pXml->AddChildElem( _T( "SOFTWARES"),m_csSoftwares);
		pXml->AddChildElem( _T( "VIRTUALMACHINES"),m_csVirtualMachines);
		pXml->AddChildElem( _T( "CPUS"),m_csCPUs);
	pXml->OutOfElem();
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

void COCSInventoryState::SetVirtualMachines( LPCTSTR lpstrValue)
{
	m_csVirtualMachines = lpstrValue;
}

void COCSInventoryState::SetCPUs( LPCTSTR lpstrValue)
{
	m_csCPUs = lpstrValue;
}

BOOL COCSInventoryState::ReadFromFile(LPCTSTR lpstrFilename, LPCTSTR lpstrSection)
{
	CMarkup	myXml;

	// Fill Device hardware properties from string
	if (!myXml.LoadFile( lpstrFilename))
		return FALSE;
	if (!ParseFromXML( &myXml, lpstrSection))
		return FALSE;
	return TRUE;
}

BOOL COCSInventoryState::WriteToFile(LPCTSTR lpstrFilename, LPCTSTR lpstrSection)
{
	CMarkup	myXml;
	TiXmlElement *pNode, *pTemp;

	// Load current state file
	if (myXml.LoadFile( lpstrFilename))
	{
		// File already exists, so remove existing section from XML 
		myXml.ResetPos();
		pNode = myXml.FindFirstElem( lpstrSection);
		while (pNode)
		{
			pTemp = pNode;
			pNode = myXml.FindNextElem( lpstrSection, pTemp);
			myXml.DeleteElem( pTemp);
		}
	}
	// Add new section
	myXml.ResetPos();
	if (!FormatXML( &myXml, lpstrSection))
		return FALSE;
	// Save state file
	if (!myXml.SaveFile( lpstrFilename))
		return FALSE;
	return TRUE;
}
