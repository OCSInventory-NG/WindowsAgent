//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DiskInfo.h: interface for the DiskInfo class.
//
// This code is derived from "Get Physical HDD Serial Number without WMI"
// code sample by dmihailescu
// http://www.codeproject.com/KB/mcpp/DriveInfoEx.aspx
//////////////////////////////////////////////////////////////////////

#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <winioctl.h>

#pragma warning(disable:4996)
//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(1)

#define  IDENTIFY_BUFFER_SIZE  512


//  IOCTL commands
#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

//  GETVERSIONOUTPARAMS contains the data returned from the 
//  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

//  Bits returned in the fCapabilities member of GETVERSIONOUTPARAMS 
#define  CAP_IDE_ID_FUNCTION             1  // ATA ID command supported
#define  CAP_IDE_ATAPI_ID                2  // ATAPI ID command supported
#define  CAP_IDE_EXECUTE_SMART_FUNCTION  4  // SMART commannds supported

//  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

// The following struct defines the interesting part of the IDENTIFY
// buffer:
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

#include <vector>
using namespace std ;
typedef vector<WORD*> LISTDATA;
#define  MAX_IDE_DRIVES  16

class DiskInfo
{
	static DiskInfo* m_instance;
	static char HardDriveSerialNumber [1024];
	LISTDATA m_list;

public:
	static DiskInfo& GetDiskInfo()
	{
		if(m_instance == NULL)
			m_instance =  new DiskInfo();
		return *m_instance;
	};

	static void Destroy()
	{
		if(m_instance != NULL)
			delete m_instance;
		m_instance = NULL;
	};
	~DiskInfo(void);
	int ReadIdeDriveAsScsiDriveInNT (void);
	long LoadDiskInfo ();
	int ReadPhysicalDriveInNTWithAdminRights (void);
	int ReadPhysicalDriveInNTWithZeroRights (void);
	int ReadDrivePortsInWin9X (void);


	unsigned __int64 DriveSize (UINT drive);
	UINT GetCount (){return m_list.size();};
	UINT BufferSize (UINT drive);
	char* ModelNumber (UINT drive);
	char* SerialNumber (UINT drive);
	char* RevisionNumber (UINT drive);
	char* DriveType (UINT drive);

private:
	DiskInfo(void);
	static char * flipAndCodeBytes (char * str);
	static char * ConvertToString (WORD diskdata [256], int firstIndex, int lastIndex);

	static BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned);
	BOOL AddIfNew(USHORT *pIdSector);
};
