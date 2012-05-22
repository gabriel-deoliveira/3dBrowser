/*
 * File:   light.h
 * Author: Gabriel de Oliveira
 *
 * Created on 29 de septiembre de 2011, 16:23
 */

#ifndef LIGHT_H
#define	LIGHT_H

#include "base.h"
#include "sptr.h"
#include "point.h"
#include "triangle.h"
#include "tmatrix.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Light
template <class T>
class Light {
public:

  // Constructor
  explicit Light(const Point<T> &origPos = Point<T>(0, 0, 0),
                 const COLORREF &color = WHITE): _origPos(origPos),
    _pos(origPos), _color(color), _ambience(0.35), _direct(0.65),
    _intR(GetRValue(_color) / static_cast<FLOAT>(255)),
    _intG(GetGValue(_color) / static_cast<FLOAT>(255)),
    _intB(GetBValue(_color) / static_cast<FLOAT>(255)) {}

  // Constructor de copia
  Light(const Light<T> &obj): _origPos(obj._origPos), _pos(obj._pos),
    _color(obj._color), _ambience(obj._ambience), _direct(obj._direct),
    _mat(obj._mat), _intR(obj._intR), _intG(obj._intG), _intB(obj._intB) {}

  // Destructor
  ~Light() {}

  // Operador de asignación
  const Light<T> &operator=(const Light<T> &obj);

  // Métodos getter
  const Point<T> &Pos() const {
    return _pos;
  }
  const COLORREF &Color() const {
    return _color;
  }
  const FLOAT &Ambience() const {
    return _ambience;
  }
  const FLOAT &Direct() const {
    return _direct;
  }

  // Métodos setter
  void Pos(const Point<T> &origPos);
  void Color(const COLORREF &color);
  void Ambience(const FLOAT &ambience);
  void Direct(const FLOAT &direct);
  void RestoreMat() {
    _mat.SetIdentity();
  }
  void RestoreProp();

  // Métodos de matrices
  void ApplyMatrix(const TMatrix<T> mat) {
    _mat *= mat;
  }
  void Transform() {
    _pos = _mat.Transform(_origPos);
  }

  // Método PaintRef
  const COLORREF PaintRef(const SPtr<Triangle<T> > &t) const;

  // Método Paint
  const COLORREF Paint(const SPtr<Triangle<T> > &t) const;

  // Método PaintPhong
  const COLORREF PaintPhong(const Position &p, const SPtr<Triangle<T> > &t) const;

  // Método Swap
  void Swap(Light<T> &obj);

private:

