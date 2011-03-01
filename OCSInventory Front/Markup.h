// Markup.h: interface for the CMarkup class.
//

#if !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
#define AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_

#pragma once

#include "OCSInventory Front Classes.h"
#include "tinyxml.h"
#include "tinystr.h"

class OCSINVENTORYFRONT_API CMarkup  
{
	/* This class encapulate TinyXML lib to have an easy way manage OCS XML doc
	   We use 2 pointers to navigate through XML document:
	   - Current node, the main XML node of the document we are dealing with
	   - Child node, a child node of main node we are managing
	   */
public:
	CMarkup();
	virtual ~CMarkup();

	// Load XML document from file
	BOOL LoadFile( LPCTSTR lpstrFile);
	// Save XML document to file
	BOOL SaveFile( LPCTSTR lpstrFile);

	// Get XML document in UTF-8 text format
	LPCSTR GetDoc();
	// Get XML document in TinyXML object
	TiXmlDocument *GetTiXmlDocument();
	// Set XML document from text string (may be UTF-8 or not)
	BOOL SetDoc( LPCSTR szDoc);
	BOOL SetDoc( LPCWSTR szDoc);
	// Set XML document from TinyXML object
	BOOL SetTinyXmlDocument( TiXmlDocument *pDoc);
	// Add XML into current working node
	BOOL AddXml( CMarkup *pSource);

	// Add an XML node as child of current working XML node, and set it as new working node
	TiXmlElement *AddElem( LPCTSTR szName, LPCTSTR szValue=NULL);
	TiXmlElement *AddElem( LPCTSTR szName, LONG lValue);
	// Add an XML node as child of current working XML node, leave current working node as is
	TiXmlElement *AddChildElem( LPCTSTR szName, LPCTSTR szValue=NULL);
	TiXmlElement *AddChildElem( LPCTSTR szName, LONG lValue);
	// Set/Add an attribute value to current working XML node
	BOOL SetAttrib( LPCTSTR szName, LPCTSTR szValue);
	BOOL SetAttrib( LPCTSTR szName, LONG lValue);
	BOOL AddAttrib( LPCTSTR szName, LPCTSTR szValue) {return SetAttrib( szName, szValue);};
	BOOL AddAttrib( LPCTSTR szName, LONG lValue) {return SetAttrib( szName, lValue);};
	// Set first child node as new working XML node
	TiXmlElement *IntoElem();
	// Set parent XML node as new working node
	TiXmlElement *OutOfElem();
	// Remove all childs named szName of provided node (current working XML node if null)
	BOOL RemoveChildElem( LPCTSTR szName, TiXmlElement *pXmlNode = NULL); 
	// Start search on child nodes of provided node (current working XML node if null), 
	// and set first node found as new working node
	TiXmlElement *FindFirstElem( LPCTSTR szName, TiXmlElement *pXmlNode = NULL);
	// Continue search from provided node (current working node if null)
	TiXmlElement *FindNextElem( LPCTSTR szName, TiXmlElement *pXmlNode = NULL);
	// Set position of current working XML node (if null, set it to document node)
	TiXmlElement *ResetPos( TiXmlElement *pXmlNode = NULL);
	// Get Text of XML Node (use current working XML node if null)
	LPCTSTR GetData( TiXmlElement *pXmlNode = NULL);
	// Get attribute value of current working XML node
	LPCTSTR GetAttrib( LPCTSTR szName);

protected:
	TiXmlDocument *m_pDoc;			// XML document
	TiXmlElement  *m_pCurrentNode;	// XML node we are currently working with
};

#endif // !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
