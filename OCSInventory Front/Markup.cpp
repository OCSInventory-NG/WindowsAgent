//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Markup.cpp: implementation of the CMarkup class.
//

#include "stdafx.h"
#include "afxconv.h"
#include "Markup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CMarkup::CMarkup()
{
	m_pDoc = new TiXmlDocument();
	TiXmlDeclaration *pDecl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	m_pDoc->LinkEndChild( pDecl );
	m_pCurrentNode = NULL;
}

CMarkup::~CMarkup()
{
	// Free all allocated node in the document
	freeXmlNode( m_pDoc);
}

void CMarkup::freeXmlNode( TiXmlNode *pNode)
{
	TiXmlNode *pChild,
		      *pParent;

	if (pNode == NULL)
		return;
	// Recursively delete sub nodes until there is no more child
	while (pChild = pNode->FirstChild())
		freeXmlNode( pChild);
	// No more child, delete current node
	if (pParent = pNode->Parent())
		// This node has a parent node
		pParent->RemoveChild( pNode);
	else
		// There is no more parent, delete the node itself
		delete pNode;
	pNode = NULL;
}

TiXmlElement *CMarkup::AddElem( LPCTSTR szName, LPCTSTR szValue)
{
	ASSERT( m_pDoc);
	ASSERT( szName);

	try
	{
		// Create XML element
		TiXmlElement *pXmlElement = new TiXmlElement( CT2A( szName, CP_UTF8));
		if (pXmlElement == NULL)
			return NULL;
		// Link it as a child of current working element
		if (m_pCurrentNode == NULL)
			// No current node, so use document as root node
			m_pDoc->LinkEndChild( pXmlElement);
		else
			m_pCurrentNode->LinkEndChild( pXmlElement);
		if ((szValue != NULL) && (_tcslen( szValue) != 0))
		{
			// There is text to set between <Element></Element>
			TiXmlText *pXmlText = new TiXmlText( CT2A( szValue, CP_UTF8));
			if (pXmlText == NULL)
			{
				delete pXmlElement;
				return NULL;
			}
			pXmlElement->LinkEndChild( pXmlText);
		}
		// Set new child element as working node
		m_pCurrentNode = pXmlElement;
		return pXmlElement;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

TiXmlElement* CMarkup::AddElem( LPCTSTR szName, LONG lValue)
{
	CString csValue;

	csValue.Format( _T( "%ld"), lValue);
	return AddElem( szName, csValue);
}

TiXmlElement *CMarkup::AddChildElem( LPCTSTR szName, LPCTSTR szValue)
{
	ASSERT( m_pDoc);
	ASSERT( szName);

	try
	{
		// Create XML element
		TiXmlElement *pXmlElement = new TiXmlElement( CT2A( szName, CP_UTF8));
		if (pXmlElement == NULL)
			return NULL;
		// Link it as a child of current working element
		if (m_pCurrentNode == NULL)
			// No current node, so use document as root node
			m_pDoc->LinkEndChild( pXmlElement);
		else
			m_pCurrentNode->LinkEndChild( pXmlElement);
		if ((szValue != NULL) && (_tcslen( szValue) != 0))
		{
			// There is text to set between <Element></Element>
			TiXmlText *pXmlText = new TiXmlText( CT2A( szValue, CP_UTF8));
			if (pXmlText == NULL)
			{
				delete pXmlElement;
				return NULL;
			}
			pXmlElement->LinkEndChild( pXmlText);
		}
		// Do not change current working node
		return pXmlElement;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

TiXmlElement *CMarkup::AddChildElem( LPCTSTR szName, LONG lValue)
{
	CString csValue;

	csValue.Format( _T( "%ld"), lValue);
	return AddChildElem( szName, csValue);
}

BOOL CMarkup::SetData( LPCTSTR szValue)
{
	ASSERT( m_pDoc);

	try
	{
		// First, remove all text child
		TiXmlNode *pChild =  NULL;
		while( pChild = m_pCurrentNode->IterateChildren( pChild))
		{
			if (pChild->Type() == TiXmlNode::TINYXML_TEXT)
			{
				// This a test child, remove it...
				m_pCurrentNode->RemoveChild( pChild);
				// ...and restart from the begining of childs
				pChild = NULL;
			}
		}
		if ((szValue != NULL) && (_tcslen( szValue) != 0))
		{
			// There is text to set between <Element></Element>
			TiXmlText *pXmlText = new TiXmlText( CT2A( szValue, CP_UTF8));
			if (pXmlText == NULL)
				return FALSE;
			m_pCurrentNode->LinkEndChild( pXmlText);
		}
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL CMarkup::SetData( LONG lValue)
{
	CString csValue;

	csValue.Format( _T( "%ld"), lValue);
	return SetData( csValue);
}

BOOL CMarkup::SetAttrib( LPCTSTR szName, LPCTSTR szValue)
{
	ASSERT( szName);
	ASSERT( szValue);
	if (m_pCurrentNode == NULL)
		return FALSE;

	try
	{
		m_pCurrentNode->SetAttribute( CT2A( szName, CP_UTF8), CT2A(  szValue, CP_UTF8));
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL CMarkup::SetAttrib( LPCTSTR szName, LONG lValue)
{
	CString csValue;

	csValue.Format( _T( "%ld"), lValue);
	return SetAttrib( szName, csValue);
}

TiXmlElement *CMarkup::IntoElem()
{
	ASSERT( m_pDoc);
	try
	{
		TiXmlElement *pXmlNode;
		if (m_pCurrentNode == NULL)
			pXmlNode = m_pDoc->FirstChildElement();
		else
			pXmlNode = m_pCurrentNode->FirstChildElement();
		if (pXmlNode)
		{
			// There is a child node, set it as working node
			m_pCurrentNode = pXmlNode;
			return pXmlNode;
		}
		// No child node
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

TiXmlElement *CMarkup::OutOfElem()
{
	if (m_pCurrentNode == NULL)
		return NULL;
	try
	{
		if (m_pCurrentNode->Parent() == m_pDoc)
		{
			// root node is document node
			m_pCurrentNode = NULL;
			return NULL;
		}
		TiXmlElement *pXmlNode = (TiXmlElement *)m_pCurrentNode->Parent();
		if (pXmlNode)
		{
			// There is a parent node
			m_pCurrentNode = pXmlNode;
			return pXmlNode;
		}
		// No parent node
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

BOOL CMarkup::RemoveChildElem( LPCTSTR szName, TiXmlElement *pXmlNode)
{
	if ((m_pCurrentNode == NULL) && (pXmlNode == NULL))
		return FALSE;

	try
	{
		TiXmlNode *pSearchNode;
		TiXmlElement *pXmlElement;

		if (pXmlNode != NULL)
			pSearchNode = pXmlNode;
		else
			pSearchNode = m_pCurrentNode;
		// Find first child element
		while (pXmlElement = pSearchNode->FirstChildElement( CT2A( szName, CP_UTF8)))
		{
			// One sibling child node found
			freeXmlNode( pXmlElement);
			// pSearchNode->RemoveChild( pXmlElement);
		}
		// No more child node
		return TRUE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}


TiXmlElement *CMarkup::FindFirstElem( LPCTSTR szName, TiXmlElement *pXmlNode)
{
	ASSERT( m_pDoc);
 	ASSERT( szName);

	try
	{
		// Initialize value of search node (doc or current node or provided node)
		TiXmlNode *pSearchNode;
		TiXmlElement *pXmlElement;
		if ((m_pCurrentNode == NULL) && (pXmlNode == NULL))
			pSearchNode = m_pDoc;
		else
		{
			if (pXmlNode != NULL)
				pSearchNode = pXmlNode;
			else
				pSearchNode = m_pCurrentNode;
		}
		// Find first child element
		pXmlElement = pSearchNode->FirstChildElement( CT2A( szName, CP_UTF8));
		if (pXmlElement)
		{
			// One sibling child node found
			m_pCurrentNode = pXmlElement;
			return pXmlElement;
		}
		// No sibling child found
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

TiXmlElement *CMarkup::FindNextElem( LPCTSTR szName, TiXmlElement *pXmlNode)
{
	ASSERT( m_pDoc);
 	ASSERT( szName);

	try
	{
		// Initialize value of search node (doc or current node or provided node)
		TiXmlNode *pSearchNode;
		TiXmlElement *pXmlElement;
		if ((m_pCurrentNode == NULL) && (pXmlNode == NULL))
			pSearchNode = m_pDoc;
		else
		{
			if (pXmlNode != NULL)
				pSearchNode = pXmlNode;
			else
				pSearchNode = m_pCurrentNode;
		}
		// We are continuing a previous search at same XML tree level
		pXmlElement = pSearchNode->NextSiblingElement( CT2A( szName, CP_UTF8));
		if (pXmlElement)
		{
			// Next sibling found
			m_pCurrentNode = pXmlElement;
			return pXmlElement;
		}
		// No more element found
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

TiXmlElement *CMarkup::ResetPos( TiXmlElement *pXmlNode)
{
	if (pXmlNode)
	{
		// Set provided node as new working node
		m_pCurrentNode = pXmlNode;
		return pXmlNode;
	}
	// Set current working node to document node
	m_pCurrentNode = NULL;
	return NULL;
}


LPCTSTR CMarkup::GetData( TiXmlElement *pXmlNode)
{
	static CString csResult;
 
	if ((pXmlNode == NULL) && (m_pCurrentNode == NULL))
		return NULL;

	try
	{
		TiXmlElement *pNode;

		if (pXmlNode == NULL)
			pNode = m_pCurrentNode;
		else
			pNode = pXmlNode;
		const char *szValue = pNode->GetText();
		if (szValue)
		{
			csResult.Format( _T( "%s"), CA2T( szValue, CP_UTF8));
			return csResult;
		}
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

LPCTSTR CMarkup::GetAttrib( LPCTSTR szAttrib)
{
	static CString csResult;

	if (m_pCurrentNode == NULL)
		return NULL;

	try
	{
		const char *szValue = m_pCurrentNode->Attribute( CT2A( szAttrib, CP_UTF8));
		if (szValue)
		{
			csResult.Format( _T( "%s"), CA2T( szValue, CP_UTF8));
			return csResult;
		}
		return NULL;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

LPCSTR CMarkup::GetDoc()
{
	try
	{
		TiXmlPrinter myPrinter;
		static CStringA csDoc;

		myPrinter.SetIndent( "    " );
		m_pDoc->Accept( &myPrinter );
		csDoc = myPrinter.CStr();
		return csDoc;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return NULL;
	}
}

// Get XML document in TinyXML object
TiXmlDocument *CMarkup::GetTiXmlDocument()
{
	return m_pDoc;
}

// Set XML document from text string
BOOL CMarkup::SetDoc( LPCSTR szDoc)
{
	try
	{
		TiXmlDocument *pDoc = new TiXmlDocument();

		if (pDoc)
		{
			pDoc->Parse( szDoc);
			if (m_pDoc)
				freeXmlNode( m_pDoc);
			m_pDoc = pDoc;
			m_pCurrentNode = NULL;
			return TRUE;
		}
		return FALSE;
	}
	catch (CException *pEx)
	{
		pEx->Delete();
		return FALSE;
	}
}

BOOL CMarkup::SetDoc( LPCWSTR szDoc)
{
	// Try to determine if UTF-8 encoding used
	CStringW csDoc = szDoc;
	csDoc.MakeLower();
	if (csDoc.Find( L"encoding=\"utf-8\"") > 0)
		// XML Uses UTF-8 encoding
		return SetDoc( CT2A( szDoc));
	// XML is not UTF-8 encoded => encoded it to UTF-8
	return SetDoc( CT2A( szDoc, CP_UTF8));
}

// Set XML document from TinyXML object
BOOL CMarkup::SetTinyXmlDocument( TiXmlDocument *pDoc)
{
	if (pDoc == NULL)
		return FALSE;
	if (m_pDoc)
		freeXmlNode( m_pDoc);
	m_pDoc = pDoc;
	m_pCurrentNode = NULL;
	return TRUE;
}

BOOL CMarkup::AddXml( CMarkup *pSource)
{
	TiXmlHandle hdl( pSource->GetTiXmlDocument());

	TiXmlElement *pElem;
	TiXmlElement *pChild;

	CString csSection, csProperty, csValue;

	for (pElem = hdl.FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
	{
		// Add section to destination document
		csSection.Format( _T( "%s"), CA2CT( pElem->Value()));
		if (!AddElem( csSection))
			return FALSE;
		for (pChild = pElem->FirstChildElement(); pChild; pChild = pChild->NextSiblingElement())
		{
			// Add child elem to destination document
			csProperty.Format( _T( "%s"), CA2CT( pChild->Value(), CP_UTF8));
			csValue.Format( _T( "%s"), CA2CT( pChild->GetText(), CP_UTF8));
			if (!AddChildElem( csProperty, csValue))
				return FALSE;
		}
		OutOfElem(); // Out of section
	}
	return TRUE;
}

// Load XML document from file
BOOL CMarkup::LoadFile( LPCTSTR lpstrFile)
{
	if (m_pDoc->LoadFile( CT2A( lpstrFile)))
	{
		ResetPos();
		return TRUE;
	}
	// Error, set XML document empty
	freeXmlNode( m_pDoc);
	m_pDoc = new TiXmlDocument();
	TiXmlDeclaration *pDecl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	m_pDoc->LinkEndChild( pDecl );
	m_pCurrentNode = NULL;
	return FALSE;
}

// Save XML document to file
BOOL CMarkup::SaveFile( LPCTSTR lpstrFile)
{
	if (m_pDoc->SaveFile( CT2A( lpstrFile)))
		return TRUE;
	return FALSE;
}

