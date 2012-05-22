/*
 * File:   sarr.h
 * Author: Gabriel de Oliveira
 *
 * Created on 17 de junio de 2011, 03:22
 */

#ifndef SARR_H
#define	SARR_H

#include "smart.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Array
template <class T>
class SArr: public Smart<T> {
public:

  // Constructor
  explicit SArr(T * const ptr = NULL): Smart<T>(ptr) {}

  // Constructor de copia
  SArr(const SArr<T> &obj): Smart<T>(obj) {}

  // Destructor
  ~SArr();

  // Operador de asignación
  const SArr<T> &operator=(SArr<T> obj);

  // Métodos setter
  void Raw(T * const ptr) {
    operator=(SArr<T>(ptr));
  }

  // Operadores de comparación
  const bool operator==(const SArr<T> &obj) const {
    return Smart<T>::operator==(obj);
  }
  const bool operator!=(const SArr<T> &obj) const {
    return !operator==(obj);
  }

  // Método Swap
  void Swap(SArr<T> &obj) {
    Smart<T>::Swap(obj);
  }
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Array

// Destructor
template <class T>
SArr<T>::~SArr() {
  if(Smart<T>::Ref() == 1) {
    delete[] Smart<T>::Raw();
  }
}

// Operador de asignación
template <class T>
const SArr<T> &SArr<T>::operator=(SArr<T> obj) {
  Swap(obj);
  return *this;
}

#endif	/* SARR_H */
