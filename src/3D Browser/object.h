/*
 * File:   object.h
 * Author: Gabriel de Oliveira
 *
 * Created on 28 de junio de 2011, 14:57
 */

#ifndef OBJECT_H
#define	OBJECT_H

#include <fstream>
#include <math.h>

#include "sptr.h"
#include "sarr.h"
#include "point.h"
#include "triangle.h"
#include "sgroup.h"
#include "tmatrix.h"
#include "rbtree.h"
#include "pnormal.h"

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Object
template <class T>
class Object {
public:

  // Constructor
  Object(): _triangle(NULL), _groups(0), _point(), _origPnt(),
    _minP(0, 0, 0), _maxP(0, 0, 0), _loaded(false), _centered(true),
    _color(), _reflection() {}

  // TODO: Constructor de copia (debe duplicar los datos).

  // Destructor
  ~Object() {
    UnlinkTriangles();
  }

  // TODO: Operador de asignaci�n (debe duplicar los datos).

  // M�todos getter
  const SGroup<Triangle<T> > &operator[](const UINT &index) const {
    return _triangle[index];
  }
  const UINT &Groups() const {
    return _groups;
  }
  const RBTree<Triangle<T> > &TTree() const {
    return ordTri;
  }

  const Point<T> &MinP() const {
    return _minP;
  }
  const Point<T> &MaxP() const {
    return _maxP;
  }

  const TMatrix<T> &Mat() const {
    return _mat;
  }

  const bool &IsCentered() const {
    return _centered;
  }
  const bool &IsLoaded() const {
    return _loaded;
  }

  const SGroup<Triangle<T> > Tri() const {
    return ordTri.InOrder();
  }

  const COLORREF& Color(const UINT &index) const {
    return *(_color[index]);
  }
  const FLOAT &Reflection(const UINT &index) const {
    return *(_reflection[index]);
  }

  // M�todos setter
  void Color(const UINT &index, const COLORREF &color) {
    *(_color[index]) = color;
  }
  void Reflection(const UINT &index, const FLOAT &reflection) {
    *(_reflection[index]) = reflection;
  }
  void RestoreMat() {
    _mat.SetIdentity();
  }
  void RestoreProp();

  // M�todo Center
  void Center();

  // M�todo OrderTriangles
  void OrderTriangles();

  // M�todo Open
  const bool Open(const PC_CHAR fileName, const WORD &fileExtOff);

  // M�todo ApplyMatrix
  void ApplyMatrix(const TMatrix<T> &mat);

  // M�todo InvalidateTriangles
  void InvalidateTriangles();

  // M�todos Tapper
  void TapperX1(const T &amount);
  void TapperX2(const T &amount);
  void TapperY1(const T &amount);
  void TapperY2(const T &amount);
  void TapperZ1(const T &amount);
  void TapperZ2(const T &amount);

  // M�todos Twist
  void TwistX1(const T &amount);
  void TwistX2(const T &amount);
  void TwistY1(const T &amount);
  void TwistY2(const T &amount);
  void TwistZ1(const T &amount);
  void TwistZ2(const T &amount);

  // M�todo Transform
  void Transform() {
    _mat.Transform(_point, _origPnt);
  }

private:

  // Constantes
  static const UINT TEXT_BUFFER_SIZE;
  static const COLORREF DEF_COLOR;
  static const FLOAT DEF_REFLECTION;

  // Enumeraciones
  enum FType {sur, other};

  // Estructuras
  struct TInfo {
    UINT grp;
    UINT tri;
  };

  // M�todo InvalidateCenters
  void InvalidateCenters();

  // M�todo LinkTriangles
  void LinkTriangles();

  // M�todo UnlinkTriangles
  void UnlinkTriangles();

  // M�todo FileType
  const FType FileType(const PC_CHAR fileName, const WORD &fileExtOff) const;

  // M�todos de carga seg�n el formato
  const bool LoadSUR(std::ifstream &file);

