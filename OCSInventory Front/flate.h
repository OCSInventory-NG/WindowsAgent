//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

#if !defined(_FLATE_H_INCLUDED_)
#define  _FLATE_H_INCLUDED_

#pragma once

class Flate
{
public:
	Flate()
	{
	}

	void Inflate(ISequentialStream* pOutput, ISequentialStream* pInput);
	void Deflate(ISequentialStream* pOutput, ISequentialStream* pInput);

	void Inflate(CFile* pOutput, CFile* pInput);
	void Deflate(CFile* pOutput, CFile* pInput);

	BOOL	Compress(CByteArray& dst, const CByteArray& src)
	{
		return Compress(dst, src.GetData(), (UINT) src.GetSize());
	}

	BOOL	Compress(CByteArray& dst, const BYTE* src, UINT srcLen);

	BOOL	Uncompress(CByteArray& dst, const CByteArray& src)
	{
		return Uncompress(dst, src.GetData(), (UINT) src.GetSize());
	}

	BOOL	Uncompress(CByteArray& dst, const BYTE* src, UINT srcLen);
};

#endif // _FLATE_H_INCLUDED_

