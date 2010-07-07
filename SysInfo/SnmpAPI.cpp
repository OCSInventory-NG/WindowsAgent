//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#include "stdafx.h"

#include <snmp.h>
#include "snmpapi.h"

//******************************************************************
//*
//*      Copyright (C) Stas Khirman 1998.  All rights reserved.         
//*                                                                     
//*       This program is distributed WITHOUT ANY WARRANTY
//*
//******************************************************************
//*     
//*       Reproduction of SNMP.LIB and SNMPAPI.LIB base
//*           functions
//*
//* Author: Stas Khirman (staskh@rocketmail.com)
//*
//*
//* Free software: no warranty; use anywhere is ok; spread the      
//* sources; note any modifications; share variations and           
//* derivatives (including sending to staskh@rocketmail.com).       
//*                                                                     
//******************************************************************

SNMPAPI SNMP_FUNC_TYPE SnmpUtilOidCpy(OUT AsnObjectIdentifier *DstObjId,
	IN AsnObjectIdentifier *SrcObjId)
{
  DstObjId->ids = (UINT *)GlobalAlloc(GMEM_ZEROINIT,SrcObjId->idLength * sizeof(UINT));
  if(!DstObjId->ids){
    SetLastError(1);
    return 0;
  }

  memcpy(DstObjId->ids,SrcObjId->ids,SrcObjId->idLength*sizeof(UINT));
  DstObjId->idLength = SrcObjId->idLength;

  return 1;
}


VOID SNMP_FUNC_TYPE SnmpUtilOidFree(IN OUT AsnObjectIdentifier *ObjId)
{
  GlobalFree(ObjId->ids);
  ObjId->ids = 0;
  ObjId->idLength = 0;
}

SNMPAPI SNMP_FUNC_TYPE SnmpUtilOidNCmp(IN AsnObjectIdentifier *ObjIdA,
	IN AsnObjectIdentifier *ObjIdB, IN UINT Len)
{
  UINT CmpLen;
  UINT i;
  int  res;

  CmpLen = Len;
  if(ObjIdA->idLength < CmpLen)
    CmpLen = ObjIdA->idLength;
  if(ObjIdB->idLength < CmpLen)
    CmpLen = ObjIdB->idLength;

  for(i=0;i<CmpLen;i++){
    res = ObjIdA->ids[i] - ObjIdB->ids[i];
    if(res!=0)
      return res;
  }
  return 0;
}

VOID SNMP_FUNC_TYPE SnmpUtilVarBindFree(IN OUT RFC1157VarBind *VarBind)
{
  BYTE asnType;
  // free object name
  SnmpUtilOidFree(&VarBind->name);

  asnType = VarBind->value.asnType;

  if(asnType==ASN_OBJECTIDENTIFIER){
	  SnmpUtilOidFree(&VarBind->value.asnValue.object);
  }
  else if(
        (asnType==ASN_OCTETSTRING) ||
        (asnType==ASN_RFC1155_IPADDRESS) ||
        (asnType==ASN_RFC1155_OPAQUE) ||
        (asnType==ASN_SEQUENCE)){
    if(VarBind->value.asnValue.string.dynamic){
      GlobalFree(VarBind->value.asnValue.string.stream);
    }
  }

  VarBind->value.asnType = ASN_NULL;
}

//******************************************************************
//*
//*      End of Copyright (C) Stas Khirman 1998
//*                                                                     
//******************************************************************

//******************************************************************
//*
//*      Following definitions are extracted from MRTG 2.9.17
//*
//* MRTG - The Multi Router Traffic Grapher. A tool to visualise
//* network traffic via a WebPage.
//* http://www.ee.ethz.ch/~oetiker/webtools/mrtg/
//*
//* Copyright (c) 1996-2001 Tobias Oetiker and all the Contributers
//* to MRTG
//* All rights reserved.
//*                                                                     
//******************************************************************