  // Miembros
  SArr<SGroup<Triangle<T> > > _triangle;
  UINT _groups;
  SGroup<Point<T> > _point;
  SGroup<Point<T> > _origPnt;
  Point<T> _minP;
  Point<T> _maxP;

  bool _loaded;
  bool _centered;

  TMatrix<T> _mat;
  RBTree<Triangle<T> > ordTri;

  SGroup<COLORREF> _color;
  SGroup<FLOAT> _reflection;

  SGroup<PNormal<T> > _pNormal;
};

// ----------------------------------------------------------------------------
// Inicializaci�n de miembros est�ticos ---------------------------------------
// ----------------------------------------------------------------------------

template <class T>
const UINT Object<T>::TEXT_BUFFER_SIZE = 128;

template <class T>
const COLORREF Object<T>::DEF_COLOR = BLUE;

template <class T>
const FLOAT Object<T>::DEF_REFLECTION = .8;

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Object

// M�todo RestoreProp
template <class T>
void Object<T>::RestoreProp() {
  for(UINT i = 0; i < _groups; i++) {
    *(_color[i]) = DEF_COLOR;
    *(_reflection[i]) = DEF_REFLECTION;
  }
}

// M�todo Center
template <class T>
void Object<T>::Center() {
  if(_loaded && !_centered) {
    Point<T> dif = (_maxP + _minP) / 2;

    for(UINT i = 0; i < _point.Elements(); i++) {
      *_point[i] -= dif;
      *_origPnt[i] -= dif;
    }

    _minP -= dif;
    _maxP -= dif;

    _centered = true;
  }
}

// M�todo OrderTriangles
template <class T>
void Object<T>::OrderTriangles() {
  ordTri.Destroy();

  for(UINT i = 0; i < _groups; i++) {
    for(UINT j = 0; j < _triangle[i].Elements(); j++) {
      if((_triangle[i][j] -> Normal()).Z() > 0) {
        ordTri.Add(_triangle[i][j]);
      }
    }
  }
}

// M�todo Open
template <class T>
const bool Object<T>::Open(const PC_CHAR fileName, const WORD &fileExtOff) {

  // Encontrar el tipo de archivo
  FType fileType = FileType(fileName, fileExtOff);

  if(fileType == other) {
    return false;
  }

  // Abrir el archivo
  std::ifstream file(fileName, std::ifstream::in);

  if(!file.is_open()) {
    return false;
  }

  // Desvincular las normales de los puntos
  UnlinkTriangles();

  // Cargar el archivo (dependiendo el formato)
  bool retVal;

  switch(fileType) {
  case sur:
    retVal = LoadSUR(file);
    break;

  default: // <- No deber�a entrar a esta rama
    file.close();
    LinkTriangles();
    return false;
  }

  // Fin de la carga del archivo
  file.close();
  LinkTriangles();

  if(retVal == true) {
    _loaded = true;
    _centered = false;

    return true;
  }

  return false;
}

// M�todo ApplyMatrix
template <class T>
void Object<T>::ApplyMatrix(const TMatrix<T> &mat) {
  if(!mat.ConservesNormal()) {
    InvalidateTriangles();
  }
  else {
    InvalidateCenters();
  }

  _mat *= mat;
}

// M�todos Tapper
template <class T>
void Object<T>::TapperX1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> X() < min) {
      min = _point[i] -> X();
    }
    if(_point[i] -> X() > max) {
      max = _point[i] -> X();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (max - _point[i] -> X()) / length;

    _point[i] -> Y(f * _point[i] -> Y());
    _point[i] -> Z(f * _point[i] -> Z());
  }
}

template <class T>
void Object<T>::TapperX2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> X() < min) {
      min = _point[i] -> X();
    }
    if(_point[i] -> X() > max) {
      max = _point[i] -> X();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (_point[i] -> X() - min) / length;

    _point[i] -> Y(f * _point[i] -> Y());
    _point[i] -> Z(f * _point[i] -> Z());
  }
}

