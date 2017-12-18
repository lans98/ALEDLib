#ifndef QAED_STACK_H
#define QAED_STACK_H 

#include <stdexcept>

namespace qaed {

template <class Type>
class Stack {
private: 

  struct Node {
    Type  data;
    Node* prev;

    Node(Type&& data)      : data(data), prev(nullptr) {}
    Node(const Type& data) : data(data), prev(nullptr) {}
  };

  Node* m_top;

public:
  Stack() : m_top(nullptr) {}
 ~Stack() { while(!isEmpty()) pop(); }

  void push(const Type& data) {
    Node* tmp = new Node(data);

    if (isEmpty()) {
      m_top = tmp;
    } else {
      tmp->prev = m_top;
      m_top     = tmp;
    }
  }

  void pop() {
    if (isEmpty()) return;

    Node* tmp;

    tmp   = m_top;
    m_top = m_top->prev;
    delete tmp;
  }

  Type top() {
    if (isEmpty()) throw std::runtime_error("Stack empty");
    return m_top->data;
  }

  bool isEmpty() { return (!m_top); }
};

}

#endif 
