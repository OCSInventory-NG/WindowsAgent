//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// HTTPConnexion.cpp: implementation of the CHTTPConnexion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPConnexion.h"
#include "OcsUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//
//  libcurl write callback function
//

size_t HTTPConnexionHeaderWriter( char *data, size_t size, size_t nmemb, CString *csData)
{
	try
	{
		size_t		iter;

		ASSERT( data);

		if (csData == NULL)
			return 0;

		for (iter=0; iter < size * nmemb; iter++)
		{
#if defined( _UNICODE) || defined( _MBCS)
			csData->AppendFormat( _T( "%c"), data[iter]);
#else
			csData->Insert( csData->GetLength(), data[iter]);
#endif
		}

		return size * nmemb;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return -1;
	}
}

size_t HTTPConnexionByteArrayWriter( BYTE *data, size_t size, size_t nmemb, CByteArray *writerData)
{
	try
	{
		size_t		iter;
	
		ASSERT( data);

		if (writerData == NULL)
			return 0;

		for (iter=0; iter < size * nmemb; iter++)
			writerData->Add( data[iter]);
		
		return size * nmemb;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return -1;
	}
}

size_t HTTPConnexionFileWriter( BYTE *data, size_t size, size_t nmemb, CFile *writerData)
{
	try
	{
		ASSERT( data);

		if (writerData == NULL)
			return 0;

		writerData->SeekToEnd();
		writerData->Write( data, size * nmemb);
		
		return size * nmemb;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPConnexion::CHTTPConnexion( CHTTPConfig *pConfig)
{
	ASSERT( pConfig);

	m_pConfig = pConfig;
	/* General headers */
	m_csUserAgent.Format( _T( "OCS-NG_WINDOWS_AGENT_%s"), getAgentConfig()->getVersion());
	m_pCurl = NULL;
}

CHTTPConnexion::~CHTTPConnexion()
{
	if (m_pCurl != NULL)
		curl_easy_cleanup( m_pCurl);
}

BOOL CHTTPConnexion::initLibCurl()
{
	CString		csAuth;
	CURLcode	codeCurl;

	// Init cURL library if not already done
	if (m_pCurl == NULL)
		m_pCurl = curl_easy_init();
    if (m_pCurl == NULL)
	{
		// cURL init failed
		m_pLogger->log( LOG_PRIORITY_ERROR, _T( "COM SERVER => Failed to initialize cURL"));
		return FALSE;
	}
	// First, set cURL default settings
	curl_easy_reset( m_pCurl);
	// Do not show progress
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_NOPROGRESS, 1)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL No Progress option <%s>"), curl_easy_strerror( codeCurl));
	}
	// Enable cURL to follow redirect
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_FOLLOWLOCATION, TRUE)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL Follow Location option <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL user agent
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_USERAGENT, GetAnsiFromTString( m_csUserAgent))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL User Agent <%s>"), curl_easy_strerror( codeCurl));
	}
	// Initialize server authentication and credentials if needed
	if (m_pConfig->isHttpAuthRequired())
	{
		// Use server authentication
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL with server authentication"));
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_ANY)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL server authentication <%s>"), curl_easy_strerror( codeCurl));
		}
		csAuth.Format( _T( "%s:%s"), m_pConfig->getHttpUser(), m_pConfig->getHttpPwd());
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_USERPWD, GetAnsiFromTString( csAuth))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL server credentials <%s>"), curl_easy_strerror( codeCurl));
		}
	}
	else
		// Server authentication not used
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL without server authentication"));
	// Initialize proxy use if needed
	switch (m_pConfig->getProxyType())
	{
	case OCS_HTTP_PROXY_TYPE_HTTP: // HTTP proxy
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL HTTP type proxy <%s:%u>"), m_pConfig->getProxy(), m_pConfig->getProxyPort());
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP proxy type <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXY, GetAnsiFromTString( m_pConfig->getProxy()))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy address <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYPORT, m_pConfig->getProxyPort())) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy port <%s>"), curl_easy_strerror( codeCurl));
		}
		// Use proxy authentication if needed
		setProxyAuth();
		break;
	case OCS_HTTP_PROXY_TYPE_SOCKS_4: // Socks 4 proxy
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL Socks 4 type proxy <%s:%u>"), m_pConfig->getProxy(), m_pConfig->getProxyPort());
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL Socks 4 proxy type <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXY, GetAnsiFromTString( m_pConfig->getProxy()))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy address <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYPORT, m_pConfig->getProxyPort())) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy port <%s>"), curl_easy_strerror( codeCurl));
		}
		// Use proxy authentication if needed
		setProxyAuth();
		break;
	case OCS_HTTP_PROXY_TYPE_SOCKS_5: // Socks 5 proxy
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL Socks 5 type proxy <%s:%u>"), m_pConfig->getProxy(), m_pConfig->getProxyPort());
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL Socks 5 proxy type <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXY, GetAnsiFromTString( m_pConfig->getProxy()))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy address <%s>"), curl_easy_strerror( codeCurl));
		}
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYPORT, m_pConfig->getProxyPort())) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy port <%s>"), curl_easy_strerror( codeCurl));
		}
		// Use proxy authentication if needed
		setProxyAuth();
		break;
	case OCS_HTTP_PROXY_TYPE_NONE: // No proxy
	default:
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Disabling cURL proxy support"));
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXY, "")) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to disable cURL proxy use <%s>"), curl_easy_strerror( codeCurl));
		}
		break;
	}
	// Initialize server SSL validation if needed
	if (m_pConfig->isSslServerValidationRequired())
	{
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Enabling cURL SSL server validation support"));
		// Disconnect if we can't validate server's cert
		if ((codeCurl = curl_easy_setopt( m_pCurl,CURLOPT_SSL_VERIFYPEER,1)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to enable cURL SSL server certificate validation <%s>"), curl_easy_strerror( codeCurl));
		}
		// Set default CA certificate chain file path
	    if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_CAINFO, GetAnsiFromTString( m_pConfig->getCaBundle()))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL CA chain file <%s>"), curl_easy_strerror( codeCurl));
		}
		// Disconnect if we can't validate server's hostname (i.e. certificate CN or subject Alt Name != hostname)
	    if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_SSL_VERIFYHOST, 2)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to enable cURL SSL server hostname validation <%s>"), curl_easy_strerror( codeCurl));
		}
	}
	else
	{
		// Don't care about server certificate and hostname validation
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Disabling cURL SSL server validation support"));
		if ((codeCurl = curl_easy_setopt( m_pCurl,CURLOPT_SSL_VERIFYPEER, 0)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to disable cURL SSL server certificate validation <%s>"), curl_easy_strerror( codeCurl));
		}
	    if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_SSL_VERIFYHOST, 0)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to disable cURL SSL server hostname validation <%s>"), curl_easy_strerror( codeCurl));
		}
	}
	return TRUE;
}

