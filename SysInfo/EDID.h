//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// EDID.h: interface for the CEdid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEDID_H__1530F58D_06DF_4021_BEA3_79A900DD1BCD__INCLUDED_)
#define AFX_CEDID_H__1530F58D_06DF_4021_BEA3_79A900DD1BCD__INCLUDED_

#pragma once

// See http://faydoc.tripod.com/structures/

// EDID data format in registry key
typedef struct tag_EDIDRecord
{
    BYTE Padding[8];			// padding (all FFh, or 00h FFh..FFh 00h)
    BYTE Manufacturer_ID[2];		// big-endian manufacturer ID (see #00136) bits 14 - 10 first letter (01h='A', 02h='B', etc.) 
								// bits 9 - 5 second letter 
								// bits 4 - 0 third letter 
    WORD EDID_ID_Code;			// EDID ID code -- identifies monitor model
    DWORD Serial_Number;		// serial number or FFFFFFFFh  for "MAG", subtract 7000000 to get actual serial number 
								// for "OQI", subtract 456150000 
								// for "PHL", subtract ??? 
								// for "VSC", subtract 640000000 
    BYTE Week_Number_Manufacture;// week number of manufacture
    BYTE Manufacture_Year;		// manufacture year - 1990
    BYTE EDID_Version;			// EDID version
    BYTE EDID_Revision;			// EDID revision
    BYTE Video_Input_Type;		// video input type (see #00129)
    BYTE Maximum_Horizontal_Size; // maximum horizontal size in cm
    BYTE Maximum_Vertical_Size; // maximum vertical size in cm
    BYTE Gamma_Factor;			// gamma factor (gamma = 1.0 + factor/100, so max = 3.55)
    BYTE DPMS_Flags;			// DPMS flags (see #00128)
    BYTE Chroma_Information_Green_X_Y_Red_X_Y; // chroma information: green X'/Y' and red X'/Y'
    BYTE Chroma_Information_White_X_Y_Blue_X_Y; // chroma information: white X'/Y' and blue X'/Y'
    BYTE Chroma_Information_Red_Y; // chroma information: red Y
    BYTE Chroma_Information_Red_X; // chroma information: red X
    BYTE Chroma_Information_Green_Y; // chroma information: green Y
    BYTE Chroma_Information_Green_X; // chroma information: green X
    BYTE Chroma_Information_Blue_Y; // chroma information: blue Y
    BYTE Chroma_Information_Blue_X; // chroma information: blue X
    BYTE Chroma_Information_White_Y; // chroma information: white Y
    BYTE Chroma_Information_White_X; // chroma information: white X
    BYTE Established_Timings_1;	// established timings 1 (see #00130)
    BYTE Established_Timings_2; // established timings 2 (see #00131)
    BYTE Manufacturer_Reserved_Timing;	// manufacturer's reserved timing or 00h for none
										// bit 7: 1152x870 @ 75 Hz (Mac II, Apple)
    WORD Standard_Timing_Identification[8]; // standard timing identification 
											// resolution (low byte) and vertical frequency (high byte) for each of eight modes (see #00132) 
											// X resolution = (lowbyte + 31) * 8
    BYTE Detailed_Timing_Description1[18]; // detailed timing description #1 (see #00133) (v1.1) alternately, text identification (see #00135)
    BYTE Detailed_Timing_Description2[18]; // detailed timing description #2(v1.1) alternately, text identification (see #00135) (v1.1) alternately, text identification (see #00135)
    BYTE Detailed_Timing_Description3[18]; // detailed timing description #3(v1.1) alternately, text identification (see #00135) (v1.1) alternately, text identification (see #00135)
    BYTE Detailed_Timing_Description4[18]; // detailed timing description #4(v1.1) alternately, text identification (see #00135) (v1.1) alternately, text identification (see #00135) 7Eh BYTE unused???
    BYTE Unused;					// unused???
    BYTE Checksum;					// checksum (radix-complement: 256-low byte of 16-bit sum of 00h-7Eh)
} EDIDRecord;