LPCTSTR GetIfType( UINT uType)
{
	switch (uType)
	{
	case 1: return _T( "Other");
	case 2: return _T( "regular1822");
	case 3: return _T( "hdh1822");
	case 4: return _T( "ddnX25");
	case 5: return _T( "rfc877x25");
	case 6: return _T( "ethernetCsmacd");
	case 7: return _T( "iso88023Csmacd");
	case 8: return _T( "iso88024TokenBus");
	case 9: return _T( "iso88025TokenRing");
	case 10: return _T( "iso88026Man");
	case 11: return _T( "starLan");
	case 12: return _T( "proteon10Mbit");
	case 13: return _T( "proteon80Mbit");
	case 14: return _T( "hyperchannel");
	case 15: return _T( "fddi");
	case 16: return _T( "lapb");
	case 17: return _T( "sdlc");
	case 18: return _T( "ds1");
	case 19: return _T( "e1");
	case 20: return _T( "basicISDN");
	case 21: return _T( "primaryISDN");
	case 22: return _T( "propPointToPointSerial");
	case 23: return _T( "ppp");
	case 24: return _T( "softwareLoopback");
	case 25: return _T( "eon");
	case 26: return _T( "ethernet-3Mbit");
	case 27: return _T( "nsip");
	case 28: return _T( "slip");
	case 29: return _T( "ultra");
	case 30: return _T( "ds3");
	case 31: return _T( "sip");
	case 32: return _T( "frame-relay");
	case 33: return _T( "rs232");
	case 34: return _T( "para");
	case 35: return _T( "arcnet");
	case 36: return _T( "arcnetPlus");
	case 37: return _T( "atm");
	case 38: return _T( "miox25");
	case 39: return _T( "sonet");
	case 40: return _T( "x25ple");
	case 41: return _T( "iso88022llc");
	case 42: return _T( "localTalk");
	case 43: return _T( "smdsDxi");
	case 44: return _T( "frameRelayService");
	case 45: return _T( "v35");
	case 46: return _T( "hssi");
	case 47: return _T( "hippi");
	case 48: return _T( "modem");
	case 49: return _T( "aal5");
	case 50: return _T( "sonetPath");
	case 51: return _T( "sonetVT");
	case 52: return _T( "smdsIcip");
	case 53: return _T( "propVirtual");
	case 54: return _T( "propMultiplexor");
	case 55: return _T( "100BaseVG");
	case 56: return _T( "Fibre Channel");
	case 57: return _T( "HIPPI Interface");
	case 58: return _T( "Obsolete for FrameRelay");
	case 59: return _T( "ATM Emulation of 802.3 LAN");
	case 60: return _T( "ATM Emulation of 802.5 LAN");
	case 61: return _T( "ATM Emulation of a Circuit");
	case 62: return _T( "FastEthernet (100BaseT)");
	case 63: return _T( "ISDN & X.25");
	case 64: return _T( "CCITT V.11/X.21");
	case 65: return _T( "CCITT V.36");
	case 66: return _T( "CCITT G703 at 64Kbps");
	case 67: return _T( "Obsolete G702 see DS1-MIB");
	case 68: return _T( "SNA QLLC");
	case 69: return _T( "Full Duplex Fast Ethernet (100BaseFX)");
	case 70: return _T( "Channel");
	case 71: return _T( "Radio Spread Spectrum (802.11)");
	case 72: return _T( "IBM System 360/370 OEMI Channel");
	case 73: return _T( "IBM Enterprise Systems Connection");
	case 74: return _T( "Data Link Switching");
	case 75: return _T( "ISDN S/T Interface");
	case 76: return _T( "ISDN U Interface");
	case 77: return _T( "Link Access Protocol D (LAPD)");
	case 78: return _T( "IP Switching Opjects");
	case 79: return _T( "Remote Source Route Bridging");
	case 80: return _T( "ATM Logical Port");
	case 81: return _T( "AT&T DS0 Point (64 Kbps)");
	case 82: return _T( "AT&T Group of DS0 on a single DS1");
	case 83: return _T( "BiSync Protocol (BSC)");
	case 84: return _T( "Asynchronous Protocol");
	case 85: return _T( "Combat Net Radio");
	case 86: return _T( "ISO 802.5r DTR");
	case 87: return _T( "Ext Pos Loc Report Sys");
	case 88: return _T( "Apple Talk Remote Access Protocol");
	case 89: return _T( "Proprietary Connectionless Protocol");
	case 90: return _T( "CCITT-ITU X.29 PAD Protocol");
	case 91: return _T( "CCITT-ITU X.3 PAD Facility");
	case 92: return _T( "MultiProtocol Connection over Frame/Relay");
	case 93: return _T( "CCITT-ITU X213");
	case 94: return _T( "Asymetric Digitial Subscriber Loop (ADSL)");
	case 95: return _T( "Rate-Adapt Digital Subscriber Loop (RDSL)");
	case 96: return _T( "Symetric Digitial Subscriber Loop (SDSL)");
	case 97: return _T( "Very High Speed Digitial Subscriber Loop (HDSL)");
	case 98: return _T( "ISO 802.5 CRFP");
	case 99: return _T( "Myricom Myrinet");
	case 100: return _T( "Voice recEive and transMit (voiceEM)");
	case 101: return _T( "Voice Foreign eXchange Office (voiceFXO)");
	case 102: return _T( "Voice Foreign eXchange Station (voiceFXS)");
	case 103: return _T( "Voice Encapulation");
	case 104: return _T( "Voice Over IP Encapulation");
	case 105: return _T( "ATM DXI");
	case 106: return _T( "ATM FUNI");
	case 107: return _T( "ATM IMA");
	case 108: return _T( "PPP Multilink Bundle");
	case 109: return _T( "IBM IP over CDLC");
	case 110: return _T( "IBM Common Link Access to Workstation");
	case 111: return _T( "IBM Stack to Stack");
	case 112: return _T( "IBM Virtual IP Address (VIPA)");
	case 113: return _T( "IBM Multi-Protocol Channel Support");
	case 114: return _T( "IBM IP over ATM");
	case 115: return _T( "ISO 802.5j Fiber Token Ring");
	case 116: return _T( "IBM Twinaxial Data Link Control (TDLC)");
	case 117: return _T( "Gigabit Ethernet");
	case 118: return _T( "Higher Data Link Control (HDLC)");
	case 119: return _T( "Link Access Protocol F (LAPF)");
	case 120: return _T( "CCITT V.37");
	case 121: return _T( "CCITT X.25 Multi-Link Protocol");
	case 122: return _T( "CCITT X.25 Hunt Group");
	case 123: return _T( "Transp HDLC");
	case 124: return _T( "Interleave Channel");
	case 125: return _T( "Fast Channel");
	case 126: return _T( "IP (for APPN HPR in IP Networks)");
	case 127: return _T( "CATV MAC Layer");
	case 128: return _T( "CATV Downstream Interface");
	case 129: return _T( "CATV Upstream Interface");
	case 130: return _T( "Avalon Parallel Processor");
	case 131: return _T( "Encapsulation Interface");
	case 132: return _T( "Coffee Pot");
	case 133: return _T( "Circuit Emulation Service");
	case 134: return _T( "ATM Sub Interface");
	case 135: return _T( "Layer 2 Virtual LAN using 802.1Q");
	case 136: return _T( "Layer 3 Virtual LAN using IP");
	case 137: return _T( "Layer 3 Virtual LAN using IPX");
	case 138: return _T( "IP Over Power Lines");
	case 139: return _T( "Multi-Media Mail over IP");
	case 140: return _T( "Dynamic synchronous Transfer Mode (DTM)");
	case 141: return _T( "Data Communications Network");
	case 142: return _T( "IP Forwarding Interface");
	case 143: return _T( "Multi-rate Symmetric DSL (MSDSL)");
	case 144: return _T( "IEEE1394 High Perf Serial Bus");
	case 145: return _T( "TV adapter type");
	case 162: return _T( "Cisco Express Forwarding Interface");
	default: return _T( "N/A");
	}
}