BOOL CHTTPConnexion::setProxyAuth()
{
	CString		csAuth;
	CURLcode	codeCurl;

	if (m_pConfig->isProxyAuthRequired())
	{
		// Use proxy authentication
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL proxy with authentication"));
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYAUTH, CURLAUTH_ANY)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy authentication <%s>"), curl_easy_strerror( codeCurl));
		}
		csAuth.Format( _T( "%s:%s"), m_pConfig->getProxyUser(), m_pConfig->getProxyPwd());
		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYUSERPWD, GetAnsiFromTString( csAuth))) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL proxy credentials <%s>"), curl_easy_strerror( codeCurl));
		}
	}
	else
	{
		// Disable proxy authentication
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Using cURL proxy without authentication"));
/*		if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_PROXYAUTH, CURLAUTH_NONE)) != CURLE_OK)
		{
			m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to disable cURL proxy authentication <%s>"), curl_easy_strerror( codeCurl));
		}
*/	}
	return TRUE;
}

DWORD CHTTPConnexion::getHttpCode(CString &csHeader)
{
	int nPosHttp = -1,
		nPosCode;
	CString csCode;

	// Search last HTTP/1.X status code (ex "HTTP/1.1 200 OK")
	nPosCode = csHeader.Find( _T( "HTTP/"));
	while (nPosCode != -1)
	{
		nPosHttp = nPosCode;
		nPosCode = csHeader.Find( _T( "HTTP/"), nPosCode+1);
	}
	if (nPosHttp == -1)
		return OCS_RESPONSE_ERROR_CURL;
	// Search first space after HTTP/ to get code start
	if ((nPosCode = csHeader.Find( _T( " "), nPosHttp+5)) == -1)
		return OCS_RESPONSE_ERROR_CURL;
	// Extract code from header
	csCode = csHeader.Mid( nPosCode+1, 4);
	return _ttoi( csCode);
}

