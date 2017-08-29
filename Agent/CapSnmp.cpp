//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2015 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Snmp.cpp => Implement snmp call interface.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CapSnmp.h"
#include "ExecCommand.h"
#include "SnmpQuery.h"
#include "SnmpResponse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCapSnmp::CCapSnmp()
{

}

CCapSnmp::~CCapSnmp()
{

}

//////////////////////////////////////////////////////////////////////
// Call OcsSnmp.exe with ipdiscover statment
//////////////////////////////////////////////////////////////////////

void CCapSnmp::CallSnmp(CConnexionAbstract	*pConnexion, CConfig *m_pConfig){

	// Phases objects
	CString csMessage;
	CSnmpQuery	*pSnmp = NULL;
	CSnmpResponse	*pSnmpResponse = NULL;
	int m_nExitCode = OCS_APP_NO_ERROR;

	/*****
	*
	*	Launch SNMP inventory
	*
	****/
	m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Launch snmp scan"));
	pSnmp = new CSnmpQuery(m_pConfig->isNotifyRequired());

	/*****
	*
	*	SNMP handler
	*
	****/

	//////////////////////////////////////////////////////////////////////////////////
	//  If the snmp feature is enable on OCS Inventory => SNMP Scan for this host.  //
	//////////////////////////////////////////////////////////////////////////////////

	CString csCmd;
	CString Type = _T("Unknow");
	bool SnmpOk;

	// Check for additional community in OCS inventory.


	////////////////////////
	//  Test device type  //
	////////////////////////

	m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Create Array"));

	///////////////////////////////////////////////////////////
	//  Defaults OID to scan for each SNMP responsive device //
	///////////////////////////////////////////////////////////

	// Select default oids list whatever snmp type detected
	CStringArray default_oids;
	default_oids.Add(_T("1.3.6.1.2.1.1.5.0"));
	default_oids.Add(_T("1.3.6.1.2.1.1.1.0"));
	default_oids.Add(_T("1.3.6.1.2.1.1.6.0"));
	default_oids.Add(_T("1.3.6.1.2.1.1.3.0"));
	default_oids.Add(_T("1.3.6.1.2.1.1.2.0"));
	default_oids.Add(_T("1.3.6.1.2.1.1.4.0"));
	default_oids.Add(_T("1.3.6.1.2.1.2.2.1.6."));
	default_oids.Add(_T("1.3.6.1.2.1.2.2.1.2."));
	default_oids.Add(_T("1.3.6.1.2.1.2.2.1.3"));

	// Set default name according to oids list
	CStringArray default_names;
	default_names.Add(_T("NAME"));
	default_names.Add(_T("DESCRIPTION"));
	default_names.Add(_T("LOCATION"));
	default_names.Add(_T("UPTIME"));
	default_names.Add(_T("SNMPDEVICEID")); //Oid Object
	default_names.Add(_T("CONTACT"));
	default_names.Add(_T("MACADDR"));
	default_names.Add(_T("IPADDR")); //ifdescr
	default_names.Add(_T("TYPE")); // iftype

	//////////////////////////////////////
	//  Test if device anwser SNMP call //
	//////////////////////////////////////

	std::string line;
	std::ifstream ipList("snmplist.txt");
	CString Line;
	CString IP;
	CString MAC;
	CString Community;
	CExecCommand cmProcess;

	if (ipList.is_open())
	{
		while (std::getline(ipList, line))
		{

			// Format line in file
			Line.Format(_T("%S"), line.c_str());

			// Split IP / MAC
			IP = Line.Left(Line.Find(_T(" ")));
			Line.Replace(IP, _T(""));
			Line.Replace(_T(" "), _T(""));
			MAC = Line;

			// Test with default snmp request if snmp anwser to call
			csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s"), getInstallFolder(), IP, Community);
			cmProcess.execWait(csCmd, getDataFolder());
			//_wsystem(csCmd);

			// Check if device anwser default snmp community

			std::ifstream myfile("snmp.txt");
			CString c_line;
			CString bad_line = _T("Timeout");
			Community = _T("public");

			if (myfile.is_open())
			{
				while (std::getline(myfile, line))
				{
					c_line.Format(_T("%S"), line.c_str());
					if (c_line.Find(bad_line) != -1){
						m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => %s do not anwser public snmp call, skip this host"), IP);
						SnmpOk = false;
					}
					else{
						m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => %s anwser public snmp call, initiate SNMP scan for this host"), IP);
						SnmpOk = true;
					}

				}
				myfile.close();
			}

			// Try with other community if not ok with public community
			if (!SnmpOk){
				m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Try with community retrieved by ocs server"));
				std::ifstream comfile("snmp_com.txt");
				while (std::getline(comfile, line))
				{
					c_line.Format(_T("%S"), line.c_str());
					BOOL okCom = this->CheckOid(c_line, IP, _T("1.3.6.1.2.1.1.1.0"));

					if (okCom){
						m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Custom community work on selected device, using it for scaning ..."));
						Community = c_line;
						SnmpOk = true;
					}

				}
				myfile.close();
			}

			////////////////////////////////////////////////////////////////
			//  Scan defaults oids on this device if he anwser snmp calls //
			////////////////////////////////////////////////////////////////

			if (SnmpOk){

				m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Perform types check ..."));
				BOOL okPrinter = this->CheckOid(Community, IP, _T("1.3.6.1.2.1.25.3.5.1.1.1"));
				BOOL okPhysical = this->CheckOid(Community, IP, _T("1.3.6.1.2.1.47.1.1.1.1.5"));
				CString physEnt = this->PhysicalModule(Community, IP);
				//std::string physEnt = "Switches";

				CMarkup *snmpXml = pSnmp->getXmlPointerContent();
				snmpXml->AddElem(_T("DEVICE"));
				snmpXml->AddElem(_T("COMMON"));

				// Send snmp request.
				for (int i = 0; i < 9; i++)
				{
					CString Oid = default_oids[i];

					csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP CMD => %s"), csCmd);
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP OID => %s"), Oid);
					cmProcess.execWait(csCmd, getDataFolder());
					//_wsystem(csCmd);

					std::string line;
					std::ifstream myfile("snmp.txt");
					CString c_line;

					if (myfile.is_open())
					{
						while (std::getline(myfile, line))
						{
							c_line.Format(_T("%S"), line.c_str());
							m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => Result : %s"), c_line);
						}
						myfile.close();
					}

					if (i == 4){
						c_line = MAC;
					}
					if (i == 6){
						c_line = MAC;
					}
					if (i == 7){
						c_line = IP;
					}
					if (i == 8){

						if (okPrinter){
							c_line = _T("Printer");
						}

					}

					c_line.Replace(_T("\""), _T(" "));
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP ADD XML => %s"), c_line);
					snmpXml->AddChildElem(default_names[i], c_line);

				}

				// Check for devices.
				snmpXml->OutOfElem();

				// Printer ?
				if (okPrinter){
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => printer module executing ..."));
					this->PrinterModule(Community, IP, snmpXml);
				}

				// Switch ? Cards ? Fan ?
				if (okPhysical){
					m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => physical module executing ..."));
					this->ScanPhysicalModule(Community, IP, snmpXml, physEnt);
				}

			}

		}

	}

	/*****
	*
	*	Send inventory
	*
	****/
	m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Sending Inventory"));
	pSnmpResponse = new CSnmpResponse(pConnexion->sendRequest(pSnmp));
	pSnmpResponse->setErrorStatus(pConnexion->getErrorStatus());
	pSnmpResponse->setErrorString(pConnexion->getErrorString());
	if (pSnmpResponse->isSuccess())
	{
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Inventory successfully sent"));
		pSnmp->setSuccess();
	}
	else
	{
		m_pLogger->log(LOG_PRIORITY_ERROR, _T("SNMP => Failed to send Inventory <%s>"), pSnmpResponse->getErrorString());
		m_nExitCode = OCS_APP_NETWORK_ERROR;
	}

	/*****
	*
	*	Writes the plain xml file on disk (for debugging purpose)
	*	 only if the "/xml" option was provided
	*
	****/
	if (m_pConfig->isXmlRequired())
	{
		csMessage.Format(_T("%s\\snmp.xml"), m_pConfig->getXmlFolder(), getOcsDeviceID()->getDeviceID());
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T("AGENT => Writing standard XML file <%s> required by argument /XML"), csMessage);
		WriteVoidToFile(pSnmp->getMessage(), pSnmp->getMessage().GetLength(), csMessage);
	}

}

