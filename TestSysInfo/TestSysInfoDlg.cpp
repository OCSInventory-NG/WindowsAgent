//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// TestSysInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "atlconv.h"
#include "TestSysInfo.h"
#include "TestSysInfoDlg.h"
#include "tinyxml.h"
#include "tinystr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CTestSysInfoDlg dialog




CTestSysInfoDlg::CTestSysInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestSysInfoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestSysInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSysInfoDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestSysInfoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SMBIOS, &CTestSysInfoDlg::OnBnClickedSmbios)
	ON_BN_CLICKED(IDC_WMI, &CTestSysInfoDlg::OnBnClickedWmi)
	ON_BN_CLICKED(IDC_SYSINFO, &CTestSysInfoDlg::OnBnClickedSysinfo)
	ON_BN_CLICKED(IDC_LOAD_XML, &CTestSysInfoDlg::OnBnClickedLoadXml)
	ON_BN_CLICKED(IDC_SAVE_XML, &CTestSysInfoDlg::OnBnClickedSaveXml)
END_MESSAGE_MAP()


// CTestSysInfoDlg message handlers

BOOL CTestSysInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestSysInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestSysInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestSysInfoDlg::OnDestroy() 
{
	m_List.ResetContent();
	CDialog::OnDestroy();
}