template <class T>
void Object<T>::TapperY1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Y() < min) {
      min = _point[i] -> Y();
    }
    if(_point[i] -> Y() > max) {
      max = _point[i] -> Y();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (max - _point[i] -> Y()) / length;

    _point[i] -> X(f * _point[i] -> X());
    _point[i] -> Z(f * _point[i] -> Z());
  }
}

template <class T>
void Object<T>::TapperY2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Y() < min) {
      min = _point[i] -> Y();
    }
    if(_point[i] -> Y() > max) {
      max = _point[i] -> Y();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (_point[i] -> Y() - min) / length;

    _point[i] -> X(f * _point[i] -> X());
    _point[i] -> Z(f * _point[i] -> Z());
  }
}

template <class T>
void Object<T>::TapperZ1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Z() < min) {
      min = _point[i] -> Z();
    }
    if(_point[i] -> Z() > max) {
      max = _point[i] -> Z();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (max - _point[i] -> Z()) / length;

    _point[i] -> X(f * _point[i] -> X());
    _point[i] -> Y(f * _point[i] -> Y());
  }
}

template <class T>
void Object<T>::TapperZ2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Z() < min) {
      min = _point[i] -> Z();
    }
    if(_point[i] -> Z() > max) {
      max = _point[i] -> Z();
    }
  }

  T length = max - min;
  T extra = 1 - amount;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = amount + extra * (_point[i] -> Z() - min) / length;

    _point[i] -> X(f * _point[i] -> X());
    _point[i] -> Y(f * _point[i] -> Y());
  }
}

// M�todos Twist
template <class T>
void Object<T>::TwistX1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> X() < min) {
      min = _point[i] -> X();
    }
    if(_point[i] -> X() > max) {
      max = _point[i] -> X();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (max - _point[i] -> X()) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newY = _point[i] -> Y() * coseno - _point[i] -> Z() * seno;
    T newZ = _point[i] -> Y() * seno + _point[i] -> Z() * coseno;

    _point[i] -> Y(newY);
    _point[i] -> Z(newZ);
  }
}

template <class T>
void Object<T>::TwistX2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> X() < min) {
      min = _point[i] -> X();
    }
    if(_point[i] -> X() > max) {
      max = _point[i] -> X();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (_point[i] -> X() - min) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newY = _point[i] -> Y() * coseno - _point[i] -> Z() * seno;
    T newZ = _point[i] -> Y() * seno + _point[i] -> Z() * coseno;

    _point[i] -> Y(newY);
    _point[i] -> Z(newZ);
  }
}

template <class T>
void Object<T>::TwistY1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Y() < min) {
      min = _point[i] -> Y();
    }
    if(_point[i] -> Y() > max) {
      max = _point[i] -> Y();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (max - _point[i] -> Y()) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newX = _point[i] -> X() * coseno + _point[i] -> Z() * seno;
    T newZ = -_point[i] -> X() * seno + _point[i] -> Z() * coseno;

    _point[i] -> X(newX);
    _point[i] -> Z(newZ);
  }
}

template <class T>
void Object<T>::TwistY2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Y() < min) {
      min = _point[i] -> Y();
    }
    if(_point[i] -> Y() > max) {
      max = _point[i] -> Y();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (_point[i] -> Y() - min) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newX = _point[i] -> X() * coseno + _point[i] -> Z() * seno;
    T newZ = -_point[i] -> X() * seno + _point[i] -> Z() * coseno;

    _point[i] -> X(newX);
    _point[i] -> Z(newZ);
  }
}

template <class T>
void Object<T>::TwistZ1(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Z() < min) {
      min = _point[i] -> Z();
    }
    if(_point[i] -> Z() > max) {
      max = _point[i] -> Z();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (max - _point[i] -> Z()) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newX = _point[i] -> X() * coseno - _point[i] -> Y() * seno;
    T newY = _point[i] -> X() * seno + _point[i] -> Y() * coseno;

    _point[i] -> X(newX);
    _point[i] -> Y(newY);
  }
}

