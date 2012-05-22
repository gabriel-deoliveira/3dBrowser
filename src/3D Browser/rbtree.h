/*
 * File:   rbtree.h
 * Author: Gabriel de Oliveira
 *
 * Created on 24 de septiembre de 2011, 12:22
 */

#ifndef RBTREE_H
#define	RBTREE_H

#include "base.h"
#include "sptr.h"
#include "sgroup.h"

// ----------------------------------------------------------------------------
// Declaración de clases ------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Red Black Tree
template <class T>
class RBTree {
private:

  // Enumeraciones
  enum NColor {red, black};

  // Estructuras
  struct Node {
    SPtr<T> element;
    NColor color;
    Node *left;
    Node *right;
    Node *parent;
  };

public:

  // Constructor
  RBTree(): _root(NULL), _elements(0) {}

  // TODO: Constructor de copia debe duplicar la estructura

  // Destructor
  ~RBTree();

  // TODO: Operador de asignación debe duplicar la estructura

  // Método Add
  void Add(const SPtr<T> &element);

  // Método Destroy
  void Destroy();

  // Método InOrder
  const SGroup<T> InOrder() const;

private:

  // Método InOrder (recursivo)
  void InOrder(const Node *p, UINT &cnt, SGroup<T> &grp) const;

  // Métodos de rotación
  void LeftRotate(Node *n);
  void RightRotate(Node *n);

  // Método Destroy
  void Destroy(const Node *n);

  // Miembros
  Node *_root;
  UINT _elements;
};

// ----------------------------------------------------------------------------
// Definición de clases -------------------------------------------------------
// ----------------------------------------------------------------------------

// Clase parametrizada Red Black Tree

// Destructor
template <class T>
RBTree<T>::~RBTree() {
  Destroy(_root);
}

// Método Add
template <class T>
void RBTree<T>::Add(const SPtr<T> &element) {

  // Crear un nodo nuevo
  Node *n = new Node; //TODO: Comprobar excepciones

  // Asignarle propiedades
  n -> element = element;
  n -> color = red;
  n -> left = n -> right = n -> parent = NULL;

  // Insertar el nuevo nodo en el árbol
  if(_root == NULL) {
    _root = n;
  }
  else {
    Node *t = _root;
    bool isLeaf;

    do {
      isLeaf = false;

      if(*(t -> element) > *(n -> element) && (t -> left) != NULL) {
        t = t -> left;
      }
      else if(*(t -> element) <= *(n -> element) && (t -> right) != NULL) {
        t = t -> right;
      }
      else {
        isLeaf = true;
      }
    } while(!isLeaf);

    n -> parent = t;

    if(*(t -> element) > *(n -> element)) {
      t -> left = n;
    }
    else {
      t -> right = n;
    }
  }

  ++_elements;

  // Verificar las propiedades del árbol RB
  while((n != _root) && (n -> parent -> color == red)) {
    Node *u;

    if(n -> parent == n -> parent -> parent -> left) {
      u = n -> parent -> parent -> right;

      if(u != NULL && u -> color == red) {
        n -> parent -> color = black;
        u -> color = black;
        n -> parent -> parent -> color = red;
        n = n -> parent -> parent;
      }
      else {
        if(n == n -> parent -> right) {
          n = n -> parent;
          LeftRotate(n);
        }
        n -> parent -> color = black;
        n -> parent -> parent -> color = red;
        RightRotate(n -> parent -> parent);
      }
    }
    else {
      u = n -> parent -> parent -> left;

      if(u != NULL && u -> color == red) {
        n -> parent -> color = black;
        u -> color = black;
        n -> parent -> parent -> color = red;
        n = n -> parent -> parent;
      }
      else {
        if(n == n -> parent -> left) {
          n = n -> parent;
          RightRotate(n);
        }
        n -> parent -> color = black;
        n -> parent -> parent -> color = red;
        LeftRotate(n -> parent -> parent);
      }
    }
  }

  _root -> color = black;
}

// Método Destroy
template <class T>
void RBTree<T>::Destroy() {
  Destroy(_root);
  _root = NULL;
  _elements = 0;
}

// Método InOrder
template <class T>
const SGroup<T> RBTree<T>::InOrder() const {
  if(_elements == 0) {
    return SGroup<T>();
  }

  SPtr<T> *p = new SPtr<T>[_elements];
  SGroup<T> grp(SArr<SPtr<T> >(p), _elements);

  UINT cnt = 0;

  InOrder(_root, cnt, grp);

  return grp;
}

// Método InOrder (recursivo)
template <class T>
void RBTree<T>::InOrder(const Node *p, UINT &cnt, SGroup<T> &grp) const {
  if(p == NULL) {
    return;
  }

  InOrder(p -> left, cnt, grp);
  grp[cnt++] = p -> element;
  InOrder(p -> right, cnt, grp);
}

// Métodos de rotación
template <class T>
void RBTree<T>::LeftRotate(Node *n) {
  Node *p = n -> right;

  n -> right = p -> left;
  if(p -> left != NULL) {
    p -> left -> parent = n;
  }

  p -> parent = n -> parent;

  if(n -> parent == NULL) {
    _root = p;
  }
  else {
    if(n == (n -> parent) -> left) {
      n -> parent -> left = p;
    }
    else {
      n -> parent -> right = p;
    }
  }

  p -> left = n;
  n -> parent = p;
}

template <class T>
void RBTree<T>::RightRotate(Node *n) {
  Node *p = n -> left;

  n -> left = p -> right;
  if(p -> right != NULL) {
    p -> right -> parent = n;
  }

  p -> parent = n -> parent;

  if(n -> parent == NULL) {
    _root = p;
  }
  else {
    if(n == (n -> parent) -> right) {
      n -> parent -> right = p;
    }
    else {
      n -> parent -> left = p;
    }
  }

  p -> right = n;
  n -> parent = p;
}

// Método Destroy
template <class T>
void RBTree<T>::Destroy(const Node *n) {
  if(n != NULL) {
    Destroy(n -> left);
    Destroy(n -> right);
    delete n;
  }
}

#endif	/* RBTREE_H */
