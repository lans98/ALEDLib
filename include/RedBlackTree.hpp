#ifndef ALED_REDBLACK_TREE_H
#define ALED_REDBLACK_TREE_H

#include <algorithm>
#include <iostream>
#include <stack>
#include <queue>

#include "tools/Sfinae.hpp"

namespace aled {

template <class Type>
class RedBlackTree {
private:
  // Enum type color
  enum Color { RED, BLACK };

  // Implementation for Node
  struct Node {
    Type  data;
    Color color;
    Node* parent;
    Node* left;
    Node* right;

    Node*& child(bool id) { return (id? right : left); }

    Node* uncle() {
      // p for parent
      Node* p = parent;
      if (!p) return nullptr;

      // g for grandparent
      Node* g = p->parent;
      if (!g) return nullptr;

      // Opposite to default
      return g->child(g->data > p->data);
    }

    Node(Type&& data)      : data(data), color(RED), parent(0), left(0), right(0) {}
    Node(const Type& data) : data(data), color(RED), parent(0), left(0), right(0) {}
  };

  // Root node member
  Node* m_root;



public:
  // Constructors and destructors
  RedBlackTree(): m_root(nullptr) {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }
 ~RedBlackTree(){ clear(m_root); }

  // Public methods

  void reset() { clear(m_root); m_root = 0; }

  bool add(const Type& data) {
    Node** n;
    Node*  p;

    if (find(data, n, p))
      return false;

    (*n)         = new Node(data);
    (*n)->parent = p;

    update(*n);
    return  true;
  }


  bool add(Type&& data) {
    Node** n;
    Node*  p;

    if (find(data, n, p))
      return false;

    (*n)         = new Node(data);
    (*n)->parent = p;

    update(*n);
    return  true;
  }

  void printBFS(std::ostream& out = std::cout) {
    std::queue<Node*> q;
    q.push(m_root);

    Node* temp;
    while (!q.empty()) {
      temp = q.front();
             q.pop();

      out << temp->data << "(" << (temp->color == RED? "RED" : "BLACK") << ") ";

      if (temp->left)
        q.push(temp->left);
      else
        out << "[n](BLACK) ";

      if (temp->right)
        q.push(temp->right);
      else
        out << "[n](BLACK) ";

    }
  }

private:

  void clear(Node* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
  }

  bool find(const Type& data, Node**& node, Node*& parent) {
    node   = &m_root;
    parent = nullptr;

    while (*node) {
      if ((*node)->data == data) return true;

      parent = (*node);
      node   = &((*node)->child((*node)->data < data));
    }

    return false;
  }

  Color get_color(Node* n) { return (n? n->color : BLACK); }

  void update(Node* n) {
    if (!n->parent) {
      n->color = BLACK;
    } else if (get_color(n->parent) == BLACK) {
      return;
    } else if (get_color(n->uncle()) == RED) {
      n->parent->color         = BLACK;
      n->uncle()->color        = BLACK;
      n->parent->parent->color = RED;

      update(n->parent->parent);
    } else if (n->parent->parent){
      rebalance_case(n);
    }
  }

  void rebalance_case(Node* n) {
    Node* p = n->parent;
    Node* g = p->parent;

    if (g->left) {
      if (n == g->left->right) {
        rotate_left(p);
        n = n->left;
      }
    } else if (g->right) {
      if (n == g->right->left) {
        rotate_right(p);
        n = n->right;
      }

    }

    p = n->parent;
    g = p->parent;

    if (n == p->left)
      rotate_right(g);
    else
      rotate_left(g);

    g->color = RED;
    p->color = BLACK;
  }

  void rotate_left(Node* n) {
    // Possible parent
    Node* p = n->parent;

    // Right child
    Node* c = n->right;

    n->right  = c->left;
    c->left   = n;
    n->parent = c;
    c->parent = p;

    if (p)
      p->child(p->data < n->data) = c;
    else
      m_root = c;
  }

  void rotate_right(Node* n) {
    // Possible parent
    Node* p = n->parent;

    // Left child
    Node* c = n->left;

    n->left   = c->right;
    c->right  = n;
    n->parent = c;
    c->parent = p;

    if (p)
      p->child(p->data < n->data) = c;
    else
      m_root = c;
  }

};

}

#endif
