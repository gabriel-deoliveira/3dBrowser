/*
 * File:   list.h
 * Author: Gabriel de Oliveira
 *
 * Created on 4 de octubre de 2011, 21:32
 */

#include "base.h"

#ifndef LIST_H
#define	LIST_H

// ----------------------------------------------------------------------------
// Declaraci�n de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada List
template <class T>
class List {

  // Estructuras
  struct Node {
    SPtr<T> element;
    Node *next;
  };

public:

  // Constructor
  List(): _root(NULL), _elements(0), _current(NULL) {}

  // Constructor de copia
  List(const List<T> &obj): _root(NULL), _elements(obj._elements),
    _current(NULL) {
    Copy(_root, obj._root);
  }

  // Destructor
  ~List();

  // Operador de asignaci�n
  const List<T> &operator=(List<T> obj);

  // M�todo Add
  void Add(const SPtr<T> &element);

  // M�todo Destroy
  void Destroy();

  // M�todos de navegaci�n
  const SPtr<T> &First() const;
  const SPtr<T> &Next() const;
  const bool HasNext() const {
    return (_current -> next != NULL)? true : false;
  }

  // M�todos de consulta
  const bool IsEmpty() const {
    return (_root == NULL)? true : false;
  }

  // M�todo Swap
  void Swap(List<T> &obj);

private:

  // M�todo Copy recursivo
  void Copy(Node *&des, const Node *&ori);

  // M�todo Destroy
  void Destroy(const Node *n);

  // Miembros
  Node *_root;
  UINT _elements;

  mutable Node *_current;
};

// ----------------------------------------------------------------------------
// Definici�n de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada List

// Destructor
template <class T>
List<T>::~List() {
  Destroy();
}

// Operador de asignaci�n
template <class T>
const List<T> &List<T>::operator=(List<T> obj) {
  Swap(obj);
  return *this;
}

// M�todo Add
template <class T>
void List<T>::Add(const SPtr<T> &element) {
  // Crear un nodo nuevo
  Node *n = new Node;

  // Agregar el elemento
  n -> element = element;

  // Vincularlo a la lista
  n -> next = _root;
  _root = n;
  ++_elements;
}

// M�todo Destroy
template <class T>
void List<T>::Destroy() {
  Destroy(_root);
  _root = NULL;
  _elements = 0;
}

// M�todos de navegaci�n
template <class T>
const SPtr<T> &List<T>::First() const {
  _current = _root;
  return _current -> element;
}

template <class T>
const SPtr<T> &List<T>::Next() const {
  _current = _current -> next;
  return _current -> element;
}

// M�todo Swap
template <class T>
void List<T>::Swap(List<T> &obj) {
  Node * const t_root = obj._root;
  const UINT t_elements = obj._elements;
  Node * const t_current = obj._current;

  obj._root = _root;
  obj._elements = _elements;
  obj._current = _current;

  _root = t_root;
  _elements = t_elements;
  _current = t_current;
}

// M�todo Copy recursivo
template <class T>
void List<T>::Copy(Node *&des, const Node *&ori) {
  if(ori != NULL) {
    des = new Node;
    des -> element = ori -> element;
    Copy(des -> next, ori -> next);
  }
  else {
    des -> next = NULL;
  }
}

// M�todo Destroy
template <class T>
void List<T>::Destroy(const Node *n) {
  if(n != NULL) {
    Destroy(n -> next);
    delete n;
  }
}

#endif	/* LIST_H */
