#ifndef ALED_QUEUE_H
#define ALED_QUEUE_H

#include <stdexcept>

namespace aled {

template <class Type>
class Queue {
private:

  struct Node {
    Type  data;
    Node* next;

    Node(Type&& data)      : data(data), next(nullptr) {}
    Node(const Type& data) : data(data), next(nullptr) {}
  };

  Node* m_head;
  Node* m_tail;

public:
  Queue(): m_head(nullptr), m_tail(nullptr) {}

  void push(const Type& data) {
    Node* tmp = new Node(data);

    if (isEmpty()) {
      m_head = tmp;
      m_tail = tmp;
    } else {
      m_tail->next = tmp;
      m_tail = tmp;
    }
  }

  void pop() {
    if (isEmpty()) return;

    Node* tmp = m_head;
    m_head = m_head->next;
    delete tmp;
  }

  Type top() {
    if (isEmpty()) throw std::runtime_error("Empty queue");
    return m_head->data;
  }

  bool isEmpty() { return (!m_head); }

};

}

#endif