// format of 8 modes for Standard_Timing_Identification
typedef struct tag_EDIDMode
{
    int X_Resolution;
    int Y_Resolution;
    float Aspect_Ratio;
    BYTE Vertical_Frequency;
} EDIDMode;

// Format of Detailed_Timing_DescriptionX if it is a detailed timing
typedef struct tag_EDID_Detailled_Timing
{
	BYTE Horizontal_Frequency;	// horizontal frequency in kHz (if 00h, may be text)(see #00135)
	BYTE Vertical_Frequency;	// vertical frequency in Hz
	BYTE Horizontal_Active_Time_and_X_Resolution; // horizontal active time (pixels) and X resolution
	BYTE Horizontal_Blanking_Time; // horizontal blanking time (pixels)
	BYTE Horizontal_Active_Time_2_or_Horizontal_Blanking_Time_2; // horizontal active time 2 / horizontal blanking time 2
	BYTE Vertical_Active_Time_and_Y_resolution; //vertical active time (lines) and Y resolution
	BYTE Vertical_Blanking_Time; // vertical blanking time (lines)
	BYTE Vertical_Active_Time_2_or_Vertical_Blanking_Time_2; // vertical active time 2 / vertical blanking time 2
	BYTE Horizontal_Sync_Offset; // horizontal sync offset (pixels)
	BYTE Horizontal_Sync_Pulsewidth; // (horizontal sync pulsewidth (pixels)
	BYTE Vertical_Sync_Offset_or_Vertical_Sync_Pulsewidth; // vertical sync offset / vertical sync pulsewidth
	BYTE Vertical_or_Horizontal_Sync_Offset_2_or_Sync_Pulsewidth_2; // vertical/horizontal sync offset 2 / vert/hor. sync pulsewidth 2
	BYTE Horizontal_Image_Size;	// horizontal image size (mm)
	BYTE Vertical_Image_Size;	// vertical image size (mm)
	BYTE Horizontal_Image_Size_2_or_Vertical_Image_Size_2; // horizontal image size 2 / vertical image size 2
	BYTE Horizontal_Border;		// horizontal border (pixels)
	BYTE Vertical_Border;		// vertical border (lines)
	BYTE Type_of_Display;		// type of display (see #00134)
} EDID_Detailled_Timing;

// Format of Detailed_Timing_DescriptionX if it is a text zone
// Allow to know which type is
typedef struct tag_EDID_Text_Identification_Strings
{
	BYTE Structure_Identification[2]; // 00h 00h 00h (to distinguish from detailed timing description)
	BYTE Text_Identifier;			// FFh serial number 'FEh vendor name 'FDh vertical/horizontal frequency range 'FCh model name
} EDID_Text_Identification_Strings;

// Format of Detailed_Timing_DescriptionX (from offset 4) if it is a text zone Freqency Range
typedef struct tag_EDID_Text_Frequency_Range
{
	BYTE Unknown;							// 00h ???
	BYTE Minimum_Vertical_Refresh_Frequency; // in Hz
	BYTE Maximum_Vertical_Refresh;
	BYTE Minimum_Horizontal_Frequency;		// in kHz
	BYTE Maximum_Horizontal_Frequency;
	BYTE Unknown2;							// 'FFh ???
} EDID_Text_Frequency_Range;

// Format of Detailed_Timing_DescriptionX (from offset 4) if it is a text zone Text (Serial Number, Model Name,Vendor Name)
#define DESCRIPTOR_DATA_OFFSET 4
typedef struct tag_EDID_Text_String
{
    char Text[14]; // may be terminated with either a NUL (00h) or LF (0Ah)
} EDID_Text_String;

