//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// DiskInfo.cpp: implementation of DiskInfo class.
//
// This code is derived from "Get Physical HDD Serial Number without WMI"
// code sample by dmihailescu
// http://www.codeproject.com/KB/mcpp/DriveInfoEx.aspx
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DiskInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char DiskInfo::HardDriveSerialNumber [1024];

DiskInfo* DiskInfo::m_instance = NULL;

BOOL DiskInfo::AddIfNew(USHORT *pIdSector)
{
	BOOL bAdd = TRUE;
	for(UINT i =0; i< m_list.size();i++)
	{
		if(memcmp(pIdSector,m_list[i],256 * sizeof(WORD)) == 0)
		{
			bAdd = false;
			break;
		}
	}
	if(bAdd)
	{
		WORD* diskdata = new WORD[256];
		::memcpy(diskdata,pIdSector,256*sizeof(WORD));
		m_list.push_back(diskdata);
	}
	return bAdd;
}

int DiskInfo::ReadPhysicalDriveInNTWithAdminRights (void)
{
	int done = FALSE;
	int drive = 0;
	BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR driveName [256];
		swprintf (driveName, L"\\\\.\\PhysicalDrive%d", drive);
		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile (driveName,
										GENERIC_READ | GENERIC_WRITE, 
										FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
										OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) &VersionParams, 0, sizeof(VersionParams));

			if (!DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
								NULL, 
								0,
								&VersionParams,
								sizeof(VersionParams),
								&cbBytesReturned, NULL) )
			{         
				// printf ("DFP_GET_VERSION failed for drive %d\n", i);
				// continue;
			}

			// If there is a IDE device at number "i" issue commands
			// to the device
			if (VersionParams.bIDEDeviceMap > 0)
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;

				// Now, get the ID sector for all IDE devices in the system.
				// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
				// otherwise use the IDE_ATA_IDENTIFY command
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset (&scip, 0, sizeof(scip));
				memset (IdOutCmd, 0, sizeof(IdOutCmd));

				if ( DoIDENTIFY( hPhysicalDriveIOCTL, 
								&scip, 
								(PSENDCMDOUTPARAMS)&IdOutCmd, 
								(BYTE) bIDCmd,
								(BYTE) drive,
								&cbBytesReturned))
				{
					USHORT *pIdSector = (USHORT *) ((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
					AddIfNew(pIdSector);	  
					done = TRUE;
				}
			}
		CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	return done;
}

//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(4)

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

//  function to decode the serial numbers of IDE hard drives
//  using the IOCTL_STORAGE_QUERY_PROPERTY command 
char * DiskInfo::flipAndCodeBytes (char * str)
{
	static char flipped [1000];
	int num = strlen (str);
	strcpy (flipped, "");
	for (int i = 0; i < num; i += 4)
	{
		for (int j = 1; j >= 0; j--)
		{
			int sum = 0;
			for (int k = 0; k < 2; k++)
			{
				sum *= 16;
				switch (str [i + j * 2 + k])
				{
					case '0': sum += 0; break;
					case '1': sum += 1; break;
					case '2': sum += 2; break;
					case '3': sum += 3; break;
					case '4': sum += 4; break;
					case '5': sum += 5; break;
					case '6': sum += 6; break;
					case '7': sum += 7; break;
					case '8': sum += 8; break;
					case '9': sum += 9; break;
					case 'a': sum += 10; break;
					case 'b': sum += 11; break;
					case 'c': sum += 12; break;
					case 'd': sum += 13; break;
					case 'e': sum += 14; break;
					case 'f': sum += 15; break;
				}
			}
			if (sum > 0) 
			{
				char sub [2];
				sub [0] = (char) sum;
				sub [1] = 0;
				strcat (flipped, sub);
			}
		}
	}
	return flipped;
}

typedef struct _MEDIA_SERAL_NUMBER_DATA {
  ULONG  SerialNumberLength; 
  ULONG  Result;
  ULONG  Reserved[2];
  UCHAR  SerialNumberData[1];
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

int DiskInfo::ReadPhysicalDriveInNTWithZeroRights (void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR driveName [256];
		swprintf (driveName, L"\\\\.\\PhysicalDrive%d", drive);
		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = CreateFile (driveName, 0,
											FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
											OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];

			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			memset (buffer, 0, sizeof (buffer));

			if (DeviceIoControl( hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
								& query,
								sizeof (query),
								& buffer,
								sizeof (buffer),
								& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				char serialNumber [1000];

				strcpy (serialNumber, flipAndCodeBytes ( & buffer [descrip -> SerialNumberOffset]));
			}
			else
			{
				DWORD err = GetLastError ();
			}
			memset (buffer, 0, sizeof (buffer));
			if (DeviceIoControl( hPhysicalDriveIOCTL, IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER,
								NULL,
								0,
								& buffer,
								sizeof (buffer),
								& cbBytesReturned, NULL) )
			{         
				MEDIA_SERIAL_NUMBER_DATA * mediaSerialNumber = (MEDIA_SERIAL_NUMBER_DATA *) & buffer;
				char serialNumber [1000];
				strcpy( serialNumber, (char *) mediaSerialNumber -> SerialNumberData);
			}
			else
			{
				DWORD err = GetLastError ();
			}
			CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	return done;
}

// DoIDENTIFY
// FUNCTION: Send an IDENTIFY command to the drive
// bDriveNum = 0-3
// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL DiskInfo::DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned)
{
	  // Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;

	  // Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	  // The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
			   (LPVOID) pSCIP,
			   sizeof(SENDCMDINPARAMS) - 1,
			   (LPVOID) pSCOP,
			   sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
			   lpcbBytesReturned, NULL) );
}

