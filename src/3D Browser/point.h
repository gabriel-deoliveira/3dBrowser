/*
 * File:   point.h
 * Author: Gabriel de Oliveira
 *
 * Created on 17 de junio de 2011, 00:54
 */

#ifndef POINT_H
#define	POINT_H

#include <math.h>

#include "triangle.h"
#include "sptr.h"
#include "pnormal.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Point
template <class T>
class Point {
public:

  // Constructores
  Point() {}
  Point(const T &x, const T &y, const T &z): _x(x), _y(y), _z(z),
    _normal(SPtr<PNormal<T> >(NULL)) {}

  // Constructor de copia
  Point(const Point<T> &obj): _x(obj._x), _y(obj._y), _z(obj._z),
    _normal(obj._normal) {}

  // Destructor
  virtual ~Point() {}

  // Operador de asignación
  const Point<T> &operator=(const Point<T> &obj);

  // Miembros getter
  const T &X() const {
    return _x;
  }
  const T &Y() const {
    return _y;
  }
  const T &Z() const {
    return _z;
  }
  const Point<T> &Normal() const {
    return _normal -> Normal();
  }

  // Miembros setter
  virtual void X(const T &x) {
    _x = x;
  }
  virtual void Y(const T &y) {
    _y = y;
  }
  virtual void Z(const T &z) {
    _z = z;
  }
  virtual void Normal(const SPtr<PNormal<T> > &normal) {
    _normal = normal;
  }

  // Mëtodo Add
  void Add(const SPtr<Triangle<T> > &tri) const {
    _normal -> Add(tri);
  }

  // Método Unlink
  void Unlink() const {
    _normal -> Destroy();
  }

  // Método InvalidateNormal
  void InvalidateNormal() const {
    _normal -> Invalidate();
  }

  // Miembro Dot (multiplicación escalar)
  const T Dot(const Point<T> &obj) const {
    return _x * obj._x + _y * obj._y + _z * obj._z;
  }

  // Miembro Module
  const T Module() const {
    return sqrt(_x * _x + _y * _y + _z * _z);
  }

  // Miembro Versor
  const Point<T> Versor() const {
    return *this / Module();
  }

  // Operadores aritméticos
  const Point<T> operator+(const Point<T> &obj) const {
    return Point<T>(_x + obj._x, _y + obj._y, _z + obj._z);
  }
  const Point<T> operator-(const Point<T> &obj) const {
    return Point<T>(_x - obj._x, _y - obj._y, _z - obj._z);
  }
  const Point<T> operator*(const Point<T> &obj) const {
    return Point<T>(_y * obj._z - _z * obj._y,
                    _z * obj._x - _x * obj._z,
                    _x * obj._y - _y * obj._x);
  }

  const Point<T> operator*(const T &num) const {
    return Point<T>(_x * num, _y * num, _z * num);
  }
  const Point<T> operator/(const T &num) const {
    return Point<T>(_x / num, _y / num, _z / num);
  }

  // Operadores de asignación adicionales
  const Point<T> &operator+=(const Point<T> &obj);
  const Point<T> &operator-=(const Point<T> &obj);
  const Point<T> &operator*=(const Point<T> &obj);

  const Point<T> &operator*=(const T &num);
  const Point<T> &operator/=(const T &num);

  // Operadores de comparación
  const bool operator==(const Point<T> &obj) const {
    return (_x == obj._x && _y == obj._y && _z == obj._z)? true : false;
  }
  const bool operator!=(const Point<T> &obj) const {
    return !operator==(obj);
  }

  // Método Swap
  void Swap(Point<T> &obj);

private:

  // Miembros
  T _x;
  T _y;
  T _z;

  SPtr<PNormal<T> > _normal;
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Point

// Operador de asignación
template <class T>
const Point<T> &Point<T>::operator=(const Point<T> &obj) {
  _x = obj._x;
  _y = obj._y;
  _z = obj._z;
  _normal = obj._normal;
  return *this;
}

// Operadores de asignación adicionales
template <class T>
const Point<T> &Point<T>::operator+=(const Point<T> &obj) {
  _x += obj._x;
  _y += obj._y;
  _z += obj._z;
  return *this;
}

template <class T>
const Point<T> &Point<T>::operator-=(const Point<T> &obj) {
  _x -= obj._x;
  _y -= obj._y;
  _z -= obj._z;
  return *this;
}

template <class T>
const Point<T> &Point<T>::operator*=(const Point<T> &obj) {
  *this = *this * obj;
  return *this;
}

template <class T>
const Point<T> &Point<T>::operator*=(const T &num) {
  _x *= num;
  _y *= num;
  _z *= num;
  return *this;
}

template <class T>
const Point<T> &Point<T>::operator/=(const T &num) {
  _x /= num;
  _y /= num;
  _z /= num;
  return *this;
}

// Método Swap
template <class T>
void Point<T>::Swap(Point<T> &obj) {
  _normal.Swap(obj._normal);

  const T t_x = obj._x;
  const T t_y = obj._y;
  const T t_z = obj._z;

  obj._x = _x;
  obj._y = _y;
  obj._z = _z;

  _x = t_x;
  _y = t_y;
  _z = t_z;
}

#endif	/* POINT_H */
