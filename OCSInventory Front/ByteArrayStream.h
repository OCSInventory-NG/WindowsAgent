//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#ifndef _BYTEARRAYSTREAM_H_INCLUDED_
#define _BYTEARRAYSTREAM_H_INCLUDED_

#pragma once

class CBAStreamReader : public ISequentialStream
{
public:
	CBAStreamReader(const BYTE* pData, UINT nSize)
		: m_pData(pData), m_Size(nSize), m_Pos(0) {}
	CBAStreamReader(CBAStreamReader& bsr)
		: m_pData(bsr.m_pData), m_Size(bsr.m_Size), m_Pos(bsr.m_Pos) {}
	~CBAStreamReader()
	{}

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void ** ppvObject )
	{
		if ( IsEqualIID(iid, IID_ISequentialStream) )
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }

	HRESULT STDMETHODCALLTYPE Read(void * pv, ULONG cb, ULONG * pcbRead )
	{
		UINT nDiff = m_Size - m_Pos;
		UINT nMin = min(nDiff, cb);
		memcpy(pv, m_pData + m_Pos, nMin);
		m_Pos += nMin;
		if ( pcbRead )
		{
			*pcbRead = nMin;
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}

	HRESULT STDMETHODCALLTYPE Write(const void * pv, ULONG cb, ULONG * pcbWritten )
	{
		UNREFERENCED_PARAMETER(pv);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(pcbWritten);
		return E_ACCESSDENIED;
	}

protected:
	const BYTE*	m_pData;
	UINT		m_Size;
	UINT		m_Pos;
};

class CBAStreamWriter : public ISequentialStream
{
public:
	CBAStreamWriter(CByteArray& ba)
		: m_ba(ba), m_Pos(0) {}
	CBAStreamWriter(CBAStreamWriter& bas)
		: m_ba(bas.m_ba), m_Pos(bas.m_Pos) {}
	~CBAStreamWriter()
	{}

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void ** ppvObject )
	{
		if ( IsEqualIID(iid, IID_ISequentialStream) )
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }

	HRESULT STDMETHODCALLTYPE Read(void * pv, ULONG cb, ULONG * pcbRead )
	{
		UNREFERENCED_PARAMETER(pv);
		UNREFERENCED_PARAMETER(cb);
		UNREFERENCED_PARAMETER(pcbRead);
		return E_ACCESSDENIED;
	; }

	HRESULT STDMETHODCALLTYPE Write(const void * pv, ULONG cb, ULONG * pcbWritten )
	{
		UINT nDiff = m_ba.GetSize() - m_Pos;
		if ( nDiff < cb )
		{
			m_ba.SetSize( m_ba.GetSize() + cb - nDiff );
		}

		memcpy(m_ba.GetData() + m_Pos, pv, cb);
		m_Pos += cb;

		*pcbWritten = cb;

		return S_OK;
	}

protected:
	CByteArray&	m_ba;
	UINT		m_Pos;
};

#endif // _BYTEARRAYSTRAM_H_INCLUDED_
