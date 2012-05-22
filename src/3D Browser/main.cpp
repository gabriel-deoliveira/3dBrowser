/*
 * File:   main.cpp
 * Author: Gabriel de Oliveira
 *
 * Created on 16 de junio de 2011, 21:21
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <math.h>

#include "resource.h"
#include "base.h"
#include "object.h"
#include "map.h"
#include "tmatrix.h"
#include "light.h"
#include "sarr.h"

// ----------------------------------------------------------------------------
// Definición de tipos --------------------------------------------------------
// ----------------------------------------------------------------------------
typedef FLOAT PREC;

// ----------------------------------------------------------------------------
// Definición de enumeraciones ------------------------------------------------
// ----------------------------------------------------------------------------
enum EventType {none, translating, rotating};

// ----------------------------------------------------------------------------
// Definición de estructuras --------------------------------------------------
// ----------------------------------------------------------------------------
struct MouseEvent {
  EventType event;
  bool lightEvent;
  bool tnlEvent;
  int prevX;
  int prevY;
};

struct Choices {
  bool drawCenter;
  bool drawLight;
  bool drawPhong;
};

// ----------------------------------------------------------------------------
// Definición de constantes ---------------------------------------------------
// ----------------------------------------------------------------------------
const TMatrix<PREC> TM_INC_SIZE(1.25, 0, 0, 0,
                                0, 1.25, 0, 0,
                                0, 0, 1.25, 0,
                                0, 0, 0, 1);

const TMatrix<PREC> TM_DEC_SIZE(.8, 0, 0, 0,
                                0, .8, 0, 0,
                                0, 0, .8, 0,
                                0, 0, 0, 1);

const PREC ANGLE = 0.005;

const COLORREF CENTER_COLOR = RED;
const COLORREF ALIGHT_COLOR = GREEN;
const LONG CIRCLE_RADIUS = 5;
const LONG ACTIVE_RADIUS = 10;

// ----------------------------------------------------------------------------
// Declaración de variables globales ------------------------------------------
// ----------------------------------------------------------------------------
HWND hwAyuda;
HWND hwMA;
HWND hwMain;
HWND hwPLuz;
HWND hwPObj;
HWND hwTrans;

HBITMAP canvas;

Object<PREC> obj;
Map<PREC> map;
Light<PREC> lig;

MouseEvent me = {none, false, false, 0, 0};
Choices ch = {true, true, false};
bool mustRender = false;
LPARAM currClientSize = 0;

// ----------------------------------------------------------------------------
// Declaración de funciones ---------------------------------------------------
// ----------------------------------------------------------------------------
void ActivateMenu();
void CmdCargarObjeto();
void CmdRestaurarTodo();
void CmdRestaurarObjeto();
void CmdRestaurarLuz();
void CmdRestaurarMatObjeto();
void CmdRestaurarPropObjeto();
void CmdRestaurarMatLuz();
void CmdRestaurarPropLuz();
const TMatrix<PREC> GetRotMatrix(const int &x, const int &y);
const TMatrix<PREC> GetTransMatrix(const int &x, const int &y);
const RECT GetTriRect(const POINT * const p);
const bool InsideTriangle(const int &x, const int &y, const POINT * const p, Position &tPos);
void InvalidateMain();
LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCreateMain(HWND hWnd);
void OnDestroyMain();
void OnKeyDownMain(WPARAM wParam);
void OnKeyUpMain(WPARAM wParam);
void OnLButtonDownMain(LPARAM lParam);
void OnLButtonUpMain();
void OnMouseMoveMain(LPARAM lParam);
void OnMouseWheelMain(WPARAM wParam);
void OnPaintMain();
void OnRButtonDownMain(LPARAM lParam);
void OnRButtonUpMain();
void OnSizeMain(WPARAM wParam, LPARAM lParam);
inline void PopError(const PC_CHAR msg);
void PostTNL();
void RenderPhong();
void ToggleAyuda();
void ToggleCenterInd();
void ToggleLightInd();
void ToggleMatrizArbitraria();
void TogglePropLuz();
void TogglePropObjeto();
void ToggleTransformaciones();

// Diálogo de transformaciones
void CmdRestaurarTrans();
inline PREC GetTapperValue();
inline PREC GetTwistValue();
void OnHScrollTrans(WPARAM wParam, LPARAM lParam);
void OnInitDialogTrans(HWND hWnd);
BOOL CALLBACK TransProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdatePropObject();
void UpdateTapperValue();
void UpdateTwistValue();

// Diálogo de matriz arbitraria
void ApplyMA();
void CmdIdentidadMA();
BOOL CALLBACK MAProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnInitDialogMA(HWND hWnd);

// Diálogo de propiedades del objeto
void CmdCambiarColorPObj();
LRESULT CALLBACK ObjColorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnHScrollPObj();
void OnInitDialogPObj(HWND hWnd);
void OnPaintObjColor();
BOOL CALLBACK PObjProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateGrpProp();

// Diálogo de propiedades de la luz
void CmdCambiarColorPLuz();
LRESULT CALLBACK LuzColorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnHScrollPLuz(HWND hWnd);
void OnInitDialogPLuz(HWND hWnd);
void OnPaintLuzColor();
BOOL CALLBACK PLuzProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateLuzProp();

// Diálogo de ayuda
BOOL CALLBACK AyudaProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ----------------------------------------------------------------------------
// WinMain - Punto de entrada -------------------------------------------------
// ----------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {

  // Registrar la clase de la ventana principal
  const CHAR WC_NAME[] = "Main WC";

  WNDCLASSEX wc;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = MainProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
  wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
  wc.lpszClassName = WC_NAME;
  wc.hIconSm = NULL;

  if(!RegisterClassEx(&wc)) {
    PopError("No pudo registrarse la clase de la ventana principal.");
    return -1;
  }

  // Crear la ventana principal
  const CHAR W_NAME[] = "3D Browser - Final de VC1 - Gabriel de Oliveira";

  hwMain = CreateWindow(WC_NAME, W_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
                        hInstance, NULL);

  if(hwMain == NULL) {
    PopError("No pudo crearse la ventana principal.");
    return -1;
  }

  // Mostrar y actualizar la ventana principal
  ShowWindow(hwMain, nCmdShow);
  UpdateWindow(hwMain);

  // Atender los mensajes de la ventana principal
  MSG msg;

  while(GetMessage(&msg, NULL, 0, 0) > 0) {
    if(!IsDialogMessage(hwTrans, &msg) &&
       !IsDialogMessage(hwMA, &msg) &&
       !IsDialogMessage(hwPObj, &msg)&&
       !IsDialogMessage(hwPLuz, &msg) &&
       !IsDialogMessage(hwAyuda, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return 0;
}

// ----------------------------------------------------------------------------
// Definición de funciones ----------------------------------------------------
// ----------------------------------------------------------------------------

// Activa el menu cuando se carga un objeto 3D
void ActivateMenu() {
  static bool active = false;

  if(!active) {
    EnableMenuItem(GetMenu(hwMain), IDM_PHONG_SHADING, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_CENTRO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_LUZ, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_TRANSFORMACIONES, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_MATRIZ_ARBITRARIA, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_PROP_OBJETO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_PROP_LUZ, MF_ENABLED);

    EnableMenuItem(GetMenu(hwMain), IDM_TODO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_OBJETO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_LUZ, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_MATRIZ_DEL_OBJETO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_PROPIEDADES_DEL_OBJETO, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_MATRIZ_DE_LA_LUZ, MF_ENABLED);
    EnableMenuItem(GetMenu(hwMain), IDM_PROPIEDADES_DE_LA_LUZ, MF_ENABLED);

    active = true;
  }
}

// Carga un objeto desde un archivo
void CmdCargarObjeto() {
  OPENFILENAME ofn;
  char szFileName[MAX_PATH] = "";

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwMain;
  ofn.lpstrFilter = "Objeto SUR (*.sur)\0*.sur\0Todos los archivos (*.*)\0*.*\0";
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = "Cargar Objeto 3D";
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = "sur";

  if(!GetOpenFileName(&ofn)) {
    return;
  }

  SetCursor(LoadCursor(NULL, IDC_WAIT));

  if(!obj.Open(szFileName, ofn.nFileExtension)) {
    PopError("Error al abrir el archivo.");
    return;
  }

  obj.Center();

  obj.OrderTriangles();

  lig.Pos(obj.MaxP());

  map.SetObject(obj);

  ActivateMenu();

  UpdatePropObject();

  SetCursor(LoadCursor(NULL, IDC_ARROW));

  InvalidateMain();
}

// Restaura los valores de objeto y luz
void CmdRestaurarTodo() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  obj.RestoreMat();
  obj.RestoreProp();

  lig.RestoreMat();
  lig.RestoreProp();

  obj.Transform();
  obj.InvalidateTriangles();
  obj.OrderTriangles();

  lig.Transform();

  UpdateGrpProp();
  UpdateLuzProp();
  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarObjeto() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  obj.RestoreMat();
  obj.RestoreProp();

  obj.Transform();
  obj.InvalidateTriangles();
  obj.OrderTriangles();

  UpdateGrpProp();
  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarLuz() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  lig.RestoreMat();
  lig.RestoreProp();

  lig.Transform();

  UpdateLuzProp();
  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarMatObjeto() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  obj.RestoreMat();

  obj.Transform();
  obj.InvalidateTriangles();
  obj.OrderTriangles();

  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarPropObjeto() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  obj.RestoreProp();

  UpdateGrpProp();
  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarMatLuz() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  lig.RestoreMat();

  lig.Transform();

  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CmdRestaurarPropLuz() {
  SetCursor(LoadCursor(NULL, IDC_WAIT));

  lig.RestoreProp();

  UpdateLuzProp();
  InvalidateMain();

  SetCursor(LoadCursor(NULL, IDC_ARROW));
}

// Crea una matriz de rotación
const TMatrix<PREC> GetRotMatrix(const int &x, const int &y) {
  PREC angX = x * ANGLE;
  PREC angY = y * ANGLE;

  PREC cosX = cos(angX);
  PREC sinX = sin(angX);
  PREC cosY = cos(angY);
  PREC sinY = sin(angY);

  TMatrix<PREC> retVal(cosX, 0, sinX, 0,
                       0, 1, 0, 0,
                       -sinX, 0, cosX, 0,
                       0, 0, 0, 1);

  return retVal *= TMatrix<PREC> (1, 0, 0, 0,
                                  0, cosY, -sinY, 0,
                                  0, sinY, cosY, 0,
                                  0, 0, 0, 1);
}

// Crea una matriz de desplazamiento acorde con las dimensiones del objeto y la
// ventana
const TMatrix<PREC> GetTransMatrix(const int &x, const int &y) {
  return TMatrix<PREC>(1, 0, 0, x * map.Dif(),
                       0, 1, 0, -y * map.Dif(),
                       0, 0, 1, 0,
                       0, 0, 0, 1);
}

// Crea el rectángulo que contiene a un triángulo
const RECT GetTriRect(const POINT * const p) {
  RECT r;

  r.left = Min(p[0].x, Min(p[1].x, p[2].x));
  r.top = Min(p[0].y, Min(p[1].y, p[2].y));
  r.right = Max(p[0].x, Max(p[1].x, p[2].x));
  r.bottom = Max(p[0].y, Max(p[1].y, p[2].y));

  return r;
}

// Verifica si un punto se encuentra dentro del triángulo
const bool InsideTriangle(const int &x, const int &y, const POINT * const p,
                          Position &tPos) {
  // Utilizando técnica baricéntrica
  const Point<PREC> A(p[0].x, p[0].y, 0);
  const Point<PREC> B(p[1].x, p[1].y, 0);
  const Point<PREC> C(p[2].x, p[2].y, 0);
  const Point<PREC> P(x, y, 0);

  const Point<PREC> v0 = C - A;
  const Point<PREC> v1 = B - A;
  const Point<PREC> v2 = P - A;

  const PREC dot00 = v0.Dot(v0);
  const PREC dot01 = v0.Dot(v1);
  const PREC dot02 = v0.Dot(v2);
  const PREC dot11 = v1.Dot(v1);
  const PREC dot12 = v1.Dot(v2);

  const PREC invDenom = 1 / (dot00 * dot11 - dot01 * dot01);

  tPos.c = (dot11 * dot02 - dot01 * dot12) * invDenom;
  tPos.b = (dot00 * dot12 - dot01 * dot02) * invDenom;
  tPos.a = 1 - tPos.b - tPos.c;

  return (tPos.b >= 0) && (tPos.c >= 0) && (tPos.b + tPos.c < 1.0001);
}

// Invalida la ventana principal y la prepara para renderizar
void InvalidateMain() {
  mustRender = true;
  InvalidateRect(hwMain, NULL, FALSE);
}

// Maneja los mensajes de la ventana principal
LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDM_CARGAR_OBJETO:
      CmdCargarObjeto();
      break;

    case IDM_SALIR:
      PostMessage(hWnd, WM_CLOSE, 0, 0);
      break;

    case IDM_INDICADOR_DE_CENTRO:
      ToggleCenterInd();
      break;

    case IDM_INDICADOR_DE_LUZ:
      ToggleLightInd();
      break;

    case IDM_TRANSFORMACIONES:
      ToggleTransformaciones();
      break;

    case IDM_MATRIZ_ARBITRARIA:
      ToggleMatrizArbitraria();
      break;

    case IDM_PROP_OBJETO:
      TogglePropObjeto();
      break;

    case IDM_PROP_LUZ:
      TogglePropLuz();
      break;

    case IDM_PHONG_SHADING:
      RenderPhong();
      break;

    case IDM_TODO:
      CmdRestaurarTodo();
      break;

    case IDM_OBJETO:
      CmdRestaurarObjeto();
      break;

    case IDM_LUZ:
      CmdRestaurarLuz();
      break;

    case IDM_MATRIZ_DEL_OBJETO:
      CmdRestaurarMatObjeto();
      break;

    case IDM_PROPIEDADES_DEL_OBJETO:
      CmdRestaurarPropObjeto();
      break;

    case IDM_MATRIZ_DE_LA_LUZ:
      CmdRestaurarMatLuz();
      break;

    case IDM_PROPIEDADES_DE_LA_LUZ:
      CmdRestaurarPropLuz();
      break;

    case IDM_CONTROLES:
      ToggleAyuda();
      break;

    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    break;

  case WM_CREATE:
    OnCreateMain(hWnd);
    break;

  case WM_DESTROY:
    OnDestroyMain();
    break;

  case WM_KEYDOWN:
    OnKeyDownMain(wParam);
    break;

  case WM_KEYUP:
    OnKeyUpMain(wParam);
    break;

  case WM_LBUTTONDOWN:
    OnLButtonDownMain(lParam);
    break;

  case WM_LBUTTONUP:
    OnLButtonUpMain();
    break;

  case WM_MOUSEACTIVATE:
    switch(LOWORD(lParam)) {
    case HTCLIENT:
      SetFocus(hwMain);
      return MA_ACTIVATEANDEAT;
    }
    return MA_ACTIVATE;

  case WM_MOUSEMOVE:
    OnMouseMoveMain(lParam);
    break;

  case WM_MOUSEWHEEL:
    OnMouseWheelMain(wParam);
    break;

  case WM_PAINT:
    OnPaintMain();
    break;

  case WM_RBUTTONDOWN:
    OnRButtonDownMain(lParam);
    break;

  case WM_RBUTTONUP:
    OnRButtonUpMain();
    break;

  case WM_SIZE:
    OnSizeMain(wParam, lParam);
    break;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

// Inicializa la ventana principal
void OnCreateMain(HWND hWnd) {

  // Crear el canvas de dibujo
  HDC hDC = GetDC(hWnd);
  canvas = CreateCompatibleBitmap(hDC, GetSystemMetrics(SM_CXSCREEN),
                                  GetSystemMetrics(SM_CYSCREEN));

  if(canvas == NULL) {
    PopError("No pudo crearse el canvas de dibujo.");
  }

  ReleaseDC(hWnd, hDC);

  // Obtener las dimensiones de la ventana del cliente y cargarlas en el mapa
  RECT client;
  if(!GetClientRect(hWnd, &client)) {
    PopError("No pudo obtenerse la dimensión del área de cliente.");
  }

  map.SetScreen(client);

  // Crear el diálogo de transformaciones
  hwTrans = CreateDialog(GetModuleHandle(NULL),
                         MAKEINTRESOURCE(IDD_TRANSFORMACIONES), hWnd,
                         TransProc);

  if(hwTrans == NULL) {
    PopError("No pudo crearse el diálogo de transformaciones.");
  }

  // Crear el diálogo de matriz arbitraria
  hwMA = CreateDialog(GetModuleHandle(NULL),
                      MAKEINTRESOURCE(IDD_MATRIZ_ARBITRARIA), hWnd, MAProc);

  if(hwMA == NULL) {
    PopError("No pudo crearse el diálogo de matriz arbitraria.");
  }

  ShowWindow(hwMA, SW_HIDE);

  // Registrar la ventana de color del objeto
  WNDCLASSEX wc;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = ObjColorProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetModuleHandle(NULL);
  wc.hIcon = NULL;
  wc.hCursor = NULL;
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "ObjColor WC";
  wc.hIconSm = NULL;

  if(!RegisterClassEx(&wc)) {
    PopError("No pudo registrarse la ventana de color del objeto.");
  }

  // Crear el diálogo de propiedades del objeto
  hwPObj = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROP_OBJETO),
                        hWnd, PObjProc);

  if(hwPObj == NULL) {
    PopError("No pudo crearse el diálogo de propiedades del objeto.");
  }

  ShowWindow(hwPObj, SW_HIDE);

  // Registrar la ventana de color de la luz
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = LuzColorProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetModuleHandle(NULL);
  wc.hIcon = NULL;
  wc.hCursor = NULL;
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "LuzColor WC";
  wc.hIconSm = NULL;

  if(!RegisterClassEx(&wc)) {
    PopError("No pudo registrarse la ventana de color de la luz.");
  }

  // Crear el diálogo de propiedades de la luz
  hwPLuz = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROP_LUZ),
                        hWnd, PLuzProc);

  if(hwPLuz == NULL) {
    PopError("No pudo crearse el diálogo de propiedades de la luz.");
  }

  ShowWindow(hwPLuz, SW_HIDE);

  // Crear el diálogo de ayuda
  hwAyuda = CreateDialog(GetModuleHandle(NULL),
                         MAKEINTRESOURCE(IDD_AYUDA_CONTROLES), hWnd, AyudaProc);

  if(hwAyuda == NULL) {
    PopError("No pudo crearse el diálogo de ayuda.");
  }

  ShowWindow(hwAyuda, SW_HIDE);
}

// Destruye la ventana principal
void OnDestroyMain() {
  // Destruir el diálogo de ayuda
  DestroyWindow(hwAyuda);

  // Destruir el diálogo de propiedades de la luz
  DestroyWindow(hwPLuz);

  // Destruir el diálogo de propiedades del objeto
  DestroyWindow(hwPObj);

  // Destruir el diálogo de matriz arbitraria
  DestroyWindow(hwMA);

  // Destruir el diálogo de transformaciones
  DestroyWindow(hwTrans);

  // Destruir el canvas de dibujo
  DeleteObject(canvas);

  // Salir de la aplicación
  PostQuitMessage(0);
}

// Captura una tecla presionada
void OnKeyDownMain(WPARAM wParam) {
  if(me.event == none && !me.lightEvent) {
    if(wParam == VK_SHIFT) {
      me.lightEvent = true;

      InvalidateMain();
    }
    else if(wParam == 'R') {
      RenderPhong();
    }
    else if(wParam == 'C') {
      ToggleCenterInd();
    }
    else if(wParam == 'L') {
      ToggleLightInd();
    }
  }
}

void OnKeyUpMain(WPARAM wParam) {
  if(wParam == VK_SHIFT && me.lightEvent) {
    me.lightEvent = false;

    InvalidateMain();
  }
}

// Captura el clic del botón izquierdo del ratón
void OnLButtonDownMain(LPARAM lParam) {
  if(obj.IsLoaded() && me.event == none) {
    me.event = translating;
    me.prevX = GET_X_LPARAM(lParam);
    me.prevY = GET_Y_LPARAM(lParam);

    SetCapture(hwMain);
  }
}

// Captura el levante del botón izquierdo del ratón
void OnLButtonUpMain() {
  if(me.event == translating) {
    me.event = none;

    ReleaseCapture();
  }
}

// Captura el movimiento del ratón
void OnMouseMoveMain(LPARAM lParam) {
  if(me.event == none) {
    return;
  }

  if(!me.lightEvent) {
    if(me.event == translating) {
      obj.ApplyMatrix(GetTransMatrix(GET_X_LPARAM(lParam) - me.prevX,
                                     GET_Y_LPARAM(lParam) - me.prevY));
    }
    else {
      obj.ApplyMatrix(GetRotMatrix(GET_X_LPARAM(lParam) - me.prevX,
                                   GET_Y_LPARAM(lParam) - me.prevY));
    }

    obj.Transform();

    if(me.tnlEvent) {
      obj.InvalidateTriangles();
      obj.OrderTriangles();
      me.tnlEvent = false;
    }
  }
  else {
    if(me.event == translating) {
      lig.ApplyMatrix(GetTransMatrix(GET_X_LPARAM(lParam) - me.prevX,
                                     GET_Y_LPARAM(lParam) - me.prevY));
    }
    else {
      lig.ApplyMatrix(GetRotMatrix(GET_X_LPARAM(lParam) - me.prevX,
                                   GET_Y_LPARAM(lParam) - me.prevY));
    }

    lig.Transform();
  }

  me.prevX = GET_X_LPARAM(lParam);
  me.prevY = GET_Y_LPARAM(lParam);

  InvalidateMain();
}

// Captura el movimiento de la rueda del ratón
void OnMouseWheelMain(WPARAM wParam) {
  if(obj.IsLoaded() && !me.lightEvent) {
    if(GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
      obj.ApplyMatrix(TM_INC_SIZE);
    }
    else {
      obj.ApplyMatrix(TM_DEC_SIZE);
    }

    obj.Transform();

    if(me.tnlEvent) {
      obj.InvalidateTriangles();
      obj.OrderTriangles();
      me.tnlEvent = false;
    }

    InvalidateMain();
  }
}

// Pinta la ventana principal
void OnPaintMain() {
  RECT client;
  GetClientRect(hwMain, &client);

  PAINTSTRUCT ps;

  HDC hDC = BeginPaint(hwMain, &ps);

  if(!obj.IsLoaded()) {

    // Mostrar un mensaje si no se ha cargado un objeto
    DrawText(hDC, "Sin objeto cargado", -1, &client,
             DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  }
  else if (!mustRender) {
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP oldCanvas = (HBITMAP)SelectObject(hMemDC, canvas);
    BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right,
           ps.rcPaint.bottom, hMemDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
    SelectObject(hMemDC, oldCanvas);
    DeleteDC(hMemDC);
  }
  else {
    mustRender = false;

    // Crear un DC compatible para dibujar el objeto
    HDC hMemDC = CreateCompatibleDC(hDC);

    // Selecciono mi canvas y guardo el anterior
    HBITMAP oldCanvas = (HBITMAP)SelectObject(hMemDC, canvas);

    // Relleno el fondo de mi canvas de blanco
    FillRect(hMemDC, &client, (HBRUSH)COLOR_WINDOWFRAME);

    // Pinto el objeto
    POINT p[4];

    if(me.event == rotating && !me.lightEvent) {
      for(UINT i = 0; i < obj.Groups(); i++) {
        for(UINT j = 0; j < obj[i].Elements(); j++) {
          map.TriPoint(p, obj[i][j]);
          Polyline(hMemDC, p, 4);
        }
      }
    }
    else if(!ch.drawPhong) {

      // Flat shading
      SGroup<Triangle<PREC> > grp = obj.Tri();

      HPEN oldPen = SelectPen(hMemDC, CreatePen(PS_NULL, 0, WHITE));
      HBRUSH oldBrush = (HBRUSH)GetCurrentObject(hMemDC, OBJ_BRUSH);

      for(UINT i = 0; i < grp.Elements(); i++) {
        map.TriPoint(p, grp[i]);

        HBRUSH myBrush = CreateSolidBrush(lig.PaintRef(grp[i]));

        SelectObject(hMemDC, myBrush);
        Polygon(hMemDC, p, 4);
        DeleteObject(SelectObject(hMemDC, oldBrush));
      }

      DeleteObject(SelectObject(hMemDC, oldPen));
    }
    else {

      // Phong shading
      SetCursor(LoadCursor(NULL, IDC_WAIT));

      SGroup<Triangle<PREC> > grp = obj.Tri();
      Position tPos;

      for(UINT i = 0; i < grp.Elements(); i++) {
        map.TriPoint(p, grp[i]);

        RECT r = GetTriRect(p);

        int startY = Max(static_cast<int>(r.top), 0);
        int startX = Max(static_cast<int>(r.left), 0);
        int endY = static_cast<int>(Min(r.bottom, client.bottom));
        int endX = static_cast<int>(Min(r.right, client.right));

        for(int y = startY; y < endY; y++) {
          for(int x = startX; x < endX; x++) {
            if(InsideTriangle(x, y, p, tPos)) {
              SetPixelV(hMemDC, x, y, lig.PaintPhong(tPos, grp[i]));
            }
          }
        }
      }

      SetCursor(LoadCursor(NULL, IDC_ARROW));

      ch.drawPhong = false;
    }

    // Pintado del centro
    if(!me.lightEvent && ch.drawCenter) {
      const LONG zeroX = client.right / 2;
      const LONG zeroY = client.bottom / 2;

      HPEN oldPen = (HPEN)SelectObject(hMemDC, CreatePen(PS_SOLID, 0, CENTER_COLOR));

      MoveToEx(hMemDC, zeroX + CIRCLE_RADIUS, zeroY, NULL);
      AngleArc(hMemDC, zeroX, zeroY, CIRCLE_RADIUS, 0, 360);

      p[0] = map.GetPoint(obj.Mat().Transform(Point<PREC>(0, 0, 0)));

      MoveToEx(hMemDC, p[0].x + CIRCLE_RADIUS, p[0].y, NULL);
      AngleArc(hMemDC, p[0].x, p[0].y, CIRCLE_RADIUS, 0, 360);

      MoveToEx(hMemDC, zeroX, zeroY, NULL);
      LineTo(hMemDC, p[0].x, p[0].y);

      DeleteObject(SelectObject(hMemDC, oldPen));
    }

    // Pintado de la luz
    if(!me.lightEvent && ch.drawLight) {
      HBRUSH oldBrush = (HBRUSH)SelectObject(hMemDC, CreateSolidBrush(lig.Color()));

      p[0] = map.GetPoint(lig.Pos());

      MoveToEx(hMemDC, p[0].x + CIRCLE_RADIUS, p[0].y, NULL);

      BeginPath(hMemDC);
      LineTo(hMemDC, p[0].x, p[0].y + CIRCLE_RADIUS);
      LineTo(hMemDC, p[0].x - CIRCLE_RADIUS, p[0].y);
      LineTo(hMemDC, p[0].x, p[0].y - CIRCLE_RADIUS);
      EndPath(hMemDC);

      StrokeAndFillPath(hMemDC);

      DeleteObject(SelectObject(hMemDC, oldBrush));
    }

    // Pintado de actividad de la luz
    if(me.lightEvent) {
      HPEN oldPen = (HPEN)SelectObject(hMemDC, CreatePen(PS_SOLID, 0, ALIGHT_COLOR));
      HBRUSH oldBrush = (HBRUSH)SelectObject(hMemDC, CreateSolidBrush(lig.Color()));

      const LONG zeroX = client.right / 2;
      const LONG zeroY = client.bottom / 2;

      MoveToEx(hMemDC, zeroX + CIRCLE_RADIUS, zeroY, NULL);
      AngleArc(hMemDC, zeroX, zeroY, CIRCLE_RADIUS, 0, 360);

      p[0] = map.GetPoint(lig.Pos());

      MoveToEx(hMemDC, zeroX, zeroY, NULL);
      LineTo(hMemDC, p[0].x, p[0].y);

      MoveToEx(hMemDC, p[0].x + ACTIVE_RADIUS, p[0].y, NULL);

      BeginPath(hMemDC);
      LineTo(hMemDC, p[0].x, p[0].y + ACTIVE_RADIUS);
      LineTo(hMemDC, p[0].x - ACTIVE_RADIUS, p[0].y);
      LineTo(hMemDC, p[0].x, p[0].y - ACTIVE_RADIUS);
      EndPath(hMemDC);

      StrokeAndFillPath(hMemDC);

      DeleteObject(SelectObject(hMemDC, oldBrush));
      DeleteObject(SelectObject(hMemDC, oldPen));
    }

    // Pinto el resultado en la pantalla principal
    BitBlt(hDC, 0, 0, client.right, client.bottom, hMemDC, 0, 0, SRCCOPY);

    // Elimino las estructuras temporales
    SelectObject(hMemDC, oldCanvas);
    DeleteDC(hMemDC);
  }

  EndPaint(hwMain, &ps);
}

// Captura el clic del botón derecho del ratón
void OnRButtonDownMain(LPARAM lParam) {
  if(obj.IsLoaded() && me.event == none) {
    me.event = rotating;
    me.prevX = GET_X_LPARAM(lParam);
    me.prevY = GET_Y_LPARAM(lParam);

    SetCapture(hwMain);
  }
}

// Captura el levante del botón derecho del ratón
void OnRButtonUpMain() {
  if(me.event == rotating) {
    me.event = none;
    ReleaseCapture();

    if(!me.lightEvent) {
      obj.OrderTriangles();
      InvalidateMain();
    }
  }
}

// Maneja el cambio de tamaño de la ventana principal
void OnSizeMain(WPARAM wParam, LPARAM lParam) {
  if(wParam != SIZE_MINIMIZED) {
    if(currClientSize != lParam) {
      currClientSize = lParam;

      RECT client;

      GetClientRect(hwMain, &client);
      map.SetScreen(client);
      mustRender = true;
      InvalidateRect(hwMain, NULL, TRUE);
    }
  }
}

// Muestra un mensaje de error
void PopError(const PC_CHAR msg) {
  MessageBox(NULL, msg, "Error", MB_ICONERROR | MB_OK);
}

// Realiza los métodos posteriores a la TNL
void PostTNL() {
  obj.OrderTriangles();

  me.tnlEvent = true;

  InvalidateMain();
}

// Inicia la renderización con Phong shading
void RenderPhong() {
  if(obj.IsLoaded()) {
    ch.drawPhong = true;

    InvalidateMain();
  }
}

// Muestra o esconde el diálogo de ayuda
void ToggleAyuda() {
  MENUITEMINFO mii;

  ZeroMemory(&mii, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;

  GetMenuItemInfo(GetMenu(hwMain), IDM_CONTROLES, FALSE, &mii);

  if(mii.fState == MFS_CHECKED) {
    mii.fState = MFS_UNCHECKED;
    ShowWindow(hwAyuda, SW_HIDE);
  }
  else {
    mii.fState = MFS_CHECKED;
    ShowWindow(hwAyuda, SW_SHOW);
  }

  SetMenuItemInfo(GetMenu(hwMain), IDM_CONTROLES, FALSE, &mii);
}

// Muestra o esconde el indicador del centro de la imágen
void ToggleCenterInd() {
  if(ch.drawCenter) {
    CheckMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_CENTRO, MF_UNCHECKED);
  }
  else {
    CheckMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_CENTRO, MF_CHECKED);
  }

  ch.drawCenter = !ch.drawCenter;

  InvalidateMain();
}

// Muestra o esconde el indicador de la luz
void ToggleLightInd() {
  if(ch.drawLight) {
    CheckMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_LUZ, MF_UNCHECKED);
  }
  else {
    CheckMenuItem(GetMenu(hwMain), IDM_INDICADOR_DE_LUZ, MF_CHECKED);
  }

  ch.drawLight = !ch.drawLight;

  InvalidateMain();
}

// Muestra o esconde el diálogo de matriz arbitraria
void ToggleMatrizArbitraria() {
  MENUITEMINFO mii;

  ZeroMemory(&mii, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;

  GetMenuItemInfo(GetMenu(hwMain), IDM_MATRIZ_ARBITRARIA, FALSE, &mii);

  if(mii.fState == MFS_CHECKED) {
    mii.fState = MFS_UNCHECKED;
    ShowWindow(hwMA, SW_HIDE);
  }
  else {
    mii.fState = MFS_CHECKED;
    ShowWindow(hwMA, SW_SHOW);
  }

  SetMenuItemInfo(GetMenu(hwMain), IDM_MATRIZ_ARBITRARIA, FALSE, &mii);
}

// Muestra o esconde el diálogo de propiedades de la luz
void TogglePropLuz() {
  MENUITEMINFO mii;

  ZeroMemory(&mii, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;

  GetMenuItemInfo(GetMenu(hwMain), IDM_PROP_LUZ, FALSE, &mii);

  if(mii.fState == MFS_CHECKED) {
    mii.fState = MFS_UNCHECKED;
    ShowWindow(hwPLuz, SW_HIDE);
  }
  else {
    mii.fState = MFS_CHECKED;
    ShowWindow(hwPLuz, SW_SHOW);
  }

  SetMenuItemInfo(GetMenu(hwMain), IDM_PROP_LUZ, FALSE, &mii);
}

// Muestra o esconde el diálogo de propiedades del objeto
void TogglePropObjeto() {
  MENUITEMINFO mii;

  ZeroMemory(&mii, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;

  GetMenuItemInfo(GetMenu(hwMain), IDM_PROP_OBJETO, FALSE, &mii);

  if(mii.fState == MFS_CHECKED) {
    mii.fState = MFS_UNCHECKED;
    ShowWindow(hwPObj, SW_HIDE);
  }
  else {
    mii.fState = MFS_CHECKED;
    ShowWindow(hwPObj, SW_SHOW);
  }

  SetMenuItemInfo(GetMenu(hwMain), IDM_PROP_OBJETO, FALSE, &mii);
}

// Muestra o esconde el diálogo de transformaciones
void ToggleTransformaciones() {
  MENUITEMINFO mii;

  ZeroMemory(&mii, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_STATE;

  GetMenuItemInfo(GetMenu(hwMain), IDM_TRANSFORMACIONES, FALSE, &mii);

  if(mii.fState == MFS_CHECKED) {
    mii.fState = MFS_UNCHECKED;
    ShowWindow(hwTrans, SW_HIDE);
  }
  else {
    mii.fState = MFS_CHECKED;
    ShowWindow(hwTrans, SW_SHOW);
  }

  SetMenuItemInfo(GetMenu(hwMain), IDM_TRANSFORMACIONES, FALSE, &mii);
}

// ----------------------------------------------------------------------------
// Diálogo de Transformaciones ------------------------------------------------
// ----------------------------------------------------------------------------

// Restaura la figura original
void CmdRestaurarTrans() {
  if(me.tnlEvent) {
    obj.Transform();
    obj.InvalidateTriangles();
    obj.OrderTriangles();
    me.tnlEvent = false;
  }

  InvalidateMain();
}

// Devuelve el valor de tapping
PREC GetTapperValue() {
  return SendMessage(GetDlgItem(hwTrans, IDC_TAPPER_SLIDER), TBM_GETPOS, 0, 0) / 100.;
}

PREC GetTwistValue() {
  return (SendMessage(GetDlgItem(hwTrans, IDC_TWIST_SLIDER), TBM_GETPOS, 0, 0) - 100) / 100.;
}

// Maneja las track bars
void OnHScrollTrans(WPARAM wParam, LPARAM lParam) {
  if(GetDlgCtrlID((HWND) lParam) == IDC_TAPPER_SLIDER) {
    UpdateTapperValue();
  }
  else {
    UpdateTwistValue();
  }
}

// Inicializa el diálogo
void OnInitDialogTrans(HWND hWnd) {
  // Setea el foco en el primer botón del diálogo
  SetFocus(GetDlgItem(hWnd, IDC_TAPPER_ATRAS));

  // Setea el TrackBar de Tapping
  SendMessage(GetDlgItem(hWnd, IDC_TAPPER_SLIDER), TBM_SETRANGE, (WPARAM) TRUE,
              MAKELONG(0, 200));
  SendMessage(GetDlgItem(hWnd, IDC_TAPPER_SLIDER), TBM_SETPAGESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_TAPPER_SLIDER), TBM_SETLINESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_TAPPER_SLIDER), TBM_SETPOS, (WPARAM) TRUE,
              (LPARAM) 80);

  // Setea el TrackBar de Twisting
  SendMessage(GetDlgItem(hWnd, IDC_TWIST_SLIDER), TBM_SETRANGE, (WPARAM) TRUE,
              MAKELONG(0, 200));
  SendMessage(GetDlgItem(hWnd, IDC_TWIST_SLIDER), TBM_SETPAGESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_TWIST_SLIDER), TBM_SETLINESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_TWIST_SLIDER), TBM_SETPOS, (WPARAM) TRUE,
              (LPARAM) 110);

  // Inicializa el valor de Tapping
  Edit_SetText(GetDlgItem(hWnd, IDC_TAPPER_VALUE), "0.80");

  // Inicializa el valor de Twisting
  Edit_SetText(GetDlgItem(hWnd, IDC_TWIST_VALUE), "0.10");
}

// Maneja los mensajes al diálogo
BOOL CALLBACK TransProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_INITDIALOG:
    OnInitDialogTrans(hWnd);
    break;

  case WM_CLOSE:
    ToggleTransformaciones();
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_TAPPER_ATRAS:
      obj.TapperZ2(GetTapperValue());
      PostTNL();
      break;
    case IDC_TAPPER_ADELANTE:
      obj.TapperZ1(GetTapperValue());
      PostTNL();
      break;
    case IDC_TAPPER_ARRIBA:
      obj.TapperY1(GetTapperValue());
      PostTNL();
      break;
    case IDC_TAPPER_ABAJO:
      obj.TapperY2(GetTapperValue());
      PostTNL();
      break;
    case IDC_TAPPER_IZQUIERDA:
      obj.TapperX2(GetTapperValue());
      PostTNL();
      break;
    case IDC_TAPPER_DERECHA:
      obj.TapperX1(GetTapperValue());
      PostTNL();
      break;

    case IDC_TWIST_ATRAS:
      obj.TwistZ1(GetTwistValue());
      PostTNL();
      break;
    case IDC_TWIST_ADELANTE:
      obj.TwistZ2(GetTwistValue());
      PostTNL();
      break;
    case IDC_TWIST_ARRIBA:
      obj.TwistY2(GetTwistValue());
      PostTNL();
      break;
    case IDC_TWIST_ABAJO:
      obj.TwistY1(GetTwistValue());
      PostTNL();
      break;
    case IDC_TWIST_IZQUIERDA:
      obj.TwistX1(GetTwistValue());
      PostTNL();
      break;
    case IDC_TWIST_DERECHA:
      obj.TwistX2(GetTwistValue());
      PostTNL();
      break;

    case IDC_TNL_RESTAURAR:
      CmdRestaurarTrans();
      break;

    default:
      return FALSE;
    }
    break;

  case WM_HSCROLL:
    OnHScrollTrans(wParam, lParam);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

// Actualiza el diálogo de propiedades del objeto
void UpdatePropObject() {

  // Modificar la lista de grupos
  SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST), LB_RESETCONTENT, 0, 0);

  char textBuf[39];

  for(UINT i = 0; i < obj.Groups(); i++) {
    sprintf(textBuf, "Grupo %d", i + 1);
    SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST), LB_ADDSTRING, 0, (LPARAM)textBuf);
  }

  SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST), LB_SETCURSEL, 0, 0);

  UpdateGrpProp();
}

// Actualiza el valor de Tapper
void UpdateTapperValue() {
  FLOAT val = SendMessage(GetDlgItem(hwTrans, IDC_TAPPER_SLIDER), TBM_GETPOS, 0, 0) / 100.;

  char textBuf[5];

  sprintf(textBuf, "%.2f", val);

  Edit_SetText(GetDlgItem(hwTrans, IDC_TAPPER_VALUE), textBuf);
}

// Actualiza el valor de Twist
void UpdateTwistValue() {
  FLOAT val = (SendMessage(GetDlgItem(hwTrans, IDC_TWIST_SLIDER), TBM_GETPOS, 0, 0) - 100) / 100.;

  char textBuf[6];

  sprintf(textBuf, "%.2f", val);

  Edit_SetText(GetDlgItem(hwTrans, IDC_TWIST_VALUE), textBuf);
}

// ----------------------------------------------------------------------------
// Diálogo de Matriz Arbitraria -----------------------------------------------
// ----------------------------------------------------------------------------

// Aplicar la matriz arbitraria
void ApplyMA() {
  const UINT BUF_SIZE = 20;
  char textBuf[BUF_SIZE];

  // ---
  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V00), textBuf, BUF_SIZE);
  PREC v00 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v00);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V00), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V01), textBuf, BUF_SIZE);
  PREC v01 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v01);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V01), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V02), textBuf, BUF_SIZE);
  PREC v02 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v02);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V02), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V03), textBuf, BUF_SIZE);
  PREC v03 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v03);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V03), textBuf);

  // ---
  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V10), textBuf, BUF_SIZE);
  PREC v10 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v10);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V10), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V11), textBuf, BUF_SIZE);
  PREC v11 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v11);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V11), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V12), textBuf, BUF_SIZE);
  PREC v12 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v12);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V12), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V13), textBuf, BUF_SIZE);
  PREC v13 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v13);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V13), textBuf);

  // ---
  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V20), textBuf, BUF_SIZE);
  PREC v20 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v20);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V20), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V21), textBuf, BUF_SIZE);
  PREC v21 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v21);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V21), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V22), textBuf, BUF_SIZE);
  PREC v22 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v22);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V22), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V23), textBuf, BUF_SIZE);
  PREC v23 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v23);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V23), textBuf);

  // ---
  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V30), textBuf, BUF_SIZE);
  PREC v30 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v30);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V30), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V31), textBuf, BUF_SIZE);
  PREC v31 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v31);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V31), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V32), textBuf, BUF_SIZE);
  PREC v32 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v32);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V32), textBuf);

  Edit_GetText(GetDlgItem(hwMA, IDC_MA_V33), textBuf, BUF_SIZE);
  PREC v33 = static_cast<PREC>(atof(textBuf));
  sprintf(textBuf, "%f", v33);
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V33), textBuf);

  TMatrix<PREC> mat(v00, v01, v02, v03,
                    v10, v11, v12, v13,
                    v20, v21, v22, v23,
                    v30, v31, v32, v33);

  obj.ApplyMatrix(mat);
  obj.Transform();

  if(me.tnlEvent) {
    obj.InvalidateTriangles();
    me.tnlEvent = false;
  }

  obj.OrderTriangles();

  InvalidateMain();
}

// Restaura la matriz a la identidad
void CmdIdentidadMA() {
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V00), "1.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V01), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V02), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V03), "0.000000");

  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V10), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V11), "1.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V12), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V13), "0.000000");

  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V20), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V21), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V22), "1.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V23), "0.000000");

  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V30), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V31), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V32), "0.000000");
  Edit_SetText(GetDlgItem(hwMA, IDC_MA_V33), "1.000000");
}

// Maneja los mensajes del diálogo
BOOL CALLBACK MAProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_INITDIALOG:
    OnInitDialogMA(hWnd);
    break;

  case WM_CLOSE:
    ToggleMatrizArbitraria();
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_MA_APLICAR:
      ApplyMA();
      break;

    case IDC_MA_IDENTIDAD:
      CmdIdentidadMA();
      break;

    default:
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

// Inicializa el diálogo
void OnInitDialogMA(HWND hWnd) {
  // Setea el foco en el botón del diálogo
  SetFocus(GetDlgItem(hWnd, IDC_MA_APLICAR));

  // Inicializa la matriz como identidad
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V00), "1.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V01), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V02), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V03), "0.000000");

  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V10), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V11), "1.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V12), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V13), "0.000000");

  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V20), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V21), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V22), "1.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V23), "0.000000");

  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V30), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V31), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V32), "0.000000");
  Edit_SetText(GetDlgItem(hWnd, IDC_MA_V33), "1.000000");
}

// ----------------------------------------------------------------------------
// Diálogo de Propiedades del Objeto ------------------------------------------
// ----------------------------------------------------------------------------

// Cambia el color de un grupo en el objeto
void CmdCambiarColorPObj() {
  LONG index = (LONG)SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST),
                                 LB_GETCURSEL, 0, 0);

  if(index == LB_ERR || !obj.IsLoaded()) {
    return;
  }

  static COLORREF defColor[16] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA,
                                  DARKRED, DARKGREEN, DARKBLUE, DARKYELLOW,
                                  DARKCYAN, DARKMAGENTA, WHITE, LIGHTGRAY,
                                  DARKGRAY, BLACK};


  CHOOSECOLOR cc;

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwPObj;
  cc.hInstance = NULL;
  cc.rgbResult = obj.Color(index);
  cc.lpCustColors = defColor;
  cc.Flags = CC_RGBINIT;
  cc.lCustData = 0;
  cc.lpfnHook = NULL;
  cc.lpTemplateName = NULL;

  if(!ChooseColor(&cc)) {
    return;
  }

  obj.Color(index, cc.rgbResult);

  InvalidateRect(GetDlgItem(hwPObj, IDC_POBJ_COLOR), NULL, FALSE);
  InvalidateMain();
}

// Maneja los mensajes de la ventana de color
LRESULT CALLBACK ObjColorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_PAINT:
    OnPaintObjColor();
    break;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

// Maneja el movimiento del trackbar de reflexión
void OnHScrollPObj() {
  LONG index = (LONG)SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST),
                                 LB_GETCURSEL, 0, 0);

  if(index == LB_ERR || !obj.IsLoaded()) {
    return;
  }

  FLOAT val = SendMessage(GetDlgItem(hwPObj, IDC_POBJ_REFSLIDER), TBM_GETPOS, 0, 0) / 100.;

  char textBuf[5];

  sprintf(textBuf, "%.2f", val);

  Edit_SetText(GetDlgItem(hwPObj, IDC_POBJ_REFLECTION), textBuf);

  obj.Reflection(index, val);

  InvalidateMain();
}

// Inicializa el diálogo
void OnInitDialogPObj(HWND hWnd) {
  SetFocus(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST));

  SendMessage(GetDlgItem(hWnd, IDC_POBJ_REFSLIDER), TBM_SETRANGE, (WPARAM) TRUE,
              MAKELONG(0, 100));
  SendMessage(GetDlgItem(hWnd, IDC_POBJ_REFSLIDER), TBM_SETPAGESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_POBJ_REFSLIDER), TBM_SETLINESIZE, 0,
              (LPARAM) 10);
}

// Pinta el indicador de color
void OnPaintObjColor() {
  RECT client;

  GetClientRect(GetDlgItem(hwPObj, IDC_POBJ_COLOR), &client);

  PAINTSTRUCT ps;

  HDC hDC = BeginPaint(GetDlgItem(hwPObj, IDC_POBJ_COLOR), &ps);

  LONG index = (LONG)SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST),
                                 LB_GETCURSEL, 0, 0);

  if(index == LB_ERR || !obj.IsLoaded()) {
    POINT p[5];

    p[0].x = p[3].x = p[4].x = 0;
    p[1].x = p[2].x = client.right - 1;

    p[0].y = p[1].y = p[4].y = 0;
    p[2].y = p[3].y = client.bottom - 1;

    Polyline(hDC, p, 5);

    MoveToEx(hDC, 0, 0, NULL);
    LineTo(hDC, client.right - 1, client.bottom - 1);
    MoveToEx(hDC, client.right - 1, 0, NULL);
    LineTo(hDC, 0, client.bottom - 1);
  }
  else {
    HBRUSH myBrush = CreateSolidBrush(obj.Color(index));

    myBrush = (HBRUSH)SelectObject(hDC, myBrush);

    Rectangle(hDC, 0, 0, client.right - 1, client.bottom - 1);

    DeleteObject(SelectObject(hDC, myBrush));
  }

  EndPaint(GetDlgItem(hwPObj, IDC_POBJ_COLOR), &ps);
}

// Maneja los mensajes del diálogo
BOOL CALLBACK PObjProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_INITDIALOG:
    OnInitDialogPObj(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_POBJ_GRPLIST:
      switch(HIWORD(wParam)) {
      case LBN_SELCHANGE:
        UpdateGrpProp();
        break;

      default:
        return FALSE;
      }
      break;

    case IDC_POBJ_CAMBIARCOLOR:
      CmdCambiarColorPObj();
      break;

    default:
      return FALSE;
    }
    break;

  case WM_CLOSE:
    TogglePropObjeto();
    break;

  case WM_HSCROLL:
    OnHScrollPObj();
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

// Actualiza las propiedades del grupo
void UpdateGrpProp() {

  // Actualiza el recuadro de color
  InvalidateRect(GetDlgItem(hwPObj, IDC_POBJ_COLOR), NULL, FALSE);

  // Verifica que la selección sea correcta
  LONG index = (LONG)SendMessage(GetDlgItem(hwPObj, IDC_POBJ_GRPLIST),
                                 LB_GETCURSEL, 0, 0);

  if(index == LB_ERR || !obj.IsLoaded()) {
    // No hay selección
    SetDlgItemText(hwPObj, IDC_POBJ_CTRIANGULOS, "0");
    SetDlgItemText(hwPObj, IDC_POBJ_REFLECTION, "0.00");
    return;
  }

  // Actualiza la cantidad de triángulos
  SetDlgItemInt(hwPObj, IDC_POBJ_CTRIANGULOS, obj[index].Elements(), FALSE);

  // Actualiza el índice de reflección
  FLOAT ref = obj.Reflection(index);

  char textBuf[5];

  sprintf(textBuf, "%.2f", ref);

  SetDlgItemText(hwPObj, IDC_POBJ_REFLECTION, textBuf);

  // Actualiza el track bar de reflexión
  SendMessage(GetDlgItem(hwPObj, IDC_POBJ_REFSLIDER), TBM_SETPOS, (WPARAM) TRUE,
              (LPARAM)(ref * 100));
}

// ----------------------------------------------------------------------------
// Diálogo de Propiedades de la Luz -------------------------------------------
// ----------------------------------------------------------------------------

// Cambia el color de la luz
void CmdCambiarColorPLuz() {
  static COLORREF defColor[16] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA,
                                  DARKRED, DARKGREEN, DARKBLUE, DARKYELLOW,
                                  DARKCYAN, DARKMAGENTA, WHITE, LIGHTGRAY,
                                  DARKGRAY, BLACK};


  CHOOSECOLOR cc;

  cc.lStructSize = sizeof(cc);
  cc.hwndOwner = hwPLuz;
  cc.hInstance = NULL;
  cc.rgbResult = lig.Color();
  cc.lpCustColors = defColor;
  cc.Flags = CC_RGBINIT;
  cc.lCustData = 0;
  cc.lpfnHook = NULL;
  cc.lpTemplateName = NULL;

  if(!ChooseColor(&cc)) {
    return;
  }

  lig.Color(cc.rgbResult);

  InvalidateRect(GetDlgItem(hwPLuz, IDC_PLUZ_COLOR), NULL, FALSE);
  InvalidateMain();
}

// Maneja los mensajes de la ventana de color
LRESULT CALLBACK LuzColorProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_PAINT:
    OnPaintLuzColor();
    break;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}

// Maneja el movimiento del trackbar de ambiente
void OnHScrollPLuz() {
  FLOAT val = SendMessage(GetDlgItem(hwPLuz, IDC_PLUZ_AMBSLIDER), TBM_GETPOS, 0, 0) / 100.;

  char textBuf[5];

  sprintf(textBuf, "%.2f", val);

  SetDlgItemText(hwPLuz, IDC_PLUZ_AMBIENCE, textBuf);

  lig.Ambience(val);

  InvalidateMain();
}

// Inicializa el diálogo
void OnInitDialogPLuz(HWND hWnd) {
  SetFocus(GetDlgItem(hwPLuz, IDC_PLUZ_CAMBIARCOLOR));

  SendMessage(GetDlgItem(hWnd, IDC_PLUZ_AMBSLIDER), TBM_SETRANGE, (WPARAM) TRUE,
              MAKELONG(0, 100));
  SendMessage(GetDlgItem(hWnd, IDC_PLUZ_AMBSLIDER), TBM_SETPAGESIZE, 0,
              (LPARAM) 10);
  SendMessage(GetDlgItem(hWnd, IDC_PLUZ_AMBSLIDER), TBM_SETLINESIZE, 0,
              (LPARAM) 10);

  FLOAT amb = lig.Ambience();

  char textBuf[5];

  sprintf(textBuf, "%.2f", amb);

  SetDlgItemText(hWnd, IDC_PLUZ_AMBIENCE, textBuf);

  SendMessage(GetDlgItem(hWnd, IDC_PLUZ_AMBSLIDER), TBM_SETPOS, (WPARAM) TRUE,
              (LPARAM)(amb * 100));
}

// Pinta el indicador de color
void OnPaintLuzColor() {
  RECT client;

  GetClientRect(GetDlgItem(hwPLuz, IDC_PLUZ_COLOR), &client);

  PAINTSTRUCT ps;

  HDC hDC = BeginPaint(GetDlgItem(hwPLuz, IDC_PLUZ_COLOR), &ps);

  HBRUSH myBrush = CreateSolidBrush(lig.Color());

  myBrush = (HBRUSH)SelectObject(hDC, myBrush);

  Rectangle(hDC, 0, 0, client.right - 1, client.bottom - 1);

  DeleteObject(SelectObject(hDC, myBrush));

  EndPaint(GetDlgItem(hwPLuz, IDC_PLUZ_COLOR), &ps);
}

// Maneja los mensajes del diálogo
BOOL CALLBACK PLuzProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_INITDIALOG:
    OnInitDialogPLuz(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_PLUZ_CAMBIARCOLOR:
      CmdCambiarColorPLuz();
      break;

    default:
      return FALSE;
    }
    break;

  case WM_CLOSE:
    TogglePropLuz();
    break;

  case WM_HSCROLL:
    OnHScrollPLuz();
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

// Actualiza las propiedades de la luz
void UpdateLuzProp() {
  // Actualiza el recuadro de color
  InvalidateRect(GetDlgItem(hwPLuz, IDC_PLUZ_COLOR), NULL, FALSE);

  // Actualiza los valores de la luz ambiente
  FLOAT amb = lig.Ambience();

  char textBuf[5];

  sprintf(textBuf, "%.2f", amb);

  SetDlgItemText(hwPLuz, IDC_PLUZ_AMBIENCE, textBuf);

  // Actualiza el track bar de ambiente
  SendMessage(GetDlgItem(hwPLuz, IDC_PLUZ_AMBSLIDER), TBM_SETPOS, (WPARAM) TRUE,
              (LPARAM)(amb * 100));
}

// ----------------------------------------------------------------------------
// Diálogo de Ayuda -----------------------------------------------------------
// ----------------------------------------------------------------------------

// Maneja los mensajes del diálogo
BOOL CALLBACK AyudaProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_CLOSE:
    ToggleAyuda();
    break;

  default:
    return FALSE;
  }

  return TRUE;
}
