/*
 * File:   map.h
 * Author: Gabriel de Oliveira
 *
 * Created on 10 de septiembre de 2011, 02:32
 */

#ifndef MAP_H
#define	MAP_H

#include <windows.h>

#include "object.h"
#include "tmatrix.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Map
template <class T>
class Map {
public:

  // Constructor
  Map(): _wStartX(0), _wStartY(0), _wWidth(1), _wHeight(1), _extStartX(0),
    _extStartY(0), _extWidth(1), _extHeight(1), _sWidth(1), _sHeight(1),
    _dif(0) {}

  // Constructor
  Map(const Object<T> &world, const RECT &screen);

  // Constructor de copia
  Map(const Map<T> &obj): _wStartX(obj._wStartX), _wStartY(obj._wStartY),
    _wWidth(obj._wWidth), _wHeight(obj._wHeight), _extStartX(obj._extStartX),
    _extStartY(obj._extStartY), _extWidth(obj._extWidth),
    _extHeight(obj._extHeight), _sWidth(obj._sWidth), _sHeight(obj._sHeight),
    _dif(obj._dif) {}

  // Destructor
  ~Map() {}

  // Operador de asignación
  const Map<T> &operator=(const Map<T> &obj);

  // Métodos getter
  const T &Dif() const {
    return _dif;
  }

  // Métodos setter
  bool SetObject(const Object<T> &world);
  void SetScreen(const RECT &screen);
  bool Set(const Object<T> &world, const RECT &screen);

  // Método GetPoint
  const POINT GetPoint(const Point<T> &wPnt) const;

  // Método GetWPoint
  const Point<T> GetWPoint(const int &x, const int &y) const;

  // Método TriPoint
  void TriPoint(POINT * const sPnt, const SPtr<Triangle<T> > &t) const;

  // Método Swap
  void Swap(Map<T> &obj);

private:

  // Método GetPoint
  void GetPoint(POINT &sPnt, const SPtr<Point<T> > &wPnt) const;

  // Member PSetObject
  bool PSetObject(const Object<T> &world);

  // Member PSetScreen
  void PSetScreen(const RECT &screen);

  // Member PSetRel
  void PSetRel();

  // Miembros
  T _wStartX;
  T _wStartY;
  T _wWidth;
  T _wHeight;

  T _extStartX;
  T _extStartY;
  T _extWidth;
  T _extHeight;

  LONG _sWidth;
  LONG _sHeight;

  T _dif;
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Map

// Constructor
template <class T>
Map<T>::Map(const Object<T> &world, const RECT &screen) {
  if(!PSetObject(world)) {
    // TODO: Throw exception.
  }

  PSetScreen(screen);
  PSetRel();
}

// Operador de asignación
template <class T>
const Map<T> &Map<T>::operator=(const Map<T> &obj) {
  _wStartX = obj._wStartX;
  _wStartY = obj._wStartY;
  _wWidth = obj._wWidth;
  _wHeight = obj._wHeight;

  _extStartX = obj._extStartX;
  _extStartY = obj._extStartY;
  _extWidth = obj._extWidth;
  _extHeight = obj._extHeight;

  _sWidth = _sWidth;
  _sHeight = _sHeight;

  _dif = obj._dif;

  return *this;
}

// Método SetObject
template <class T>
bool Map<T>::SetObject(const Object<T> &world) {
  if(!PSetObject(world)) {
    return false;
  }

  PSetRel();

  return true;
}

// Método SetScreen
template <class T>
void Map<T>::SetScreen(const RECT &screen) {
  PSetScreen(screen);
  PSetRel();
}

// Método Set
template <class T>
bool Map<T>::Set(const Object<T> &world, const RECT &screen) {
  if(!PSetObject(world)) {
    return false;
  }

  PSetScreen(screen);
  PSetRel();

  return true;
}

// Método GetPoint
template <class T>
const POINT Map<T>::GetPoint(const Point<T> &wPnt) const {
  POINT p;

  p.x = ((wPnt.X() - _extStartX) / _extWidth) * _sWidth;
  p.y = ((_extStartY - wPnt.Y()) / _extHeight) * _sHeight;

  return p;
}

// Método GetPoint
template <class T>
void Map<T>::GetPoint(POINT &sPnt, const SPtr<Point<T> > &wPnt) const {
  sPnt.x = ((wPnt -> X() - _extStartX) / _extWidth) * _sWidth;
  sPnt.y = ((_extStartY - wPnt -> Y()) / _extHeight) * _sHeight;
}

// Método TriPoint
template <class T>
void Map<T>::TriPoint(POINT * const sPnt, const SPtr<Triangle<T> > &t) const {
  GetPoint(sPnt[0], t -> P1());
  GetPoint(sPnt[1], t -> P2());
  GetPoint(sPnt[2], t -> P3());
  sPnt[3] = sPnt[0];
}

// Método Swap
template <class T>
void Map<T>::Swap(Map<T> &obj) {
  const T t_wStartX = obj._wStartX;
  const T t_wStartY = obj._wStartY;
  const T t_wWidth = obj._wWidth;
  const T t_wHeight = obj._wHeight;

  const T t_extStartX = obj._extStartX;
  const T t_extStartY = obj._extStartY;
  const T t_extWidth = obj._extWidth;
  const T t_extHeight = obj._extHeight;

  const LONG t_sWidth = obj._sWidth;
  const LONG t_sHeight = obj._sHeight;

  const T t_dif = obj._dif;

  obj._wStartX = _wStartX;
  obj._wStartY = _wStartY;
  obj._wWidth = _wWidth;
  obj._wHeight = _wHeight;

  obj._extStartX = _extStartX;
  obj._extStartY = _extStartY;
  obj._extWidth = _extWidth;
  obj._extHeight = _extHeight;

  obj._sWidth = _sWidth;
  obj._sHeight = _sHeight;

  obj._dif = _dif;

  _wStartX = t_wStartX;
  _wStartY = t_wStartY;
  _wWidth = t_wWidth;
  _wHeight = t_wHeight;

  _extStartX = t_extStartX;
  _extStartY = t_extStartY;
  _extWidth = t_extWidth;
  _extHeight = t_extHeight;

  _sWidth = t_sWidth;
  _sHeight = t_sHeight;

  _dif = t_dif;
}

// Método PSetObject
template <class T>
bool Map<T>::PSetObject(const Object<T> &world) {
  if(world.IsLoaded()) {
    _wWidth = 1.25 * (world.MaxP().X() - world.MinP().X());
    _wHeight = 1.25 * (world.MaxP().Y() - world.MinP().Y());

    _wStartX = world.MinP().X() - _wWidth * .1;
    _wStartY = world.MaxP().Y() + _wHeight * .1;

    return true;
  }

  return false;
}

// Método PSetScreen
template <class T>
void Map<T>::PSetScreen(const RECT &screen) {
  _sWidth = screen.right;
  _sHeight = screen.bottom;
}

// Método PSetRel
template <class T>
void Map<T>::PSetRel() {
  T rel = (_sWidth / static_cast<T>(_sHeight)) / (_wWidth / _wHeight);

  if(rel < 1) {
    _extWidth = _wWidth;
    _extStartX = _wStartX;

    _extHeight = _wHeight / rel;
    _extStartY = _wStartY + (_extHeight - _wHeight) / 2;
  }
  else {
    _extHeight = _wHeight;
    _extStartY = _wStartY;

    _extWidth = _wWidth * rel;
    _extStartX = _wStartX - (_extWidth - _wWidth) / 2;
  }

  _dif = (1 / static_cast<T>(_sWidth)) * _extWidth;
}

#endif	/* MAP_H */
