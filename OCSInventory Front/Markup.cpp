// Markup.cpp: implementation of the CMarkup class.
//
// CMarkup Release 6.5 Lite
// Copyright (C) 1999-2003 First Objective Software, Inc. All rights reserved
// This entire notice must be retained in this source code
// Redistributing this source code requires written permission
// This software is provided "as is", with no warranty.
// Latest fixes enhancements and documentation at www.firstobject.com

#include "stdafx.h"
#include "afxconv.h"
#include "Markup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _MBCS
#pragma message( "Note: MBCS build (not UTF-8)" )
// For UTF-8, remove _MBCS from project settings C/C++ preprocessor definitions
#endif

// Defines for Windows CE
#ifdef _WIN32_WCE
#define _tclen(p) 1
#define _tccpy(p1,p2) *(p1)=*(p2)
#endif

void CMarkup::operator=( const CMarkup& markup )
{
	m_iPosParent = markup.m_iPosParent;
	m_iPos = markup.m_iPos;
	m_iPosChild = markup.m_iPosChild;
	m_iPosFree = markup.m_iPosFree;
	m_nNodeType = markup.m_nNodeType;
	m_aPos.RemoveAll();
	m_aPos.Append( markup.m_aPos );
	m_csDoc = markup.m_csDoc;
	MARKUP_SETDEBUGSTATE;
}

bool CMarkup::SetDoc( LPCTSTR szDoc )
{
	// Reset indexes
	m_iPosFree = 1;
	ResetPos();

	// Set document text
	if ( szDoc )
		m_csDoc = szDoc;
	else
		m_csDoc.Empty();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	int nStartSize = m_csDoc.GetLength() / 64 + 8;
	if ( m_aPos.GetSize() < nStartSize )
		m_aPos.SetSize( nStartSize );

	// Parse document
	bool bWellFormed = false;
	if ( m_csDoc.GetLength() )
	{
		m_aPos[0].Clear();
		int iPos = x_ParseElem( 0 );
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			bWellFormed = true;
		}
	}

	// Clear indexes if parse failed or empty document
	if ( ! bWellFormed )
	{
		m_aPos[0].Clear();
		m_iPosFree = 1;
	}

	ResetPos();
	return bWellFormed;
};

bool CMarkup::IsWellFormed()
{
	if ( m_aPos.GetSize() && m_aPos[0].iElemChild )
		return true;
	return false;
}

bool CMarkup::FindElem( LPCTSTR szName )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkup::FindChildElem( LPCTSTR szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}


CString CMarkup::GetTagName() const
{
	// Return the tag name at the current main position
	CString csTagName;


	if ( m_iPos )
		csTagName = x_GetTagName( m_iPos );
	return csTagName;
}

bool CMarkup::IntoElem()
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkup::OutOfElem()
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

int CMarkup::x_GetFreePos()
{
	//
	// This returns the index of the next unused ElemPos in the array
	//
	if ( m_iPosFree == m_aPos.GetSize() )
		m_aPos.SetSize( m_iPosFree + m_iPosFree / 2 );
	++m_iPosFree;
	return m_iPosFree - 1;
}

int CMarkup::x_ReleasePos()
{
	//
	// This decrements the index of the next unused ElemPos in the array
	// allowing the element index returned by GetFreePos() to be reused
	//
	--m_iPosFree;
	return 0;
}

int CMarkup::x_ParseError( LPCTSTR szError, LPCTSTR szName )
{
	if ( szName )
		m_csError.Format( szError, szName );
	else
		m_csError = szError;
	x_ReleasePos();
	return -1;
}

