#ifndef ALED_AVL_TREE_H
#define ALED_AVL_TREE_H

#include <algorithm>
#include <iostream>
#include <stack>
#include <queue>

#include "tools/Sfinae.hpp"

namespace aled {

enum PRINT_ORDER {
  PRE_ORDER,
  IN_ORDER,
  POS_ORDER
};

template <class Type>
class AVL {
private:

  struct Node {
    short bal_factor;
    Type  data;
    Node* parent;
    Node* left;
    Node* right;

    Node(Type&& data)      : bal_factor(0), data(data), left(0), right(0) {}
    Node(const Type& data) : bal_factor(0), data(data), left(0), right(0) {}

    Node*& child(bool id) { return (id == 0? left : right); }
  };

  Node* m_root;

public:
  AVL(): m_root(0) {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }

 ~AVL() { clear(m_root); }

  void clear() { clear(m_root); m_root = 0; }

  bool add(const Type& data) {
    Node** p;
    Node*  q;

    if (find(data, p, q))
      return false;

    (*p) = new Node(data);
    (*p)->parent = q;
    update(*p);
    return true;
  }

  bool add(Type&& data) {
    Node** p;
    Node*  q;

    if (find(data, p, q))
      return false;

    (*p) = new Node(data);
    (*p)->parent = q;
    update(*p);
    return true;
  }

  bool rmvRecursive(const Type& data) {
    Node** p;
    Node*  q;

    if (!find(data, p, q))
      return false;

    clear(*p);
    (*p) = nullptr;
  }

  // TODO: add cases to update balance factor
  bool rmv(const Type& data) {
    Node** p;
    Node*  q;

    if (!find(data, p, q))
      return false;

    if (is_leaf(*p)) {
      delete (*p);
      (*p) = nullptr;

    } else if (has_only_one_child(*p)) {
      Node* temp;

      if ((*p)->left) {
        temp = (*p)->left;
        delete (*p);
        (*p) = temp;
      } else {
        temp = (*p)->right;
        delete (*p);
        (*p) = temp;
      }

    } else {
      Node** r = &((*p)->left);
      while ((*r)->right)
        r = &((*r)->right);

      std::swap((*p)->data, (*r)->data);
      if (is_leaf(*r)) {
        delete (*r);
        (*r) = nullptr;

      } else {
        Node* temp = (*r)->left;
        delete (*r);
        (*r) = temp;
      }
    }
  }

  void print(PRINT_ORDER po = IN_ORDER, std::ostream& out = std::cout) {
    switch(po) {
    case PRE_ORDER:
      pre_order(out, m_root);
      break;
    case IN_ORDER:
      in_order(out, m_root);
      break;
    case POS_ORDER:
      pos_order(out, m_root);
      break;
    default:
      throw std::domain_error("Unknown print order code");
      break;
    }
  }

  void printLn(PRINT_ORDER po = IN_ORDER, std::ostream& out = std::cout) {
    print(po, out);
    out << std::endl;
  }

  void printBFS(std::ostream& out = std::cout) {
    std::queue<Node*> queue;
    queue.push(m_root);

    Node* temp = 0;
    while (!queue.empty()) {
      temp = queue.front();
             queue.pop();

      if (temp)
        out << temp->data << "(" << temp->bal_factor << ") ";
      else
        out << "[n] ";

      queue.push(temp->left);
      queue.push(temp->right);
    }
  }

private:

  bool is_leaf(Node* p) {
    if (!p) throw std::runtime_error("Null pointer (is_leaf: argument p is null)");
    return (!p->left && !p->right);
  }

  bool has_only_one_child(Node* p) {
    if (!p) throw std::runtime_error("Null pointer (has_only_one_child: argument p is null)");

    if (p->left && !p->right)
      return true;

    if (p->right && !p->left)
      return true;

    return false;
  }

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

  void simple_rotate(Node* root, bool child_id) {
    Node* pivot  = root->child(child_id);
    Node* middle = pivot->child(!child_id);

    root->child(child_id) = middle;
    if (middle)
      middle->parent = root;

    pivot->child(!child_id) = root;
    pivot->parent = root->parent;

    if (root->parent)
      root->parent->child(root->parent->data < root->data) = pivot;
    else
      m_root = pivot;

    root->parent  = pivot;

    root->bal_factor  = 0;
    pivot->bal_factor = 0;
  }


