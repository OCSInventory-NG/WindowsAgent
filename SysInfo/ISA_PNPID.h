//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ISA_PNPID.h
//
// As said in EDID standard (http://www.vesa.org/vesa-standards/standards-faq/), 
// ID Manufacturer Name is obtained from Microsoft. Plug and Play device ID for a monitor/LCD
// consists of seven characters. The first three characters represent the Vendor ID which is
// assigned by Microsoft. The four-character Product ID, which is a 2-byte hexadecimal number, 
// is assigned by the company producing the monitor. Companies can apply online for a Vendor ID 
// at http://www.microsoft.com/whdc/system/pnppwr/pnp/pnpid.mspx. Microsoft does not maintain Product
// IDs. It is the individual company’s responsibility to assure that they do not assign the same 
// Product ID to two different devices.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ISA_PNPID_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_)
#define ISA_PNPID_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_

#pragma once

LPCSTR ISA_PNPID_search( LPCSTR lpstrID);

#endif // !defined(ISA_PNPID_H__D965CF4B_51B1_474D_B615_3D4150A16F44__INCLUDED_)