void CTestSysInfoDlg::SysInfoLog (CString str)
{
/*
	static FILE *Log=NULL;

	if (!Log) 
		Log = fopen("TestSysInfo.txt", _T( "wt");
	if (Log) {
		fputs (str, Log);
		fputs ("\n", Log);
	}
*/
	m_List.AddString( str);
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/*
 *
 *
 * SMBIOS
 *
 *
 *
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
void CTestSysInfoDlg::OnBnClickedSmbios()
{
	// TODO: Add your control notification handler code here
	CWaitCursor cWait;
	CDMI myDMI;
	CString csVendor,
			csVersion,
			csDate,
			csModel,
			csSN,
			csType,
			csAssetTag,
			str;
	POSITION pos;
	BOOL bContinue;

	m_List.ResetContent();
	if (myDMI.Connect())
		m_List.AddString( _T( "Connected to DMI"));
	else
	{
		m_List.AddString( _T( "Error connecting to DMI"));
		return ;
	}
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "BIOS infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	myDMI.GetBios( csVendor, csVersion, csDate);
	str.Format( _T( "Vendor: %s"), csVendor);
	m_List.AddString( str);
	str.Format( _T( "Version: %s"), csVersion);
	m_List.AddString( str);
	str.Format( _T( "Date: %s"), csDate);
	m_List.AddString( str);

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "System infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	myDMI.GetSystemInformation( csVendor, csModel, csSN);
	str.Format( _T( "Vendor: %s"), csVendor);
	m_List.AddString( str);
	str.Format( _T( "Model: %s"), csModel);
	m_List.AddString( str);
	str.Format( _T( "S/N: %s"), csSN);
	m_List.AddString( str);

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Base Board infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	myDMI.GetBaseBoard( csVendor, csModel, csVersion, csSN, csAssetTag);
	str.Format( _T( "Vendor: %s"), csVendor);
	m_List.AddString( str);
	str.Format( _T( "Model: %s"), csModel);
	m_List.AddString( str);
	str.Format( _T( "Version: %s"), csVersion);
	m_List.AddString( str);
	str.Format( _T( "S/N: %s"), csSN);
	m_List.AddString( str);
	str.Format( _T( "Asset TAG: %s"), csAssetTag);
	m_List.AddString( str);

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "System Enclosure infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	myDMI.GetSystemEnclosure( csVendor, csType, csSN, csAssetTag);
	str.Format( _T( "Vendor: %s"), csVendor);
	m_List.AddString( str);
	str.Format( _T( "Type: %s"), csType);
	m_List.AddString( str);
	str.Format( _T( "S/N: %s"), csSN);
	m_List.AddString( str);
	str.Format( _T( "Asset TAG: %s"), csAssetTag);
	m_List.AddString( str);
	m_List.AddString( _T( ""));

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "System UUID"));
	m_List.AddString( _T( "------------------------------------------------------"));
	myDMI.GetUUID( csSN);
	str.Format( _T( "UUID: %s"), csSN);
	m_List.AddString( str);
	m_List.AddString( _T( ""));

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "System Slots infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	myDMI.GetSystemSlots( &m_SlotList);
	CSystemSlot mySlot;
	pos = m_SlotList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		mySlot = m_SlotList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Name: %s"), mySlot.GetName());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), mySlot.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Slot Designation: %s"), mySlot.GetSlotDesignation());
		SysInfoLog( str);
		str.Format( _T( "Usage: %s"), mySlot.GetUsage());
		SysInfoLog( str);
		str.Format( _T( "Status: %s"), mySlot.GetStatus());
		SysInfoLog( str);
		str.Format( _T( "Is shared: %s"), mySlot.IsShared() ? _T( "TRUE") : _T( "FALSE"));
		SysInfoLog( str);
		if (pos != NULL)
		{
			mySlot = m_SlotList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "System Slots Hash: %s"), m_SlotList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "System Port infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	myDMI.GetSystemPorts( &m_PortList);
	CSystemPort myPort;
	pos = m_PortList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myPort = m_PortList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myPort.GetType());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myPort.GetName());
		SysInfoLog( str);
		str.Format( _T( "Caption: %s"), myPort.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myPort.GetDescription());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myPort = m_PortList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "System Ports Hash: %s"), m_PortList.GetHash());
	SysInfoLog( str);

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Memory Slots infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	myDMI.GetMemorySlots( &m_MemoryList);
	CMemorySlot myMemSlot;
	pos = m_MemoryList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myMemSlot = m_MemoryList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Caption: %s"), myMemSlot.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myMemSlot.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Capacity: %s MB"), myMemSlot.GetCapacity());
		SysInfoLog( str);
		str.Format( _T( "Usage: %s"), myMemSlot.GetUsage());
		SysInfoLog( str);
		str.Format( _T( "Type: %s"), myMemSlot.GetType());
		SysInfoLog( str);
		str.Format( _T( "Speed: %s MHz"), myMemSlot.GetSpeed());
		SysInfoLog( str);
		str.Format( _T( "Slot Number: %u"), myMemSlot.GetSlotNumber());
		SysInfoLog( str);
		str.Format( _T( "Serial Number: %s"), myMemSlot.GetSN());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myMemSlot = m_MemoryList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Memory Slots Hash: %s"), m_MemoryList.GetHash());
	SysInfoLog( str);
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/*
 *
 *
 * WMI
 *
 *
 *
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
void CTestSysInfoDlg::OnBnClickedWmi()
{
	CWaitCursor cWait;
	COcsWmi		myWmiDll;

	m_List.ResetContent();
	if (myWmiDll.ConnectWMI( _T( "root\\CIMV2")))
		m_List.AddString( _T( "Connected to WMI"));
	else
	{
		m_List.AddString( _T( "Error connecting to WMI"));
		return ;
	}
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "System Enclosure infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_SystemEnclosure")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "System Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "System Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "System S/N = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			str = _T( "SKU = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SKU"));
			str += res;
			m_List.AddString( str);
			str = _T( "ChassisTypes = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ChassisTypes"));
			str += res;
			m_List.AddString( str);
			str = _T( "SMBIOSAssetTag = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SMBIOSAssetTag"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_SystemEnclosure objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Computer System infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_ComputerSystem")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_ComputerSystem objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Computer System Product infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_ComputerSystemProduct")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Vendor = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Vendor"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Identifying Number = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Identifying Number"));
			str += res;
			m_List.AddString( str);
			str = _T( "UUID = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "UUID"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_ComputerSystemProduct objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Base board infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_BaseBoard")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Base board Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Base board Product = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Product"));
			str += res;
			m_List.AddString( str);
			str = _T( "Base board S/N = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_BaseBoard objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "BIOS infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_BIOS")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "BIOS Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "SMBIOS BIOS Version = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SMBIOSBIOSVersion"));
			str += res;
			m_List.AddString( str);
			str = _T( "BIOS Version = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "BIOSVersion"));
			str += res;
			m_List.AddString( str);
			str = _T( "Version = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Version"));
			str += res;
			m_List.AddString( str);
			str = _T( "BIOS Release Ddate = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ReleaseDate"));
			str += res;
			m_List.AddString( str);
			str = _T( "BIOS Serial Number = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_BIOS objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "PhysicalMemory infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PhysicalMemory")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "BankLabel = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "BankLabel"));
			str += res;
			str = _T( "DeviceLocator = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DeviceLocator"));
			str += res;
			m_List.AddString( str);
			str = _T( "Capacity = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Capacity"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "MemoryType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MemoryType"));
			str += res;
			m_List.AddString( str);
			str = _T( "Speed = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Speed"));
			str += res;
			m_List.AddString( str);
			str = _T( "Status = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Status"));
			str += res;
			m_List.AddString( str);
			str = _T( "SerialNumber = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			str = _T( "PartNumber = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PartNumber"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PhysicalMemory objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "PhysicalMemoryArray infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PhysicalMemoryArray")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "MaxCapacity = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MaxCapacity"));
			str += res;
			m_List.AddString( str);
			str = _T( "MemoryErrorCorrection = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MemoryErrorCorrection"));
			str += res;
			m_List.AddString( str);
			str = _T( "MemoryDevices = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MemoryDevices"));
			str += res;
			m_List.AddString( str);
			str = _T( "Use = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Use"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Status = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Status"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PhysicalMemoryArray objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "System Slot infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_SystemSlot")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "PurposeDescription = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PurposeDescription"));
			str += res;
			m_List.AddString( str);
			str = _T( "SlotDesignation = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SlotDesignation"));
			str += res;
			m_List.AddString( str);
			str = _T( "Shared = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Shared"));
			str += res;
			m_List.AddString( str);
			str = _T( "ConnectorType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ConnectorType"));
			str += res;
			m_List.AddString( str);
			str = _T( "CurrentUsage = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CurrentUsage"));
			str += res;
			m_List.AddString( str);
			str = _T( "Status = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Status"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_SystemSlot objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Battery infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Battery")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Availability = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Availability"));
			str += res;
			m_List.AddString( str);
			str = _T( "BatteryStatus = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "BatteryStatus"));
			str += res;
			m_List.AddString( str);
			str = _T( "EstimatedChargeRemaining = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "EstimatedChargeRemaining"));
			str += res;
			m_List.AddString( str);
			str = _T( "EstimatedRunTime = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "EstimatedRunTime"));
			str += res;
			m_List.AddString( str);
			str = _T( "ExpectedLife = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ExpectedLife"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Battery objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "PortableBattery infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PortableBattery")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Availability = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Availability"));
			str += res;
			m_List.AddString( str);
			str = _T( "BatteryStatus = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "BatteryStatus"));
			str += res;
			m_List.AddString( str);
			str = _T( "EstimatedChargeRemaining = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "EstimatedChargeRemaining"));
			str += res;
			m_List.AddString( str);
			str = _T( "EstimatedRunTime = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "EstimatedRunTime"));
			str += res;
			m_List.AddString( str);
			str = _T( "ExpectedLife = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ExpectedLife"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PortableBattery objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Processor infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Processor")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Processor Family = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Architecture = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Architecture"));
			str += res;
			m_List.AddString( str);
			str = _T( "Number Of Cores = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "NumberOfCores"));
			str += res;
			m_List.AddString( str);
			str = _T( "Number Of Logical Processors = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "NumberOfLogicalProcessors"));
			str += res;
			m_List.AddString( str);
			str = _T( "Current Clock Speed = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CurrentClockSpeed"));
			str += res;
			m_List.AddString( str);
			str = _T( "Max Clock Speed = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MaxClockSpeed"));
			str += res;
			m_List.AddString( str);
			str = _T( "L2 Cache Size = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "L2CacheSize"));
			str += res;
			m_List.AddString( str);
			str = _T( "Address Width = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "AddressWidth"));
			str += res;
			m_List.AddString( str);
			str = _T( "Data Width = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DataWidth"));
			str += res;
			m_List.AddString( str);
			str = _T( "Voltage Caps = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "VoltageCaps"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Processor objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "MotherBoard Device infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_MotherboardDevice")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "PrimaryBusType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PrimaryBusType"));
			str += res;
			m_List.AddString( str);
			str = _T( "SecondaryBusType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SecondaryBusType"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_MotherboardDevice objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "OnBoard Device infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_OnBoardDevice")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_OnBoardDevice objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Video Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_VideoController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Chipset = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "VideoProcessor"));
			str += res;
			m_List.AddString( str);
			str = _T( "Memoire = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "AdapterRAM"));
			str += res;
			m_List.AddString( str);
			str = _T( "Horz res = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CurrentHorizontalResolution"));
			str += res;
			m_List.AddString( str);
			str = _T( "Vert res = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CurrentVerticalResolution"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_VideoController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Desktop Monitor infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_DesktopMonitor")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "MonitorManufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MonitorManufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "MonitorType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MonitorType"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str.Format( _T( "Status %lu"), myWmiDll.GetClassObjectDwordValue( _T( "Status")));
			m_List.AddString( str);
			str.Format( _T( "StatusInfo %lu"), myWmiDll.GetClassObjectDwordValue( _T( "StatusInfo")));
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_DesktopMonitor objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Serial Port infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_SerialPort")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			str = _T( "OSAutoDiscovered = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "OSAutoDiscovered"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_SerialPort objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Parallel Port infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_ParallelPort")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			str = _T( "OSAutoDiscovered = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "OSAutoDiscovered"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_ParallelPort objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( " Port Connector infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PortConnector")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "ExternalReferenceDesignator = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ExternalReferenceDesignator"));
			str += res;
			m_List.AddString( str);
			str = _T( "PortType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PortType"));
			str += res;
			m_List.AddString( str);
			str = _T( "ConnectorType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ConnectorType"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PortConnector objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( " Keyboard infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Keyboard")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "NumberOfFunctionKeys = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "NumberOfFunctionKeys"));
			str += res;
			m_List.AddString( str);
			str = _T( "Layout = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Layout"));
			str += res;
			m_List.AddString( str);
			str = _T( "Status = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Status"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Keyboard objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( " Pointing Device infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PointingDevice")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "HardwareType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "HardwareType"));
			str += res;
			m_List.AddString( str);
			str = _T( "PointingType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PointingType"));
			str += res;
			m_List.AddString( str);
			str = _T( "DeviceInterface = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DeviceInterface"));
			str += res;
			m_List.AddString( str);
			str = _T( "NumberOfButtons = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "NumberOfButtons"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PointingDevice objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Floppy Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_FloppyController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_FloppyController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "IDE Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_IDEController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_IDEController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "SCSI Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_SCSIController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "HardwareVersion = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "HardwareVersion"));
			str += res;
			m_List.AddString( str);
			str = _T( "DriverName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DriverName"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_SCSIController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "InfraRed Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_InfraredDevice")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_InfraredDevice objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "USB Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_USBController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_USBController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "IEEE 1394 Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_1394Controller")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_1394Controller objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "PCMCIA Controler infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_PCMCIAController")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_PCMCIAController objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Floppy Drives infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_FloppyDrive")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;
			unsigned __int64  u64;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "MediaType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MediaType"));
			str += res;
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "MaxMediaSize"));
			str.Format( _T( "MaxMediaSize in MB = %lu "), u64/1000);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "Size"));
			str.Format( _T( "Size in Bytes = %lu "), u64);
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_FloppyDrive objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Disk Drives infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_DiskDrive")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;
			unsigned __int64  u64;
			ULONG	ulCylinder,
					ulTracksPerCylinder,
					ulSectorPerTrack,
					ulBytesPerSector;
			

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "Firmware Revision  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "FirmwareRevision"));
			str += res;
			m_List.AddString( str);
			str = _T( "S/N  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			str = _T( "MediaType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MediaType"));
			str += res;
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "MaxMediaSize"));
			str.Format( _T( "MaxMediaSize in MB = %lu "), u64/1000);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "Size"));
			str.Format( _T( "Size in Bytes = %lu "), u64);
			m_List.AddString( str);
			ulCylinder = myWmiDll.GetClassObjectDwordValue( _T( "TotalCylinders"));
			str.Format( _T( "Cylinders = %lu "), ulCylinder);
			m_List.AddString( str);
			ulTracksPerCylinder = myWmiDll.GetClassObjectDwordValue( _T( "TracksPerCylinder"));
			str.Format( _T( "Tracks per Cylinder = %lu "), ulTracksPerCylinder);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "TotalTracks"));
			str.Format( _T( "Total Tracks = %lu "), u64);
			m_List.AddString( str);
			ulSectorPerTrack = myWmiDll.GetClassObjectDwordValue( _T( "SectorsPerTrack"));
			str.Format( _T( "Sectors per Track = %lu "), ulSectorPerTrack);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "TotalSectors"));
			str.Format( _T( "Total Sectors = %lu "), u64);
			m_List.AddString( str);
			ulBytesPerSector = myWmiDll.GetClassObjectDwordValue( _T( "BytesPerSector"));
			str.Format( _T( "Bytes per Sector = %lu "), ulBytesPerSector);
			m_List.AddString( str);
			str.Format( _T( "Total size = %lu "), ulCylinder*ulTracksPerCylinder*ulSectorPerTrack*ulBytesPerSector);
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_DiskDrive objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "CD-ROM Drives infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_CDROMDrive")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;
			unsigned __int64  u64;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Revision Level  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "RevisionLevel"));
			str += res;
			m_List.AddString( str);
			str = _T( "MediaType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MediaType"));
			str += res;
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "MaxMediaSize"));
			str.Format( _T( "MaxMediaSize in MB = %lu "), u64/1000);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "Size"));
			str.Format( _T( "Size in MB = %lu "), u64/1000000);
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_CDROMDrive objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Tape Drives infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_TapeDrive")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;
			unsigned __int64  u64;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Id  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Id"));
			str += res;
			m_List.AddString( str);
			str = _T( "MediaType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "MediaType"));
			str += res;
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "MaxMediaSize"));
			str.Format( _T( "MaxMediaSize in MB = %lu "), u64/1000);
			m_List.AddString( str);
			u64 = myWmiDll.GetClassObjectU64Value( _T( "Size"));
			str.Format( _T( "Size in MB = %lu "), u64/1000000);
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_TapeDrive objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Sound Device infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_SoundDevice")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "ProductName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ProductName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_SoundDevice objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Network Adapter infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_NetworkAdapterSetting")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "InterfaceIndex = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "InterfaceIndex"));
			str += res;
			m_List.AddString( str);
			str = _T( "AdapterType = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "AdapterType"));
			str += res;
			m_List.AddString( str);
			str = _T( "ProductName  = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "ProductName"));
			str += res;
			m_List.AddString( str);
			str = _T( "MaxSpeed = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Speed"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "MACAddress = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "MACAddress"));
			str += res;
			m_List.AddString( str);
			str = _T( "IPAddress = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "IPAddress"));
			str += res;
			m_List.AddString( str);
			str = _T( "IPSubnet = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "IPSubnet"));
			str += res;
			m_List.AddString( str);
			str = _T( "DefaultIPGateway = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DefaultIPGateway"));
			str += res;
			m_List.AddString( str);
			str = _T( "DNSDomain = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DNSDomain"));
			str += res;
			m_List.AddString( str);
			str = _T( "DNSSearchOrder = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DNSSearchOrder"));
			str += res;
			m_List.AddString( str);
			str = _T( "DHCPEnabled = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DHCPEnabled"));
			str += res;
			m_List.AddString( str);
			str = _T( "DHCPServer = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Setting"), _T( "DHCPServer"));
			str += res;
			m_List.AddString( str);
			str = _T( "Status = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "Status"));
			str += res;
			m_List.AddString( str);
			str = _T( "NetConnectionStatus = ");
			res = myWmiDll.GetRefElementClassObjectStringValue( _T( "Element"), _T( "NetConnectionStatus"));
			str += res;
			m_List.AddString( str);
			str = _T( "IPEnabled = ");
			res.Format( _T( "%lu"), myWmiDll.GetRefElementClassObjectDwordValue( _T( "Setting"), _T( "IPEnabled")));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_NetworkAdapterSetting objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Telephony Modem infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_POTSModem")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "DeviceType  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DeviceType"));
			str += res;
			m_List.AddString( str);
			str = _T( "Model = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Model"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "StatusInfo  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StatusInfo"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_POTSModem objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Logical Drives infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_LogicalDisk")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "DriveType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DriveType"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Filesystem  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Filesystem"));
			str += res;
			m_List.AddString( str);
			str = _T( "VolumeName  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "VolumeName"));
			str += res;
			m_List.AddString( str);
			str = _T( "ProviderName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ProviderName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Size = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Size"));
			str += res;
			m_List.AddString( str);
			str = _T( "FreeSpace = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "FreeSpace"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_LogicalDisk objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Printer infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Printer")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Comment = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Comment"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "DriverName  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DriverName"));
			str += res;
			m_List.AddString( str);
			str = _T( "PortName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PortName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Shared = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Shared"));
			str += res;
			m_List.AddString( str);
			str = _T( "Network = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Network"));
			str += res;
			m_List.AddString( str);
			str = _T( "ServerName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ServerName"));
			str += res;
			m_List.AddString( str);
			str = _T( "ShareName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ShareName"));
			str += res;
			m_List.AddString( str);
			str = _T( "ExtendedDetectedErrorState = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ExtendedDetectedErrorState"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Printer objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "OS infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_OperatingSystem")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Version = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Version"));
			str += res;
			m_List.AddString( str);
			str = _T( "CSDVersion = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CSDVersion"));
			str += res;
			m_List.AddString( str);
			str = _T( "Manufacturer = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Manufacturer"));
			str += res;
			m_List.AddString( str);
			str = _T( "SerialNumber = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SerialNumber"));
			str += res;
			m_List.AddString( str);
			str = _T( "Scoping System = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CSName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "OSProductSuite = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "OSProductSuite"));
			str += res;
			m_List.AddString( str);
			str = _T( "OtherTypeDescription = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "OtherTypeDescription"));
			str += res;
			m_List.AddString( str);
			str = _T( "ProductType = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "ProductType"));
			str += res;
			m_List.AddString( str);
			str = _T( "SuiteMask = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "SuiteMask"));
			str += res;
			m_List.AddString( str);
			str = _T( "Organization = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Organization"));
			str += res;
			m_List.AddString( str);
			str = _T( "RegisteredUser = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "RegisteredUser"));
			str += res;
			m_List.AddString( str);
			str = _T( "InstallDate = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "InstallDate"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_OperatingSystem objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Domain infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_NTDomain")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "DcSiteName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DcSiteName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "DNSForestName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DNSForestName"));
			str += res;
			m_List.AddString( str);
			str = _T( "DomainControllerName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DomainControllerName"));
			str += res;
			m_List.AddString( str);
			str = _T( "DomainName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DomainName"));
			str += res;
			m_List.AddString( str);
			str = _T( "DomainGUID = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DomainGUID"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_NTDomain objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Services infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Service")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "DisplayName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "DisplayName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "PathName = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "PathName"));
			str += res;
			m_List.AddString( str);
			str = _T( "StartMode = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StartMode"));
			str += res;
			m_List.AddString( str);
			str = _T( "Start account = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "StartName"));
			str += res;
			m_List.AddString( str);
			str = _T( "State = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "State"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Service objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Scheduled Job infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_ScheduledJob")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Caption  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "Name = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Command = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Command"));
			str += res;
			m_List.AddString( str);
			str = _T( "JobStatus = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "JobStatus"));
			str += res;
			m_List.AddString( str);
			str = _T( "Owner account = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Owner"));
			str += res;
			m_List.AddString( str);
			str = _T( "Priority = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Priority"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_ScheduledJob objects"));
		m_List.AddString( _T( ""));
	}

	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Hot fix infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_QuickFixEngineering")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "CSName  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "CSName"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "FixComments = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "FixComments"));
			str += res;
			m_List.AddString( str);
			str = _T( "HotFixID = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "HotFixID"));
			str += res;
			m_List.AddString( str);
			str = _T( "InstallDate = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "InstallDate"));
			str += res;
			m_List.AddString( str);
			str = _T( "InstalledOn = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "InstalledOn"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_QuickFixEngineering objects"));
		m_List.AddString( _T( ""));
	}


	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Software infos"));
	m_List.AddString( _T( "------------------------------------------------------"));
	if (myWmiDll.BeginEnumClassObject( _T( "Win32_Product")))
	{
		while (myWmiDll.MoveNextEnumClassObject())
		{
			CString str, res;

			str = _T( "Name  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Name"));
			str += res;
			m_List.AddString( str);
			str = _T( "Caption  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Caption"));
			str += res;
			m_List.AddString( str);
			str = _T( "Description  = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Description"));
			str += res;
			m_List.AddString( str);
			str = _T( "InstallDate = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "InstallDate"));
			str += res;
			m_List.AddString( str);
			str = _T( "InstallLocation = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "InstallLocation"));
			str += res;
			m_List.AddString( str);
			str = _T( "Language = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Language"));
			str += res;
			m_List.AddString( str);
			str = _T( "Vendor = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Vendor"));
			str += res;
			m_List.AddString( str);
			str = _T( "Version = ");
			res = myWmiDll.GetClassObjectStringValue( _T( "Version"));
			str += res;
			m_List.AddString( str);
			m_List.AddString( _T( ""));
		}
		myWmiDll.CloseEnumClassObject();
	}
	else
	{
		m_List.AddString( _T( "Error enumerating Win32_Product objects"));
		m_List.AddString( _T( ""));
	}


	myWmiDll.DisconnectWMI();
	m_List.AddString( _T( ""));
	m_List.AddString( _T( "Disconnected from WMI"));
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/*
 *
 *
 * SYSINFO
 *
 *
 *
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
void CTestSysInfoDlg::OnBnClickedSysinfo()
{
	CWaitCursor		cWait;
	CString			str;
	POSITION		pos;
	BOOL			bContinue;

	DeleteFile( _T( "c:\\SysInfo.log"));
	runSysInfo();
	m_List.ResetContent();
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "BIOS infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	str.Format( _T( "System Manufacturer: %s"), m_BIOS.GetSystemManufacturer());
	SysInfoLog( str);
	str.Format( _T( "System Model: %s"), m_BIOS.GetSystemModel());
	SysInfoLog( str);
	str.Format( _T( "System S/N: %s"), m_BIOS.GetSystemSerialNumber());
	SysInfoLog( str);
	str.Format( _T( "Machine Type: %s"), m_BIOS.GetMachineType());
	SysInfoLog( str);
	str.Format( _T( "BIOS Manufacturer: %s"), m_BIOS.GetBiosManufacturer());
	SysInfoLog( str);
	str.Format( _T( "BIOS Version: %s"), m_BIOS.GetBiosVersion());
	SysInfoLog( str);
	str.Format( _T( "BIOS Date: %s"), m_BIOS.GetBiosDate());
	SysInfoLog( str);
	str.Format( _T( "Asset TAG: %s"), m_BIOS.GetAssetTag());
	SysInfoLog( str);
	SysInfoLog( _T( ""));
	str.Format( _T( "Bios Hash: %s"), m_BIOS.GetHash());
	SysInfoLog( str);

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Processors infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	str.Format( _T( "Number of Processors: %u"), m_Device.GetNumberOfProcessors());
	SysInfoLog( str);
	str.Format( _T( "Processor name: %s"), m_Device.GetProcessorType());
	SysInfoLog( str);
	str.Format( _T( "Processor speed: %s"), m_Device.GetProcessorSpeed());
	SysInfoLog( str);

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "CPU List infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CCpu myCpu;
	pos = m_CpuList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myCpu = m_CpuList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Manufacturer: %s"), myCpu.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myCpu.GetName());
		SysInfoLog( str);
		str.Format( _T( "Architecture: %s"), myCpu.GetArchitecture());
		SysInfoLog( str);
		str.Format( _T( "Number Of Cores: %u"), myCpu.GetNumberOfCores());
		SysInfoLog( str);
		str.Format( _T( "Number Of Logical Processors: %u"), myCpu.GetNumberOfLogicalProcessors());
		SysInfoLog( str);
		str.Format( _T( "Current Clock Speed: %u MHz"), myCpu.GetCurrentClockSpeed());
		SysInfoLog( str);
		str.Format( _T( "Max Clock Speed: %u MHz"), myCpu.GetMaxClockSpeed());
		SysInfoLog( str);
		str.Format( _T( "Level 2 Cache Size: %u KB"), myCpu.GetL2CacheSize());
		SysInfoLog( str);
		str.Format( _T( "Address Width: %u"), myCpu.GetAddressWidth());
		SysInfoLog( str);
		str.Format( _T( "Data Width: %u"), myCpu.GetDataWidth());
		SysInfoLog( str);
		str.Format( _T( "Voltage: %s"), myCpu.GetVoltage());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myCpu = m_CpuList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "CPU List Hash: %s"), m_CpuList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Memory Slots infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CMemorySlot myMemSlot;
	pos = m_MemoryList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myMemSlot = m_MemoryList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Caption: %s"), myMemSlot.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myMemSlot.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Capacity: %s MB"), myMemSlot.GetCapacity());
		SysInfoLog( str);
		str.Format( _T( "Usage: %s"), myMemSlot.GetUsage());
		SysInfoLog( str);
		str.Format( _T( "Type: %s"), myMemSlot.GetType());
		SysInfoLog( str);
		str.Format( _T( "Speed: %s MHz"), myMemSlot.GetSpeed());
		SysInfoLog( str);
		str.Format( _T( "Slot Number: %u"), myMemSlot.GetSlotNumber());
		SysInfoLog( str);
		str.Format( _T( "Serial Number: %s"), myMemSlot.GetSN());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myMemSlot = m_MemoryList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Memory Slots Hash: %s"), m_MemoryList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Video Controlers infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CVideoAdapter myVideo;
	pos = m_VideoList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myVideo = m_VideoList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Name: %s"), myVideo.GetName());
		SysInfoLog( str);
		str.Format( _T( "Chipset: %s"), myVideo.GetChipset());
		SysInfoLog( str);
		str.Format( _T( "Memory: %s MB"), myVideo.GetMemory());
		SysInfoLog( str);
		str.Format( _T( "Resolution: %s"), myVideo.GetScreenResolution());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myVideo = m_VideoList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Video Controlers Hash: %s"), m_VideoList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Desktop Monitors infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CMonitor myMonitor;
	pos = m_MonitorList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myMonitor = m_MonitorList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Manufacturer: %s"), myMonitor.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Caption: %s"), myMonitor.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myMonitor.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Type: %s"), myMonitor.GetType());
		SysInfoLog( str);
		str.Format( _T( "S/N: %s"), myMonitor.GetSerial());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myMonitor = m_MonitorList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Monitors Hash: %s"), m_MonitorList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Sound Devices infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CSoundDevice mySound;
	pos = m_SoundList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		mySound = m_SoundList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Manufacturer: %s"), mySound.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), mySound.GetName());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), mySound.GetDescription());
		SysInfoLog( str);
		if (pos != NULL)
		{
			mySound = m_SoundList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Sound Devices Hash: %s"), m_SoundList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "System Slots infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CSystemSlot mySlot;
	pos = m_SlotList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		mySlot = m_SlotList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Name: %s"), mySlot.GetName());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), mySlot.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Slot Designation: %s"), mySlot.GetSlotDesignation());
		SysInfoLog( str);
		str.Format( _T( "Usage: %s"), mySlot.GetUsage());
		SysInfoLog( str);
		str.Format( _T( "Status: %s"), mySlot.GetStatus());
		SysInfoLog( str);
		str.Format( _T( "Is shared: %s"), mySlot.IsShared() ? _T( "TRUE") : _T( "FALSE"));
		SysInfoLog( str);
		if (pos != NULL)
		{
			mySlot = m_SlotList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "System Slots Hash: %s"), m_SlotList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "System Port infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CSystemPort myPort;
	pos = m_PortList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myPort = m_PortList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myPort.GetType());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myPort.GetName());
		SysInfoLog( str);
		str.Format( _T( "Caption: %s"), myPort.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myPort.GetDescription());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myPort = m_PortList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "System Ports Hash: %s"), m_PortList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Input Devices infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CInputDevice myInput;
	pos = m_InputList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myInput = m_InputList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myInput.GetType());
		SysInfoLog( str);
		str.Format( _T( "Manufacturer: %s"), myInput.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Caption: %s"), myInput.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myInput.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "PointingType: %s"), myInput.GetPointingType());
		SysInfoLog( str);
		str.Format( _T( "Pointing Interface: %s"), myInput.GetPointingInterface());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myInput = m_InputList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Input Devices Hash: %s"), m_InputList.GetHash());
	SysInfoLog( str);

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "System Controlers infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CSystemController myController;
	pos = m_SystemControllerList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myController = m_SystemControllerList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myController.GetType());
		SysInfoLog( str);
		str.Format( _T( "Manufacturer: %s"), myController.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myController.GetName());
		SysInfoLog( str);
		str.Format( _T( "Caption: %s"), myController.GetCaption());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myController.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "HW version: %s"), myController.GetHardwareVersion());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myController = m_SystemControllerList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "System Controlers Hash: %s"), m_SystemControllerList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Storage Peripherals infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CStoragePeripheral myStorage;
	pos = m_StorageList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myStorage = m_StorageList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myStorage.GetType());
		SysInfoLog( str);
		str.Format( _T( "Manufacturer: %s"), myStorage.GetManufacturer());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myStorage.GetName());
		SysInfoLog( str);
		str.Format( _T( "Model: %s"), myStorage.GetModel());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myStorage.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Size: %I64u MB"), myStorage.GetSize());
		SysInfoLog( str);
		str.Format( _T( "S/N: %s"), myStorage.GetSN());
		SysInfoLog( str);
		str.Format( _T( "Firmware Revsion: %s"), myStorage.GetFirmware());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myStorage = m_StorageList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Storage Peripherals Hash: %s"), m_StorageList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Network Adapters infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CNetworkAdapter myNetwork;
	pos = m_NetworkList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myNetwork = m_NetworkList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myNetwork.GetType());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myNetwork.GetDescription());
		SysInfoLog( str);
		str.Format( _T( "Speed: %s"), myNetwork.GetSpeed());
		SysInfoLog( str);
		str.Format( _T( "MAC: %s"), myNetwork.GetMACAddress());
		SysInfoLog( str);
		str.Format( _T( "IP @: %s"), myNetwork.GetIPAddress());
		SysInfoLog( str);
		str.Format( _T( "IP mask: %s"), myNetwork.GetIPNetMask());
		SysInfoLog( str);
		str.Format( _T( "IP Gateway: %s"), myNetwork.GetGateway());
		SysInfoLog( str);
		str.Format( _T( "DHCP Server: %s"), myNetwork.GetDhcpServer());
		SysInfoLog( str);
		str.Format( _T( "Status: %s"), myNetwork.GetOperationalStatus());
		SysInfoLog( str);
		str.Format( _T( "MIB Type: %s"), myNetwork.GetTypeMIB());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myNetwork = m_NetworkList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Network Adapters Hash: %s"), m_NetworkList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Telephony Modems infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CModem myModem;
	pos = m_ModemList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myModem = m_ModemList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myModem.GetType());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myModem.GetName());
		SysInfoLog( str);
		str.Format( _T( "Model: %s"), myModem.GetModel());
		SysInfoLog( str);
		str.Format( _T( "Description: %s"), myModem.GetDescription());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myModem = m_ModemList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Telephony Modems Hash: %s"), m_ModemList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Logical Drive infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CLogicalDrive myDrive;
	pos = m_DriveList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myDrive = m_DriveList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Type: %s"), myDrive.GetDriveType());
		SysInfoLog( str);
		str.Format( _T( "Letter: %s"), myDrive.GetDriveLetter());
		SysInfoLog( str);
		str.Format( _T( "Filesystem: %s"), myDrive.GetFileSystem());
		SysInfoLog( str);
		str.Format( _T( "Size in MB: %ld"), myDrive.GetTotalMB());
		SysInfoLog( str);
		str.Format( _T( "Free space in MB: %ld"), myDrive.GetFreeMB());
		SysInfoLog( str);
		str.Format( _T( "Volume Name: %s"), myDrive.GetVolumName());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myDrive = m_DriveList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Logical Drive Hash: %s"), m_DriveList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Printers infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CPrinter myPrinter;
	pos = m_PrinterList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myPrinter = m_PrinterList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Name: %s"), myPrinter.GetName());
		SysInfoLog( str);
		str.Format( _T( "Driver: %s"), myPrinter.GetDriver());
		SysInfoLog( str);
		str.Format( _T( "Port: %s"), myPrinter.GetPort());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myPrinter = m_PrinterList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Printers Hash: %s"), m_PrinterList.GetHash());
	SysInfoLog( str);


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Physical or VM infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	str.Format( _T( "UUID: %s"), m_Device.GetUUID());
	SysInfoLog( str);
	str.Format( _T( "VM System: %s"), m_Device.GetVMSystem());
	SysInfoLog( str);
	SysInfoLog( _T( ""));


	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "OS infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	str.Format( _T( "Name: %s"), m_Device.GetOSName());
	SysInfoLog( str);
	str.Format( _T( "Version: %s"), m_Device.GetOSVersion());
	SysInfoLog( str);
	str.Format( _T( "Comment: %s"), m_Device.GetOSComment());
	SysInfoLog( str);
	str.Format( _T( "Description: %s"), m_Device.GetDescription());
	SysInfoLog( str);
	str.Format( _T( "Domain or Workgroup: %s"), m_Device.GetDomainOrWorkgroup());
	SysInfoLog( str);
	str.Format( _T( "OS Registered Company: %s"), m_Device.GetWindowsRegisteredCompany());
	SysInfoLog( str);
	str.Format( _T( "OS Registered Owner: %s"), m_Device.GetWindowsRegisteredOwner());
	SysInfoLog( str);
	str.Format( _T( "OS Product ID : %s"), m_Device.GetWindowsProductID());
	SysInfoLog( str);
	str.Format( _T( "OS Product Key : %s"), m_Device.GetWindowsProductKey());
	SysInfoLog( str);
	str.Format( _T( "64 bits OS: %s"), (m_Device.Is64bitsOS() ? _T( "TRUE") : _T( "FALSE")));
	SysInfoLog( str);
	SysInfoLog( _T( ""));
	str.Format( _T( "Device Hash: %s"), m_Device.GetHash());
	SysInfoLog( str);

	SysInfoLog( _T( ""));
	SysInfoLog( _T( "------------------------------------------------------"));
	SysInfoLog( _T( "Registered Applications infos"));
	SysInfoLog( _T( "------------------------------------------------------"));
	CSoftware myApp;
	pos = m_SoftwareList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myApp = m_SoftwareList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Publisher: %s"), myApp.GetPublisher());
		SysInfoLog( str);
		str.Format( _T( "Name: %s"), myApp.GetName());
		SysInfoLog( str);
		str.Format( _T( "Version: %s"), myApp.GetVersion());
		SysInfoLog( str);
		str.Format( _T( "GUID: %s"), myApp.GetGUID());
		SysInfoLog( str);
		str.Format( _T( "Memory Address Width: %lu"), myApp.GetMemoryAddressWidth());
		SysInfoLog( str);
		str.Format( _T( "Folder: %s"), myApp.GetFolder());
		SysInfoLog( str);
		str.Format( _T( "Comments: %s"), myApp.GetComments());
		SysInfoLog( str);
		str.Format( _T( "Language: %s"), myApp.GetLanguage());
		SysInfoLog( str);
		str.Format( _T( "InstallDate: %s"), myApp.GetInstallDate());
		SysInfoLog( str);
		if (myApp.IsFromRegistry())
			SysInfoLog( _T( "Extracted from registry"));
		else
			SysInfoLog( _T( "Find on disk"));
		if (pos != NULL)
		{
			myApp = m_SoftwareList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
	SysInfoLog( _T( ""));
	str.Format( _T( "Registered Applications Hash: %s"), m_SoftwareList.GetHash());
	SysInfoLog( str);

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Checking all registry values for key HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"));
	m_List.AddString( _T( "------------------------------------------------------"));
	CRegistryValue myRegVal;
	CRegistryValueList myRegList;
	CRegistry myReg;
	myReg.GetRegistryMultipleValues( _T( "Windows Setup"), HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), &myRegList);
	pos = myRegList.GetHeadPosition();
	while (pos)
	{
		myRegVal = myRegList.GetNext( pos);
		m_List.AddString( myRegVal.GetValue());
	}
	m_List.AddString( _T( ""));

	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Checking some registry value on key HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"));
	m_List.AddString( _T( "------------------------------------------------------"));
	CString csValue;
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), _T( "CDInstall"), csValue);
	str.Format( _T( "REG_DWORD CDInstall=%s"), csValue);
	m_List.AddString( str);
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\"), _T( "InstallDate"), csValue);
	str.Format( _T( "REG_DWORD Windows NT InstallDate=%s"), csValue);
	m_List.AddString( str);
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), _T( "BootDir"), csValue);
	str.Format( _T( "REG_SZ BootDir=%s"), csValue);
	m_List.AddString( str);
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), _T( "DriverCachePath"), csValue);
	str.Format( _T( "REG_EXPAND_SZ DriverCachePath=%s"), csValue);
	m_List.AddString( str);
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), _T( "Installation Sources"), csValue);
	str.Format( _T( "REG_MULTI_SZ Installation Sources=%s"), csValue);
	m_List.AddString( str);
	myReg.GetRegistryValue( HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup"), _T( "PrivateHash"), csValue);
	str.Format( _T( "REG_BINARY PrivateHash=%s"), csValue);
	m_List.AddString( str);
	m_List.AddString( _T( ""));
	m_List.AddString( _T( "------------------------------------------------------"));
	m_List.AddString( _T( "Checking multiple registry value on key SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\BITS"));
	m_List.AddString( _T( "------------------------------------------------------"));
	CRegistryValueList	myRegValueList;
	CRegistryValue		myRegValue;
	myReg.GetRegistryMultipleValues( _T( "BITS Multiple Search"), HKLM_TREE, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\BITS"), &myRegValueList);
	pos = myRegValueList.GetHeadPosition();
	bContinue = (pos != NULL);
	if (bContinue)
		// There is one record => get the first
		myRegValue = myRegValueList.GetNext( pos);
	while (bContinue)
	{
		bContinue = (pos != NULL);
		str.Format( _T( "Name: %s"), myRegValue.GetName());
		SysInfoLog( str);
		str.Format( _T( "Value: %s"), myRegValue.GetValue());
		SysInfoLog( str);
		if (pos != NULL)
		{
			myRegValue = myRegValueList.GetNext( pos);
			SysInfoLog( _T( ""));
		}
	}
}

BOOL CTestSysInfoDlg::runSysInfo()
{
	CString		cs1, cs2, cs3, cs4, cs5;
	DWORD		dwValue;
	ULONG		ulPhysicalMemory, ulSwapSize;
	CSoftware	cSoftOS;
	TCHAR		szTempPath[ _MAX_PATH+1];
	GetTempPath( _MAX_PATH, szTempPath);
	CSysInfo    mySysInfo( TRUE, szTempPath);

	// Get logged on user
	if (!mySysInfo.getUserName( cs1))
		AfxMessageBox( _T( "Failed to get username !"));
	m_Device.SetLoggedOnUser( cs1);
	// Last logged on user
	if (!mySysInfo.getLastLoggedUser( cs1))
		AfxMessageBox( _T( "Failed to get last logged on username !"));
	m_Device.SetLastLoggedUser( cs1);
	// Get OS informations and device type (windows station or windows server)
	if (!mySysInfo.getOS( cs1, cs2, cs3, cs4, cs5))
		AfxMessageBox( _T( "Failed to get OS informations !"));
	m_Device.SetOS( cs1, cs2, cs3);
	m_Device.SetDescription (cs4);
	m_Device.SetAddressWidthOS( mySysInfo.getAddressWidthOS());
	// Prepare to also store OS information to software list
	cSoftOS.Set( MICROSOFT_CORP_STRING, cs1, cs2, NOT_AVAILABLE, cs3, NOT_AVAILABLE, 0, TRUE);
	cSoftOS.SetInstallDate( cs5);
	cSoftOS.SetMemoryAddressWidth( mySysInfo.getAddressWidthOS());
	// Get NT Domain or Workgroup
	if (!mySysInfo.getDomainOrWorkgroup( cs1))
		AfxMessageBox( _T( "Failed to get Domain or Workgroup informations !"));
	m_Device.SetDomainOrWorkgroup( cs1);
	// Get NT user Domain
	if (!mySysInfo.getUserDomain( cs1))
		AfxMessageBox( _T( "Failed to get User Domain informations !"));
	m_Device.SetUserDomain( cs1);
	// Get BIOS informations
	if (!mySysInfo.getBiosInfo( &m_BIOS))
		AfxMessageBox( _T( "Failed to get BIOS informations !"));
	// Get Processor infos (0 means error)
	if ((dwValue = mySysInfo.getProcessors( cs1, cs2, &m_CpuList)) == 0)
		AfxMessageBox( _T( "Failed to get Processors informations !"));
	m_Device.SetProcessor( cs1, cs2, dwValue);
	// Get memory informations
	if (!mySysInfo.getMemory( &ulPhysicalMemory, &ulSwapSize))
		AfxMessageBox( _T( "Failed to get Memory informations !"));
	m_Device.SetMemory( ulPhysicalMemory, ulSwapSize);
	if (!mySysInfo.getMemorySlots( &m_MemoryList))
		AfxMessageBox( _T( "Failed to get Memory Slots informations !"));
	// Get Input Devices
	if (!mySysInfo.getInputDevices( &m_InputList))
		AfxMessageBox( _T( "Failed to get Input Devices informations !"));
	// Get System ports
	if (!mySysInfo.getSystemPorts( &m_PortList))
		AfxMessageBox( _T( "Failed to get System Ports informations !"));
	// Get System Slots
	if (!mySysInfo.getSystemSlots( &m_SlotList))
		AfxMessageBox( _T( "Failed to get System slots informations !"));
	// Get System controlers
	if (!mySysInfo.getSystemControllers( &m_SystemControllerList))
		AfxMessageBox( _T( "Failed to get System Controllers informations !"));
	// Get Physical storage devices
	if (!mySysInfo.getStoragePeripherals( &m_StorageList))
		AfxMessageBox( _T( "Failed to get Physical Storages informations !"));
	// Get Logical Drives
	if (!mySysInfo.getLogicalDrives( &m_DriveList))
		AfxMessageBox( _T( "Failed to get Logical Drives informations !"));
	// Get Sound Devices
	if (!mySysInfo.getSoundDevices( &m_SoundList))
		AfxMessageBox( _T( "Failed to get Sound Devices informations !"));
	// Get Modems
	if (!mySysInfo.getModems( &m_ModemList))
		AfxMessageBox( _T( "Failed to get Modems informations !"));
	// Get network adapter(s) hardware and IP informations
	if (!mySysInfo.getNetworkAdapters( &m_NetworkList))
		AfxMessageBox( _T( "Failed to get Network Adapters informations !"));
	// Get Printer(s) informations
	if (!mySysInfo.getPrinters( &m_PrinterList))
		AfxMessageBox( _T( "Failed to get Printers informations !"));
	// Get Video adapter(s) informations
	if (!mySysInfo.getVideoAdapters( &m_VideoList))
		AfxMessageBox( _T( "Failed to get Video Adapters informations !"));
	if (!mySysInfo.getMonitors( &m_MonitorList))
		AfxMessageBox( _T( "Failed to get Monitors informations !"));
	// Get the primary local IP Address 
	m_Device.SetIPAddress( mySysInfo.getLocalIP());
	// Get Windows registration infos
	if (!mySysInfo.getWindowsRegistration( cs1, cs2, cs3))
		AfxMessageBox( _T( "Failed to get Windows Registration informations !"));
	m_Device.SetWindowsRegistration( cs1, cs2, cs3);
	// Get Windows product key
	if (!mySysInfo.getWindowsProductKey( cs1))
		AfxMessageBox( _T( "Failed to get Windows Product Key informations !"));
	m_Device.SetWindowsProductKey( cs1);
	// Get Software list
	if (!mySysInfo.getInstalledApplications( &m_SoftwareList, TRUE))
		AfxMessageBox( _T( "Failed to get Installed Softwares informations !"));
	m_SoftwareList.AddTail( cSoftOS);
	// Get UUID
	if (!mySysInfo.getUUID( cs1))
		AfxMessageBox( _T( "Failed to get computer or VM UUID !"));
	m_Device.SetUUID( cs1);
	// Check if we're running on VM
	CVMSystem myVM;
	if (!myVM.DetectVM( &m_BIOS, cs1) && !myVM.DetectVM( &m_SystemControllerList, cs1) && 
		!myVM.DetectVM( &m_StorageList, cs1) && !myVM.DetectVM( &m_VideoList, cs1))
		cs1 = VMSYSTEM_PHYSYCAL;
	m_Device.SetVMSystem( cs1);
	return TRUE;
}


void CTestSysInfoDlg::OnBnClickedLoadXml()
{
	// TODO: Add your control notification handler code here
	CStdioFile myFile;
	CString csFile;

	CFileDialog cDlg( TRUE, NULL, NULL,
		              OFN_EXPLORER|OFN_FILEMUSTEXIST,
					  _T( "XML files|*.xml|All files|*.*||"));

	if (cDlg.DoModal() == IDCANCEL)
		return;
	TiXmlDocument *pXmlDoc = new TiXmlDocument();
	TiXmlElement *pXmlRequest, *pXmlLine;

	if (!pXmlDoc->LoadFile( CT2A( cDlg.GetPathName())))
	{
		AfxMessageBox( _T( "Unable to load/parse selected file !"), MB_ICONSTOP);
		return;
	}
	m_List.ResetContent();
	pXmlRequest = pXmlDoc->FirstChildElement( "Request");
	if (pXmlRequest)
	{
		pXmlLine = pXmlRequest->FirstChildElement( "Line");
		while( pXmlLine)
		{
			const char *pszText = pXmlLine->GetText();
			// Thanks to Tommy Teague
			m_List.AddString( pszText ? CA2T( pszText, CP_UTF8) : _T(""));
			pXmlLine = pXmlLine->NextSiblingElement( "Line");
		}
	}
	else
	{
		AfxMessageBox( _T( "XML file is not an OCS Inventory TextSysinfo file !"));
		return;
	}
}

void CTestSysInfoDlg::OnBnClickedSaveXml()
{
	// TODO: Add your control notification handler code here
	CString csLine;
	CStdioFile myFile;

	CFileDialog cDlg( FALSE, _T( "xml"), _T( "TestSysinfo.xml"),
		              OFN_EXPLORER|OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT,
					  _T( "XML files|*.xml|All files|*.*||"));

	TiXmlDocument *pXmlDoc = new TiXmlDocument();
	TiXmlDeclaration *pXmlDecl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	pXmlDoc->LinkEndChild( pXmlDecl);
	TiXmlElement *pXmlRequest = new TiXmlElement( "Request");
	pXmlDoc->LinkEndChild( pXmlRequest);
	
	for ( int i=0; i< m_List.GetCount(); i++)
	{
		TiXmlElement *pXmlLine = new TiXmlElement( "Line");
		m_List.GetText( i, csLine);
		TiXmlText *pXmlText = new TiXmlText( CT2A( csLine, CP_UTF8));
		pXmlLine->LinkEndChild( pXmlText);
		pXmlRequest->LinkEndChild( pXmlLine);
	}
	if (cDlg.DoModal() != IDOK)
		return;
	if (!pXmlDoc->SaveFile( CT2A( cDlg.GetPathName())))
		AfxMessageBox( _T( "Failed to save XML file !"), MB_ICONSTOP);
}