  void double_rotate(Node* root, bool child_id) {
    Node* pivot_0 = root->child(child_id);
    Node* pivot_1 = pivot_0->child(!child_id);

    root->child(child_id)     = pivot_1->child(!child_id);
    pivot_0->child(!child_id) = pivot_1->child( child_id);

    if (pivot_1->child(!child_id))
      root->child(child_id)->parent = root;

    if (pivot_1->child( child_id))
      pivot_0->child(!child_id)->parent = pivot_0;

    pivot_1->parent = root->parent;

    if (root->parent)
      root->parent->child(root->parent->data < root->data) = pivot_1;
    else
      m_root = pivot_1;

    pivot_1->child(child_id)  = pivot_0;
    pivot_1->child(!child_id) = root;

    pivot_0->parent = pivot_1;
    root->parent    = pivot_1;

    switch (pivot_1->bal_factor) {
    case 0:
      root->bal_factor    = 0;
      pivot_0->bal_factor = 0;
      break;
    case 1:
      if (child_id) {
        root->bal_factor    = -1;
        pivot_0->bal_factor =  0;
      } else {
        root->bal_factor    =  0;
        pivot_0->bal_factor = -1;
      }
      break;
    case -1:
      if (child_id) {
        root->bal_factor    = 0;
        pivot_0->bal_factor = 1;
      } else {
        root->bal_factor    = 1;
        pivot_0->bal_factor = 0;
      }
      break;
    }
  }

  void rebalance_checking(Node* p) {
    if (p->bal_factor > 1) {
      if (p->right->bal_factor >= 1) {
        simple_rotate(p, 1);
      } else {
        double_rotate(p, 1);
      }
    } else {
      if (p->left->bal_factor <= -1) {
        simple_rotate(p, 0);
      } else {
        double_rotate(p ,0);
      }
    }
  }

  void update(Node* node) {
    Node* parent = node->parent;

    while (parent) {

      if (parent->left  == node)
        parent->bal_factor -= 1;

      if (parent->right == node)
        parent->bal_factor += 1;

      if (parent->bal_factor == 0)
        break;

      if (parent->bal_factor > 1 || parent->bal_factor < - 1) {
        rebalance_checking(parent);
        return;
      }

      node   = parent;
      parent = parent->parent;
    }
  }

  void pre_order(std::ostream& out, Node* p) {
    if (!p) return;
    out << p->data << ' ';
    pre_order(out, p->left);
    pre_order(out, p->right);
  }

  void in_order(std::ostream& out, Node* p) {
    if (!p) return;
    in_order(out, p->left);
    out << p->data << ' ';
    in_order(out, p->right);
  }

  void pos_order(std::ostream& out, Node* p) {
    if (!p) return;
    pos_order(out, p->left);
    pos_order(out, p->right);
    out << p->data << ' ';
  }

public:
  friend class Iterator;

  class Iterator {
  private:
    AVL<Type>::Node* current;
  public:
    Iterator(): current(0) {}

    Iterator(AVL<Type>::Node* root) {
      current = root;

      if (!current) return;

      while (current->left)
        current = current->left;
    }

    Iterator(const Iterator& it): current(it.current) {}
    Iterator(Iterator&& it): current(it.current) {}

    Iterator& operator=(const Iterator& it) {
      current = it.current;
      return (*this);
    }

    Iterator& operator=(Iterator&& it) {
      current = it.current;
      return (*this);
    }

    Iterator operator++() {
      if (!current) return (*this);

      Iterator r(current);

      if (current->right) {
        current = current->right;
        while (current->left)
          current = current->left;

        return r;
      }

      while (true) {
        if (current->parent == nullptr) {
          current = nullptr;
          return r;
        }

        if (current->parent->left == current) {
          current = current->parent;
          return r;
        }

        current = current->parent;
      }
    }

    bool operator==(const Iterator& it) { return (current == it.current); }
    bool operator!=(const Iterator& it) { return (current != it.current); }

    Type& operator*() {
      if (!current) throw std::runtime_error("Null pointer (iterator: value is null)");
      return current->data;
    }

  };

  Iterator begin() const { return Iterator(m_root); }
  Iterator end()   const { return Iterator(nullptr); }

  Iterator last()  const {
    Node* temp = m_root;
    while (temp->right)
      temp = temp->right;

    return Iterator(temp);
  }

  Iterator find(const Type& data) {
    Node** p;
    Node*  q;
    if (find(data, p, q))
      return Iterator(*p);
    else
      return Iterator(0);
  }

};

}

#endif