template <class T>
void Object<T>::TwistZ2(const T &amount) {
  InvalidateTriangles();

  T min = INFINITY;
  T max = -INFINITY;

  for(UINT i = 0; i < _point.Elements(); i++) {
    if(_point[i] -> Z() < min) {
      min = _point[i] -> Z();
    }
    if(_point[i] -> Z() > max) {
      max = _point[i] -> Z();
    }
  }

  T length = max - min;
  T angle = amount * 2 * M_PI;

  for(UINT i = 0; i < _point.Elements(); i++) {
    T f = angle * (_point[i] -> Z() - min) / length;
    T coseno = cos(f);
    T seno = sin(f);
    T newX = _point[i] -> X() * coseno - _point[i] -> Y() * seno;
    T newY = _point[i] -> X() * seno + _point[i] -> Y() * coseno;

    _point[i] -> X(newX);
    _point[i] -> Y(newY);
  }
}

// M�todo InvalidateCenters
template <class T>
void Object<T>::InvalidateCenters() {
  for(UINT i = 0; i < _groups; i++) {
    for(UINT j = 0; j < _triangle[i].Elements(); j++) {
      _triangle[i][j] -> InvalidateCenter();
    }
  }
}

// M�todo InvalidateTriangles
template <class T>
void Object<T>::InvalidateTriangles() {
  if(_triangle[0][0] -> IsNormalValid() || _triangle[0][0] -> IsCenterValid()) {
    for(UINT i = 0; i < _groups; i++) {
      for(UINT j = 0; j < _triangle[i].Elements(); j++) {
        _triangle[i][j] -> Invalidate();
      }
    }

    for(UINT i = 0; i < _point.Elements(); i++) {
      _point[i] -> InvalidateNormal();
    }
  }
}

// M�todo LinkTriangles
template <class T>
void Object<T>::LinkTriangles() {
  for(UINT i = 0; i < _groups; i++) {
    for(UINT j = 0; j < _triangle[i].Elements(); j++) {
      _triangle[i][j] -> P1() -> Add(_triangle[i][j]);
      _triangle[i][j] -> P2() -> Add(_triangle[i][j]);
      _triangle[i][j] -> P3() -> Add(_triangle[i][j]);
    }
  }
}

// M�todo UnlinkTriangles
template <class T>
void Object<T>::UnlinkTriangles() {
  for(UINT i = 0; i < _point.Elements(); i++) {
    _point[i] -> Unlink();
  }
}

// M�todo FileType
template <class T>
const typename Object<T>::FType Object<T>::FileType(const PC_CHAR fileName,
                                           const WORD &fileExtOff) const {

  // Archivo del tipo SUR
  if((fileName[fileExtOff] == 's' || fileName[fileExtOff] == 'S') &&
     (fileName[fileExtOff + 1] == 'u' || fileName[fileExtOff + 1] == 'U') &&
     (fileName[fileExtOff + 2] == 'r' || fileName[fileExtOff + 2] == 'R')) {
    return sur;
  }

  // Otro tipo de archivo
  return other;
}