// EDID Structure for this module
typedef struct tag_Standard_EDID
{
    char Manufacturer_ID[4];
    WORD EDID_ID_Code;
    DWORD Serial_Number;
    BYTE Week_Number_Manufacture;
    int Manufacture_Year;  // + 1990
    BYTE EDID_Version;
    BYTE EDID_Revision;
    BYTE Video_Input_Type;
    BYTE Maximum_Horizontal_Size; // in cm
    BYTE Maximum_Vertical_Size; // in cm
    BYTE Gamma_Factor; // (gamma = 1.0 + factor/100, so max = 3.55)
    BYTE DPMS_Flags; // (see #00128)
    BYTE Chroma_Information_Green_X_Y_Red_X_Y;
    BYTE Chroma_Information_White_X_Y_Blue_X_Y;
    BYTE Chroma_Information_Red_Y; // 
    BYTE Chroma_Information_Red_X; // 
    BYTE Chroma_Information_Green_Y; // 
    BYTE Chroma_Information_Green_X; // 
    BYTE Chroma_Information_Blue_Y; // 
    BYTE Chroma_Information_Blue_X; // 
    BYTE Chroma_Information_White_Y; // 
    BYTE Chroma_Information_White_X; // 
    BYTE Established_Timings_1; //  (see #00130)
    BYTE Established_Timings_2; //  (see #00131)
    BYTE Manufacturer_Reserved_Timing; // or 00h for none
    EDIDMode Standard_Timing_Identification[8];
    BYTE Detailed_Timing_Description1[18];
    BYTE Detailed_Timing_Description2[18];
    BYTE Detailed_Timing_Description3[18];
    BYTE Detailed_Timing_Description4[18];
    BYTE Checksum; // (radix-complement: 256-low byte of 16-bit sum of 00h-7Eh)
} Standard_EDID;

 // Detailed Timing enumeration types
enum DetailTiming
{
    Detailed_Timing_Description = 0,
    Serial_Number,
    Vendor_Name,
    Frequency_Range,
    Model_Name,
};


class SYSINFO_API CEdid  
{
public:
	CEdid();
	virtual ~CEdid();

	// Connect to SetupAPI on specified host (NULL for localhost)
	// WARNING : only Windows 2K or later is supported for remote operations
	BOOL Connect( LPCTSTR lpstrDevice = NULL);
	// Get monitor information from EDID standard
	BOOL GetMonitors( CMonitorList *pMyList);
	// Disconnect from SetupAPI
	BOOL Disconnect();
	BOOL IsConnected();

protected: // Methods
	// Parse EDID data and fill structure
	BOOL		ParseEDID( LPBYTE lpByte, Standard_EDID &myEDID);
	// Return manufacturer code (3 letters)
	LPCSTR		GetManufacturerID(BYTE ID[2]);
	// Retrieve EDID data from registry
	BOOL		GetDisplayEDID( HDEVINFO hDeviceInfoSet, SP_DEVINFO_DATA *pDevInfoData, Standard_EDID &myEDID);
	// Decode DPMS flags to get monitor type
	LPCSTR		DecodeDPMSFlag( BYTE Flag);
	// Parse TEXT zone of Detailed_Timing
	LPCSTR		GetEdidText( BYTE lpByte[18]);
	// Get manufacturer name from manufacturer code
	LPCSTR		GetManufacturerName( LPCSTR ID);
	// Get Detailed_Timing type
	DetailTiming GetDetailledTimingDescriptionType(BYTE Detailed_Timing_Descript[]);
	LPCSTR		GetDescription (Standard_EDID *myRecord);

	// Acer monitor specific because of bogus serial
	BOOL		AcerHack( CStringA &csSerial, Standard_EDID *myRecord);
	
protected: // Members variables
	HINSTANCE	m_hDll;		// Handle to setupapi.dll loaded
	HDEVINFO	m_hDevInfo;	// Device Enumerator
	CString		m_csDevice;	// Device to connect to
	CStringA	m_csBuffer;	// Temporary buffer
	BOOL		m_bIsWin2K;	// Flag to know if is Win 2K or higher, or Win 98/Me (95 and NT4 unsupported !)
};

#endif // !defined(AFX_CEDID_H__1530F58D_06DF_4021_BEA3_79A900DD1BCD__INCLUDED_)

