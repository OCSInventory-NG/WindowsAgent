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
	CMarkup x;

	x.SetDoc( lpstrXml);

	x.FindElem( _T( "LAST_STATE"));
	x.FindChildElem( _T( "HARDWARE"));		
	m_csHardware = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "BIOS"));
	m_csBios = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "MEMORIES"));
	m_csMemories = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "SLOTS"));
	m_csSlots = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "REGISTRY"));
	m_csRegistry = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "CONTROLLERS"));
	m_csControllers = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "MONITORS"));
	m_csMonitors = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "PORTS"));
	m_csPorts = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "STORAGES"));
	m_csStorages = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "DRIVES"));
	m_csDrives = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "INPUTS"));
	m_csInputs = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "MODEMS"));
	m_csModems = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "NETWORKS"));
	m_csNetworks = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "PRINTERS"));
	m_csPrinters = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "SOUNDS"));
	m_csSounds = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "VIDEOS"));
	m_csVideos = x.GetChildData();
	x.ResetChildPos();

	x.FindChildElem( _T( "SOFTWARES"));
	m_csSoftwares = x.GetChildData();
	x.ResetChildPos();

	return TRUE;
}

BOOL COCSInventoryState::FormatXML(CMarkup* pX)
{
	pX->AddElem( _T( "LAST_STATE"));
	pX->IntoElem();
		pX->AddElemNV( _T( "HARDWARE"),m_csHardware);
		pX->AddElemNV( _T( "BIOS"), m_csBios);
		pX->AddElemNV( _T( "MEMORIES"),m_csMemories);
		pX->AddElemNV( _T( "SLOTS"),m_csSlots);
		pX->AddElemNV( _T( "REGISTRY"),m_csRegistry);
		pX->AddElemNV( _T( "CONTROLLERS"),m_csControllers);
		pX->AddElemNV( _T( "MONITORS"),m_csMonitors);
		pX->AddElemNV( _T( "PORTS"),m_csPorts);
		pX->AddElemNV( _T( "STORAGES"),m_csStorages);
		pX->AddElemNV( _T( "DRIVES"),m_csDrives);
		pX->AddElemNV( _T( "INPUTS"),m_csInputs);
		pX->AddElemNV( _T( "MODEMS"),m_csModems);
		pX->AddElemNV( _T( "NETWORKS"),m_csNetworks);
		pX->AddElemNV( _T( "PRINTERS"),m_csPrinters);
		pX->AddElemNV( _T( "SOUNDS"),m_csSounds);
		pX->AddElemNV( _T( "VIDEOS"),m_csVideos);
		pX->AddElemNV( _T( "SOFTWARES"),m_csSoftwares);
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

#ifdef _MBCS
	myXml.SetDoc( _T( "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<!DOCTYPE REQUEST>\r\n"));
#else
	// UTF-8 / Unicode
	myXml.SetDoc( _T( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<!DOCTYPE REQUEST>\r\n"));
#endif
	FormatXML( &myXml);
	if (!WriteTextToFile( myXml.GetDoc(), myXml.GetDoc().GetLength(), lpstrFilename))
		return FALSE;
	return TRUE;
}