int CMarkup::x_ParseElem( int iPosParent )
{
	// This is either called by SetDoc, x_AddSubDoc, or itself recursively
	// m_aPos[iPosParent].nEndL is where to start parsing for the child element
	// This returns the new position if a tag is found, otherwise zero
	// In all cases we need to get a new ElemPos, but release it if unused
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = m_aPos[iPosParent].nEndL;
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;

	// Start Tag
	// A loop is used to ignore all remarks tags and special tags
	// i.e. <?xml version="1.0"?>, and <!-- comment here -->
	// So any tag beginning with ? or ! is ignored
	// Loop past ignored tags
	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPosParent].nEndL;
	CString csName;
	while ( csName.IsEmpty() )
	{
		// Look for left angle bracket of start tag
		m_aPos[iPos].nStartL = token.nNext;
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nStartL, _T('<') ) )
			return x_ParseError( _T("Element tag not found") );

		// Set parent's End tag to start looking from here (or later)
		m_aPos[iPosParent].nEndL = m_aPos[iPos].nStartL;

		// Determine whether this is an element, or bypass other type of node
		token.nNext = m_aPos[iPos].nStartL + 1;
		if ( x_FindToken( token ) )
		{
			if ( token.bIsString )
				return x_ParseError( _T("Tag starts with quote") );
			_TCHAR cFirstChar = m_csDoc[token.nL];
			if ( cFirstChar == _T('?') || cFirstChar == _T('!') )
			{
				token.nNext = m_aPos[iPos].nStartL;
				if ( ! x_ParseNode(token) )
					return x_ParseError( _T("Invalid node") );
			}
			else if ( cFirstChar != _T('/') )
			{
				csName = x_GetToken( token );
				// Look for end of tag
				if ( ! x_FindChar(token.szDoc, token.nNext, _T('>')) )
					return x_ParseError( _T("End of tag not found") );
			}
			else
				return x_ReleasePos(); // probably end tag of parent
		}
		else
			return x_ParseError( _T("Abrupt end within tag") );
	}
	m_aPos[iPos].nStartR = token.nNext;

	// Is ending mark within start tag, i.e. empty element?
	if ( m_csDoc[m_aPos[iPos].nStartR-1] == _T('/') )
	{
		// Empty element
		// Close tag left is set to ending mark, and right to open tag right
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR-1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nStartR;
	}
	else // look for end tag
	{
		// Element probably has contents
		// Determine where to start looking for left angle bracket of end tag
		// This is done by recursively parsing the contents of this element
		int iInner, iInnerPrev = 0;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + 1;
		while ( (iInner = x_ParseElem( iPos )) > 0 )
		{
			// Set links to iInner
			if ( iInnerPrev )
				m_aPos[iInnerPrev].iElemNext = iInner;
			else
				m_aPos[iPos].iElemChild = iInner;
			iInnerPrev = iInner;

			// Set offset to reflect child
			m_aPos[iPos].nEndL = m_aPos[iInner].nEndR + 1;
		}
		if ( iInner == -1 )
			return -1;

		// Look for left angle bracket of end tag
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nEndL, _T('<') ) )
			return x_ParseError( _T("End tag of %s element not found"), csName );

		// Look through tokens of end tag
		token.nNext = m_aPos[iPos].nEndL + 1;
		int nTokenCount = 0;
		while ( x_FindToken( token ) )
		{
			++nTokenCount;
			if ( ! token.bIsString )
			{
				// Is first token not an end slash mark?
				if ( nTokenCount == 1 && m_csDoc[token.nL] != _T('/') )
					return x_ParseError( _T("Expecting end tag of element %s"), csName );

				else if ( nTokenCount == 2 && ! token.Match(csName) )
					return x_ParseError( _T("End tag does not correspond to %s"), csName );

				// Else is it a right angle bracket?
				else if ( m_csDoc[token.nL] == _T('>') )
					break;
			}
		}

		// Was a right angle bracket not found?
		if ( ! token.szDoc[token.nL] || nTokenCount < 2 )
			return x_ParseError( _T("End tag not completed for element %s"), csName );
		m_aPos[iPos].nEndR = token.nL;
	}

	// Successfully parsed element (and contained elements)
	return iPos;
}

bool CMarkup::x_FindChar( LPCTSTR szDoc, int& nChar, _TCHAR c )
{
	// static function
	LPCTSTR pChar = &szDoc[nChar];
	while ( *pChar && *pChar != c )
		pChar += _tclen( pChar );
	nChar = pChar - szDoc;
	if ( ! *pChar )
		return false;
	/*
	while ( szDoc[nChar] && szDoc[nChar] != c )
		nChar += _tclen( &szDoc[nChar] );
	if ( ! szDoc[nChar] )
		return false;
	*/
	return true;
}

