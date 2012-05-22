/*
 * File:   smart.h
 * Author: Gabriel de Oliveira
 *
 * Created on 17 de junio de 2011, 02:19
 */

#ifndef SMART_H
#define	SMART_H

#include "base.h"

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada abstracta Smart
template <class T>
class Smart {
public:

  // Destructor
  virtual ~Smart() = 0;

  // M�todos getter
  T * const Raw() const {
    return _ptr;
  }
  T * const operator->() const {
    return Raw();
  }
  T &operator*() const {
    return *Raw();
  }
  T &operator[](const UINT &index) const {
    return Raw()[index];
  }
  const UINT &Ref() const {
    return *_ref;
  }

  // M�todo IsNull
  const bool IsNull() const {
    return (_ptr == NULL)? true : false;
  }

  // Operadores de comparaci�n
  virtual const bool operator==(const T * const ptr) const {
    return IsEqual(ptr);
  }
  virtual const bool operator!=(const T * const ptr) const {
    return !IsEqual(ptr);
  }

protected:

  // Constructor
  explicit Smart(T * const ptr);

  // Constructor de copia
  Smart(const Smart<T> &obj): _ptr(obj._ptr), _ref(obj._ref) {
    ++(*_ref);
  }

  // M�todos setter
  void Raw(T * const ptr) {
    _ptr = ptr;
  }

  // Operadores de comparaci�n
  const bool operator==(const Smart<T> &obj) const {
    return (_ref == obj._ref)? true : false;
  }
  const bool operator!=(const Smart<T> &obj) const {
    return !operator==(obj);
  }

  // M�todo Swap
  void Swap(Smart<T> &obj);

private:

  // Operador de asignaci�n
  const Smart<T> &operator=(const Smart<T> &obj);

  // M�todo IsEqual
  const bool IsEqual(const T * const ptr) const {
    return (_ptr == ptr)? true : false;
  }

  // Miembros
  T *_ptr;
  P_UINT _ref;
};

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada abstracta Smart

// Destructor
template <class T>
Smart<T>::~Smart() {
  if(--(*_ref) == 0) {
    delete _ref;
  }
}

// Constructor
template <class T>
Smart<T>::Smart(T * const ptr): _ptr(ptr), _ref(NULL) {
  _ref = new UINT; // TODO: Comprobar excepciones.

  *_ref = 1;
}

// M�todo Swap
template <class T>
void Smart<T>::Swap(Smart<T> &obj) {
  T * const t_ptr = obj._ptr;
  const P_UINT t_ref = obj._ref;

  obj._ptr = _ptr;
  obj._ref = _ref;

  _ptr = t_ptr;
  _ref = t_ref;
}

#endif	/* SMART_H */
