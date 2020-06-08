//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// Singleton.h: interface for the CSingleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__C88DCA63_0593_470E_9348_FB599A13BA05__INCLUDED_)
#define AFX_SINGLETON_H__C88DCA63_0593_470E_9348_FB599A13BA05__INCLUDED_

#pragma once

template <typename T> 
class CSingleton
{
protected:
  // Constructeur
	CSingleton () {}

public:
  
  static T *getInstance()
{
	if (NULL == m_singleton)
		{
			m_singleton = new T();
		}
	return (static_cast<T*> (m_singleton));
}

  ~CSingleton () {}

private:
  // Unique instance
  static T *m_singleton;
};

template <typename T> 
T *CSingleton<T>::m_singleton = NULL;

#endif // !defined(AFX_SINGLETON_H__C88DCA63_0593_470E_9348_FB599A13BA05__INCLUDED_)