//////////////////////////////////////////////////////////////////////
// Printer module check and scan if needed +  statut translation
//////////////////////////////////////////////////////////////////////
void CCapSnmp::PrinterModule(CString Community, CString IP, CMarkup *Xml){

	CExecCommand cmProcess;
	CString csCmd;

	// Printer basics informations
	CStringArray printer_oids;
	printer_oids.Add(_T("1.3.6.1.2.1.43.5.1.1.16.1"));
	printer_oids.Add(_T("1.3.6.1.2.1.43.5.1.1.17.1"));
	printer_oids.Add(_T("1.3.6.1.2.1.43.10.2.1.4.1.1"));
	printer_oids.Add(_T("1.3.6.1.2.1.25.3.5.1.1.1"));
	printer_oids.Add(_T("1.3.6.1.2.1.25.3.5.1.2.1"));

	CStringArray printer_names;
	printer_names.Add(_T("NAME"));
	printer_names.Add(_T("SERIALNUMBER"));
	printer_names.Add(_T("COUNTER"));
	printer_names.Add(_T("STATUS"));
	printer_names.Add(_T("ERRORSTATE")); //Oid Object

	Xml->AddElem(_T("PRINTERS"));

	for (size_t i = 0; i < 5; i++)
	{
		CString Oid = printer_oids[i];

		csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
		cmProcess.execWait(csCmd, getDataFolder());
		//_wsystem(csCmd);

		std::string line;
		std::ifstream myfile("snmp.txt");
		CString c_line;

		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				c_line.Format(_T("%S"), line.c_str());
			}
			myfile.close();
		}

		if (i == 3){
				
			if (c_line.Find(_T("1")) != -1)
				c_line = _T("other");
			if (c_line.Find(_T("2")) != -1)
				c_line = _T("unknown");
			if (c_line.Find(_T("3")) != -1)
				c_line = _T("idle");
			if (c_line.Find(_T("4")) != -1)
				c_line = _T("printing");
			if (c_line.Find(_T("5")) != -1)
				c_line = _T("warmup");

		}
		c_line.Replace(_T("\""), _T(" "));
		Xml->AddChildElem(printer_names[i], c_line);
	}

	Xml->OutOfElem();

	BOOL okCartridges = this->CheckOid(Community, IP, _T("1.3.6.1.2.1.43.11.1.1.6"));
	if (okCartridges){
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => cartridges module executing ..."));
		this->CartridgesModule(Community,IP,Xml);
	}

	BOOL okTrays = this->CheckOid(Community, IP, _T("1.3.6.1.2.1.43.8.2.1.13"));
	if (okTrays){
		m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => trays module executing ..."));
		this->TraysModule(Community, IP, Xml);
	}

}

