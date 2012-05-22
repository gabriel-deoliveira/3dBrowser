/*
 * File:   sptr.h
 * Author: Gabriel de Oliveira
 *
 * Created on 17 de junio de 2011, 02:45
 */

#ifndef SPTR_H
#define	SPTR_H

#include "smart.h"

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Pointer
template <class T>
class SPtr: public Smart<T> {
public:

  // Constructor
  explicit SPtr(T * const ptr = NULL): Smart<T>(ptr) {}

  // Constructor de copia
  SPtr(const SPtr<T> &obj): Smart<T>(obj) {}

  // Destructor
  ~SPtr();

  // Operador de asignaci�n
  const SPtr<T> &operator=(SPtr<T> obj);

  // M�todos setter
  void Raw(T * const ptr) {
    operator=(SPtr<T>(ptr));
  }

  // Operadores de comparaci�n
  const bool operator==(const SPtr<T> &obj) const {
    return Smart<T>::operator==(obj);
  }
  const bool operator!=(const SPtr<T> &obj) const {
    return !operator==(obj);
  }

  // M�todo Swap
  void Swap(SPtr<T> &obj) {
    Smart<T>::Swap(obj);
  }
};

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Pointer

// Destructor
template <class T>
SPtr<T>::~SPtr() {
  if(Smart<T>::Ref() == 1) {
    delete Smart<T>::Raw();
  }
}

// Operador de asignaci�n
template <class T>
const SPtr<T> &SPtr<T>::operator=(SPtr<T> obj) {
  Swap(obj);
  return *this;
}

#endif	/* SPTR_H */