// M�todo LoadSUR
template <class T>
const bool Object<T>::LoadSUR(std::ifstream &file) {
  char text_buf[TEXT_BUFFER_SIZE] = "";

  // TODO: Reemplazar sentencias getline por >>.
  // Luego utilizar tellg y seekg para volver a *INCIDENCE

  // Carga de *ELEMENT GROUPS
  do {
    file.getline(text_buf, TEXT_BUFFER_SIZE);
  } while(strcmp(text_buf, "") == 0 && !file.eof());

  if(strcmp(text_buf, "*ELEMENT GROUPS") != 0 || file.eof()) {
    return false;
  }

  // --------------------------------------------------------------------------
  // El formato es ambiguo en este punto, ya que deber�a figurar aqu� la
  // cantidad de grupos y los tri�ngulos que conforman a cada uno, pero en
  // algunos archivos esta informaci�n no est� disponible. Consideraremos que
  // ambas formas son v�lidas, pero esto dificulta la carga del objeto: si
  // se desea mantener el soporte para los subgrupos (como es el caso en este
  // programa), la carga se divide aqu� en dos m�todos diferenciados de
  // realizarla.
  // --------------------------------------------------------------------------

  // �Contiene *ELEMENT GROUPS informaci�n v�lida?
  do {
    file.getline(text_buf, TEXT_BUFFER_SIZE);
  } while(strcmp(text_buf, "") == 0 && !file.eof());

  bool hasGroupInfo;

  if(file.eof()) {
    return false;
  }
  else if(strcmp(text_buf, "*INCIDENCE") == 0) {
    hasGroupInfo = false;
  }
  else {
    hasGroupInfo = true;
  }

  // Preparar los grupos dependiendo de si hay informaci�n v�lida o no
  UINT groups;

  if(hasGroupInfo) {
    file.seekg(0, std::ios::beg);

    do {
      file.getline(text_buf, TEXT_BUFFER_SIZE);
    } while(strcmp(text_buf, "") == 0 && !file.eof());

    file >> groups;
  }
  else {
    groups = 1;
  }

  if(file.eof()) {
    return false;
  }

  SGroup<Triangle<T> > *grpPtr = new SGroup<Triangle<T> >[groups];
  SArr<SGroup<Triangle<T> > > triangle(grpPtr);

  // Si existe informaci�n sobre los grupos cargarlos, sino contar los
  // tri�ngulos existentes como si perteneciesen a un �nico grupo.
  SPtr<Triangle<T> > *triPtr;
  UINT triNumber;

  if(hasGroupInfo) {
    UINT grpNumber;

    for(UINT i = 0; i < groups; i++) {
      file >> grpNumber;
      if(grpNumber != i + 1 || file.eof()) {
        return false;
      }

      file >> triNumber;

      file >> text_buf;
      if((strcmp(text_buf, "Tri3") != 0 && strcmp(text_buf, "tri3") != 0) ||
         file.eof()) {
        return false;
      }

      triPtr = new SPtr<Triangle<T> >[triNumber];
      triangle[i].Set(SArr<SPtr<Triangle<T> > >(triPtr), triNumber);
    }
  }
  else {
    triNumber = 0;

    do {
      file.getline(text_buf, TEXT_BUFFER_SIZE);
      if(strcmp(text_buf, "") != 0) {
        ++triNumber;
      }
    } while(strcmp(text_buf, "*COORDINATES") != 0 && !file.eof());

    if((--triNumber) == 0 || file.eof()) {
      return false;
    }

    triPtr = new SPtr<Triangle<T> >[triNumber];
    triangle[0].Set(SArr<SPtr<Triangle<T> > >(triPtr), triNumber);
  }

  // Carga de *COORDINATES
  if(hasGroupInfo) {
    do {
      file.getline(text_buf, TEXT_BUFFER_SIZE);
    } while(strcmp(text_buf, "*COORDINATES") != 0 && !file.eof());

    if(file.eof()) {
      return false;
    }
  }

  // Carga de la cantidad de puntos
  UINT points;

  file >> points;

  if(file.eof()) {
    return false;
  }

  SPtr<Point<T> > *pntPtr = new SPtr<Point<T> >[points];
  SGroup<Point<T> > point(SArr<SPtr<Point<T> > >(pntPtr), points);

  pntPtr = new SPtr<Point<T> >[points];
  SGroup<Point<T> > origPnt(SArr<SPtr<Point<T> > >(pntPtr), points);

  // Carga de los datos de los puntos
  Point<T> minP(INFINITY, INFINITY, INFINITY);
  Point<T> maxP(-INFINITY, -INFINITY, -INFINITY);
  Point<T> *pPtr;
  UINT pntNumber;
  T x;
  T y;
  T z;

  for(UINT i = 0; i < points; i++) {
    file >> pntNumber;
    if(file.eof()) {
      return false;
    }

    file >> x;
    file >> y;
    file >> z;

    if(file.eof()) {
      return false;
    }

    if(minP.X() > x) {
      minP.X(x);
    }
    if(minP.Y() > y) {
      minP.Y(y);
    }
    if(minP.Z() > z) {
      minP.Z(z);
    }

    if(maxP.X() < x) {
      maxP.X(x);
    }
    if(maxP.Y() < y) {
      maxP.Y(y);
    }
    if(maxP.Z() < z) {
      maxP.Z(z);
    }

    pPtr = new Point<T>(x, y, z);

    point[i] = SPtr<Point<T> >(pPtr);

    pPtr = new Point<T>(x, y, z);

    origPnt[i] = SPtr<Point<T> >(pPtr);
  }

  // Correcci�n debida a ambiguedades del formato
  UINT pntCorrect = (pntNumber == points)? 1 : 0;

  // Vuelvo al principio para cargar los tri�ngulos
  file.seekg(0, std::ios::beg);

  // Carga de *INCIDENCE
  do {
    file.getline(text_buf, TEXT_BUFFER_SIZE);
  } while(strcmp(text_buf, "*INCIDENCE") != 0 && !file.eof());

  if(file.eof()) {
    return false;
  }

  // Carga de los datos de los tri�ngulos
  Triangle<T> *tPtr;
  UINT p1;
  UINT p2;
  UINT p3;

  for(UINT i = 0; i < groups; i++) {
    for(UINT j = 0; j < triangle[i].Elements(); j++) {
      file >> p1;
      file >> p2;
      file >> p3;

      p1 -= pntCorrect;
      p2 -= pntCorrect;
      p3 -= pntCorrect;

      if(p1 >= points || p2 >= points || p3 >= points || file.eof()) {
        return false;
      }

      tPtr = new Triangle<T>;
      triangle[i][j] = SPtr<Triangle<T> >(tPtr);

      triangle[i][j] -> P1(point[p1]);
      triangle[i][j] -> P2(point[p2]);
      triangle[i][j] -> P3(point[p3]);
    }
  }

  // Asignar colores
  SPtr<COLORREF> *pCol = new SPtr<COLORREF>[groups];
  SGroup<COLORREF> color(SArr<SPtr<COLORREF> >(pCol), groups);

  // Asignar �ndices de reflejo
  SPtr<FLOAT> *pRef = new SPtr<FLOAT>[groups];
  SGroup<FLOAT> reflection(SArr<SPtr<FLOAT> >(pRef), groups);

  for(UINT i = 0; i < groups; i++) {
    COLORREF *p_col = new COLORREF(DEF_COLOR);
    color[i] = SPtr<COLORREF>(p_col);

    FLOAT *p_ref = new FLOAT(DEF_REFLECTION);
    reflection[i] = SPtr<FLOAT>(p_ref);

    for(UINT j = 0; j < triangle[i].Elements(); j++) {
      triangle[i][j] -> Color(color[i]);
      triangle[i][j] -> Reflection(reflection[i]);
    }
  }

  // Crear arreglo de normales para los puntos
  SPtr<PNormal<T> > *pNor = new SPtr<PNormal<T> >[point.Elements()];
  SGroup<PNormal<T> > pNormal(SArr<SPtr<PNormal<T> > >(pNor), point.Elements());

  for(UINT i = 0; i < pNormal.Elements(); i++) {
    PNormal<T> *p_nor = new PNormal<T>;
    pNormal[i] = SPtr<PNormal<T> >(p_nor);
    point[i] -> Normal(pNormal[i]);
  }

  // Cargo los datos en el objeto
  _triangle = triangle;
  _point = point;
  _origPnt = origPnt;
  _groups = groups;
  _minP = minP;
  _maxP = maxP;
  _mat.SetIdentity();
  _color = color;
  _reflection = reflection;
  _pNormal = pNormal;

  return true;
}

#endif	/* OBJECT_H */
