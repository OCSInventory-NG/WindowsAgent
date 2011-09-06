//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPConfig.cpp: implementation of the CHTTPConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPConfig::CHTTPConfig( LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	Clear();
	// Load default configuration from default file
	load( lpstrFile, lpstrSection);
}

CHTTPConfig::~CHTTPConfig()
{
}

void CHTTPConfig::Clear()
{
	// Server address
	m_csServerName = OCS_HTTP_DEFAULT_SERVER;

	// SSL connection required
	m_uSSL = OCS_HTTP_SSL_VALIDATION_REQUIRED;
	// Path to CA certificate chain file in PEM format
	m_csCaBundle.Format( _T( "%s\\%s"), getDataFolder(), OCS_HTTP_CA_DEFAULT_BUNDLE);

	// HTTP credentials if needed
	m_bAuthRequired = OCS_HTTP_AUTH_TYPE_NONE;
	m_csHttpUser.Empty();
	m_csHttpPwd.Empty();
	
	// Proxy type
	m_uProxyType = OCS_HTTP_PROXY_TYPE_NONE;
	m_csProxyName.Empty();
	// Proxy port
	m_uProxyPort = OCS_HTTP_DEFAULT_PROXY_PORT;
	// Proxy auth
	m_bProxyAuthRequired = OCS_HTTP_AUTH_TYPE_NONE;
	// Proxy credentials
	m_csProxyUser.Empty();
	m_csProxyPwd.Empty();
}

LPCTSTR CHTTPConfig::getServer()
{
	return m_csServerName;
}

BOOL CHTTPConfig::isSslServerValidationRequired()
{
	return (m_uSSL == OCS_HTTP_SSL_VALIDATION_REQUIRED);
}

LPCTSTR CHTTPConfig::getCaBundle()
{
	return m_csCaBundle;
}

BOOL CHTTPConfig::isHttpAuthRequired()
{
	return m_bAuthRequired;
}

LPCTSTR CHTTPConfig::getHttpUser()
{
	return m_csHttpUser;
}

LPCTSTR CHTTPConfig::getHttpPwd()
{
	return m_csHttpPwd;
}

UINT CHTTPConfig::getProxyType()
{
	return m_uProxyType;
}

LPCTSTR CHTTPConfig::getProxy()
{
	return m_csProxyName;
}

UINT CHTTPConfig::getProxyPort()
{
	return m_uProxyPort;
}

BOOL CHTTPConfig::isProxyAuthRequired()
{
	return m_bProxyAuthRequired;
}

LPCTSTR	CHTTPConfig::getProxyUser()
{
	return m_csProxyUser;
}

LPCTSTR CHTTPConfig::getProxyPwd()
{
	return m_csProxyPwd;
}

void CHTTPConfig::setServer( LPCTSTR lpstrServerName)
{
	m_csServerName = lpstrServerName;
}

void CHTTPConfig::setServerSSL( UINT uRequireSSL)
{
	m_uSSL = uRequireSSL;
}

void CHTTPConfig::setServerCA( LPCTSTR lpstrCaBundle)
{
	m_csCaBundle = lpstrCaBundle;
}

void CHTTPConfig::setServerAuthUser( LPCTSTR lpstrHttpUser)
{
	m_csHttpUser = lpstrHttpUser;
	m_bAuthRequired = TRUE;
}

void CHTTPConfig::setServerAuthPasswd( LPCTSTR lpstrHttpPwd)
{
	m_csHttpPwd = lpstrHttpPwd;
	m_bAuthRequired = TRUE;
}

void CHTTPConfig::setProxyType( UINT uProxyType)
{
	m_uProxyType = uProxyType;
}

void CHTTPConfig::setProxy( LPCTSTR lpstrServerName)
{
	m_csProxyName = lpstrServerName;
}

void CHTTPConfig::setProxyPort( UINT uPort)
{
	m_uProxyPort = uPort;
}

void CHTTPConfig::setProxyAuthUser( LPCTSTR lpstrProxyUser)
{
	m_csProxyUser = lpstrProxyUser;
	m_bProxyAuthRequired = TRUE;
}