bool CMarkup::x_FindAny( LPCTSTR szDoc, int& nChar )
{
	// Starting at nChar, find a non-whitespace char
	// return false if no non-whitespace before end of document, nChar points to end
	// otherwise return true and nChar points to non-whitespace char
	while ( szDoc[nChar] && _tcschr(_T(" \t\n\r"),szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != '\0';
}

bool CMarkup::x_FindToken( CMarkup::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next token
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	LPCTSTR szDoc = token.szDoc;
	int nChar = token.nNext;
	token.bIsString = false;

	// By-pass leading whitespace
	if ( ! x_FindAny(szDoc,nChar) )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar;
		token.nNext = nChar;
		return false;
	}

	// Is it an opening quote?
	_TCHAR cFirstChar = szDoc[nChar];
	if ( cFirstChar == _T('\"') || cFirstChar == _T('\'') )
	{
		token.bIsString = true;

		// Move past opening quote
		++nChar;
		token.nL = nChar;

		// Look for closing quote
		x_FindChar( token.szDoc, nChar, cFirstChar );

		// Set right to before closing quote
		token.nR = nChar - 1;

		// Set nChar past closing quote unless at end of document
		if ( szDoc[nChar] )
			++nChar;
	}
	else
	{
		// Go until special char or whitespace
		token.nL = nChar;
		while ( szDoc[nChar] && ! _tcschr(_T(" \t\n\r<>=\\/?!"),szDoc[nChar]) )
			nChar += _tclen(&szDoc[nChar]);

		// Adjust end position if it is one special char
		if ( nChar == token.nL )
			++nChar; // it is a special char
		token.nR = nChar - 1;
	}

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

CString CMarkup::x_GetToken( const CMarkup::TokenPos& token ) const
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return _T("");
	return m_csDoc.Mid( token.nL,
		token.nR - token.nL + ((token.nR<m_csDoc.GetLength())? 1:0) );
}

int CMarkup::x_FindElem( int iPosParent, int iPos, LPCTSTR szPath )
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_csDoc );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStartL + 1;
		x_FindToken( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;
}

int CMarkup::x_ParseNode( CMarkup::TokenPos& token )
{
	// Call this with token.nNext set to the start of the node
	// This returns the node type and token.nNext set to the char after the node
	// If the node is not found or an element, token.nR is not determined
	int nTypeFound = 0;
	LPCTSTR szDoc = token.szDoc;
	token.nL = token.nNext;
	if ( szDoc[token.nL] == '<' )
	{
		// Started with <, could be:
		// <!--...--> comment
		// <!DOCTYPE ...> dtd
		// <?target ...?> processing instruction
		// <![CDATA[...]]> cdata section
		// <NAME ...> element
		//
		if ( ! szDoc[token.nL+1] || ! szDoc[token.nL+2] )
			return 0;
		_TCHAR cFirstChar = szDoc[token.nL+1];
		LPCTSTR szEndOfNode = NULL;
		if ( cFirstChar == _T('?') )
		{
			nTypeFound = MNT_PROCESSING_INSTRUCTION;
			szEndOfNode = _T("?>");
		}
		else if ( cFirstChar == _T('!') )
		{
			_TCHAR cSecondChar = szDoc[token.nL+2];
			if ( cSecondChar == _T('[') )
			{
				nTypeFound = MNT_CDATA_SECTION;
				szEndOfNode = _T("]]>");
			}
			else if ( cSecondChar == _T('-') )
			{
				nTypeFound = MNT_COMMENT;
				szEndOfNode = _T("-->");
			}
			else
			{
				// Document type requires tokenizing because of strings and brackets
				nTypeFound = 0;
				int nBrackets = 0;
				while ( x_FindToken(token) )
				{
					if ( ! token.bIsString )
					{
						_TCHAR cChar = szDoc[token.nL];
						if ( cChar == _T('[') )
							++nBrackets;
						else if ( cChar == _T(']') )
							--nBrackets;
						else if ( nBrackets == 0 && cChar == _T('>') )
						{
							nTypeFound = MNT_DOCUMENT_TYPE;
							break;
						}
					}
				}
				if ( ! nTypeFound )
					return 0;
			}
		}
		else if ( cFirstChar == _T('/') )
		{
			// End tag means no node found within parent element
			return 0;
		}
		else
		{
			nTypeFound = MNT_ELEMENT;
		}

		// Search for end of node if not found yet
		if ( szEndOfNode )
		{
			LPCTSTR pEnd = _tcsstr( &szDoc[token.nNext], szEndOfNode );
			if ( ! pEnd )
				return 0; // not well-formed
			token.nNext = (pEnd - szDoc) + _tcslen(szEndOfNode);
		}
	}
	else if ( szDoc[token.nL] )
	{
		// It is text or whitespace because it did not start with <
		nTypeFound = MNT_WHITESPACE;
		token.nNext = token.nL;
		if ( x_FindAny(szDoc,token.nNext) )
		{
			if ( szDoc[token.nNext] != _T('<') )
			{
				nTypeFound = MNT_TEXT;
				x_FindChar( szDoc, token.nNext, _T('<') );
			}
		}
	}
	return nTypeFound;
}