DWORD CHTTPConnexion::getErrorStatus()
{
	return m_dwErrorCode;
}

LPCTSTR CHTTPConnexion::getErrorString()
{
	return m_csErrorString;
}

CByteArray * CHTTPConnexion::sendRequest( CRequestAbstract * pRequest)
{
	CString		csUrl,
				csHeader;
	CByteArray	*pContentToSend,
				*pResponse = NULL;
	DWORD		dwContentLength;
    struct curl_slist *pRequestHeaders = NULL;
	CURLcode	codeCurl;

	ASSERT( pRequest);

	// Reset Error Code
	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "No error");
	// Finalize and compress data to be sent
	if (!pRequest->final())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_ZLIB;
		m_csErrorString = _T( "Zlib compress error");
		return pResponse;
	}
	if ((pContentToSend = CZip::deflate( pRequest->getMessage())) == NULL)
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_ZLIB;
		m_csErrorString = _T( "Zlib compress error");
		return pResponse;
	}
	dwContentLength = pContentToSend->GetSize();
	// Initialize cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Initializing cURL library for sendRequest"));
	if (!initLibCurl())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_CURL;
		m_csErrorString = _T( "Failed to initialize cURL");
		return pResponse;
	}
	// Set the URL to receive our POST
	csUrl =  m_pConfig->getServer();
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_URL, GetAnsiFromTString( csUrl))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post address <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive POST response data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEFUNCTION, HTTPConnexionByteArrayWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer callback <%s>"), curl_easy_strerror( codeCurl));
	}
	pResponse = new CByteArray();
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEDATA, pResponse)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer buffer <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive POST response header
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HEADERFUNCTION, HTTPConnexionHeaderWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header callback <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEHEADER, &csHeader)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header buffer <%s>"), curl_easy_strerror( codeCurl));
	}
    // Now specify cURL POST data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POST, 1)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post method <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POSTFIELDSIZE, dwContentLength)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post data length <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POSTFIELDS, pContentToSend->GetData())) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post data <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL content type
	pRequestHeaders = curl_slist_append( pRequestHeaders, "Content-Type: application/x-compressed");
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HTTPHEADER, pRequestHeaders)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response Content-Type <%s>"), curl_easy_strerror( codeCurl));
	}
    // Perform the request
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Sending HTTP Post request to URL <%s>"), csUrl);
	codeCurl = curl_easy_perform( m_pCurl);
    if (codeCurl != CURLE_OK)
	{
		m_dwErrorCode = codeCurl;
		m_csErrorString = curl_easy_strerror( codeCurl);
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to send HTTP Post request <%s>"), m_csErrorString);
	}
	else
	{
		m_dwErrorCode = getHttpCode( csHeader);
		m_csErrorString.Format( _T( "HTTP Status Code #%lu"), m_dwErrorCode);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => HTTP Post response received <%s>"), m_csErrorString);
	}

	// Delete sent buffer
	delete pContentToSend;

	// Clean cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Cleaning cURL library"));
	curl_slist_free_all( pRequestHeaders);
	curl_easy_cleanup( m_pCurl);
	m_pCurl = NULL;
	// Return the response pointer
	return pResponse;
}