//////////////////////////////////////////////////////////////////////
// Check the exsitance of an Oid in the remote host
//////////////////////////////////////////////////////////////////////
BOOL CCapSnmp::CheckOid(CString Community, CString IP, CString Oid){

	CExecCommand cmProcess;
	CString csCmd;
	std::string line;

	// Test with default snmp request if snmp anwser to call
	csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
	cmProcess.execWait(csCmd, getDataFolder());
	//_wsystem(csCmd);


	std::ifstream myfile("snmp.txt");
	CString c_line;
	CString bad_line = _T("No such name");
	CString bad_line2 = _T("Timeout");

	if (myfile.is_open())
	{
		while (std::getline(myfile, line))
		{
			c_line.Format(_T("%S"), line.c_str());
			if (c_line.Find(bad_line2) != -1){
				return false;
			}
			if (c_line.Find(bad_line) != -1){
				return false;
			}
			else{
				return true;
			}

		}
		myfile.close();
	}

}

//////////////////////////////////////////////////////////////////////
// Search for Trays oid
//////////////////////////////////////////////////////////////////////
void CCapSnmp::TraysModule(CString Community, CString IP, CMarkup *Xml){

	CExecCommand cmProcess;
	CString csCmd;
	std::string line;

	// trayinfos
	CStringArray trays_oids;
	trays_oids.Add(_T("1.3.6.1.2.1.43.8.2.1.13"));
	trays_oids.Add(_T("1.3.6.1.2.1.43.8.2.1.18"));
	trays_oids.Add(_T("1.3.6.1.2.1.43.8.2.1.10"));
	trays_oids.Add(_T("1.3.6.1.2.1.43.8.2.1.9"));

	CStringArray trays_names;
	trays_names.Add(_T("NAME"));
	trays_names.Add(_T("DESCRIPTION"));
	trays_names.Add(_T("LEVEL"));
	trays_names.Add(_T("MAXCAPACITY"));

	Xml->AddElem(_T("TRAYS"));

	for (size_t i = 0; i < 4; i++)
	{
		CString Oid = trays_oids[i];

		csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
		cmProcess.execWait(csCmd, getDataFolder());
		//_wsystem(csCmd);

		std::string line;
		std::ifstream myfile("snmp.txt");
		CString c_line;

		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				c_line.Format(_T("%S"), line.c_str());
			}
			myfile.close();
		}

		c_line.Replace(_T("\""), _T(" "));
		Xml->AddChildElem(trays_names[i], c_line);
	}

	Xml->OutOfElem();

}

