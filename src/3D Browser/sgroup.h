/*
 * File:   sgroup.h
 * Author: Gabriel de Oliveira
 *
 * Created on 28 de junio de 2011, 11:09
 */

#ifndef SGROUP_H
#define	SGROUP_H

#include "point.h"
#include "sptr.h"
#include "sarr.h"

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Group
template <class T>
class SGroup {
public:

  // Constructores
  SGroup(): _elem(NULL), _elements(0) {}
  SGroup(const SArr<SPtr<T> > &elem, const UINT &elements): _elem(elem),
    _elements(elements) {}

  // Constructor de copia
  SGroup(const SGroup<T> &obj): _elem(obj._elem), _elements(obj._elements) {}

  // Destructor
  virtual ~SGroup() {}

  // Operador de asignaci�n
  const SGroup<T> &operator=(const SGroup<T> &obj);

  // Miembros getter
  const SArr<SPtr<T> > &Raw() const {
    return _elem;
  }
  SPtr<T> &operator[](const UINT &index) {
    return _elem[index];
  }
  const SPtr<T> &operator[](const UINT &index) const {
    return _elem[index];
  }
  const UINT &Elements() const {
    return _elements;
  }

  // Miembros setter
  void Set(const SArr<SPtr<T> > &elem, const UINT &elements) {
    operator=(SGroup<T>(elem, elements));
  }

  // M�todo Swap
  void Swap(SGroup<T> &obj);

private:

  // Miembros
  SArr<SPtr<T> > _elem;
  UINT _elements;
};

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Smart Group

// Operador de asignaci�n
template <class T>
const SGroup<T> &SGroup<T>::operator=(const SGroup<T> &obj) {
  _elem = obj._elem;
  _elements = obj._elements;
  return *this;
}

// M�todo Swap
template <class T>
void SGroup<T>::Swap(SGroup<T> &obj) {
  _elem.Swap(obj._elem);

  const UINT t_elements = obj._elements;

  obj._elements = _elements;

  _elements = t_elements;
}

#endif	/* SGROUP_H */