CByteArray * CHTTPConnexion::simplePost( LPCTSTR lpstrURL, LPCTSTR lpstrPost)
{
	CString		csHeader;
	CByteArray	*pResponse = NULL;
    struct curl_slist *pRequestHeaders = NULL;
	CURLcode	codeCurl;

	ASSERT( lpstrURL);
	ASSERT( lpstrPost);

	// Reset Error Code
	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "No error");
	// Initialize cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Initializing cURL library for simplePost"));
	if (!initLibCurl())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_CURL;
		m_csErrorString = _T( "Failed to initialize cURL");
		return pResponse;
	}
	// Set the URL to receive our POST
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_URL, GetAnsiFromTString( lpstrURL))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL simplePost address <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive POST response data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEFUNCTION, HTTPConnexionByteArrayWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer callback <%s>"), curl_easy_strerror( codeCurl));
	}
	pResponse = new CByteArray();
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEDATA, pResponse)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer buffer <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive POST response header
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HEADERFUNCTION, HTTPConnexionHeaderWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header callback <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEHEADER, &csHeader)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header buffer <%s>"), curl_easy_strerror( codeCurl));
	}
    // Now specify cURL POST data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POST, 1)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post method <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POSTFIELDSIZE, _tcslen( lpstrPost))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post data length <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_POSTFIELDS, GetAnsiFromTString( lpstrPost))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Post data <%s>"), curl_easy_strerror( codeCurl));
	}
    // Perform the request
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Sending HTTP Post request to URL <%s>"), lpstrURL);
	codeCurl = curl_easy_perform( m_pCurl);
    if (codeCurl != CURLE_OK)
	{
		m_dwErrorCode = codeCurl;
		m_csErrorString = curl_easy_strerror( codeCurl);
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to send HTTP Post request <%s>"), m_csErrorString);
	}
	else
	{
		m_dwErrorCode = getHttpCode( csHeader);
		m_csErrorString.Format( _T( "HTTP Status Code #%lu"), m_dwErrorCode);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => HTTP Post response received <%s>"), m_csErrorString);
	}

	// Clean cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Cleaning cURL library"));
	curl_slist_free_all( pRequestHeaders);
	curl_easy_cleanup( m_pCurl);
	m_pCurl = NULL;
	// Return the response pointer
	return pResponse;
}

CByteArray * CHTTPConnexion::simpleGet( LPCTSTR lpstrURL)
{
	CString		csHeader;
	CByteArray	*pResponse = NULL;
    struct curl_slist *pRequestHeaders = NULL;
	CURLcode	codeCurl;

	ASSERT( lpstrURL);

	// Reset Error Code
	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "No error");
	// Initialize cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Initializing cURL library for simpleGet"));
	if (!initLibCurl())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_CURL;
		m_csErrorString = _T( "Failed to initialize cURL");
		return pResponse;
	}
	// Set the URL to receive our GET
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_URL, GetAnsiFromTString( lpstrURL))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL simpleGet address <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive Get response data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEFUNCTION, HTTPConnexionByteArrayWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer callback <%s>"), curl_easy_strerror( codeCurl));
	}
	pResponse = new CByteArray();
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEDATA, pResponse)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer buffer <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive Get response header
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HEADERFUNCTION, HTTPConnexionHeaderWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header callback <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEHEADER, &csHeader)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header buffer <%s>"), curl_easy_strerror( codeCurl));
	}
    // Now specify cURL GET
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HTTPGET, 1)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Get method <%s>"), curl_easy_strerror( codeCurl));
	}
    // Perform the request
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Sending HTTP Get request to URL <%s>"), lpstrURL);
	codeCurl = curl_easy_perform( m_pCurl);
    if (codeCurl != CURLE_OK)
	{
		m_dwErrorCode = codeCurl;
		m_csErrorString = curl_easy_strerror( codeCurl);
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to send HTTP Get request <%s>"), m_csErrorString);
	}
	else
	{
		m_dwErrorCode = getHttpCode( csHeader);
		m_csErrorString.Format( _T( "HTTP Status Code #%lu"), m_dwErrorCode);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => HTTP Get response received <%s>"), m_csErrorString);
	}
	// Clean cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Cleaning cURL library"));
	curl_slist_free_all( pRequestHeaders);
	curl_easy_cleanup( m_pCurl);
	m_pCurl = NULL;
	// Return the response pointer
	return pResponse;
}