  // Miembros
  Point<T> _origPos;
  Point<T> _pos;
  COLORREF _color;
  FLOAT _ambience;
  FLOAT _direct;
  TMatrix<T> _mat;
  FLOAT _intR;
  FLOAT _intG;
  FLOAT _intB;
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Light

// Operador de asignación
template <class T>
const Light<T> &Light<T>::operator=(const Light<T> &obj) {
  _origPos = obj._origPos;
  _pos = obj._pos;
  _color = obj._color;
  _ambience = obj._ambience;
  _direct = obj._direct;
  _mat = obj._mat;
  _intR = obj._intR;
  _intG = obj._intG;
  _intB = obj._intB;
  return *this;
}

// Métodos setter
template <class T>
void Light<T>::Pos(const Point<T> &origPos) {
  _origPos = _pos = origPos;
  _mat.SetIdentity();
}

template <class T>
void Light<T>::Color(const COLORREF &color) {
  _color = color;
  _intR = GetRValue(_color) / static_cast<FLOAT>(255);
  _intG = GetGValue(_color) / static_cast<FLOAT>(255);
  _intB = GetBValue(_color) / static_cast<FLOAT>(255);
}

template <class T>
void Light<T>::Ambience(const FLOAT &ambience) {
  if(ambience >= 0 && ambience <= 1) {
    _ambience = ambience;
    _direct = 1 - _ambience;
  }
}

template <class T>
void Light<T>::Direct(const FLOAT &direct) {
  if(direct >= 0 && direct <= 1) {
    _direct = direct;
    _ambience = 1 - _direct;
  }
}

// Restaurar las propiedades de la luz
template <class T>
void Light<T>::RestoreProp() {
  Color(WHITE);
  Ambience(0.35);
}

// Método PaintPhong
template <class T>
const COLORREF Light<T>::PaintPhong(const Position &p, const SPtr<Triangle<T> > &t) const {
  Point<T> normal = t -> P1() -> Normal() * p.a +
                    t -> P2() -> Normal() * p.b +
                    t -> P3() -> Normal() * p.c;

  Point<T> center((t -> P1() -> X() * p.a + t -> P2() -> X() * p.b + t -> P3() -> X() * p.c),
                  (t -> P1() -> Y() * p.a + t -> P2() -> Y() * p.b + t -> P3() -> Y() * p.c),
                  (t -> P1() -> Z() * p.a + t -> P2() -> Z() * p.b + t -> P3() -> Z() * p.c));

  Point<T> inc = _pos - center;

  T cosNormal = inc.Dot(normal) / (inc.Module() * normal.Module());

  T intensity = (cosNormal < 0)? 0 : cosNormal * _direct;

  inc += Point<T>(0, 0, _pos.Z()) - center;

  T ref = inc.Dot(normal) / (inc.Module() * normal.Module());

  ref = (ref - t -> Reflection()) / (1 - t -> Reflection());

  ref = (ref < 0)? 0 : ref;

  T notRef = 1 - ref;

  T colorR = GetRValue(t -> Color()) * _intR;
  T colorG = GetGValue(t -> Color()) * _intG;
  T colorB = GetBValue(t -> Color()) * _intB;

  return RGB(ref * GetRValue(_color) + notRef * (colorR * intensity + colorR * _ambience),
             ref * GetGValue(_color) + notRef * (colorG * intensity + colorG * _ambience),
             ref * GetBValue(_color) + notRef * (colorB * intensity + colorB * _ambience));
}

// Método PaintRef
template <class T>
const COLORREF Light<T>::PaintRef(const SPtr<Triangle<T> > &t) const {
  Point<T> inc = _pos - t -> Center();

  T cosNormal = inc.Dot(t -> Normal()) /
                (inc.Module() * t -> Normal().Module());

  T intensity = (cosNormal < 0)? 0 : cosNormal * _direct;

  inc += Point<T>(0, 0, _pos.Z()) - t -> Center();

  T ref = inc.Dot(t -> Normal()) /
          (inc.Module() * t -> Normal().Module());

  ref = (ref - t -> Reflection()) / (1 - t -> Reflection());

  ref = (ref < 0)? 0 : ref;

  T notRef = 1 - ref;

  T colorR = GetRValue(t -> Color()) * _intR;
  T colorG = GetGValue(t -> Color()) * _intG;
  T colorB = GetBValue(t -> Color()) * _intB;

  return RGB(ref * GetRValue(_color) + notRef * (colorR * intensity + colorR * _ambience),
             ref * GetGValue(_color) + notRef * (colorG * intensity + colorG * _ambience),
             ref * GetBValue(_color) + notRef * (colorB * intensity + colorB * _ambience));
}

// Método Paint
template <class T>
const COLORREF Light<T>::Paint(const SPtr<Triangle<T> > &t) const {
  Point<T> inc = _pos - t -> Center();

  T cosNormal = inc.Dot(t -> Normal()) /
                (inc.Module() * t -> Normal().Module());

  T intensity = (cosNormal < 0)? 0 : cosNormal * _direct;

  T colorR = GetRValue(t -> Color()) * _intR;
  T colorG = GetGValue(t -> Color()) * _intG;
  T colorB = GetBValue(t -> Color()) * _intB;

  return RGB(colorR * intensity + colorR * _ambience,
             colorG * intensity + colorG * _ambience,
             colorB * intensity + colorB * _ambience);
}

// Método Swap
template <class T>
void Light<T>::Swap(Light<T> &obj) {
  _origPos.Swap(obj._origPos);
  _pos.Swap(obj._pos);
  _mat.Swap(obj._mat);

  const COLORREF t_color = obj._color;
  const FLOAT t_ambience = obj._ambience;
  const FLOAT t_direct = obj._direct;
  const FLOAT t_intR = obj._intR;
  const FLOAT t_intG = obj._intG;
  const FLOAT t_intB = obj._intB;

  obj._color = _color;
  obj._ambience = _ambience;
  obj._direct = _direct;
  obj._intR = _intR;
  obj._intG = _intG;
  obj._intB = _intB;

  _color = t_color;
  _ambience = t_ambience;
  _direct = t_direct;
  _intR = t_intR;
  _intG = t_intG;
  _intB = t_intB;
}

#endif	/* LIGHT_H */