BOOL IsEthernet( UINT uType)
{
	return ((uType == 6) || (uType == 7) || (uType == 26) ||
			(uType == 62) || (uType == 69) || (uType == 117));
}

BOOL IsTokenRing( UINT uType)
{
	return ((uType == 9) || (uType == 115));
}

BOOL IsTokenBus( UINT uType)
{
	return (uType == 8);
}

BOOL IsISDN( UINT uType)
{
	return ((uType == 20) || (uType == 21) || (uType == 63) ||
			(uType == 75) || (uType == 76) || (uType == 77));
}

BOOL IsATM( UINT uType)
{
	return ((uType == 37) || (uType == 49) || (uType == 107) ||
			(uType == 105) || (uType == 106) || (uType == 114) ||
			(uType == 134));
}

BOOL IsLAN( UINT uType)
{
	return (IsEthernet( uType) || IsTokenBus( uType) || IsTokenRing( uType) ||
			(uType == 11) || (uType == 15) || (uType == 55) || (uType == 59) ||
			(uType == 60));
}

BOOL IsDSL( UINT uType)
{
	return ((uType == 94) || (uType == 95) || (uType == 96) || (uType == 97) ||
			(uType == 143));
}

BOOL IsDialup( UINT uType)
{
	return (IsISDN( uType) || IsDSL( uType) || (uType == 23) || (uType == 81) ||
			(uType == 82) || (uType == 108));
}

BOOL IsLoopback( UINT uType)
{
	return (uType == 24);
}

LPCTSTR GetAdapterType( UINT uType)
{
	if (IsEthernet( uType))
		return ETHERNET_ADAPTER;
	if (IsTokenRing( uType))
		return TOKENRING_ADAPTER;
	if (IsTokenBus( uType))
		return TOKENBUS_ADAPTER;
	if (IsATM( uType))
		return ATM_ADAPTER;
	if (IsDialup( uType))
		return DIALUP_ADAPTER;
	if (IsLoopback( uType))
		return LOOPBACK_ADAPTER;
	return OTHER_ADAPTER;
}