//////////////////////////////////////////////////////////////////////
// Search for cartridges Oid
//////////////////////////////////////////////////////////////////////
void CCapSnmp::CartridgesModule(CString Community, CString IP, CMarkup *Xml){

	CExecCommand cmProcess;
	CString csCmd;
	std::string line;

	// trayinfos
	CStringArray cartridges_oids;
	cartridges_oids.Add(_T("1.3.6.1.2.1.43.11.1.1.6"));
	cartridges_oids.Add(_T("1.3.6.1.2.1.43.11.1.1.5"));
	cartridges_oids.Add(_T("1.3.6.1.2.1.43.11.1.1.9"));
	cartridges_oids.Add(_T("1.3.6.1.2.1.43.11.1.1.8"));
	cartridges_oids.Add(_T("1.3.6.1.2.1.43.12.1.1.4"));

	CStringArray cartridges_names;
	cartridges_names.Add(_T("DESCRIPTION"));
	cartridges_names.Add(_T("TYPE"));
	cartridges_names.Add(_T("LEVEL"));
	cartridges_names.Add(_T("MAXCAPACITY"));
	cartridges_names.Add(_T("COLOR")); //Oid Object

	Xml->AddElem(_T("CARTRIDGES"));

	for (size_t i = 0; i < 5; i++)
	{
		CString Oid = cartridges_oids[i];

		csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
		cmProcess.execWait(csCmd, getDataFolder());
		//_wsystem(csCmd);

		std::string line;
		std::ifstream myfile("snmp.txt");
		CString c_line;

		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				c_line.Format(_T("%S"), line.c_str());
			}
			myfile.close();
		}

		c_line.Replace(_T("\""), _T(" "));
		Xml->AddChildElem(cartridges_names[i], c_line);
	}

	Xml->OutOfElem();

}

//////////////////////////////////////////////////////////////////////
// Search for physical entities (Switch / snmpcard etc etc )
//////////////////////////////////////////////////////////////////////
CString CCapSnmp::PhysicalModule(CString Community, CString IP){

	CExecCommand cmProcess;
	CString Oid = _T("1.3.6.1.2.1.47.1.1.1.1.5");
	CString csCmd ;
	std::string line;
	CString Type;

	// Test with default snmp request if snmp anwser to call
	csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
	cmProcess.execWait(csCmd, getDataFolder());
	//_wsystem(csCmd);

	std::ifstream myfile("snmp.txt");
	CString c_line;
	CString bad_line = _T("No such name");

	if (myfile.is_open())
	{
		while (std::getline(myfile, line))
		{
			c_line.Format(_T("%S"), line.c_str());
			m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => %s ..."), c_line);
			if (c_line.Find(bad_line) != -1){
				m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => None"), c_line);
				Type = _T("None");
				return Type;
			}
			else{

				m_pLogger->log(LOG_PRIORITY_DEBUG, _T("SNMP => %s type"), c_line);
				if (c_line.Find(_T("3")) != -1){
					Type = _T("Switchs");
				}
				if (c_line.Find(_T("6")) != -1){
					Type = _T("PowerSupplies");
				}
				if (c_line.Find(_T("7")) != -1){
					Type = _T("Fans");
				}
				if (c_line.Find(_T("9")) != -1){
					Type = _T("Cards");
				}
				return Type;

			}

		}
		myfile.close();
	}

}

