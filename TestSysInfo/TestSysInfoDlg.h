//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// TestSysInfoDlg.h : header file
//

#pragma once

#include "OcsWmi.h"
#include "SysInfo.h"

// CTestSysInfoDlg dialog
class CTestSysInfoDlg : public CDialog
{
// Construction
public:
	CTestSysInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTSYSINFO_DIALOG };
	CListBox	m_List;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	// General device properties
	CDeviceProperties m_Device;
	// BIOS informations
	CBios		 m_BIOS;
	// List of Input devices infos
	CInputDeviceList m_InputList;
	// List of Logical Drives (Volum name, File system, Total size, Free space...)
	CLogicalDriveList m_DriveList;
	// List of Memory slots
	CMemorySlotList m_MemoryList;
	// List of Modems
	CModemList m_ModemList;
	// List of Monitors
	CMonitorList m_MonitorList;
	// List of Network adapters
	CNetworkAdapterList m_NetworkList;
	// List of Printers
	CPrinterList m_PrinterList;
	// List of Device registry values infos
	CRegistryValueList m_RegistryList;
	// List of Software
	CSoftwareList m_SoftwareList;
	// List of Sound Devices
	CSoundDeviceList m_SoundList;
	// List of Storage devices (Floppy disk, IDE Disk, SCSI Disk, CD-Rom...)
	CStoragePeripheralList m_StorageList;
	// List of System Controlers (Floppy, IDE, SCSI, USB, IEEE1394, PCMCIA, SATA)
	CSystemControllerList m_SystemControllerList;
	// List of System Slots (PCI, AGP...)
	CSystemSlotList m_SlotList;
	// List of ports
	CSystemPortList m_PortList;
	// List of Video adapters
	CVideoAdapterList m_VideoList;

	void SysInfoLog (CString str);
	BOOL runSysInfo();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSmbios();
	afx_msg void OnBnClickedWmi();
	afx_msg void OnBnClickedSysinfo();
};
