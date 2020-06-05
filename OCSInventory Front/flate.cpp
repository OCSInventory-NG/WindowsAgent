//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Flate.cpp : implementation file
//

#include "stdafx.h"
#include "zlib.h"
#include "flate.h"
#include "ByteArrayStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	COMP_BUFSIZE		0x1000
#define	UNCOMP_BUFSIZE		0x8000

void Flate::Deflate(ISequentialStream* pOutput, ISequentialStream* pInput)
{
	z_stream zstm;
	memset(&zstm,0,sizeof(z_stream));

	BYTE zBufIn[UNCOMP_BUFSIZE];
	BYTE zBufOut[COMP_BUFSIZE];

	deflateInit(&zstm, Z_DEFAULT_COMPRESSION);

	int err = Z_OK;
	while ( TRUE )
	{
		ULONG cbRead = 0;
		HRESULT hResult = pInput->Read(zBufIn, sizeof(zBufIn), &cbRead);
		if ( hResult != S_OK || cbRead == 0 )
			break;

		zstm.next_in = (Bytef*)zBufIn;
		zstm.avail_in = (uInt)cbRead;

		while ( TRUE )
		{
			zstm.next_out = (Bytef*)zBufOut;
			zstm.avail_out = sizeof(zBufOut);

			err = deflate(&zstm, Z_SYNC_FLUSH);
			if (err != Z_OK)
				break;

			ULONG cbWrite = sizeof(zBufOut) - zstm.avail_out;
			hResult = pOutput->Write(zBufOut, cbWrite, &cbWrite );
			if ( hResult != S_OK || cbWrite == 0 )
				break;

			if ( zstm.avail_out != 0 )
				break;
		}
	}

	err = deflateEnd(&zstm);
}

void Flate::Inflate(ISequentialStream* pOutput, ISequentialStream* pInput)
{
	z_stream zstm;
	memset(&zstm,0,sizeof(z_stream));

	BYTE zBufIn[COMP_BUFSIZE];
	BYTE zBufOut[UNCOMP_BUFSIZE];

	inflateInit(&zstm);

	int err = Z_OK;
	while ( TRUE )
	{
		ULONG cbRead = 0;
		HRESULT hResult = pInput->Read(zBufIn, sizeof(zBufIn), &cbRead);
		if ( hResult != S_OK || cbRead == 0 )
			break;

		zstm.next_in = (Bytef*)zBufIn;
		zstm.avail_in = (uInt)cbRead;

		while ( TRUE )
		{
			zstm.next_out = (Bytef*)zBufOut;
			zstm.avail_out = sizeof(zBufOut);

			err = inflate(&zstm, Z_SYNC_FLUSH);
			if (err != Z_OK && err != Z_STREAM_END)
				break;

			ULONG cbWrite = sizeof(zBufOut) - zstm.avail_out;
			hResult = pOutput->Write(zBufOut, cbWrite, &cbWrite );
			if ( hResult != S_OK || cbWrite == 0 )
				break;

			if ( zstm.avail_out != 0 )
				break;
		}
	}

	err = inflateEnd(&zstm);
}

void Flate::Deflate(CFile* pOutput, CFile* pInput)
{
	z_stream zstm;
	memset(&zstm,0,sizeof(z_stream));

	BYTE zBufIn[UNCOMP_BUFSIZE];
	BYTE zBufOut[COMP_BUFSIZE];

	deflateInit(&zstm, Z_DEFAULT_COMPRESSION);

	int err = Z_OK;
	while ( TRUE )
	{
		UINT cbRead = 0;
		cbRead = pInput->Read(zBufIn, sizeof(zBufIn));
		if ( cbRead == 0 )
			break;

		zstm.next_in = (Bytef*)zBufIn;
		zstm.avail_in = (uInt)cbRead;

		while ( TRUE )
		{
			zstm.next_out = (Bytef*)zBufOut;
			zstm.avail_out = sizeof(zBufOut);

			err = deflate(&zstm, Z_FINISH);
			if (err != Z_OK)
				break;

			UINT cbWrite = sizeof(zBufOut) - zstm.avail_out;
			if ( cbWrite == 0 )
				break;
			pOutput->Write(zBufOut, cbWrite);

			if ( zstm.avail_out != 0 )
				break;
		}
	}

	err = deflateEnd(&zstm);
}

void Flate::Inflate(CFile* pOutput, CFile* pInput)
{
	z_stream zstm;
	memset(&zstm,0,sizeof(z_stream));

	BYTE zBufIn[COMP_BUFSIZE];
	BYTE zBufOut[UNCOMP_BUFSIZE];

	inflateInit(&zstm);

	int err = Z_OK;
	while ( TRUE )
	{
		UINT cbRead = 0;
		cbRead = pInput->Read(zBufIn, sizeof(zBufIn));
		if ( cbRead == 0 )
			break;

		zstm.next_in = (Bytef*)zBufIn;
		zstm.avail_in = (uInt)cbRead;

		while ( TRUE )
		{
			zstm.next_out = (Bytef*)zBufOut;
			zstm.avail_out = sizeof(zBufOut);

			err = inflate(&zstm, Z_SYNC_FLUSH);
			if (err != Z_OK)
				break;

			UINT cbWrite = sizeof(zBufOut) - zstm.avail_out;
			if ( cbWrite == 0 )
				break;
			pOutput->Write(zBufOut, cbWrite);

			if ( zstm.avail_out != 0 )
				break;
		}
	}

	err = inflateEnd(&zstm);
}

BOOL Flate::Compress(CByteArray& dst, const BYTE* src, UINT srcLen)
{
	CBAStreamReader sr(src, srcLen);
	CBAStreamWriter sw(dst);

	Deflate(&sw, &sr);

	return dst.GetSize() != 0;
}

BOOL Flate::Uncompress(CByteArray& dst, const BYTE* src, UINT srcLen)
{
	CBAStreamReader sr(src, srcLen);
	CBAStreamWriter sw(dst);

	Inflate(&sw, &sr);

	return dst.GetSize() != 0;
}
