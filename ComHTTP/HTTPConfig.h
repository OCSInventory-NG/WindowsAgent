//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPConfig.h: interface for the CHTTPConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_)
#define AFX_HTTPCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_

#pragma once

#define OCS_HTTP_DEFAULT_SERVER				_T( "ocsinventory-ng")
#define OCS_HTTP_DEFAULT_PORT				80
#define OCS_HTTP_SSL_STANDARD				0
#define OCS_HTTP_SSL_VALIDATION_REQUIRED	1
#define OCS_HTTP_CA_DEFAULT_BUNDLE			_T( "cacert.pem")
#define OCS_HTTP_AUTH_TYPE_NONE				0
#define OCS_HTTP_AUTH_TYPE_REQUIRED			1
#define OCS_HTTP_DEFAULT_PROXY_PORT			80
#define OCS_HTTP_PROXY_TYPE_NONE			0
#define OCS_HTTP_PROXY_TYPE_HTTP			1
#define OCS_HTTP_PROXY_TYPE_SOCKS_4			2
#define OCS_HTTP_PROXY_TYPE_SOCKS_5			3

#include "Provider Classes.h"
#include "Config.h"
#include "ServerConfig.h"
#include "HTTPCrypt.h"

class OCS_PROVIDER_API CHTTPConfig : public CServerConfig
{
public: // Methods
	// The constructor
	CHTTPConfig( LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = OCS_HTTP_PROVIDER_SECTION);
	// Standard destructor
	virtual ~CHTTPConfig();

	// Reset default values
	void Clear();

	// Load configuration from specified file (default agent config used if NULL)
	// Use specified section (Communication server used if NULL)
	BOOL load( LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = OCS_HTTP_PROVIDER_SECTION);

	// Save configuration to specified file (default agent config used if NULL)
	// Use specified section (Communication server used if NULL)
	BOOL save( LPCTSTR lpstrFile = NULL, LPCTSTR lpstrSection = OCS_HTTP_PROVIDER_SECTION);

	// Allow parsing agent command line to grab command line params for
	// Communication provider
	BOOL parseCommandLine( LPCTSTR lpstrCommand);

	/****
	 *
	 * Server
	 *
	 ****/
	// Return the server name
	LPCTSTR getServer();
	// Return if we have to validate the server
	BOOL isSslServerValidationRequired();
	// Return the path to CA certificate chain file in PEM format
	LPCTSTR getCaBundle();
	// Set server name, port and if SSL server validation required
	void setServer( LPCTSTR lpstrServerName);
	void setServerSSL( UINT uRequireSSL = OCS_HTTP_SSL_STANDARD);
	void setServerCA( LPCTSTR lpstrCaBundle = NULL);

	/****
	 *
	 * Server authentication 
	 *
	 ****/
	// Return if http authentication is required or not
	BOOL	isHttpAuthRequired();
	// Return the http user
	LPCTSTR getHttpUser();
	// Return the http password
	LPCTSTR getHttpPwd();
	// Set server credentials
	void setServerAuthUser( LPCTSTR lpstrHttpUser);
	void setServerAuthPasswd( LPCTSTR lpstrHttpPwd);

	/****
	 *
	 * Proxy 
	 *
	 ****/
	// Return if we have a proxy, and which kind (HTTP, SOCKS4, SOCKS5)
	UINT	getProxyType();
	// Return specific proxies list
	LPCTSTR getProxy();
	// Return the proxy port
	UINT	getProxyPort();
	// Set proxy informations
	void setProxyType( UINT uProxyType = OCS_HTTP_PROXY_TYPE_HTTP);
	void setProxy( LPCTSTR lpstrServerName);
	void setProxyPort( UINT uPort = OCS_HTTP_DEFAULT_PORT);

	/****
	 *
	 * Proxy authentication 
	 *
	 ****/
	// Return if proxy authentication is required or not
	BOOL	isProxyAuthRequired();
	// Return the proxy user
	LPCTSTR	getProxyUser();
	// Return the proxy password
	LPCTSTR	getProxyPwd();
	// Set proxy credentials
	void setProxyAuthUser( LPCTSTR lpstrProxyUser);
	void setProxyAuthPasswd( LPCTSTR lpstrProxyPwd);

protected: // Attributes
	CString		m_csServerName;		// Server address
	UINT		m_uSSL;				// Use SSL/SSL with server cert validation or not
	CString		m_csCaBundle;		// Path to CA cert bundle in PEM format
	BOOL		m_bAuthRequired;	// Server authentication required or not
	CString		m_csHttpUser;		// Server credentials
	CString		m_csHttpPwd;

	// Proxy
	UINT		m_uProxyType;		// Use HTTP/Socks 4/Socks 5 proxy or not
	CString		m_csProxyName;		// Proxy address
	UINT		m_uProxyPort;		// Proxy port
	BOOL		m_bProxyAuthRequired;// Proxy authentication required or not
	CString		m_csProxyUser;		// Proxy credentials
	CString		m_csProxyPwd;
};

#endif // !defined(AFX_HTTPCONFIG_H__777B6FBD_A9CB_45E3_96BA_4E657722C00F__INCLUDED_)