CString CMarkup::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPos].nStartL + 1;
	if ( ! iPos || ! x_FindToken( token ) )
		return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::x_FindAttrib( CMarkup::TokenPos& token, LPCTSTR szAttrib ) const
{
	// If szAttrib is NULL find next attrib, otherwise find named attrib
	// Return true if found
	int nAttrib = 0;
	for ( int nCount = 0; x_FindToken(token); ++nCount )
	{
		if ( ! token.bIsString )
		{
			// Is it the right angle bracket?
			_TCHAR cChar = m_csDoc[token.nL];
			if ( cChar == _T('>') || cChar == _T('/') || cChar == _T('?') )
				break; // attrib not found

			// Equal sign
			if ( cChar == _T('=') )
				continue;

			// Potential attribute
			if ( ! nAttrib && nCount )
			{
				// Attribute name search?
				if ( ! szAttrib || ! szAttrib[0] )
					return true; // return with token at attrib name

				// Compare szAttrib
				if ( token.Match(szAttrib) )
					nAttrib = nCount;
			}
		}
		else if ( nAttrib && nCount == nAttrib + 2 )
		{
			return true;
		}
	}

	// Not found
	return false;
}

CString CMarkup::x_GetAttrib( int iPos, LPCTSTR szAttrib ) const
{
	// Return the value of the attrib
	TokenPos token( m_csDoc );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStartL + 1;
	else
		return _T("");

	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return x_TextFromDoc( token.nL, token.nR - ((token.nR<m_csDoc.GetLength())?0:1) );
	return _T("");
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue )
{
	// Set attribute in iPos element
	TokenPos token( m_csDoc );
	int nInsertAt;
	if ( iPos && m_nNodeType == MNT_ELEMENT )
	{
		token.nNext = m_aPos[iPos].nStartL + 1;
		nInsertAt = m_aPos[iPos].nStartR - (m_aPos[iPos].IsEmptyElement()?1:0);
	}
	else
		return false;

	// Create insertion text depending on whether attribute already exists
	int nReplace = 0;
	CString csInsert;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Replace value only
		// Decision: for empty value leaving attrib="" instead of removing attrib
		csInsert = x_TextToDoc( szValue, true );
		nInsertAt = token.nL;
		nReplace = token.nR-token.nL+1;
	}
	else
	{
		// Insert string name value pair
		CString csFormat;
		csFormat = _T(" ");
		csFormat += szAttrib;
		csFormat += _T("=\"");
		csFormat += x_TextToDoc( szValue, true );
		csFormat += _T("\"");
		csInsert = csFormat;
	}

	x_DocChange( nInsertAt, nReplace, csInsert );
	int nAdjust = csInsert.GetLength() - nReplace;
	m_aPos[iPos].nStartR += nAdjust;
	m_aPos[iPos].AdjustEnd( nAdjust );
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

CString CMarkup::x_GetData( int iPos ) const
{

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// See if it is a CDATA section
		LPCTSTR szDoc = (LPCTSTR)m_csDoc;
		int nChar = m_aPos[iPos].nStartR + 1;
		if ( x_FindAny( szDoc, nChar ) && szDoc[nChar] == _T('<')
				&& nChar + 11 < m_aPos[iPos].nEndL
				&& _tcsncmp( &szDoc[nChar], _T("<![CDATA["), 9 ) == 0 )
		{
			nChar += 9;
			int nEndCDATA = m_csDoc.Find( _T("]]>"), nChar );
			if ( nEndCDATA != -1 && nEndCDATA < m_aPos[iPos].nEndL )
			{
				return m_csDoc.Mid( nChar, nEndCDATA - nChar );
			}
		}
		return x_TextFromDoc( m_aPos[iPos].nStartR+1, m_aPos[iPos].nEndL-1 );
	}
	return _T("");
}

