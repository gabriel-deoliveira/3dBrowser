/*
 * File:   triangle.h
 * Author: Gabriel de Oliveira
 *
 * Created on 27 de junio de 2011, 20:17
 */

#ifndef TRIANGLE_H
#define	TRIANGLE_H

#include "sptr.h"
#include "point.h"

template <class T>
class Point;

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Triangle
template <class T>
class Triangle {
public:

  // Constructores
  Triangle(): _p1(SPtr<Point<T> >(NULL)), _p2(_p1), _p3(_p1),
    _validCenter(false), _validNormal(false), _color(NULL), _reflection(NULL) {}
  Triangle(const SPtr<Point<T> > &p1, const SPtr<Point<T> > &p2,
           const SPtr<Point<T> > &p3): _p1(p1), _p2(p2), _p3(p3),
    _validCenter(false), _validNormal(false), _color(NULL), _reflection(NULL) {}

  // Constructor de copia
  Triangle(const Triangle<T> &obj): _p1(obj._p1), _p2(obj._p2), _p3(obj._p3),
    _validCenter(obj._validCenter), _center(obj._center),
    _validNormal(obj._validNormal), _normal(obj._normal), _color(obj._color),
    _reflection(obj._reflection) {}

  // Destructor
  virtual ~Triangle() {}

  // Operador de asignación
  const Triangle<T> &operator=(const Triangle<T> &obj);

  // Métodos getter
  const SPtr<Point<T> > &P1() const {
    return _p1;
  }
  const SPtr<Point<T> > &P2() const {
    return _p2;
  }
  const SPtr<Point<T> > &P3() const {
    return _p3;
  }
  const bool IsCenterValid() const {
    return _validCenter;
  }
  const bool IsNormalValid() const {
    return _validNormal;
  }
  const bool IsValid() const {
    return _validCenter && _validNormal;
  }
  const Point<T> &Normal() const;
  const Point<T> &Center() const;
  const COLORREF &Color() const {
    return *_color;
  }
  const FLOAT &Reflection() const {
    return *_reflection;
  }

  // Métodos setter
  virtual void P1(const SPtr<Point<T> > &p1) {
    _p1 = p1;
  }
  virtual void P2(const SPtr<Point<T> > &p2) {
    _p2 = p2;
  }
  virtual void P3(const SPtr<Point<T> > &p3) {
    _p3 = p3;
  }
  virtual void ValidateCenter() const;
  virtual void InvalidateCenter() {
    _validCenter = false;
  }
  virtual void ValidateNormal() const;
  virtual void InvalidateNormal() {
    _validNormal = false;
  }
  virtual void Invalidate() {
    _validCenter = _validNormal = false;
  }
  virtual void Color(const SPtr<COLORREF> &color) {
    _color = color;
  }
  virtual void Reflection(const SPtr<FLOAT> &reflection) {
    _reflection = reflection;
  }

  // Operadores de comparación
  const bool operator==(const Triangle<T> &obj) const {
    return (Center().Z() == obj.Center().Z())? true : false;
  }
  const bool operator<(const Triangle<T> &obj) const {
    return (Center().Z() < obj.Center().Z())? true : false;
  }
  const bool operator>(const Triangle<T> &obj) const {
    return (Center().Z() > obj.Center().Z())? true : false;
  }
  const bool operator!=(const Triangle<T> &obj) const {
    return (Center().Z() != obj.Center().Z())? true : false;
  }
  const bool operator<=(const Triangle<T> &obj) const {
    return (Center().Z() <= obj.Center().Z())? true : false;
  }
  const bool operator>=(const Triangle<T> &obj) const {
    return (Center().Z() >= obj.Center().Z())? true : false;
  }

  // Método swap
  void Swap(Triangle<T> &obj);

private:

  // Miembros
  SPtr<Point<T> > _p1;
  SPtr<Point<T> > _p2;
  SPtr<Point<T> > _p3;

  mutable bool _validCenter;
  mutable Point<T> _center;
  mutable bool _validNormal;
  mutable Point<T> _normal;

  SPtr<COLORREF> _color;
  SPtr<FLOAT> _reflection;
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada triangle

// Operador de asignación
template <class T>
const Triangle<T> &Triangle<T>::operator=(const Triangle<T> &obj) {
  _p1 = obj._p1;
  _p2 = obj._p2;
  _p3 = obj._p3;
  _validCenter = obj._validCenter;
  _center = obj._center;
  _validNormal = obj._validNormal;
  _normal = obj._normal;
  _color = obj._color;
  _reflection = obj._reflection;
  return *this;
}

// Métodos getter
template <class T>
const Point<T> &Triangle<T>::Center() const {
  ValidateCenter();
  return _center;
}
template <class T>
const Point<T> &Triangle<T>::Normal() const {
  ValidateNormal();
  return _normal;
}

// Método ValidateCenter
template <class T>
void Triangle<T>::ValidateCenter() const {
  if(!_validCenter) {
    _center = Point<T>((_p1 -> X() + _p2 -> X() + _p3 -> X()) / 3,
                       (_p1 -> Y() + _p2 -> Y() + _p3 -> Y()) / 3,
                       (_p1 -> Z() + _p2 -> Z() + _p3 -> Z()) / 3);

    _validCenter = true;
  }
}

// Método ValidateData
template <class T>
void Triangle<T>::ValidateNormal() const {
  if(!_validNormal) {
    _normal = (*_p2 - *_p1) * (*_p3 - *_p1);

    //TODO: ¿Necesito convertir la normal en versor?
    //_normal = _normal.Versor();

    _validNormal = true;
  }
}

// Método Swap
template <class T>
void Triangle<T>::Swap(Triangle<T> &obj) {
  _p1.Swap(obj._p1);
  _p2.Swap(obj._p2);
  _p3.Swap(obj._p3);
  _center.Swap(obj._center);
  _normal.Swap(obj._normal);
  _color.Swap(obj._color);
  _reflection.Swap(obj._reflection);

  const bool t_validCenter = obj._validCenter;
  const bool t_validNormal = obj._validNormal;

  obj._validCenter = _validCenter;
  obj._validNormal = _validNormal;

  _validCenter = t_validCenter;
  _validNormal = t_validNormal;
}

#endif	/* TRIANGLE_H */
