/*
 * File:   base.h
 * Author: Gabriel de Oliveira
 *
 * Created on 16 de junio de 2011, 23:14
 */

#ifndef BASE_H
#define	BASE_H

// ----------------------------------------------------------------------------
// Definición de tipos --------------------------------------------------------
// ----------------------------------------------------------------------------

// Definición de tipos fundamentales
typedef char CHAR, *P_CHAR;
typedef signed char SCHAR, *P_SCHAR;
typedef unsigned char UCHAR, *P_UCHAR;

typedef short int SHORT, *P_SHORT;
typedef signed short int SSHORT, *P_SSHORT;
typedef unsigned short int USHORT, *P_USHORT;

typedef int INT, *P_INT;
typedef signed int SINT, *P_SINT;
typedef unsigned int UINT, *P_UINT;

typedef long int LONG, *P_LONG;
typedef signed long int SLONG, *P_SLONG;
typedef unsigned long int ULONG, *P_ULONG;

typedef float FLOAT, *P_FLOAT;
typedef double DOUBLE, *P_DOUBLE;
typedef long double LDOUBLE, *P_LDOUBLE;

// Definición de punteros a tipos fundamentales constantes
typedef const CHAR *PC_CHAR;
typedef const SCHAR *PC_SCHAR;
typedef const UCHAR *PC_UCHAR;

typedef const SHORT *PC_SHORT;
typedef const SSHORT *PC_SSHORT;
typedef const USHORT *PC_USHORT;

typedef const INT *PC_INT;
typedef const SINT *PC_SINT;
typedef const UINT *PC_UINT;

typedef const LONG *PC_LONG;
typedef const SLONG *PC_SLONG;
typedef const ULONG *PC_ULONG;

typedef const FLOAT *PC_FLOAT;
typedef const DOUBLE *PC_DOUBLE;
typedef const LDOUBLE *PC_LDOUBLE;

// Definición de tipos enteros sin signo
typedef UCHAR BYTE, *P_BYTE;
typedef USHORT WORD, *P_WORD;
typedef ULONG DWORD, *P_DWORD;

// Definición de color
typedef ULONG COLORREF;

// ----------------------------------------------------------------------------
// Enumeraciones --------------------------------------------------------------
// ----------------------------------------------------------------------------
enum NodeColor {red, black};

// ----------------------------------------------------------------------------
// Estructuras ----------------------------------------------------------------
// ----------------------------------------------------------------------------
struct Position {
  FLOAT a;
  FLOAT b;
  FLOAT c;
};

// ----------------------------------------------------------------------------
// Definición de constantes ---------------------------------------------------
// ----------------------------------------------------------------------------

// Constante NULL
#ifndef NULL
const UINT NULL = 0;
#endif  /* NULL */

// Constante INFINITY
#ifndef INFINITY
#define INFINITY __builtin_inf()
#endif

#define GetRValue(c) ((BYTE)(c))
#define GetGValue(c) ((BYTE)(((WORD)(c))>>8))
#define GetBValue(c) ((BYTE)((c)>>16))
#define RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))

// Constantes de color
const COLORREF BLACK = 0x00000000;
const COLORREF DARKGRAY = 0x00555555;
const COLORREF LIGHTGRAY = 0x00AAAAAA;
const COLORREF WHITE = 0x00FFFFFF;
const COLORREF RED = 0x000000FF;
const COLORREF GREEN = 0x0000FF00;
const COLORREF BLUE = 0x00FF0000;
const COLORREF YELLOW = RED | GREEN;
const COLORREF CYAN = GREEN | BLUE;
const COLORREF MAGENTA = BLUE | RED;
const COLORREF DARKRED = 0x00000080;
const COLORREF DARKGREEN = 0x00008000;
const COLORREF DARKBLUE = 0x00800000;
const COLORREF DARKYELLOW = DARKRED | DARKGREEN;
const COLORREF DARKCYAN = DARKGREEN | DARKBLUE;
const COLORREF DARKMAGENTA = DARKBLUE | DARKRED;

// ----------------------------------------------------------------------------
// Definición de funciones parametrizadas -------------------------------------
// ----------------------------------------------------------------------------
template <class T>
inline const T abs(const T &val) {
  return (val < 0)? -val : val;
}

template <class T>
inline const T &Max(const T &v1, const T &v2) {
  return (v1 < v2)? v2 : v1;
}

template <class T>
inline const T &Min(const T &v1, const T &v2) {
  return (v1 < v2)? v1 : v2;
}

template <class T>
void Swap(T &v1, T &v2) {
  T t_val = v2;

  v2 = v1;

  v1 = t_val;
}

#endif	/* BASE_H */