#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

int DiskInfo::ReadIdeDriveAsScsiDriveInNT (void)
{
   int done = FALSE;
   int controller = 0;

   for (controller = 0; controller < 16; controller++)
   {
      HANDLE hScsiDriveIOCTL = 0;
      TCHAR   driveName [256];

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      swprintf (driveName, L"\\\\.\\Scsi%d:", controller);

         //  Windows NT, Windows 2000, any rights should do
      hScsiDriveIOCTL = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);
      if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
      {
         int drive = 0;

         for (drive = 0; drive < 2; drive++)
         {
            char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
            SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
            SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
            DWORD dummy;
   
            memset (buffer, 0, sizeof (buffer));
            p -> HeaderLength = sizeof (SRB_IO_CONTROL);
            p -> Timeout = 10000;
            p -> Length = SENDIDLENGTH;
            p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
            strncpy ((char *) p -> Signature, "SCSIDISK", 8);
  
            pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
            pin -> bDriveNumber = drive;

            if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                                 buffer,
                                 sizeof (SRB_IO_CONTROL) + sizeof (SENDCMDINPARAMS) - 1,
                                 buffer,
                                 sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
                                 &dummy, NULL))
            {
               SENDCMDOUTPARAMS *pOut = (SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
               IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
               if (pId -> sModelNumber [0])
               {
                  USHORT *pIdSector = (USHORT *) pId;
				  AddIfNew(pIdSector);	
                  done = TRUE;
               }
            }
         }
         CloseHandle (hScsiDriveIOCTL);
      }
   }
   return done;
}

char* DiskInfo::ModelNumber (UINT drive)
{
	return ConvertToString(m_list[drive], 27, 46);
}

char* DiskInfo::SerialNumber (UINT drive)
{
	return ConvertToString(m_list[drive], 10, 19);
}



char* DiskInfo::RevisionNumber (UINT drive)
{
	return ConvertToString(m_list[drive], 23, 26);
}

char* DiskInfo::DriveType (UINT drive)
{
   if (m_list[drive][0] & 0x0080)
      return "Removable";
   else if (m_list[drive][0] & 0x0040)
      return "Fixed";
   else return "Unknown";
}

char * DiskInfo::ConvertToString (WORD diskdata [256], int firstIndex, int lastIndex)
{
	static char string [1024];
	int index = 0;
	int position = 0;

	//  each integer has two characters stored in it backwards
	for (index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character
		string [position] = (char) (diskdata [index] / 256);
		position++;

		//  get low byte for 2nd character
		string [position] = (char) (diskdata [index] % 256);
		position++;
	}

	//  end the string 
	string [position] = '\0';

	//  cut off the trailing blanks
	for (index = position - 1; index > 0 && ' ' == string [index]; index--)
		string [index] = '\0';

	return string;
}

unsigned __int64 DiskInfo::DriveSize (UINT drive)
{
	unsigned __int64 bytes = 0,sectors =0;
	if (m_list[drive] [83] & 0x400) 
		sectors = m_list[drive][103] * 65536I64 * 65536I64 * 65536I64 + 
					m_list[drive][102] * 65536I64 * 65536I64 + 
					m_list[drive][101] * 65536I64 + 
					m_list[drive][100];
	else
		sectors = m_list[drive][61] * 65536 + m_list[drive][60];
		//  there are 512 bytes in a sector
		bytes = sectors * 512;
	return bytes;
}

DiskInfo::DiskInfo(void)
{
	//m_DriveCount = 0;
}

DiskInfo::~DiskInfo(void)
{
	vector <WORD*>::iterator listIter;
	for (listIter = m_list.begin(); listIter != m_list.end(); listIter++)
		delete [] *listIter;
/*
	for(UINT i = 0; i< m_list.size(); i++)
		delete m_list[i];
*/
	m_list.clear();
}

long DiskInfo::LoadDiskInfo()
{
	int done = FALSE;
	__int64 id = 0;
	OSVERSIONINFO version;
	memset (&version, 0, sizeof (version));
	version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&version);
	for(UINT i = 0; i< m_list.size(); i++)
		delete [] m_list[i];
	m_list.clear();
	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		//  this works under WinNT4 or Win2K if you have admin rights
		done = ReadPhysicalDriveInNTWithAdminRights ();

		//  this should work in WinNT or Win2K if previous did not work
		//  this is kind of a backdoor via the SCSI mini port driver into
		//     the IDE drives
		done = ReadIdeDriveAsScsiDriveInNT ();

		//  this works under WinNT4 or Win2K or WinXP if you have any rights
		///				done = ReadPhysicalDriveInNTWithZeroRights ();
	}
	else
	{
		// Win9X not supported
		return -1;
	}
	return (long) m_list.size();
}

UINT DiskInfo::BufferSize (UINT drive)
{
	return m_list[drive][21] * 512;
}