CString CMarkup::x_TextToDoc( LPCTSTR szText, bool bAttrib ) const
{
	// Convert text as seen outside XML document to XML friendly
	// replacing special characters with ampersand escape codes
	// E.g. convert "6>7" to "6&gt;7"
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
	static _TCHAR* szaReplace[] = { _T("&lt;"),_T("&amp;"),_T("&gt;"),_T("&apos;"),_T("&quot;") };
	const _TCHAR* pFind = bAttrib?_T("<&>\'\""):_T("<&>");
	CString csText;
	const _TCHAR* pSource = szText;
	int nDestSize = _tcslen(pSource);
	nDestSize += nDestSize / 10 + 7;
	_TCHAR* pDest = csText.GetBuffer(nDestSize);
	int nLen = 0;
	_TCHAR cSource = *pSource;
	_TCHAR* pFound;
	while ( cSource )
	{
		if ( nLen > nDestSize - 6 )
		{
			csText.ReleaseBuffer(nLen);
			nDestSize *= 2;
			pDest = csText.GetBuffer(nDestSize);
		}
		if ( (pFound=(_TCHAR*) _tcschr(pFind,cSource)) != NULL )
		{
			pFound = szaReplace[pFound-pFind];
			_tcscpy(&pDest[nLen],pFound);
			nLen += _tcslen(pFound);
		}
		else
		{
			_tccpy( &pDest[nLen], pSource );
			nLen += _tclen( pSource );
		}
		pSource += _tclen( pSource );
		cSource = *pSource;
	}
	csText.ReleaseBuffer(nLen);

#if !defined( _MBCS) && !defined( _UNICODE)  // Convert to UTF8

	   	//http://msdn.microsoft.com/en-us/library/dd318672(VS.85).aspx
		//http://msdn.microsoft.com/en-us/library/cc500362.aspx
		//http://www.codase.com/search/call?name=MultiByteToWideChar

       nLen=csText.GetLength();
       
       int nLenOfWideCharStr=MultiByteToWideChar(CP_ACP, 0, csText, nLen, NULL, 0);
	   
       LPWSTR pWideCharStr= (PWSTR)HeapAlloc(GetProcessHeap(), 0, 
					nLenOfWideCharStr * sizeof(wchar_t));

		   //new WCHAR[buflen];
       nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, csText.GetBuffer(0), nLen, pWideCharStr, nLenOfWideCharStr);
       if (nLen > 0) {
			   int mbsize=WideCharToMultiByte( CP_UTF8, 0, pWideCharStr, nLen, NULL, 0, NULL, NULL);
               
			   LPSTR utfbuf= (PSTR)HeapAlloc(GetProcessHeap(), 0, 
					mbsize * sizeof(wchar_t));

				   //new CHAR[mbsize];
               nLen = WideCharToMultiByte(CP_UTF8, 0, pWideCharStr, nLen, utfbuf, mbsize, 0, 0);
               if (nLen > 0) {
                       utfbuf[nLen]=0;
                       csText = utfbuf;
               }
           //    delete[mbsize] utfbuf;
			   HeapFree(GetProcessHeap(), 0, utfbuf);
       }
       //delete[] buf;
	   HeapFree(GetProcessHeap(), 0, pWideCharStr);
#endif

	return csText;
}