void CHTTPConfig::setProxyAuthPasswd( LPCTSTR lpstrProxyPwd)
{
	m_csProxyPwd = lpstrProxyPwd;
	m_bProxyAuthRequired = TRUE;
}

BOOL CHTTPConfig::load( LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	try
	{
		// Crypted ocs db access
		CHTTPCrypt myCrypt;
		CString csConfigFile,
				csBuffer;

		// Get path to config file
		if (lpstrFile == NULL)
			csConfigFile = getAgentConfig()->getConfigFile();
		else
			csConfigFile = lpstrFile;
		
		// Server address
		GetPrivateProfileString( lpstrSection, _T( "Server"), OCS_HTTP_DEFAULT_SERVER, m_csServerName.GetBuffer( 1024), 1024, csConfigFile);
		m_csServerName.ReleaseBuffer();

		// SSL connection required
		m_uSSL = (UINT)GetPrivateProfileInt( lpstrSection, _T( "SSL"), OCS_HTTP_SSL_VALIDATION_REQUIRED, csConfigFile);
		if (m_uSSL == OCS_HTTP_SSL_VALIDATION_REQUIRED)
		{
			// Path to CA certificate chain file in PEM format
			csBuffer.Format( _T( "%s\\%s"), getDataFolder(), OCS_HTTP_CA_DEFAULT_BUNDLE);
			GetPrivateProfileString( lpstrSection, _T( "CaBundle"), csBuffer, m_csCaBundle.GetBuffer( 1024), 1024, csConfigFile);
			m_csCaBundle.ReleaseBuffer();
		}
		else
			m_csCaBundle.Empty();
		// HTTP credentials if needed
		m_bAuthRequired = (BOOL)GetPrivateProfileInt( lpstrSection, _T( "AuthRequired"), OCS_HTTP_AUTH_TYPE_NONE, csConfigFile);
		if (m_bAuthRequired)
		{
			// HTTP user		
			GetPrivateProfileString( lpstrSection, _T( "User"), _T( ""), csBuffer.GetBuffer( 1024), 1024, csConfigFile);
			csBuffer.ReleaseBuffer();
			myCrypt.getData( csBuffer, m_csHttpUser);
			GetPrivateProfileString( lpstrSection, _T( "Pwd"), _T( ""), csBuffer.GetBuffer( 1024), 1024, csConfigFile);
			csBuffer.ReleaseBuffer();
			myCrypt.getData( csBuffer, m_csHttpPwd);
		}
		
		// Proxy type
		m_uProxyType = (UINT)GetPrivateProfileInt( lpstrSection, _T( "ProxyType"), OCS_HTTP_PROXY_TYPE_NONE, csConfigFile);
		if (m_uProxyType)
		{
			// Retrieve an optionnal specified proxy
			GetPrivateProfileString( lpstrSection, _T( "Proxy"), _T( ""), m_csProxyName.GetBuffer( 1024), 1024, csConfigFile);
			m_csProxyName.ReleaseBuffer();
			// Proxy port
			m_uProxyPort = (UINT)GetPrivateProfileInt( lpstrSection, _T( "ProxyPort"), OCS_HTTP_DEFAULT_PROXY_PORT, csConfigFile);
			// Proxy auth
			m_bProxyAuthRequired = (BOOL)GetPrivateProfileInt( lpstrSection, _T( "ProxyAuthRequired"), OCS_HTTP_AUTH_TYPE_NONE, csConfigFile);
			// Proxy credentials
			if(m_bProxyAuthRequired)
			{
				GetPrivateProfileString( lpstrSection, _T( "ProxyUser"), _T( ""), csBuffer.GetBuffer( 1024), 1024, csConfigFile);
				csBuffer.ReleaseBuffer();
				myCrypt.getData( csBuffer, m_csProxyUser);
				GetPrivateProfileString( lpstrSection, _T( "ProxyPwd"), _T( ""), csBuffer.GetBuffer( 1024), 1024, csConfigFile);
				csBuffer.ReleaseBuffer();
				myCrypt.getData( csBuffer, m_csProxyPwd);
			}

		}
		else
		{
			m_bProxyAuthRequired = FALSE;
			m_uProxyPort = 0;
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL CHTTPConfig::save( LPCTSTR lpstrFile, LPCTSTR lpstrSection)
{
	try
	{
		// Crypted ocs db access
		CHTTPCrypt myCrypt;
		CString csConfigFile,
				csBuffer;
		BOOL	bResult;

		// Get path to config file
		if (lpstrFile == NULL)
			csConfigFile = getAgentConfig()->getConfigFile();
		else
			csConfigFile = lpstrFile;
		
		// Server address
		bResult = WritePrivateProfileString( lpstrSection, _T( "Server"), m_csServerName, csConfigFile);
		// SSL connection
		csBuffer.Format( _T( "%u"), m_uSSL);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "SSL"), csBuffer, csConfigFile);
		// Path to CA certificate chain file in PEM format
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "CaBundle"), m_csCaBundle, csConfigFile);
		// HTTP credentials 
		csBuffer.Format( _T( "%u"), m_bAuthRequired ? 1 : 0);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "AuthRequired"), csBuffer, csConfigFile);
		bResult = bResult && myCrypt.setData( m_csHttpUser, csBuffer);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "User"), csBuffer, csConfigFile);
		bResult = bResult && myCrypt.setData( m_csHttpPwd, csBuffer);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "Pwd"), csBuffer, csConfigFile);
		// Proxy server
		csBuffer.Format( _T( "%u"), m_uProxyType);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "ProxyType"), csBuffer, csConfigFile);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "Proxy"), m_csProxyName, csConfigFile);
		csBuffer.Format( _T( "%u"), m_uProxyPort);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "ProxyPort"), csBuffer, csConfigFile);
		csBuffer.Format( _T( "%u"), m_bProxyAuthRequired);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "ProxyAuthRequired"), csBuffer, csConfigFile);
		bResult = bResult && myCrypt.setData( m_csProxyUser, csBuffer);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "ProxyUser"), csBuffer, csConfigFile);
		bResult = bResult && myCrypt.setData( m_csProxyPwd, csBuffer);
		bResult = bResult && WritePrivateProfileString( lpstrSection, _T( "ProxyPwd"), csBuffer, csConfigFile);
		return bResult;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL CHTTPConfig::parseCommandLine(LPCTSTR lpstrCommand)
{
	try
	{
		// /SERVER:address
		if (isRequired( lpstrCommand, _T( "server"))) 
			// get network address
			setServer( getParamValue( lpstrCommand, _T( "server")));
		// /SSL:level
		if (isRequired( lpstrCommand, _T( "ssl"))) 
			// get SSL level
			setServerSSL( _ttol( getParamValue( lpstrCommand, _T( "ssl"))));
		// /SERVER:address
		if (isRequired( lpstrCommand, _T( "ca"))) 
			// get network address
			setServerCA( getParamValue( lpstrCommand, _T( "ca")));
		// /USER:username
		if (isRequired( lpstrCommand, _T( "user"))) 
			// get username
			setServerAuthUser( getParamValue( lpstrCommand, _T( "user")));
		// /PWD:userpwd
		if (isRequired( lpstrCommand, _T( "pwd"))) 
			// get password
			setServerAuthPasswd( getParamValue( lpstrCommand, _T( "pwd")));
		// /PROXY_TYPE:type
		if (isRequired( lpstrCommand, _T( "proxy_type"))) 
			// get proxy type
			setProxyType( _ttol( getParamValue( lpstrCommand, _T( "proxy_type"))));
		// /PROXY:address
		if (isRequired( lpstrCommand, _T( "proxy"))) 
			// get network address
			setProxy( getParamValue( lpstrCommand, _T( "proxy")));
		// /PROXY_PORT:port
		if (isRequired( lpstrCommand, _T( "proxy_port"))) 
			// get network port
			setProxyPort( _ttol( getParamValue( lpstrCommand, _T( "proxy_port"))));
		// /PROXY_USER:username
		if (isRequired( lpstrCommand, _T( "proxy_user"))) 
			// get username
			setProxyAuthUser( getParamValue( lpstrCommand, _T( "proxy_user")));
		// /PROXY_PWD:userpwd
		if (isRequired( lpstrCommand, _T( "proxy_pwd"))) 
			// get password
			setProxyAuthPasswd( getParamValue( lpstrCommand, _T( "proxy_pwd")));
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}