//////////////////////////////////////////////////////////////////////
// Scan oif from physical entities (Switch / snmpcard etc etc )
//////////////////////////////////////////////////////////////////////
void CCapSnmp::ScanPhysicalModule(CString Community, CString IP, CMarkup *Xml, CString Name){

	m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => phys in"));

	CExecCommand cmProcess;
	CString csCmd;
	std::string line;

	// trayinfos
	CStringArray phys_oids;
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.2."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.7."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.8."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.9."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.10."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.11."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.12."));
	phys_oids.Add(_T("1.3.6.1.2.1.47.1.1.1.1.13."));

	CStringArray phys_names;
	phys_names.Add(_T("DESCRIPTION"));
	phys_names.Add(_T("REFERENCE"));
	phys_names.Add(_T("REVISION"));
	phys_names.Add(_T("FIRMWARE"));
	phys_names.Add(_T("SOFTWARE")); //Oid Object
	phys_names.Add(_T("SERIALNUMBER"));
	phys_names.Add(_T("MANUFACTURER"));
	phys_names.Add(_T("TYPE"));

	Name.MakeUpper();
	Xml->AddElem(Name);

	for (size_t i = 0; i < 7; i++)
	{
		CString Oid = phys_oids[i];

		csCmd.Format(_T("\"%s\\OcsSnmp.exe\" %s %s %s"), getInstallFolder(), IP, Community, Oid);
		cmProcess.execWait(csCmd, getDataFolder());
		//_wsystem(csCmd);

		std::string line;
		std::ifstream myfile("snmp.txt");
		CString c_line;
		CString bad_line = _T("No such name");

		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				c_line.Format(_T("%S"), line.c_str());
			}
			myfile.close();
		}

		if (c_line.Find(bad_line) == -1){
			c_line.Replace(_T("\""), _T(" "));
			Xml->AddChildElem(phys_names[i], c_line);
		}
		
	}

	Xml->OutOfElem();

}

//////////////////////////////////////////////////////////////////////
// Call OcsSnmp.exe with ipdiscover statment using custom mibs
//////////////////////////////////////////////////////////////////////

void CCapSnmp::CallCustomMibs(std::string IpAddr, std::string Community){

}


/***
*
* Retrieve the communities from prolog response
*
***/
BOOL CCapSnmp::RetrieveCommunities()
{

	INT_PTR nIndex = 0;
	INT_PTR nCptr = 0;
	CMapStringToStringArray *pQueryArray = NULL;
	CMapStringToString		*pQuery = NULL;

	try
	{
		// Retrieve the CMapStringToString array (we have to delete it)
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Get SNMP parameters from the server"));
		pQueryArray = m_pPrologResp->getSnmpParameters();
		if ((pQueryArray == NULL) || pQueryArray->IsEmpty())
		{
			// No registry query defined
			m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => No communities detected, using default (public)"));
			return TRUE;
		}
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Communities available using them with default one (public)"));
		for (nIndex = 0; nIndex<pQueryArray->GetCount(); nIndex++)
		{
			CString	csName,
				csType;

			pQuery = pQueryArray->GetAt(nIndex);
			if ((pQuery == NULL) || pQuery->IsEmpty())
				continue;
			if (!pQuery->Lookup(_T("TYPE"), csType))
				csType.Empty();
			if (!pQuery->Lookup(_T("NAME"), csName))
				csName.Empty();

			if (csType.Find(_T("COMMUNITY")) != -1 ){
				std::ofstream comlist;
				comlist.open("snmp_com.txt", std::ios_base::app);
				std::string name = CT2A(csName);
				comlist << name << "\n";
				comlist.close();
				nCptr++;
			}


		}
		delete pQueryArray;
		pQueryArray = NULL;
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => %d community successfully added"), nCptr);
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		if (pQueryArray)
			delete pQueryArray;
		m_pLogger->log(LOG_PRIORITY_NOTICE, _T("SNMP => Unknown error while executing query %d"), nIndex + 1);
		return FALSE;
	}

}
