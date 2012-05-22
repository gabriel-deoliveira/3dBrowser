/*
 * File:   pnormal.h
 * Author: Gabriel de Oliveira
 *
 * Created on 5 de octubre de 2011, 17:27
 */

#ifndef PNORMAL_H
#define	PNORMAL_H

#include "point.h"
#include "triangle.h"
#include "sptr.h"
#include "list.h"

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada PNormal
template <class T>
class PNormal {
public:

  // Constructor
  PNormal(): _validNormal(false) {}

  // Constructor de copia
  PNormal(const PNormal<T> &obj): _normal(obj._normal), _triList(obj._triList),
    _validNormal(obj._validNormal) {}

  // Destructor
  ~PNormal() {}

  // Operador de asignaci�n
  const PNormal<T> &operator=(const PNormal<T> &obj);

  // M�todos getter
  const Point<T> &Normal() const;

  // M�todo Add
  void Add(const SPtr<Triangle<T> > &tri);

  // M�todo Destroy
  void Destroy();

  // M�todo Validate
  void Validate() const;

  // M�todo Invalidate
  void Invalidate() const {
    _validNormal = false;
  }

private:

  mutable Point<T> _normal;
  List<Triangle<T> > _triList;
  mutable bool _validNormal;
};

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada PNormal

// Operador de asignaci�n
template <class T>
const PNormal<T> &PNormal<T>::operator=(const PNormal<T> &obj) {
  _normal = obj._normal;
  _triList = obj._triList;
  _validNormal = obj._validNormal;
  return *this;
}

// M�todos getter
template <class T>
const Point<T> &PNormal<T>::Normal() const {
  Validate();
  return _normal;
}

// M�todo Add
template <class T>
void PNormal<T>::Add(const SPtr<Triangle<T> > &tri) {
  _triList.Add(tri);
  _validNormal = false;
}

// M�todo Destroy
template <class T>
void PNormal<T>::Destroy() {
  _triList.Destroy();
  _validNormal = false;
}

// M�todo Validate
template <class T>
void PNormal<T>::Validate() const {
  if(!_validNormal) {
    _normal = Point<T>(0, 0, 0);

    if(!_triList.IsEmpty()) {
      for(SPtr<Triangle<T> > tri = _triList.First(); _triList.HasNext();
          tri = _triList.Next()) {
        _normal += tri -> Normal();
      }
      // TODO: �Necesito convertir la normal en versor?
      // _normal = _normal.Versor();
    }
  }
}

#endif	/* PNORMAL_H */