CString CMarkup::x_TextFromDoc( int nLeft, int nRight ) const
{
	// Convert XML friendly text to text as seen outside XML document
	// ampersand escape codes replaced with special characters e.g. convert "6&gt;7" to "6>7"
	// Conveniently the result is always the same or shorter in byte length
	//
	static _TCHAR* szaCode[] = { _T("lt;"),_T("amp;"),_T("gt;"),_T("apos;"),_T("quot;") };
	static int anCodeLen[] = { 3,4,3,5,5 };
	static _TCHAR* szSymbol = _T("<&>\'\"");
	CString csText;
	const _TCHAR* pSource = m_csDoc;
	int nDestSize = nRight - nLeft + 1;
	_TCHAR* pDest = csText.GetBuffer(nDestSize);
	int nLen = 0;
	int nCharLen;
	int nChar = nLeft;
	while ( nChar <= nRight )
	{
		if ( pSource[nChar] == _T('&') )
		{
			// Look for matching &code;
			BOOL bCodeConverted = false;
			for ( int nMatch = 0; nMatch < 5; ++nMatch )
			{
				if ( nChar <= nRight - anCodeLen[nMatch]
					&& _tcsncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
				{
					// Insert symbol and increment index past ampersand semi-colon
					pDest[nLen++] = szSymbol[nMatch];
					nChar += anCodeLen[nMatch] + 1;
					bCodeConverted = true;
					break;
				}
			}

			// If the code is not converted, leave it as is
			if ( ! bCodeConverted )
			{
				pDest[nLen++] = _T('&');
				++nChar;
			}
		}
		else // not &
		{
			nCharLen = _tclen(&pSource[nChar]);
			_tccpy( &pDest[nLen], &pSource[nChar] );
			nLen += nCharLen;
			nChar += nCharLen;
		}
	}
	csText.ReleaseBuffer(nLen);
	return csText;
}

void CMarkup::x_DocChange( int nLeft, int nReplace, const CString& csInsert )
{
	// Insert csInsert int m_csDoc at nLeft replacing nReplace chars
	// Do this with only one buffer reallocation if it grows
	//
	int nDocLength = m_csDoc.GetLength();
	int nInsLength = csInsert.GetLength();

	// Make sure nLeft and nReplace are within bounds
	nLeft = max( 0, min( nLeft, nDocLength ) );
	nReplace = max( 0, min( nReplace, nDocLength-nLeft ) );

	// Get pointer to buffer with enough room
	int nNewLength = nInsLength + nDocLength - nReplace;
	int nBufferLen = nNewLength;
	_TCHAR* pDoc = m_csDoc.GetBuffer( nBufferLen );

	// Move part of old doc that goes after insert
	if ( nLeft+nReplace < nDocLength )
		memmove( &pDoc[nLeft+nInsLength], &pDoc[nLeft+nReplace], (nDocLength-nLeft-nReplace)*sizeof(_TCHAR) );

	// Copy insert
	memcpy( &pDoc[nLeft], csInsert, nInsLength*sizeof(_TCHAR) );

	// Release
	m_csDoc.ReleaseBuffer( nNewLength );
}

void CMarkup::x_Adjust( int iPos, int nShift, bool bAfterPos )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, end tag of iPos not affected)
	// 2. update next siblings and their children
	// 3. go up until there is a next sibling of a parent and update end tags
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children
	while ( iPos )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( (iPos=m_aPos[iPos].iElemParent) != 0 && iPos != iPosTop )
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].AdjustStart( nShift );
		m_aPos[iPos].AdjustEnd( nShift );
	}
}

void CMarkup::x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags )
{
	// Determine where to insert new element or node
	//
	bool bInsert = (nFlags&1)?true:false;
	bool bHonorWhitespace = (nFlags&2)?true:false;

	int nStartL;
	if ( nLength )
	{
		// Located at a non-element node
		if ( bInsert )
			nStartL = nOffset;
		else
			nStartL = nOffset + nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		if ( bInsert ) // precede iPosRel
			nStartL = m_aPos[iPosRel].nStartL;
		else // follow iPosRel
			nStartL = m_aPos[iPosRel].nEndR + 1;
	}
	else if ( ! iPosParent )
	{
		// Outside of all elements
		if ( bInsert )
			nStartL = 0;
		else
			nStartL = m_csDoc.GetLength();
	}
	else if ( m_aPos[iPosParent].IsEmptyElement() )
	{
		// Parent has no separate end tag, so split empty element
		nStartL = m_aPos[iPosParent].nStartR;
	}
	else
	{
		if ( bInsert ) // after start tag
			nStartL = m_aPos[iPosParent].nStartR + 1;
		else // before end tag
			nStartL = m_aPos[iPosParent].nEndL;
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! bHonorWhitespace && ! m_aPos[iPosParent].IsEmptyElement() )
	{
		LPCTSTR szDoc = (LPCTSTR)m_csDoc;
		int nChar = nStartL;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
			nStartL = nChar;
	}

	// Determine iPosBefore
	int iPosBefore = 0;
	if ( iPosRel )
	{
		if ( bInsert )
		{
			// Is iPosRel past first sibling?
			int iPosPrev = m_aPos[iPosParent].iElemChild;
			if ( iPosPrev != iPosRel )
			{
				// Find previous sibling of iPosRel
				while ( m_aPos[iPosPrev].iElemNext != iPosRel )
					iPosPrev = m_aPos[iPosPrev].iElemNext;
				iPosBefore = iPosPrev;
			}
		}
		else
		{
			iPosBefore = iPosRel;
		}
	}
	else if ( m_aPos[iPosParent].iElemChild )
	{
		if ( ! bInsert )
		{
			// Find last element under iPosParent
			int iPosLast = m_aPos[iPosParent].iElemChild;
			int iPosNext = iPosLast;
			while ( iPosNext )
			{
				iPosLast = iPosNext;
				iPosNext = m_aPos[iPosNext].iElemNext;
			}
			iPosBefore = iPosLast;
		}
	}

	nOffset = nStartL;
	iPosRel = iPosBefore;
}

