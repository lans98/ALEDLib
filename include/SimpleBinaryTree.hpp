#ifndef QAED_SBT_H
#define QAED_SBT_H

#include <iostream>
#include <functional>

#include "tools/Sfinae.hpp"
#include "Stack.hpp"
#include "Queue.hpp"

namespace qaed {

enum PRINT_ORDER {
  PRE_ORDER,
  IN_ORDER,
  POS_ORDER
};

template <class Type>
class SBT {
public:

  class Node {
  public:
    Type  data;
    Node* left;
    Node* right;

    Node(Type&& data)      : data(data), left(0), right(0) {}
    Node(const Type& data) : data(data), left(0), right(0) {}

    // Bool for ensure just 0 and 1 has indexes for left and right
    Node*& son(const bool& id) { return (id == 0? left : right); }
  };

private:
  Node* m_root;

public:
  SBT(): m_root(0) {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }

  ~SBT() { clear(m_root); }

  void clear() { clear(m_root);  m_root = 0; }

  bool add(const Type& data) {
    Node** q;
    if (find(data, q))
      return false;

    (*q) = new Node(data);
    return true;
  }

  bool rmvRecursive(const Type& data, bool allow_recursive_del = false) {
    Node** p;
    if (!find(data, p))
      return false;

    _clear_(*p);
    (*p) = nullptr;
  }

  bool rmv(const Type& data) {
    Node** p;

    if (!_find_(data, p))
      return false;

    if (_is_leaf_(*p)) {
      delete (*p);
      (*p) = nullptr;

    } else if (_has_only_one_child_(*p)) {
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
      Node** q = &((*p)->left);
      while ((*q)->right)
        q = &((*q)->right);

      std::swap((*p)->data, (*q)->data);

      if (_is_leaf_(*q)) {
        delete (*q);
        (*q) = nullptr;
      } else {
        Node* temp = (*q)->left;
        delete (*q);
        (*q) = temp;
      }
    }
  }


  unsigned long countNodes() { return count(m_root); }

  Node* getFatherOf(const Type& data, bool recursive = false) {
    Node* father;
    if (recursive)
      father = father_recursive(m_root, data);
    else
      father = father_iterative(data);

    if (!father)
      throw std::runtime_error("Node doesn\'t have father");
    else
      return father;
  }

  Node* getBrotherOf(const Type& data, bool recursive = false) {
    try {
      Node* father = getFatherOf(data, recursive);
      Node* bro    = father->son(!(father->data < data));

      if (!bro)
        throw std::runtime_error("Node doesn\'t have sibling");
      else
        return bro;

    } catch (const std::exception& e) { throw e; }
  }

  Node* getUncleOf(const Type& data, bool recursive = false) {
    try {
      Node* father = getFatherOf(data, recursive);
      Node* uncle  = getBrotherOf(father->data, recursive);

      if (!uncle)
        throw std::runtime_error("Node doesn\'t have uncle");
      else
        return uncle;

    } catch (const std::exception& e) { throw e; }
  }

  void print(std::ostream& out = std::cout, PRINT_ORDER po = PRE_ORDER) {
    switch (po) {
    case PRE_ORDER:
      print_pre(m_root, out);
      break;
    case IN_ORDER:
      print_in(m_root, out);
      break;
    case POS_ORDER:
      print_pos(m_root, out);
      break;
    default:
      throw std::runtime_error("Unknown print order code");
    }
  }

  void printLn(std::ostream& out = std::cout, PRINT_ORDER po = PRE_ORDER) {
    print(out, po);
    out << '\n';
  }

  void printIterative(std::ostream& out = std::cout) {
    Stack<Node*> stack;
    Node* node;
    stack.push(m_root);

    while (!stack.isEmpty()) {
      node = stack.top();
      stack.pop();
      if (node) {
        out << node->data << ' ';
        stack.push(node->right);
        stack.push(node->left);
      }
    }
  }

  int getHeight() { return get_height(m_root);  }

  void printBFS(std::ostream& out = std::cout) {
    Queue<Node*> queue;
    queue.push(m_root);
    Node* node;

    while (!queue.isEmpty()) {
      node = queue.top();
      queue.pop();
      out << node->data << ' ';
      if (node->left)  queue.push(node->left);
      if (node->right) queue.push(node->right);
    }
  }

private:
  bool find(const Type& data, Node**& p) {
    p = &m_root;
    while (*p) {
      if ((*p)->data == data) return true;
      p = &((*p)->son((*p)->data < data));
    }
    return false;
  }

  unsigned long count(Node* p) {
    if (!p) return 0UL;
    return 1UL + count(p->left) + count(p->right);
  }

  Node* father_recursive(Node* father, const Type& data) {
    // First check if node is not null, because we use it later
    if (!father) return nullptr;

    // Check if actual node->data() is equal to the node in search
    // Note this only will happen whit m_root
    if (father->data == data) return nullptr;

    // Now choose one son based on f_sort, so we don't check
    // both son (if there are two sons)
    Node* son = father->son(father->data < data);

    // Now check if the son choosen is not null, because we
    // use it later
    if (!son) return nullptr;

    // If son->data() is what we was searching return father
    if (son->data == data) return father;

    // Recursive call with the son, because that's the only
    // posible path based on f_sort, we won't go to right if
    // actual data is less than node->data(), (in case of
    // f_sort goes to right if actual data is greater than
    // node->data())
    return father_recursive(son, data);
  }

  Node* father_iterative(const Type& data) {
    if (!m_root) return nullptr;
    if (m_root->data == data) return nullptr;

    Node* father = m_root;
    Node* son    = m_root->son(m_root->data < data);
    while (son) {
      if (son->data == data) return father;
      father = son;
      son = father->son(father->data < data);
    }
    return nullptr;
  }

  void print_pre(Node* node, std::ostream& out) {
    if (!node) return;

    out << node->data << " ";
    print_pre(node->left, out);
    print_pre(node->right, out);
  }

  void print_in(Node* node, std::ostream& out) {
    if (!node) return;

    print_in(node->left, out);
    out << node->data << " ";
    print_in(node->right, out);
  }

  void print_pos(Node* node, std::ostream& out) {
    if (!node) return;

    print_pos(node->left, out);
    print_pos(node->right, out);
    out << node->data << " ";
  }

  bool is_leaf(Node* node) {
    if (!node->left && !node->right)
      return true;
    else
      return false;
  }

  int get_height(Node* node) {
    if (!node) return -1;

    int num_left  = get_height(node->left);
    int num_right = get_height(node->right);

    return (num_left > num_right? num_left : num_right) + 1;
  }

  void clear(Node* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
  }

};

}

#endif
