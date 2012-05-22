/*
 * File:   tmatrix.h
 * Author: Gabriel de Oliveira
 *
 * Created on 12 de septiembre de 2011, 05:04
 */

#ifndef TMATRIX_H
#define	TMATRIX_H

#include "base.h"
#include "point.h"
#include "sgroup.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada TMatrix
template <class T>
class TMatrix {
public:

  // Constructor
  TMatrix() {
    SetIdentity();
  }

  // Constructor
  TMatrix(const T &a00, const T &a01, const T &a02, const T &a03,
          const T &a10, const T &a11, const T &a12, const T &a13,
          const T &a20, const T &a21, const T &a22, const T &a23,
          const T &a30, const T &a31, const T &a32, const T &a33);

  // Constructor de copia
  TMatrix(const TMatrix<T> &obj) {
    SetEqual(obj);
  }

  // Destructor
  ~TMatrix() {}

  // Operador de asignación
  const TMatrix<T> &operator=(const TMatrix<T> &obj);

  // Métodos getter
  const T *operator[](const UINT &index) const {
    return _m[index];
  }

  // Métodos setter
  void SetEqual(const TMatrix<T> &obj);
  void SetIdentity();
  void SetZero();

  // Método ConservesNormal
  const bool ConservesNormal() const {
    return (_m[0][0] == _m[1][1] && _m[1][1] == _m[2][2] &&
            _m[0][1] == 0 && _m[0][2] == 0 &&
            _m[1][0] == 0 && _m[1][2] == 0 &&
            _m[2][0] == 0 && _m[2][1] == 0)? true : false;
  }

  // Método transform
  const Point<T> Transform(const Point<T> &p) const;
  void Transform(SGroup<Point<T> > &des, const SGroup<Point<T> > &ori) const;

  // Operador de asignación de multiplicación
  const TMatrix<T> &operator*=(const TMatrix<T> &obj);

  // Método Swap
  void Swap(TMatrix<T> &obj);

private:

  // Miembros
  T _m[4][4];
};

// ----------------------------------------------------------------------------
// Definción de clases --------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada TMatrix

// Constructor
template <class T>
TMatrix<T>::TMatrix(const T &a00, const T &a01, const T &a02, const T &a03,
                    const T &a10, const T &a11, const T &a12, const T &a13,
                    const T &a20, const T &a21, const T &a22, const T &a23,
                    const T &a30, const T &a31, const T &a32, const T &a33) {
  _m[0][0] = a00;
  _m[0][1] = a01;
  _m[0][2] = a02;
  _m[0][3] = a03;

  _m[1][0] = a10;
  _m[1][1] = a11;
  _m[1][2] = a12;
  _m[1][3] = a13;

  _m[2][0] = a20;
  _m[2][1] = a21;
  _m[2][2] = a22;
  _m[2][3] = a23;

  _m[3][0] = a30;
  _m[3][1] = a31;
  _m[3][2] = a32;
  _m[3][3] = a33;
}

// Operador de asignación
template <class T>
const TMatrix<T> &TMatrix<T>::operator=(const TMatrix<T> &obj) {
  SetEqual(obj);
  return *this;
}

// Métodos SetEqual
template <class T>
void TMatrix<T>::SetEqual(const TMatrix<T> &obj) {
  for(UINT i = 0; i < 4; i++) {
    for(UINT j = 0; j < 4; j++) {
      _m[i][j] = obj._m[i][j];
    }
  }
}

// Método SetIdentity
template <class T>
void TMatrix<T>::SetIdentity() {
  SetZero();
  _m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1;
}

// Método SetZero
template <class T>
void TMatrix<T>::SetZero() {
  for(UINT i = 0; i < 4; i++) {
    for(UINT j = 0; j < 4; j++) {
      _m[i][j] = 0;
    }
  }
}

// Método Transfrom
template <class T>
const Point<T> TMatrix<T>::Transform(const Point<T> &p) const {
  return Point<T>(_m[0][0] * p.X() + _m[0][1] * p.Y() + _m[0][2] * p.Z() + _m[0][3],
                  _m[1][0] * p.X() + _m[1][1] * p.Y() + _m[1][2] * p.Z() + _m[1][3],
                  _m[2][0] * p.X() + _m[2][1] * p.Y() + _m[2][2] * p.Z() + _m[2][3]);
}

// Método Transform
template <class T>
void TMatrix<T>::Transform(SGroup<Point<T> > &des, const SGroup<Point<T> > &ori) const {
  for(UINT i = 0; i < des.Elements(); i++) {
    des[i] -> X(_m[0][0] * ori[i] -> X() +
                _m[0][1] * ori[i] -> Y() +
                _m[0][2] * ori[i] -> Z() +
                _m[0][3]);

    des[i] -> Y(_m[1][0] * ori[i] -> X() +
                _m[1][1] * ori[i] -> Y() +
                _m[1][2] * ori[i] -> Z() +
                _m[1][3]);

    des[i] -> Z(_m[2][0] * ori[i] -> X() +
                _m[2][1] * ori[i] -> Y() +
                _m[2][2] * ori[i] -> Z() +
                _m[2][3]);
  }
}

// Operador de asignación de multiplicación
template <class T>
const TMatrix<T> &TMatrix<T>::operator*=(const TMatrix<T> &obj) {
  T t_m[4][4];

  for(UINT i = 0; i < 4; i++) {
    for(UINT j = 0; j < 4; j++) {
      t_m[i][j] = 0;

      for(UINT k = 0; k < 4; k++) {
        t_m[i][j] += obj._m[i][k] * _m[k][j];
      }
    }
  }

  // TODO: Mejorar esto haciendo copia del objeto TMatrix
  for(UINT i = 0; i < 4; i++) {
    for(UINT j = 0; j < 4; j++) {
      _m[i][j] = t_m[i][j];
    }
  }

  return *this;
}

// Método Swap
template <class T>
void TMatrix<T>::Swap(TMatrix<T> &obj) {
  // TODO: Chequear si es correcto este código.
  const T *t_m = obj._m;

  obj._m = _m;

  _m = t_m;
}

#endif	/* TMATRIX_H */