bool CMarkup::x_AddElem( LPCTSTR szName, LPCTSTR szValue, bool bInsert, bool bAddChild )
{
	if ( bAddChild )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}
	else if ( m_iPosParent == 0 )
	{
		// Adding root element
		if ( IsWellFormed() )
			return false;


		// Locate after any version and DTD
		m_aPos[0].nEndL = m_csDoc.GetLength();
	}

	// Locate where to add element relative to current node
	int iPosParent, iPosBefore, nOffset = 0, nLength = 0;
	if ( bAddChild )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, nLength, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent || m_aPos[iPosParent].nStartR + 1 == m_aPos[iPosParent].nEndL )
		nOffset += 2;

	// Create element and modify positions of affected elements
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = nOffset;

	// Set links
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
		m_aPos[iPosBefore].iElemNext = iPos;
	}
	else
	{
		// First child
		m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
		m_aPos[iPosParent].iElemChild = iPos;
	}

	// Create string for insert
	CString csInsert;
	int nLenName = _tcslen(szName);
	int nLenValue = szValue? _tcslen(szValue) : 0;
	if ( ! nLenValue )
	{
		// <NAME/> empty element
		csInsert = _T("<");
		csInsert += szName;
		csInsert += _T("/>\r\n");
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 2;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR - 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + 1;
	}
	else
	{
		// <NAME>value</NAME>
		CString csValue = x_TextToDoc( szValue );
		nLenValue = csValue.GetLength();
		csInsert = _T("<");
		csInsert += szName;
		csInsert += _T(">");
		csInsert += csValue;
		csInsert += _T("</");
		csInsert += szName;
		csInsert += _T(">\r\n");
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 1;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + nLenValue + 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + nLenName + 2;
	}

	// Insert
	int nReplace = 0, nLeft = m_aPos[iPos].nStartL;
	if ( bEmptyParent )
	{
		CString csParentTagName = x_GetTagName(iPosParent);
		CString csFormat;
		csFormat = _T(">\r\n");
		csFormat += csInsert;
		csFormat += _T("</");
		csFormat += csParentTagName;
		csInsert = csFormat;
		nLeft = m_aPos[iPosParent].nStartR - 1;
		nReplace = 1;
		// x_Adjust is going to update all affected indexes by one amount
		// This will satisfy all except the empty parent
		// Here we pre-adjust for the empty parent
		// The empty tag slash is removed
		m_aPos[iPosParent].nStartR -= 1;
		// For the newly created end tag, see the following example:
		// <A/> (len 4) becomes <A><B/></A> (len 11)
		// In x_Adjust everything will be adjusted 11 - 4 = 7
		// But the nEndL of element A should only be adjusted 5
		m_aPos[iPosParent].nEndL -= (csParentTagName.GetLength() + 1);
	}
	else if ( m_aPos[iPosParent].nStartR + 1 == m_aPos[iPosParent].nEndL )
	{
		csInsert = _T("\r\n") + csInsert;
		nLeft = m_aPos[iPosParent].nStartR + 1;
	}
	x_DocChange( nLeft, nReplace, csInsert );
	x_Adjust( iPos, csInsert.GetLength() - nReplace );

	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

// plemmet
bool CMarkup::AddElemNV( LPCTSTR szName, long szData) 
{
	CString tmp;
	tmp.Format( _T( "%u"),szData);
	return AddElemNV( szName, tmp );
}