BOOL CHTTPConnexion::getFile( LPCTSTR lpstrURL, LPCTSTR lpstrFilename)
{
	CString		csHeader;
	CFile		myFile;
    struct curl_slist *pRequestHeaders = NULL;
	CURLcode	codeCurl;
	BOOL		bResult = FALSE;

	ASSERT( lpstrURL);
	ASSERT( lpstrFilename);

	// Reset Error Code
	m_dwErrorCode = OCS_RESPONSE_ERROR_SUCCESS;
	m_csErrorString = _T( "No error");
	// Open file
	if (!myFile.Open( lpstrFilename, CFile::modeCreate|CFile::modeWrite))
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_FILE;
		m_csErrorString.Format( _T( "Failed to open file <%s> for writing"), lpstrFilename);
		return bResult;
	}
	// Initialize cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Initializing cURL library for getFile"));
	if (!initLibCurl())
	{
		m_dwErrorCode = OCS_RESPONSE_ERROR_CURL;
		m_csErrorString = _T( "Failed to initialize cURL");
		myFile.Abort();
		return bResult;
	}
	// Set the URL to receive our GET
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_URL, GetAnsiFromTString( lpstrURL))) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL fileGet address <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive Get response data
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEFUNCTION, HTTPConnexionFileWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer callback <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEDATA, &myFile)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response writer buffer <%s>"), curl_easy_strerror( codeCurl));
	}
	// Set cURL function and buffer to receive Get response header
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HEADERFUNCTION, HTTPConnexionHeaderWriter)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header callback <%s>"), curl_easy_strerror( codeCurl));
	}
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_WRITEHEADER, &csHeader)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL response header buffer <%s>"), curl_easy_strerror( codeCurl));
	}
    // Now specify cURL GET
	if ((codeCurl = curl_easy_setopt( m_pCurl, CURLOPT_HTTPGET, 1)) != CURLE_OK)
	{
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to set cURL HTTP Get method <%s>"), curl_easy_strerror( codeCurl));
	}
    // Perform the request
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Sending fileGet request to URL <%s>"), lpstrURL);
	codeCurl = curl_easy_perform( m_pCurl);
    if (codeCurl != CURLE_OK)
	{
		m_dwErrorCode = codeCurl;
		m_csErrorString = curl_easy_strerror( codeCurl);
		m_pLogger->log( LOG_PRIORITY_WARNING, _T( "COM SERVER => Failed to send HTTP Get request <%s>"), m_csErrorString);
		myFile.Abort();
	}
	else
	{
		m_dwErrorCode = getHttpCode( csHeader);
		m_csErrorString.Format( _T( "HTTP Status Code #%lu"), m_dwErrorCode);
		m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => fileGet response received <%s>"), m_csErrorString);
		myFile.Close();
		if (m_dwErrorCode >= 400)
			// HTTP client or server error
			bResult = FALSE;
		else
			bResult = TRUE;
	}
	// Clean cURL
	m_pLogger->log( LOG_PRIORITY_DEBUG, _T( "COM SERVER => Cleaning cURL library"));
	curl_slist_free_all( pRequestHeaders);
	curl_easy_cleanup( m_pCurl);
	m_pCurl = NULL;
	// Return the result
	return bResult;
}

BOOL CHTTPConnexion::getLabel( LPCTSTR lpstrFilename)
{
	CString csMessage;

	ASSERT( lpstrFilename);

	csMessage.Format( _T( "%s/deploy/label"),
			m_pConfig->getServer()); // server port
	return getFile( csMessage, lpstrFilename);
}
